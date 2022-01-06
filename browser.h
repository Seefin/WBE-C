/*****h* browser/browser
 *
 * NAME
 *   browser.h - Header for the main module of our browser program.
 *
 * SYNOPSIS
 *   start(options_t options)
 *
 * FUNCTION
 *   This module is the main browser component; it is responsible for retrieving
 *   documents, lexing/parsing the HTML tree, and laying out the page.
 *
 * INPUTS
 *   start(options_t options) - the URL to fetch
 *
 * SEE ALSO 
 *   browser/main.c
 *
 * SOURCE
 */

#define BFAILURE 1
#define BSUCCESS 0

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netdb.h>

int start(char *uri, int ssl, int verbose);
char * getPage(char *host, char *path);

/*
*****
*/
