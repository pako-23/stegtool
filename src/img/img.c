#include <img/img.h>
#include <img/png.h>
#include <img/jpeg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct img_struct *img_from_file(const char *fname)
{
    FILE *fp = fopen(fname, "rb"); //open the file so that it reads the bytes

    if (fp == NULL)
        return NULL;

    unsigned char magic[4];
    size_t read_bytes = fread(magic, 1, 4, fp); //store 4 bytes in magic array
    if (read_bytes < 4) {
        fclose(fp);
        return NULL;
    }
    
    const char png_magic[4] = {0x89, 'P', 'N', 'G'};
    const unsigned char jpeg_magic[3] = {0xFF, 0xD8, 0xFF};

    rewind(fp); //read from the start

    if (memcmp(magic, png_magic, 4) == 0) { //confront the magic array to the actual magic numbers

        return png_img_new(fp);
    }

    if (memcmp(magic, jpeg_magic, 3) == 0) {
        return jpeg_img_new(fp);
    }

    fclose(fp);
    return NULL;
}

void img_destroy(struct img_struct *img)
{
    free(img);
}

size_t img_width(const struct img_struct *img)
{
    return img ? img->width : 0;
}


size_t img_height(const struct img_struct *img)
{
    return img ? img->height : 0;
}

int img_save(struct img_struct *img, const char *fname)
{
    return 0;
}

//main from here
int main(int argc, char *argv[]) {
    const char *filename = argv[1];
    struct img_struct *img = img_from_file(filename);

    if (argc > 2) 
        printf("Please just enter the file you want to open.");
    if (argc <= 2) {
        printf("Opening %s\n", filename);
        return 1;
    }

    if (!img) {
        printf("Image failed to load.\n");
        return 1;
    }

    printf("Image loaded successfully.\n");
    printf("Width: %zu\n", img_width(img));
    printf("Height: %zu\n", img_height(img));

    img_destroy(img);
    return 0;
}