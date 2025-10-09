#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stegtool.h>

#define HELP_FMT                                                               \
  "Usage: %s [options] <filename>\n\n"                                         \
  "Options:\n"                                                                 \
  "  -d, --decode\n"                                                           \
  "       Run in decode mode. Extract data embedded into an image.\n"          \
  "  -m <message>, --message <message>\n"                                      \
  "       The message to embedd within the image. (required)\n"                \
  "  -o <filename>, --output <filename>\n"                                     \
  "       Write the program output to the given file. (default: stdout)\n"     \
  "  -h, --help\n"                                                             \
  "       Display this help and exit\n"



int main(int argc, const char *argv[]) 
{ 
	const char *msg = NULL;
	const char* img = NULL;
	const char* output = NULL;
    int decode = 0;

    if (argc < 2) {
        fprintf(stderr, "Missing input image\n");
        fprintf(stderr, HELP_FMT, argv[0]);
        return EXIT_FAILURE;
    }

    img = argv[argc-1];
	int i = 1;
	while (i < argc -1) {
		if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--decode") == 0) {
			if (decode) {
				fprintf(stderr, "Flag specified multiple times: %s\n", argv[i]);
				return EXIT_FAILURE;
			}

			decode = 1;
		} else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--message") == 0) {
			if (msg != NULL) {
				fprintf(stderr, "Flag specified multiple times: %s\n", argv[i]);
				return EXIT_FAILURE;
			} else if (i + 1 >= argc - 1) {
				fprintf(stderr, "Flag requires argument: %s\n", argv[i]);
				return EXIT_FAILURE;
			}
			msg = argv[i + 1];
			++i;
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printf(HELP_FMT, argv[0]);
			return EXIT_SUCCESS;
		} else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
			if (output != NULL) {
			    fprintf(stderr, "Flag specified multiple times: %s\n", argv[i]);
				return EXIT_FAILURE;
			} else if (i + 1 >= argc - 1) {
				fprintf(stderr, "Flag requires argument: %s\n", argv[i]);
				return EXIT_FAILURE;
			}
			output = argv[i + 1];
			++i;
		} else {
			fprintf(stderr, "Unrecognized flag: %s\n", argv[i]);
			fprintf(stderr, HELP_FMT, argv[0]);
			return EXIT_FAILURE;
		}
		++i;
    }
}
