#include <img/img.h>
#include <img/png.h>
#include <img/jpeg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct img_s *img_from_file(const char *fname)
{
	FILE *fp;
    unsigned char magic[8];
    struct img_s *img = NULL;
    size_t nread;
    size_t n;

	fp = fopen(fname, "rb");
	if (fp == NULL)
		return NULL;

    n = sizeof(jpeg_magic);
	nread = fread(magic, 1, n, fp);
	if (nread != n)
        goto end;

    if (memcmp(magic, jpeg_magic, n) == 0) {
        rewind(fp);
        img = (struct img_s *)jpeg_img_new(fp);
        goto end;
    }

    nread = fread(magic + n, 1, sizeof(png_magic) - n, fp);
    if (nread != sizeof(png_magic) - n)
        goto end;

    if (memcmp(magic, png_magic, sizeof(png_magic)) == 0) {
        rewind(fp);
        img = (struct img_s *)png_img_new(fp);
    }

 end:
	fclose(fp);
	return img;
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

int img_save(const struct img_s *img, const char *fname)
{

	FILE *fp;
	int ret;

	fp = fopen(fname, "wb");
	if (fp == NULL)
		return -1;

	ret = img->ops->save(img, fp);

	fclose(fp);
	return ret;
}
