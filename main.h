/*****h* browser/main
 *
 * NAME
 *   main.h - Header for the main entry point to our browser program.
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

#define OPTSTR "vu:sh"
#define USAGE_FMT  "%s [-v] [-u URI] [-s] [-h]"
#define DEFAULT_PROGNAME "TextBrowser"

extern int errno;
extern char *optarg;
extern int opterr, optind;

typedef struct {
	int verbose;
	int ssl;
	char *uri[];
} options_t;

void usage(char *progname);
int main(int argc, char *argv[]);
int printStruct(options_t opt);

/*
 *****
 */
