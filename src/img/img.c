#include <img/img.h>
#include <img/png.h>
#include <img/jpeg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct img_s *img_from_file(const char *fname)
{
    FILE *fp;
    unsigned char magic[4];
    size_t nread;

    fp = fopen(fname, "rb");
    if (fp == NULL)
        return NULL;

    
    nread = fread(magic, 1, 4, fp);
    if (nread < 4) {
        fclose(fp);
        return NULL;
    }

    rewind(fp);

    if (memcmp(magic, png_magic, 4) == 0)
        return (struct img_s *) png_img_new(fp);
    else if (memcmp(magic, jpeg_magic, 3) == 0)
        return (struct img_s *) jpeg_img_new(fp);

    fclose(fp);
    return NULL;
}

void img_destroy(struct img_s *img)
{
    img->ops->destroy(img);
}

size_t img_width(const struct img_s *img)
{
    return img->width;
}


size_t img_height(const struct img_s *img)
{
    return img->height;
}

int img_save(const struct img_s *img, const char *fname) {

  FILE *fp;
  int ret;

    fp = fopen(fname, "wb");
    if (fp == NULL)
        return -1;

    ret = img->ops->save(img, fp);

    fclose(fp);
    return ret;
}
