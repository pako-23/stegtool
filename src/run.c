#include <img/img.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stegtool.h>

#define HELP_FMT                                                        \
    "Usage: %s [options] <filename>\n\n"                                \
    "Options:\n"                                                        \
    "  -d, --decode\n"                                                  \
    "       Run in decode mode. Extract data embedded into an image.\n" \
    "  -m <message>, --message <message>\n"                             \
    "       The message to embedd within the image.\n"                  \
    "  -o <filename>, --output <filename>\n"                            \
    "       Write the program output to the given file.\n"              \
    "  -h, --help\n"                                                    \
    "       Display this help and exit\n"

int run(int argc, const char *argv[])
{
    const char *msg = NULL;
    const char *fname = NULL;
    const char *output = NULL;
    struct img_s *img;
    int decode = 0;
    int err = EXIT_SUCCESS;

    if (argc < 2) {
        fprintf(stderr, "Missing input image\n");
        fprintf(stderr, HELP_FMT, argv[0]);
        return EXIT_FAILURE;
    }

    fname = argv[argc - 1];
    if (strcmp(fname, "-h") == 0 || strcmp(fname, "--help") == 0) {
        printf(HELP_FMT, argv[0]);
        return EXIT_SUCCESS;
    }

    int i = 1;
    while (i < argc - 1) {
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--decode") == 0) {
            if (decode) {
                fprintf(stderr, "Flag specified multiple times: %s\n", argv[i]);
                return EXIT_FAILURE;
            }

            decode = 1;
        } else if (strcmp(argv[i], "-m") == 0 ||
                   strcmp(argv[i], "--message") == 0) {
            if (msg != NULL) {
                fprintf(stderr, "Flag specified multiple times: %s\n", argv[i]);
                return EXIT_FAILURE;
            } else if (i + 1 >= argc - 1) {
                fprintf(stderr, "Flag requires argument: %s\n", argv[i]);
                return EXIT_FAILURE;
            }
            msg = argv[i + 1];
            ++i;
        } else if (strcmp(argv[i], "-h") == 0 ||
                   strcmp(argv[i], "--help") == 0) {
            printf(HELP_FMT, argv[0]);
            return EXIT_SUCCESS;
        } else if (strcmp(argv[i], "-o") == 0 ||
                   strcmp(argv[i], "--output") == 0) {
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

    if (msg == NULL && !decode) {
        fprintf(stderr,
                "A message to hide is required when used in encode mode\n");
        fprintf(stderr, HELP_FMT, argv[0]);
        return EXIT_FAILURE;
    } else if (output == NULL && !decode) {
        fprintf(stderr, "A output file is required when used in encode mode\n");
        fprintf(stderr, HELP_FMT, argv[0]);
        return EXIT_FAILURE;
    }

    img = img_from_file(fname);
    if (img == NULL) {
        fprintf(stderr, "Failed to read image file: %s\n", fname);
        return EXIT_FAILURE;
    }

    if (decode) {
        uint32_t len;
        char *buf;

        if (sg_extract_len(img, &len) < 0) {
            fprintf(stderr, "Failed to extract message length\n");
            err = EXIT_FAILURE;
            goto out;
        }

        buf = malloc(len + 1);
        if (buf == NULL)
            goto out;

        if (sg_extract_msg(img, buf, len) < 0) {
            fprintf(stderr, "Failed to extract message from image\n");
            err = EXIT_FAILURE;
            free(buf);
            goto out;
        }

        buf[len] = '\0';
        printf("%s\n", buf);
        free(buf);
    } else {
        if (sg_embed(img, msg, strlen(msg)) < 0) {
            fprintf(stderr, "Failed to hide message into image\n");
            err = EXIT_FAILURE;
            goto out;
        }

        if (img_save(img, output) < 0) {
            fprintf(stderr, "Failed to save output image\n");
            err = EXIT_FAILURE;
        }
    }

out:
    img_destroy(img);

    return err;
}
