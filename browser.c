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
	 * TODO: Refactor this into something like:
	 *   char * tokenize(const char *delim, char *tokens[])
	 */
	char *host, *path, *protocol, *token;
	char *rest = strdup(uri);

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
	
	/*remove the : from the protocol */
	if ( ! realloc(protocol, strlen(protocol) - 1) )
	{
		perror("realloc of Protocol failed");
		return BFAILURE;
	}
	protocol[strlen(protocol) - 1 ] = '\0';

	/* Verbose/debug */
	if ( verbose >= 1 )
	{
		fprintf(stderr,"Rest is:    \t%s\n", rest);
		fprintf(stderr,"URI is:     \t%s\n", uri);
		fprintf(stderr,"Protocol is:\t%s\n",protocol);
		fprintf(stderr,"Host is:    \t%s\n",host);
		fprintf(stderr,"Path is:     \t%s\n",path);
	}

	/* pass to get page */
	char *page = getPage(host, path);
	/* Read the status - if not 200, fail and say why */
	char *line, *version, *status, *explanation;
	rest = strdup(page);
	line = strtok(rest, "\r\n");
	version = strtok_r(line, " ", &line);
	status = strtok_r(line, " ", &line);
	/* The rest of the line is an explanatory message if status is NOT 200 */
	explanation = line;
	assert( (strstr(status,"200") != NULL), "%s: %s\n", status, explanation);
	printf("%s\n",page);
	if ( verbose >= 1 )
	{
		fprintf(stderr,"version:   \t%s\n", version);
		fprintf(stderr,"status:    \t%s\n", status);
		fprintf(stderr,"explanation:\t%s\n", explanation);
	}
	/* Default succeed */
	free(path);
	free(protocol);
	return BSUCCESS;
}

char * getPage(char *host, char *path)
{
	char *str = calloc(2000, sizeof(char));

	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo *servinfo;
	/* Always use 'http' for protocol - SSL comes later, lads */
	if( getaddrinfo(host,"http",&hints, &servinfo) != 0 )
	{
		fprintf(stderr,"getaddrinfo failure - this is normally a DNS thing");
		exit(EXIT_FAILURE);
	}

	int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if( connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0 )
	{
		fprintf(stderr,"Connection to host '%s' failed!\n", host);
		exit(EXIT_FAILURE);
	}

	char *message = calloc(1000, sizeof(char));
	strcat(message,"GET ");
	strcat(message, path);
        strcat(message," HTTP/1.0\r\nHost: ");
	strcat(message, host);
	strcat(message, "\r\n\r\n");
	int message_length = strlen(message) + 1;
	if( ! realloc(message,message_length) )
	{
		fprintf(stderr,"Memory error - cannot realloc message buffer");
		exit(EXIT_FAILURE);
	}
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
		if( ! realloc(str, str_length) )
		{
			fprintf(stderr,"Memory error - cannot realloc reply buffer");
			exit(EXIT_FAILURE);
		}
	}

	freeaddrinfo(servinfo);
	free(message);
	return str;
}

/*
*****
*/
