#include <img/img.h>
#include <img/png.h>
#include <img/jpeg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct img_struct *img_from_file(const char *fname)
{
    FILE *f = fopen(fname, "rb");

    unsigned char magic[4];
    
    const char png_magic[4] = {0x89, 'P', 'N', 'G'};
    const unsigned char jpeg_magic[3] = {0xFF, 0xD8, 0xFF};

    if (memcmp(magic, png_magic, 4) == 0) {
        return png_img_new(f);
    }

    if (memcmp(magic, jpeg_magic, 3) == 0) {
        return jpeg_img_new(f);
    }
    return NULL;

    fclose(f);
    return NULL; //???
}

void img_destroy(struct img_struct *img)
{
}

size_t img_width(const struct img_struct *img)
{
    return 0;
}


size_t img_height(const struct img_struct *img)
{
    return 0;
}

int img_save(struct img_struct *img, const char *fname)
{
    return 0;
}
