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
#define BUFSIZE 1024

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netdb.h>

#include <ctype.h>

int start(char *uri, int ssl, int verbose);
char * request(char *host, char *path);
void show(char *page);

/**Headers
 * Struct for HTTP headers
 */
typedef struct {
	char *header;
	char *value;
} httpHeader;

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

/**Very Basic tokeniser
 * Takes a list of tokens, and splits them using a delimiter, and then returns them
 * in an array (passed into the function).
 *
 * INPUTS
 *   char *string     - The string to tokenise (obviously)
 *   char *delimiters - The delimiters to use (multiple can be used, see strtok())
 *   int n            - The number of splits to perform (0-based). The method will split the
 *                      input string n times, and then the rest of the string is put into
 *                      the final slot in the array. For example:
 *
 *                      ```
 *                      char **tokens;
 *                      char *string = "This is a string";
 *                      tokens = malloc( 3 * sizeof( char * ) );
 *                      StringTokens(string, " ", 2, tokens);
 *                      ```
 *
 *                      gives the following results:
 *
 *                      ```
 *                      tokens[0] = "This";
 *                      tokens[1] = "is";
 *                      tokens[2] = "a string";
 *                      ```
 *   char **tokens    - An array to store the results inside. This array should be big enough
 *                      to contain n + 1 pointers. If it is not, there will be issues.
 *
 * OUTPUTS
 *   None
 *
 * SIDE-EFFECTS
 *   Modifies the passed in tokens buffer
 *
 * SEE ALSO
 *   - man 3 strtok_r
 */

static inline void StringTokens(char *string, char *delimiters, int n, char **tokens)
{
	assert(strlen(string) > 0, "Provided string is 0 length!");
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

/**Change Case
 * Changes case of a string, rather than changing case of a character.
 *
 * INPUTS
 *   char *string - String to change case of
 *
 * OUTPUTS
 *   char * - Case changed version of string
 */
static inline char * StringLower(char *string)
{
	int i;
	char *output;

	output = strdup(string);

	for( i = 0; i < strlen(output); i++)
	{
		output[i] = tolower((unsigned char) string[i]);
	}
	return output;
}

static inline char * StringUpper(char *string)
{
	int i;
	char *output;

	output = strdup(string);

	for( i = 0; i < strlen(output); i++)
	{
		output[i] = toupper((unsigned char) string[i]);
	}
	return output;
}
/**Append Strings
 * Handles appending strings to other strings, and ensures that the
 * destination has enough space.
 *
 * INPUTS
 *   const char *old - the string we want to append to
 *   const char *new - the string to append
 * OUTPUTS
 *   char * - Appended string
 */
static inline char * AppendString(const char *old, const char *new_str)
{
	const size_t old_len = strlen(old), new_len = strlen(new_str);
	const size_t out_len = old_len + new_len + 1;

	char *out = malloc(out_len);

	memcpy(out, old, old_len);
	memcpy(out + old_len, new_str, new_len + 1);

	return out;
}
/*
*****
*/
