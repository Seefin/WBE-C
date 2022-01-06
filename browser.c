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
		printf("Protocol is:\t%s\n",protocol);
		printf("Host is:    \t%s\n",host);
		printf("Path is:     \t%s\n",path);
	}
	/* Default succeed */
	free(path);
	free(protocol);
	return BSUCCESS;
}

/*
*****
*/
