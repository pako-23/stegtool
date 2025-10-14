#include <stdint.h>
#include <img/img.h>
#include <stegtool.h>
#include <sys/types.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

int sg_embed(struct img_s *img, const char *msg, uint32_t len)
{
    int pxsz = img_pixel_size(img);
    size_t size = img_width(img) * img_height(img) * pxsz;
    struct img_it *itr;
    uint8_t *pixel;
    uint32_t nlen = htonl(len);

    if (size < len * 8 + 32)
        return -1;

    itr = img_iterator(img);
    if (itr == NULL)
        return -1;

    pixel = img_it_deref(itr);
    int j = 0;

    for (int i = 0; i < 32; i++, j++) {
        uint32_t mask = (1 << (31 - i));

        if (j == pxsz) {
            j = 0;
            img_it_next(itr);
            pixel = img_it_deref(itr);
        }

        if (nlen & mask)
            pixel[j] |= 1;
        else
            pixel[j] &= ~1;
    }

    for (uint32_t k = 0; k < len; k++) {
        for (int i = 0; i < 8; i++, j++) {
            uint32_t mask = (1 << (7 - i));

            if (j == pxsz) {
                j = 0;
                img_it_next(itr);
                pixel = img_it_deref(itr);
            }

            if (msg[k] & mask) {
                pixel[j] |= 1;
            } else {
                pixel[j] &= ~1;
            }
        }
    }

    img_it_destroy(itr);

    return 0;
}

int sg_extract_len(struct img_s *img, uint32_t *len)
{
    int pxsz = img_pixel_size(img);
    struct img_it *itr;
    int err = 0;
    uint8_t *pixel;

    // TODO check image length
    itr = img_iterator(img);
    if (itr == NULL)
        return -1;

    int j = 0;
    pixel = img_it_deref(itr);
    *len = 0;
    for (int i = 0; i < 32; i++, j++) {
        if (j == pxsz) {
            if (!img_it_has_next(itr)) {
                err = -1;
                goto out;
            }

            j = 0;
            img_it_next(itr);
            pixel = img_it_deref(itr);
        }

        if (pixel[j] & 1) {
            *len = (*len << 1) | 1;
        } else {
            *len <<= 1;
        }
    }

    *len = ntohl(*len);

out:
    img_it_destroy(itr);

    return err;
}

int sg_extract_msg(struct img_s *img, char *out, uint32_t len)
{
    int pxsz = img_pixel_size(img);
    struct img_it *itr;
    int err = 0;
    uint8_t *pixel;

    itr = img_iterator(img);
    if (itr == NULL)
        return -1;

    int j = 0;
    pixel = img_it_deref(itr);
    for (int i = 0; i < 32; i++, j++) {
        if (j == pxsz) {
            if (!img_it_has_next(itr)) {
                err = -1;
                goto out;
            }

            j = 0;
            img_it_next(itr);
            pixel = img_it_deref(itr);
        }
    }

    for (uint32_t k = 0; k < len; k++) {
        out[k] = 0;
        for (int i = 0; i < 8; i++, j++) {
            if (j == pxsz) {
                if (!img_it_has_next(itr)) {
                    err = -1;
                    goto out;
                }

                j = 0;
                img_it_next(itr);
                pixel = img_it_deref(itr);
            }

            if (pixel[j] & 1) {
                out[k] <<= 1;
                out[k] |= 1;
            } else {
                out[k] <<= 1;
            }
        }
    }

out:
    img_it_destroy(itr);

    return err;
}
