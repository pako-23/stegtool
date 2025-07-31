extern "C" {
#include <img/img.h>
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

struct img_mock {
  struct img_s super;
  bool destroy_called;
  bool save_called;
};

static void img_mock_destroy(struct img_s *img) {
  ((struct img_mock *)img)->destroy_called = true;
}

static int img_mock_save(const struct img_s *img, FILE *fp) {
  ((struct img_mock *)img)->save_called = true;
  return 0;
}

static int img_mock_fail_save(const struct img_s *img, FILE *fp) {
  ((struct img_mock *)img)->save_called = true;
  return -1;
}

static const struct img_ops_s success_ops = {
    .destroy = img_mock_destroy,
    .save = img_mock_save,
};
static const struct img_ops_s save_fail_ops = {
    .destroy = img_mock_destroy,
    .save = img_mock_fail_save,
};

TEST(ImgTest, Width) {
  struct img_s img;

  img.width = 400;
  EXPECT_THAT(img_width(&img), Eq(400))
      << "img_width should return the width of an image";
}

TEST(ImgTest, Height) {
  struct img_s img;

  img.height = 600;

  EXPECT_THAT(img_height(&img), Eq(600))
      << "img_height should return the height of an image";
}

TEST(ImgTest, Destroy) {
  struct img_mock img = {
      .super = {.ops = &success_ops},
      .destroy_called = false,
      .save_called = false,
  };

  img_destroy((struct img_s *)&img);
  EXPECT_FALSE(img.save_called);
  EXPECT_TRUE(img.destroy_called);
}

TEST(ImgTest, Save) {
  struct img_mock img = {
      .super = {.ops = &success_ops},
      .destroy_called = false,
      .save_called = false,
  };

  EXPECT_THAT(img_save((const struct img_s *)&img, NULL), Eq(1))
      << "img_save should return 1 on success";
  EXPECT_TRUE(img.save_called);
  EXPECT_FALSE(img.destroy_called);
}

TEST(ImgTest, SaveFailure) {
  struct img_mock img = {
      .super = {.ops = &save_fail_ops},
      .destroy_called = false,
      .save_called = false,
  };

  EXPECT_THAT(img_save((const struct img_s *)&img, NULL), Eq(0))
      << "img_save should return 0 on failure";
  EXPECT_TRUE(img.save_called);
  EXPECT_FALSE(img.destroy_called);
}

TEST(ImgTest, NotExistingFile) {
  EXPECT_THAT(img_from_file("not-existing"), IsNull())
      << "img_from_file should return NULL if the filename does not exist";
}

TEST(ImgTest, EmptyFile) {
  EXPECT_THAT(img_from_file("empty"), IsNull())
      << "img_from_file should return NULL if the filename does not contain "
         "the magic number";
}

TEST(ImgTest, UnsupportedFileFormat) {
  EXPECT_THAT(img_from_file("unsupported-format"), IsNull())
      << "img_from_file should return NULL if the magic number is not valid";
}
