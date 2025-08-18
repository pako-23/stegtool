#include <img/img.h>
#include <img/png.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

struct chunk {
    uint32_t len;
    unsigned char type[4];
    struct chunk *next;
    unsigned char data[];
};

struct png_img_s {
    struct img_s super;
    struct chunk *head;
    struct chunk *tail;
};

static int init(struct img_s *img, FILE * fp);
static void destroy(struct img_s *img);
static int save(const struct img_s *img, FILE * fp);

const unsigned char png_magic[8] = {0x89, 0x50, 0x4e, 0x47,
                                    0x0d, 0x0a, 0x1a, 0x0a};
static const unsigned char iend_chunk[4] = {'I', 'E', 'N', 'D'};

static const struct img_ops_s ops = {
	.init = init,
	.destroy = destroy,
	.save = save,
};


struct png_img_s *png_img_new(FILE *fp) {
  struct png_img_s *png;
  struct img_s *img;
  int err;

  png = malloc(sizeof(struct png_img_s));
  if (png == NULL) {
      fclose(fp);
      return NULL;
  }

  img = (struct img_s *) png;

  png->super.ops = &ops;
  png->head = NULL;
  png->tail = NULL;

  if (png->super.ops->init(img, fp) < 0) {
      img_destroy(img);
      return NULL;
  }

  return png;
}

static int init(struct img_s *img, FILE *fp) {
    unsigned char header[8];
    unsigned char crc[4];
    uint32_t len;
    struct png_img_s *png = (struct png_img_s *)img;
    struct chunk *c;

    if (fread(header, 1, 8, fp) != 8
        || memcmp(header, png_magic, sizeof(header)) != 0)
        return -1;

    while (1) {
        if (fread(header, 1, 8, fp) != 8)
          return -1;

        len = ntohl(*((uint32_t *) header));
        c = malloc(sizeof(struct chunk) + len);
        if (c == NULL)
          return -1;

        if (len > 0 && fread(c->data, 1, len, fp) != len) {
            free(c);
            return -1;
        }

        if (fread(crc, 1, 4, fp) != 4) {
          free(c);
          return -1;
        }

        c->len = len;
        memcpy(c->type, header + 4, 4);

        c->next = NULL;
        if (png->head == NULL)
          png->head = c;
        else
            png->tail->next = c;
        png->tail = c;

        if (memcmp(c->type, iend_chunk, sizeof(c->type)) == 0)
            break;
    }

    png->super.width = ntohl(*((uint32_t *)png->head->data));
    png->super.height = ntohl(*((uint32_t *)(png->head->data + 4)));

    return 0;
}

static void destroy(struct img_s *img) {
  struct chunk *n;
  struct png_img_s *png = (struct png_img_s *)img;

  while (png->head != NULL) {
    n = png->head;
    png->head = n->next;
    free(n);
  }

  free(png);
}

static void make_crc_table(uint32_t table[256])
{
    uint32_t c, n, k;

    for (n = 0; n < 256; n++) {
        c = n;
        for (k = 0; k < 8; k++) {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }

        table[n] = c;
    }
}

static int save(const struct img_s *img, FILE *fp) {
  struct chunk *p;
  struct png_img_s *png = (struct png_img_s *)img;
  uint32_t len;
  uint32_t crc_table[256];
  int n;
  uint32_t crc;

  if (fwrite(png_magic, 1, 8, fp) != 8)
    return -1;

  make_crc_table(crc_table);

  for (p = png->head; p != NULL; p = p->next) {
      len = htonl(p->len);
      if (fwrite(&len, 1, 4, fp) != 4
          || fwrite(p->type, 1, 4, fp) != 4)
          return -1;

      if (p->len != 0 && fwrite(p->data, 1, p->len, fp) != p->len)
          return -1;

      crc = 0xffffffff;

      for (n = 0; n < 4; n++)
          crc = crc_table[(crc ^ p->type[n]) & 0xff] ^ (crc >> 8);

      for (n = 0; n < p->len; n++)
          crc = crc_table[(crc ^ p->data[n]) & 0xff] ^ (crc >> 8);

      crc = crc ^ 0xffffffff;
      crc = htonl(crc);

      if (fwrite(&crc, 1, 4, fp) != 4)
          return -1;
  }

  return 0;
}
