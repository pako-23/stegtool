#ifndef STEGTOOL_H_INCLUDED
#define STEGTOOL_H_INCLUDED

#include <cstddef>
#include <stdint.h>
#include <img/img.h>

int sg_embed(struct img_s *img, const char *msg, size_t len);

size_t sg_extract_len(const struct img_s *img);
int sg_extract_msg(const struct img_s *img, char *msg);

#endif
