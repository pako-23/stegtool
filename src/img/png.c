#include <img/img.h>
#include <img/png.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Public constructor
struct png_img *png_img_new(FILE *fp)
{
    struct png_img *png = malloc(sizeof(struct png_img));
    if (!png) return NULL;

    png->base.ops = &ops;

    if (png->base.ops->init((struct img_struct *)png, fp) != 0) {
        free(png);
        return NULL;
    }

    return png;
}

// Init: read PNG signature and IHDR chunk
static int init(struct img_struct *img, FILE *fp)
{
    unsigned char sig[8];
    if (fread(sig, 1, 8, fp) != 8)
        return -1;
    if (memcmp(sig, "\x89PNG\r\n\x1a\n", 8) != 0)
        return -1;

    // Read IHDR chunk
    unsigned char chunk_len[4], chunk_type[4], ihdr_data[13];

    if (fread(chunk_len, 1, 4, fp) != 4) return -1;
    if (fread(chunk_type, 1, 4, fp) != 4) return -1;
    if (memcmp(chunk_type, "IHDR", 4) != 0) return -1;
    if (fread(ihdr_data, 1, 13, fp) != 13) return -1;

    uint32_t width = (ihdr_data[0] << 24) | (ihdr_data[1] << 16) |
                     (ihdr_data[2] << 8)  | ihdr_data[3];
    uint32_t height = (ihdr_data[4] << 24) | (ihdr_data[5] << 16) |
                      (ihdr_data[6] << 8)  | ihdr_data[7];

    img->width = width;
    img->height = height;

    return 0;
}

// Destroy: free png_img
static void destroy(struct img_struct *img)
{
    if (!img) return;

    struct png_img *png = (struct png_img *)img;
    free(png);
}

// Save: stub
static int save(struct img_struct *img, FILE *fp)
{
    (void)img;
    (void)fp;

    return -1; // Saving PNG not implemented yet
}