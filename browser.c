/*****f* browser/browser
 *
 * NAME
 *   browser.c - The main module of our browser program.
 *
 * SYNOPSIS
 *   start(options_t options)
 *
 * FUNCTION
 *   This module is the main browser component; it is responsible for retrieving
 *   documents, lexing/parsing the HTML tree, and laying out the page.
 *
 * INPUTS
 *   start(options_t options) - Options struct containing URL to fetch
 *
 * SEE ALSO 
 *   browser/main.c
 *
 * SOURCE
 */

#include "browser.h"

/** Setup for browsing 
 * Tokenises the URL into host, path and protocol, and then passes the information
 * to the getPage method to actually get the page.
 *
 * ASSERTIONS:
 *   uri starts with 'http://' or 'https://' 
 */
int start(char *uri, int ssl, int verbose)
{
	assert( ((strncmp("http://",uri,strlen("http://")) == 0) || (strncmp("https://", uri, strlen("https://")) == 0)), "URI must start with http:// or https://\n" );

	/**URI TOKENIZER
	 * We use the strtok_r function to progressively chew through the string
	 * and assign the various tokens to the variable we want to use.
	 * This is a destructive operation over the URI variable, so the original 
	 * contents are preserved in url if they are needed.
	 *
	 */
	char *host, *path, *protocol;
	char **tokens;

	char *rest = strdup(uri);
	tokens = malloc(3 * sizeof(char *));
	StringTokens(rest, "/",2, tokens);

	protocol = calloc( strlen(tokens[0]) + 3, sizeof(char) );
	protocol = strdup(tokens[0]);
	/*remove the : from the protocol */
	assert( realloc(protocol, strlen(protocol) - 1),"Could not realloc protocol buffer!\n"); 
	protocol[strlen(protocol) - 1 ] = '\0';

	host = strdup(tokens[1]);

	/* Prepend the path with '/' - this ensures we get the root page if
	 * no path is given.
	 */
	path = malloc(sizeof(char));
	path[0] = '/';
	/* Fix for 0 length path */
	int new_path_size = strlen(tokens[2] + 1) > 1 ? strlen(tokens[2] + 1) : 2;
	assert( realloc(path, new_path_size),"Could not realloc path buffer!\n");
	path = strcat(path,tokens[2]);

	/* Verbose/debug */
	if ( verbose >= 1 )
	{
		fprintf(stderr,"Rest is:    \t%s\n", rest);
		fprintf(stderr,"URI is:     \t%s\n", uri);
		fprintf(stderr,"Protocol is:\t%s\n",protocol);
		fprintf(stderr,"Host is:    \t%s\n",host);
		fprintf(stderr,"Path is:     \t%s\n",path);
	}

	/* Make connection to server, fetch base page */
	char *page = getPage(host, path);
	rest = strdup(page); /* XXX: Copy response into a modifiable buffer to parse headers - may not require this */

	/* Read the status - if not 200, fail and say why */
	assert( realloc(tokens,3 * sizeof(char *)), "Could not realloc() tokens buffer!\n" );
	char *statusline, *version, *status, *explanation;
	statusline = strtok_r(rest, "\r\n", &rest);
	StringTokens(statusline, " ", 2, tokens);
	version = strdup(tokens[0]);
	status = strdup(tokens[1]);
	explanation = strdup(tokens[2]);
	if ( verbose >= 1 )
	{
		fprintf(stderr,"version:   \t%s\n", version);
		fprintf(stderr,"status:    \t%s\n", status);
		fprintf(stderr,"explanation:\t%s\n", explanation);
	}

	assert( (strstr(status,"200") != NULL), "%s: %s\n", status, explanation);

	/*finally, show the page */
	printf("%s\n",page);

	/* Default succeed */
	free(path);
	free(protocol);
	return BSUCCESS;
}

char * getPage(char *host, char *path)
{
	char *str = calloc(BUFSIZE, sizeof(char));

	/* Construct hints for servifo struct generation */
	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	/*Create servinfo struct. Use hardcoded value "http" here for now */
	struct addrinfo *servinfo;
	assert( getaddrinfo(host, "http", &hints, &servinfo) == 0, "getaddrinfo error - this is normally a DNS issue\n" );

	/* Create socket to connect to our destiantion, and connect it */
	int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	assert( connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) >= 0, "Connection to host %s failed!\n", host );

	/* Create and send simple GET request to server */
	char *message = calloc(BUFSIZE, sizeof(char));
	strcat(message,"GET ");
	strcat(message, path);
	strcat(message," HTTP/1.0\r\nHost: ");
	strcat(message, host);
	strcat(message, "\r\n\r\n");
	int message_length = strlen(message) + 1;
	/* Don't send 1000 '\0' bytes to the server */
	assert(realloc(message, message_length), "Memory Error - cannot realloc message buffer\n");
	assert(send(sockfd, message, strlen(message), 0) >= 0, "Sending Error - cannot send message:\n%s\nto host: %s\n",message,host);

	/* Recieve and read reply from the server */
	char *reply = calloc(BUFSIZE, sizeof(char));
	assert(recv(sockfd, reply, BUFSIZE, 0) >= 0, "Receiving Error - cannot call recv() on reply from %s\n", host);
	int str_length = strlen(reply) + 1;
	str = strdup(reply);
	assert(realloc(str, str_length),"Memory Error - cannot realloc reply buffer");

	/* Free memory and return response */
	freeaddrinfo(servinfo);
	free(message);
	return str;
}

/*
 *****
 */
