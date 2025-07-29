#ifndef IMG_JPEG_H_INCLUDED
#define IMG_JPEG_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "img.h"

struct jpeg_img {
    struct img_struct *base;
};

static int jpeg_img_init(struct img_struct *img, FILE *fp) {
    unsigned char marker[2];
    if (fread(marker, 1, 2, fp) != 2)
        return -1;
    if (marker[0] != 0xFF || marker[1] != 0xD8)  // SOI check
        return -1;

    while (1) { //the SOF marker of a jpeg file is not necessarily found at the beginning of the fileis
        if (fread(marker, 1, 2, fp) != 2)
            return -1;
        if (marker[0] != 0xFF) //every chunk HAS to start with 0xFF
            return -1;

        if ((marker[1] >= 0xC0 && marker[1] <= 0xCF) &&
            marker[1] != 0xC4 && marker[1] != 0xC8 && marker[1] != 0xCC) {

            unsigned char length_bytes[2];
            if (fread(length_bytes, 1, 2, fp) != 2)
                return -1;
            uint16_t length = (length_bytes[0] << 8) | length_bytes[1];

            unsigned char *buf = malloc(length - 2);
            if (!buf)
                return -1;
            if (fread(buf, 1, length - 2, fp) != length - 2) {
                free(buf);
                return -1;
            }

            uint16_t height = (buf[1] << 8) | buf[2];
            uint16_t width = (buf[3] << 8) | buf[4];

            img->width = width;
            img->height = height;

            free(buf);
            return 0;
        } else {
            unsigned char length_bytes[2];
            if (fread(length_bytes, 1, 2, fp) != 2)
                return -1;
            uint16_t length = (length_bytes[0] << 8) | length_bytes[1];

            if (fseek(fp, length - 2, SEEK_CUR) != 0)
                return -1;
        }
    }

    return -1;
}

struct jpeg_img *jpeg_img_new(FILE *fp) {
    struct jpeg_img *jpeg = malloc(sizeof(struct jpeg_img));
    if (!jpeg) return NULL;

    jpeg->base.ops = NULL;

    if (jpeg_img_init((struct img_struct *)jpeg, fp) != 0) {
        free(jpeg);
        return NULL;
    }

    return jpeg;
}

#endif