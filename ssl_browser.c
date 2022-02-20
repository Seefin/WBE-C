#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

const char *PREFERRED_CIPHERS = "HIGH:!aNULL:!kRSA:!SRP:!PSK:!CAMELLIA:!RC4:!MD5:!DSS";
#ifndef assert
#define ASSERT(x) { if( !(x) ){fprintf(stderr, "Assertion: %s, function: %s, line: %d\n", (char *)(__FILE__), (char *)(__func__), (int)(__LINE__)); raise(SIGTRAP); } }
#endif

char * recv_with_timeout(SSL *ssl, int timeout);
static inline void StringTokens(char *string, char *delimiters, int n, char **tokens);
int request_ssl( char *host);

static inline void StringTokens(char *string, char *delimiters, int n, char **tokens)
{
	if( strlen(string) <= 0 )
	{
		fprintf(stderr,"Provided string is 0 length!");
		return;
	}
	char *rest;
	int i = 0;

	rest = strdup(string);

	/* Loop through input, and tokenise. */
	for(i = 0; i < n; i++)
	{
		tokens[i] = strtok_r(rest, delimiters, &rest);
	}
	/* Place everything else into the final slot */
	tokens[i] = rest;
}

int main(int argc, char **argv)
{
	/* Ensure appropriate No.# of arguments */
	assert( argc == 2, "Error - please provide a URL\n" );
	assert( strstr(argv[1], "https://") != NULL, "Error - must be https:// protocol\n");

	char *host, **tokens;
	char *rest = strdup(argv[1]);

	tokens = malloc(3 * sizeof(char));
	StringTokens(rest, "/", 2, tokens);

	host = strdup(tokens[1]);
	free(tokens);
	return request_ssl(host);
}

int request_ssl(char *host)
{
	long res = 1, ret = 1;
	unsigned long ssl_err = 0;

	SSL_CTX *ctx;
	BIO *web = NULL, *out = NULL;
	SSL *ssl = NULL;

	/* Infinte loop that I'm not 100% sure we need... */
	do {
		/* This is cast to void, because the return value is meaningless - errors happen
		 * once we start trying to create stuff if either of these functions fail - the functions 
		 * themselves do not fail.
		 */
		(void)SSL_library_init();
		SSL_load_error_strings();

		/* Set our method up to use TLS, not SSL */
		const SSL_METHOD *method = TLS_method();
		ssl_err = ERR_get_error();
		ASSERT( NULL != method );
		if(!(NULL != method))
		{
			print_error_string(ssl_err, "TLS_method()");
			break;
		}
		/* Create Context and set verify types */
		ctx = SSL_CTX_new(method);
		ssl_err = ERR_get_error();
		ASSERT(NULL != ctx);
		if(!(NULL != ctx))
		{
			print_error_string(ssl_err, "SSL_CTX_new()");
			break;
		}
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
		SSL_CTX_set_verify_depth(ctx, 100);
		
		/* Remove SSLv2 and SSLv3, and ensure TLS1.0 handshake used */
		const unsigned long flags = SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
		unsigned long old_opts = SSL_CTX_set_options(ctx, flags);
		UNUSED(old_opts);
		
		/* Set verify paths - where to find certificate chains to trust */
		res = SSL_CTX_set_default_verify_paths(ctx);
		ssl_err = ERR_get_error();
		ASSERT( 1 == res );
		if( !(1 == res) )
		{
			print_error_string(ssl_err, "SSL_CTX_set_default_verify_paths");
			/* Non-fatal, but will cause failure elsewhere if fails */
		}

		/* Open BIOs */
		web = BIO_new_ssl_connect(ctx);
		ssl_err = ERR_get_error();
		ASSERT( 1 == res );
		if(!(1 == res))
		{
			print_error_string(ssl_err, "BIO_new_ssl_connect");
			break;
		}

		/* Start connection */
		res = BIO_set_conn_hostname(web, hostname ":443"); 


	} while (0)
	const SSL_METHOD *ssl_method;
	X509 *server_cert;

	/*Initalise SSL and CTX */
	SSL_library_init();
	SSL_load_error_strings();
	ssl_method = TLS_client_method();
	assert( (ssl_ctx = SSL_CTX_new(ssl_method)) != NULL, "SSL Context error - ctx is NULL\n" );

	/* Load client certificates for verification */
	if( !SSL_CTX_set_default_verify_paths(ssl_ctx) )
	{
		ERR_print_errors_fp(stderr);
		return(1);
	}

	SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, NULL);
	SSL_CTX_set_verify_depth(ssl_ctx, 100); /* Alow 100 intermediate certs */
	SSL_CTX_set_ciphersuites(ssl_ctx, "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_128_GCM_SHA256");

	/* Standard socket stuff */
	struct addrinfo ai_hints;
	memset(&ai_hints, 0, sizeof(struct addrinfo));
	ai_hints.ai_family   = AF_INET;
	ai_hints.ai_socktype = SOCK_STREAM;

	struct addrinfo *si_hints;
	si_hints = NULL;

	int err = 0;
	if( getaddrinfo(host, "http", &ai_hints, &si_hints) != 0 )
	{
		return EXIT_FAILURE;
	}
	int socket_fd = socket(si_hints->ai_family, si_hints -> ai_socktype, si_hints->ai_protocol);
	assert( socket_fd > 0, "Socket Creation error: %d\n", errno);
	assert( connect(socket_fd, si_hints->ai_addr, si_hints->ai_addrlen) >= 0, "Connect error");

	/* Create SSL structure, and bind socket to it */
	ssl = SSL_new(ssl_ctx);
	assert( ssl != NULL, "SSL error - NULL on create\n" );
	SSL_set_fd(ssl, socket_fd);

	/* Perform SSL handshake */
	SSL_set_connect_state(ssl);
	err = SSL_connect(ssl);
	if( err != 1 )
	{
		fprintf(stderr, "Crypto Error:\n");
		ERR_print_errors_fp(stderr);
		fprintf(stderr, "SSL Connection Error:%d\n",SSL_get_error(ssl,err));
		return EXIT_FAILURE;
	}
	assert( SSL_connect(ssl) == 1, "SSL Connect Error\n");
	fprintf(stdout,"SSL connected using cipher %s\n", SSL_get_cipher(ssl));

	/* Print server cert info - OPTIONAL */
	server_cert = SSL_get_peer_certificate(ssl);
	if( server_cert != NULL )
	{
		char *str;
		fprintf(stdout, "Server Certificate:\n");
		str = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);
		assert( str != NULL, "Server Certificate Name Error\n" );
		fprintf(stdout,"\t Subject: %s\n", str);
		free(str);
		str = X509_NAME_oneline(X509_get_issuer_name(server_cert),0,0);
		assert( str != NULL, "Server Certificate Issuer Error\n" );
		fprintf(stdout,"\t Issuer: %s\n", str);
		free(str);
		X509_free(server_cert);
	}
	else
	{
		fprintf(stdout,"The SSL does not have a certificate\n");
	}

	/* Write data to buffer to send */
	char *message = calloc(1024, sizeof(char));
	strcat(message,"GET / HTTP/1.0\r\nHost: ");
	strcat(message,host);
	strcat(message,"\r\n\r\n");

	/* Send data */
	assert( SSL_write(ssl, message, strlen(message)) != -1, "SSL Write Error\n" );
	char *str = recv_with_timeout(ssl, 5);
	assert( str != NULL, "Recv() Error\n");
	fprintf(stdout,"Reply:\n%s\n", str);
	free(str);

	/* Close Session */
	assert( SSL_shutdown(ssl) != -1, "Error closing connection\n" );
	assert( close(socket_fd) != -1, "Error closing socket\n" );
	SSL_free(ssl);
	SSL_CTX_free(ssl_ctx);
	free(message);

	return EXIT_SUCCESS;
}


char * recv_with_timeout(SSL *ssl, int timeout)
{
	/*TODO: implement timeout */
	char *str = calloc( 4096, sizeof(char) );
	int recv;
	recv = SSL_read(ssl, str, 4095);
	if( recv == -1 )
		return NULL;
	str[4096] = '\0';

	return str;
}
