#include <stdio.h>
#include <stdlib.h>

#include "help.h"

void help() {
	printf("Croco - Efficient terminal file manager\n");
	printf("Usage: \n");
	printf("    ./croco             run from your %s directory\n", getenv("HOME"));
	printf("    ./croco .           run from your current directory\n");

	printf("\n");
	printf("Controls: \n");
	printf("    Use arrow keys or h-j-k-l to navigate through directories\n");
	printf("    To open file/directory or execute the command, press <ENTER>\n");
	
	printf("\n");
	printf("Integrated command line commands:\n");
	printf("    :a file.extension   add file with specified extension\n");
	printf("    :a folder/          add folder\n");
	printf("    :r fole.extension   remove file.extension\n");
	printf("    :r folder/          remove folder\n");
	printf("    :q                  quit croco\n");
}

