extern "C" {
#include <img/img.h>
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

using namespace testing;

struct img_mock_it {
  struct img_it super;
  bool next_called;
  bool has_next_called;
  bool read_called;
  bool write_called;
};

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

static void img_mock_it_destroy(struct img_it *it) { free(it); }

static void img_mock_it_next(struct img_it *it) {
  ((struct img_mock_it *)it)->next_called = true;
}

static int img_mock_it_has_next(const struct img_it *it) {
  if (((struct img_mock_it *)it)->has_next_called)
    return 0;

  ((struct img_mock_it *)it)->has_next_called = true;
  return 1;
}

static void img_mock_it_read(const struct img_it *it, struct pixel_s *p) {
  ((struct img_mock_it *)it)->read_called = true;
}

static void img_mock_it_write(const struct img_it *it,
                              const struct pixel_s *p) {
  ((struct img_mock_it *)it)->write_called = true;
}

static const struct img_it_ops iterator_ops = {
    .destroy = img_mock_it_destroy,
    .next = img_mock_it_next,
    .has_next = img_mock_it_has_next,
    .read = img_mock_it_read,
    .write = img_mock_it_write,
};

static struct img_it *img_mock_iterator(struct img_s *img) {
  struct img_mock_it *it;

  it = (struct img_mock_it *)malloc(sizeof(struct img_mock_it));
  if (it == NULL)
    return NULL;

  it->super.ops = &iterator_ops;
  it->next_called = false;
  it->has_next_called = false;
  it->read_called = false;
  it->write_called = false;

  return &it->super;
}

static struct img_it *img_mock_iterator_fail(struct img_s *img) { return NULL; }

static const struct img_ops_s success_ops = {
    .destroy = img_mock_destroy,
    .save = img_mock_save,
    .iterator = img_mock_iterator,
};
static const struct img_ops_s save_fail_ops = {
    .destroy = img_mock_destroy,
    .save = img_mock_fail_save,
    .iterator = img_mock_iterator_fail,
};

TEST(ImgTest, Width) {
  struct img_s img;

  img.width = 400;
  ASSERT_THAT(img_width(&img), Eq(400))
      << "img_width should return the width of an image";
}

TEST(ImgTest, Height) {
  struct img_s img;

  img.height = 600;

  ASSERT_THAT(img_height(&img), Eq(600))
      << "img_height should return the height of an image";
}

TEST(ImgTest, Destroy) {
  struct img_mock img = {
      .super = {.ops = &success_ops},
      .destroy_called = false,
      .save_called = false,
  };

  img_destroy((struct img_s *)&img);
  ASSERT_FALSE(img.save_called);
  ASSERT_TRUE(img.destroy_called);
}

TEST(ImgTest, Save) {
  struct img_mock img = {
      .super = {.ops = &success_ops},
      .destroy_called = false,
      .save_called = false,
  };

  ASSERT_THAT(img_save((const struct img_s *)&img, "img-save-test"), Eq(0))
      << "img_save should return 0 on success";
  ASSERT_TRUE(img.save_called);
  ASSERT_FALSE(img.destroy_called);
}

TEST(ImgTest, SaveFailure) {
  struct img_mock img = {
      .super = {.ops = &save_fail_ops},
      .destroy_called = false,
      .save_called = false,
  };

  ASSERT_THAT(img_save((const struct img_s *)&img, "img-save-fail-test"), Lt(0))
      << "img_save should return a negative number on failure";
  ASSERT_TRUE(img.save_called);
  ASSERT_FALSE(img.destroy_called);
}

TEST(ImgTest, NotExistingFile) {
  ASSERT_THAT(img_from_file("not-existing"), IsNull())
      << "img_from_file should return NULL if the filename does not exist";
}

TEST(ImgTest, EmptyFile) {
  ASSERT_THAT(img_from_file("empty"), IsNull())
      << "img_from_file should return NULL if the filename does not contain "
         "the magic number";
}

TEST(ImgTest, UnsupportedFileFormat) {
  ASSERT_THAT(img_from_file("unsupported-format"), IsNull())
      << "img_from_file should return NULL if the magic number is not valid";
}

TEST(ImgTest, Iterator) {
  struct img_mock img = {
      .super = {.ops = &success_ops},
      .destroy_called = false,
      .save_called = false,
  };
  struct img_it *it = img_iterator((struct img_s *)&img);

  ASSERT_THAT(it, Not(IsNull()));
  img_it_destroy(it);
}

TEST(ImgTest, IteratorFailure) {
  struct img_mock img = {
      .super = {.ops = &save_fail_ops},
      .destroy_called = false,
      .save_called = false,
  };
  struct img_it *it = img_iterator((struct img_s *)&img);

  ASSERT_THAT(it, IsNull());
}

TEST(ImgTest, IteratorNext) {
  struct img_mock img = {
      .super = {.ops = &success_ops},
      .destroy_called = false,
      .save_called = false,
  };
  struct img_it *it = img_iterator((struct img_s *)&img);

  ASSERT_THAT(it, Not(IsNull()));
  ASSERT_FALSE(((struct img_mock_it *)it)->next_called);
  img_it_next(it);
  ASSERT_TRUE(((struct img_mock_it *)it)->next_called);
  img_it_destroy(it);
}

TEST(ImgTest, IteratorHasNext) {
  struct img_mock img = {
      .super = {.ops = &success_ops},
      .destroy_called = false,
      .save_called = false,
  };
  struct img_it *it = img_iterator((struct img_s *)&img);

  ASSERT_THAT(it, Not(IsNull()));
  ASSERT_FALSE(((struct img_mock_it *)it)->has_next_called);
  ASSERT_THAT(img_it_has_next(it), Not(Eq(0)));
  ASSERT_TRUE(((struct img_mock_it *)it)->has_next_called);
  img_it_destroy(it);
}

TEST(ImgTest, IteratorNotHasNext) {
  struct img_mock img = {
      .super = {.ops = &success_ops},
      .destroy_called = false,
      .save_called = false,
  };
  struct img_it *it = img_iterator((struct img_s *)&img);

  ASSERT_THAT(it, Not(IsNull()));
  ASSERT_FALSE(((struct img_mock_it *)it)->has_next_called);
  ASSERT_THAT(img_it_has_next(it), Not(Eq(0)));
  ASSERT_TRUE(((struct img_mock_it *)it)->has_next_called);
  ASSERT_THAT(img_it_has_next(it), Eq(0));
  img_it_destroy(it);
}

TEST(ImgTest, IteratorRead) {
  struct img_mock img = {
      .super = {.ops = &success_ops},
      .destroy_called = false,
      .save_called = false,
  };
  struct img_it *it = img_iterator((struct img_s *)&img);
  struct pixel_s pixel;

  ASSERT_THAT(it, Not(IsNull()));
  ASSERT_FALSE(((struct img_mock_it *)it)->read_called);
  img_it_read(it, &pixel);
  ASSERT_TRUE(((struct img_mock_it *)it)->read_called);
  img_it_destroy(it);
}

TEST(ImgTest, IteratorWrite) {
  struct img_mock img = {
      .super = {.ops = &success_ops},
      .destroy_called = false,
      .save_called = false,
  };
  struct img_it *it = img_iterator((struct img_s *)&img);
  struct pixel_s pixel;

  ASSERT_THAT(it, Not(IsNull()));
  ASSERT_FALSE(((struct img_mock_it *)it)->write_called);
  img_it_write(it, &pixel);
  ASSERT_TRUE(((struct img_mock_it *)it)->write_called);
  img_it_destroy(it);
}
