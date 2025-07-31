extern "C" {
#include <img/img.h>
#include <img/jpeg.h>
}
#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

TEST(JPEGTest, ImgFromFile) {
  struct img_s *img = img_from_file("cat.jpg");

  EXPECT_THAT(img, Not(IsNull()));
  EXPECT_THAT(img_width(img), Eq(539));
  EXPECT_THAT(img_height(img), Eq(360));

  img_destroy(img);
}

TEST(JPEGTest, ImgFromFileIvalidMagic) {
  struct img_s *img = img_from_file("invalid-magic.jpg");

  EXPECT_THAT(img, IsNull());
}

TEST(JPEGTest, NewJPEGImg) {
  FILE *fp;
  struct img_s *img;

  fp = fopen("cat.jpg", "r");
  EXPECT_THAT(fp, Not(IsNull()));

  img = (struct img_s *)jpeg_img_new(fp);
  EXPECT_THAT(img, Not(IsNull()));
  EXPECT_THAT(img_width(img), Eq(539));
  EXPECT_THAT(img_height(img), Eq(360));

  img_destroy(img);
}

TEST(JPEGTest, NewJPEGImgInvalidMagic) {
  FILE *fp;
  struct img_s *img;

  fp = fopen("invalid-magic.jpg", "r");
  EXPECT_THAT(fp, Not(IsNull()));

  img = (struct img_s *)jpeg_img_new(fp);
  EXPECT_THAT(img, IsNull());
  fclose(fp);
}

TEST(JPEGTest, Save) {
  struct img_s *img = img_from_file("cat.jpg");

  EXPECT_THAT(img, Not(IsNull()));
  EXPECT_THAT(img_save(img, "cat-copy.jpg"), Not(Eq(0)));
  img_destroy(img);

  std::ifstream src("cat.jpg", std::ios::binary | std::ios::ate);
  std::ifstream dst("cat-copy.jpg", std::ios::binary | std::ios::ate);

  EXPECT_TRUE(src.is_open() && dst.is_open());

  const size_t buffer_size = 4096;
  std::vector<char> buf_src(buffer_size);
  std::vector<char> buf_dst(buffer_size);

  while (src.read(buf_src.data(), buffer_size) &&
         dst.read(buf_dst.data(), buffer_size))
    EXPECT_THAT(buf_src, Eq(buf_dst));

  EXPECT_TRUE(src.eof() && dst.eof());
}
