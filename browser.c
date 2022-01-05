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

int start(char *uri)
{
	assert(strncmp("http://",uri,strlen("http://")) == 0);
	return BSUCCESS;
}

/*
*****
*/
