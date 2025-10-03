#include <stdlib.h>
#include <stdio.h>
#include <stegtool.h>

#define HELP_FMT \
"help page %s\n"

int main(int argc, const char *argv[]) 
{ 
	const char *msg = NULL;
	const char* img = NULL;
	const char* output = NULL;
	int decode = 0;
	
	int i = 1;
	while (i < argc) {
		if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--decode") == 0) {
			if (decode) {
				fprintf(stderr, "flag found two or more times: %s\n", argv[i]);
				return EXIT_FAILURE;
			}

			decode = 1;
		} else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--message") == 0) {
			if (msg != NULL) {
				fprintf(stderr, "Message found two or more times.\n");
				return EXIT_FAILURE;
			} else if (i + 1 > argc - 1) {
				fprintf(stderr, "Could not find message.\n");
				return EXIT_FAILURE;
			}
			msg = argv[i + 1];
			++i;
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printf(HELP_FMT, argv[0]);
			return EXIT_SUCCESS;
		} else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
			if (output != NULL) {
				fprintf(stderr, "Output error.\n");
				return EXIT_FAILURE;
			} else if (i + 1 > argc - 1) {
				fprintf(stderr, "Could not find output.\n");
				return EXIT_FAILURE;
			}
			output = argv[i + 1];
			++i;
		} else {
			fprintf(stderr, "Unrecognized flag: %s\n", argv[i]);
			frpintf(stderr, HELP_FMT, argv[0]);
			return EXIT_FAILURE;
		}
		++i;
	}


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
		printf("Output filename: \n");
	}
}