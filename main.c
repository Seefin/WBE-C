/*****f* browser/main
 *
 * NAME
 *   main.c - The main entry point to our browser program.
 *
 * SYNOPSIS
 *   main(int argc, char *argv[], char *envp[])
 *
 * FUNCTION
 *   This module is the main entry point for our browser program; it interprets
 *   the command line instructions, and passes them through to the browser for
 *   action as an browser_args struct
 *
 * INPUTS
 *   The standard C main function accepts two inputs (well, three if we include
 *   envp, but we aren't). This function also accepts these inputs:
 *
 *   int argc     - The number of items in the argv array
 *   char *argv[] - Each argument from the command line, in string form.
 *
 * SEE ALSO 
 *   browser/start.c
 *
 * SOURCE
 */

#include "main.h"

void usage(char *progname)
{
	fprintf(stderr, USAGE_FMT, progname?progname:DEFAULT_PROGNAME);
	exit(EXIT_FAILURE);
}

int printStruct(options_t opt)
{
	fprintf(stdout,"verbose: %d\nssl:     %d\nuri:     %s\n",options.verbose, options.ssl, options.uri);
	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	opt_err = 0;
	options_t options = { 0, 0, ""};

	while ((opt = getopt(argc, argv, OPTSTR)) != EOF)
	{
		switch (opt)
		{

			case 'v':
				options.verbose += 1;
				break;
			case 'u':
				options.uri = optarg;
				break;
			case 's':
				options.ssl = 1;
				break;
			case 'h':
			default:
				usage(basename(argv[0]);
				break;
		}
	}
	if ( printStruct(options) != EXIT_SUCCESS)
	{
		perror("CANNOT PRINT OPTIONS STRUCT");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
/*
 *****
 */
