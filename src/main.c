#include <stdlib.h>
#include <stdio.h>
#include <stegtool.h>

int main(int argc, const char *argv[]) 
{ 
	const char *img = NULL;
	const char *msg = NULL;

	const char *output; 


	if (strcmp(argv[2], "-h") == 0) {
		printf("Help page\n");

	}

	else if (strcmp(argv[2], "-m") == 0) {
		printf("Hide message page\n");

		const char *msg = argv[3];
		size_t len = strlen(msg);

		//img to be assigned
		int sg_embed(img, msg, len);
	}

	else if (strcmp(argv[2], "-d") == 0) {
		printf("Extract message page\n");
	}

	else if (strcmp(argv[2], "-o") == 0) {
		printf("Output filename: %s\n", o_filename);
	}
}