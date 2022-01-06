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
	assert( (strncmp("http://",uri,strlen("http://")) == 0) || (strncmp("https://", uri, strlen("https://")) == 0) );

	/**URI TOKENIZER
	 * We use the strtok_r function to progressively chew through the string
	 * and assign the various tokens to the variable we want to use.
	 * This is a destructive operation over the URI variable, so the original 
	 * contents are preserved in url if they are needed.
	 *
	 * TODO: Refactor this into something like:
	 *   char * tokenize(const char *delim, char *tokens[])
	 */
	char *host, *path, *protocol, *token;
	char *rest = uri;

	token = strtok_r(rest, "/", &rest);
	protocol = calloc( strlen(token) + 3, sizeof(char) );
	protocol = strdup(token);
	host = strtok_r(rest, "/", &rest);
	/* Prepend the path with '/' - this ensures we get the root page if
	 * no path is given.
	 */
	path = malloc(sizeof(char));
	path[0] = '/';
	if ( realloc(path, strlen(rest) + 1) )
	{
		path = strcat(path,rest);
	}
	else
	{
		perror("realloc of Path failed");
		return BFAILURE;
	}
	
	/*Add the // back to the protocol */
	if ( realloc(protocol, strlen(protocol) + 3) )
	{
		protocol = strcat(protocol, "//");
	}
	else
	{
		perror("realloc of Protocol failed");
		return BFAILURE;
	}

	/* Verbose/debug */
	if ( verbose >= 1 )
	{
		fprintf(stderr,"Protocol is:\t%s\n",protocol);
		fprintf(stderr,"Host is:    \t%s\n",host);
		fprintf(stderr,"Path is:     \t%s\n",path);
	}

	/* pass to get page */
	char *page = getPage(host, protocol, path);
	printf("%s\n",page);
	/* Default succeed */
	free(path);
	free(protocol);
	return BSUCCESS;
}

char * getPage(char *host, char *protocol, char *path)
{
	char *str = calloc(2000, sizeof(char));

	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo *servinfo;
	int status = getaddrinfo(host,"http", &hints, &servinfo);

	int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if( connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0 )
	{
		fprintf(stderr,"Connection to host '%s' failed!\n", host);
		exit(EXIT_FAILURE);
	}

	char *message = calloc(1000, sizeof(char));
        strcat(message,"GET /index.html HTTP/1.0\r\nHost: ");
	strcat(message, host);
	strcat(message, "\r\n\r\n");
	int message_length = strlen(message) + 1;
	realloc(message,message_length);
	if( send(sockfd, message, strlen(message), 0) < 0 )
	{
		fprintf(stderr, "Sending message '%s' to host '%s' failed.\n", message, host);
		exit(EXIT_FAILURE);
	}
	char *reply = calloc(4000, sizeof(char));
	if( recv(sockfd, reply, 2000, 0) < 0 )
	{
		fprintf(stderr,"Could not call recv() on server reply\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		int str_length = strlen(reply) + 1;
		str = strdup(reply);
		realloc(str, str_length);
	}

	freeaddrinfo(servinfo);
	free(message);
	return str;
}

/*
*****
*/
