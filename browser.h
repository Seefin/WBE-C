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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netdb.h>

int start(char *uri, int ssl, int verbose);
char * getPage(char *host, char *path);

/**Assert replacement
 * The assert() macro from assert.h does not supply the ability to provide a custom error,
 * an causes the UX to be, well, unsatisfactory. This wrapper improves that functionality,
 * and provides better UX.
 *
 * INPUTS
 *   int condition - Condition to evaluate; if true, nothing happens. If false, abort() is called (Effectively)
 *   char *format  - Format string to print
 *   ...           - Optional arguments to insert into the format string
 *
 * OUTPUTS
 *   None
 *
 * SIDE-EFFECTS
 *   If condition is false, the program is dumped. So that's a pretty significant side-effect, I suppose.
 *
 * SEE ALSO
 *  - man 3 fprintf
 *  - man 3 abort
 *  - https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 */
#ifdef assert
#undef assert
#endif
#define assert(condition,format,...) if( ! (condition) ){ fprintf(stderr,format __VA_OPT__(,) __VA_ARGS__); abort(); }


/*
*****
*/
