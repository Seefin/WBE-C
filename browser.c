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
	assert( (path = malloc(sizeof(char))) != NULL, "Memory Error - cannot allocate path memory");
	path[0] = '/';
	/* Fix for 0 length path */
	int new_path_size = strlen(tokens[2] + 1) > 1 ? strlen(tokens[2] + 1) : 2;
	assert( realloc(path, new_path_size),"Could not realloc path buffer!\n");
	path = strcat(path,tokens[2]);

	/* Verbose/debug */
	if ( verbose >= 1 )
	{
		fprintf(stderr,"Protocol is: \t%s\n",protocol);
		fprintf(stderr,"Host is:     \t%s\n",host);
		fprintf(stderr,"Path is:     \t%s\n",path);
	}

	/* Make connection to server, fetch base page */
	char *page = request(host, path);
	rest = strdup(page); /* XXX: Copy response into a modifiable buffer to parse headers - may not require this */

	/* Read the status - if not 200, fail and say why */
	assert( realloc(tokens,3 * sizeof(char *)), "Could not realloc() tokens buffer!\n" );
	char *responseline, *version, *status, *explanation;
	responseline = strtok_r(rest, "\r\n", &rest);
	StringTokens(responseline, " ", 2, tokens);
	version = strdup(tokens[0]);
	status = strdup(tokens[1]);
	explanation = strdup(tokens[2]);
	if ( verbose >= 1 )
	{
		fprintf(stderr,"version:    \t%s\n", version);
		fprintf(stderr,"status:     \t%s\n", status);
		fprintf(stderr,"explanation:\t%s\n", explanation);
	}
	assert( (strstr(status,"200") != NULL), "%s: %s\n", status, explanation);

	/* Store all the headers */
	httpHeader *headers;
	int headerCount = 0;
	assert( headers = malloc( BUFSIZE * sizeof(httpHeader)), "Memory Error - cannot allocate memory for headers" );
	responseline = strtok_r(rest, "\n", &rest);
	while( responseline != NULL && responseline[0] != '\r' )
	{
		/* Parse Headers - ends in \r\n, be sure to remove the '\r'! */
		assert( realloc(tokens, 2 * sizeof(char *)), "Memory Error - cannot allocate memory for header %d\n", headerCount );
		StringTokens(responseline, ":", 1, tokens);
		headers[headerCount].header = StringLower(strdup(tokens[0]));
		headers[headerCount].value  = StringLower(strdup(tokens[1]));
		/* Normalise values - make lowercase and strip leading whitespace */
		headers[headerCount].value[strlen(headers[headerCount].value) - 1] = '\0';
		headers[headerCount].value = headers[headerCount].value + 1;
		/* Consume next line */
		responseline = strtok_r(rest, "\n", &rest);
		headerCount++;
	}
	assert( realloc(headers, headerCount * sizeof(httpHeader)), "Memory Error - cannot shrink headers buffer");
	/*finally, show the page */
	show(rest);

	/* Default succeed */
	free(path);
	free(protocol);
	free(headers);
	free(tokens);
	return BSUCCESS;
}

char * request(char *host, char *path)
{
	/* Construct hints for servifo struct generation */
	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	/*Create servinfo struct. Use hardcoded value "http" here for now */
	struct addrinfo *servinfo;
	servinfo = NULL;
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
	char *str = recv_with_timeout(sockfd, NET_CONNECT_TIMEOUT_SECS);

	/* Free memory and return response */
	close(sockfd);
	freeaddrinfo(servinfo);
	free(message);
	return str;
}

char * recv_with_timeout(int socket, int max_time)
{
	int size_recv, errno, total_size = 0;
	struct timeval begin, now;
	char chunk[BUFSIZE], *str;
	double timediff;

	/* Allocate return string */
	assert( (str=malloc(BUFSIZE)) != NULL, "Memory Error - cannot allocate inital reply memory\n" );
	memset(str, 0, BUFSIZE);

	/* Switch socket modes */
	fcntl(socket, F_SETFL, O_NONBLOCK);

	/*Record start time */
	gettimeofday(&begin, NULL);

	/* Loop until either:
	 *  - All data recieved
	 *  - Timeout reached, and some data recieved
	 *  - 2 * timeout reached and no data recieved
	 */
	while( 1 )
	{
		/* Calculate elapsed time */
		gettimeofday(&now, NULL);
		timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
		/* Break if timeout exceeded */
		if( total_size > 0 && timediff > max_time)
		{
			break;
		}
		else if( timediff > (2 * max_time ))
		{
			break;
		}
		/* recv() data */
		memset(chunk, 0, BUFSIZE);
		/* If nothing recv()'d, wait */
		size_recv = recv(socket, chunk, BUFSIZE, 0);
		if( size_recv < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
		{
			/*Still more to come */
			usleep(100000);
		}
		else if( size_recv == 0 ){
			/* Finished */
			break;
		}
		else
		{
			total_size += size_recv;
			chunk[BUFSIZE] = '\0';
			char *new_str = AppendString(str, chunk);
			str = new_str;
		}
	}
	return str;
}

void show(char *page)
{
	int in_angle, length;
	in_angle = 0;
	length = strlen(page);
	for (int i = 0; i <= length; i++)
	{
		if( page[i] == '<' )
		{
			in_angle = 1;
		}
		else if( page[i] == '>' )
		{
			in_angle = 0;
		}
		else if( ! in_angle )
		{
			printf("%c",page[i]);
		}
	}
	printf("%s","\n");
}

/*
 *****
 */
