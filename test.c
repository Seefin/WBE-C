#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "browser.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void test(int n, char **tokens);

int main(void)
{
	FILE *in;
	const char *filename = "testout.res";
	char *file_contents;
	int char_count = 0;
	struct stat stat_info;
	assert( stat(filename, &stat_info) == 0, "I/O Error - cannot stat file: %s\n", filename);
	assert( (in = fopen(filename,"r")) != NULL, "I/O Error - cannot read file: %s\n", filename );
	/* use stat_info to construct buffers to read file into */
	assert( file_contents = malloc(stat_info.st_size), "Memory Error - Cannot allocate memory to read %s (requires %lu bytes)\n", filename, (unsigned long) stat_info.st_size);
	/* read file into buffer */
	while( (file_contents[char_count] = fgetc(in)) != EOF )
	{
		char_count++;
	}
	file_contents[char_count] = '\0';
	
	char *line;
	line = strtok_r(file_contents, "\n", &file_contents);
	while( line != NULL && line[0] != '\r' )
	{
		printf("%s\n", line);
		line = strtok_r(file_contents, "\n", &file_contents);
	}
	printf("Line: %s\n", line);
}	
void test(int n, char **tokens)
{
	size_t x;
	x = strlen(*tokens);
	printf("%lu\n", x);
}
