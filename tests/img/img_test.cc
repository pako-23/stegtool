extern "C" {
#include <img/img.h>
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstdlib>
#include <cstdint>

using namespace testing;

struct img_mock {
    struct img_s super;
    bool destroy_called;
    bool save_called;
    size_t row, col;
};

static void img_mock_destroy(struct img_s *img)
{
    ((struct img_mock *)img)->destroy_called = true;
}

static int img_mock_save(const struct img_s *img, FILE *)
{
    ((struct img_mock *)img)->save_called = true;
    return 0;
}

static int img_mock_fail_save(const struct img_s *img, FILE *)
{
    ((struct img_mock *)img)->save_called = true;
    return -1;
}

static uint8_t *img_mock_pixel(struct img_s *img, size_t row, size_t col)
{
    struct img_mock *mock = (struct img_mock *)img;
    mock->row = row;
    mock->col = col;

    return nullptr;
}

static const struct img_ops_s success_ops = {
    .destroy = img_mock_destroy,
    .save = img_mock_save,
    .pixel = img_mock_pixel,
};

static const struct img_ops_s save_fail_ops = {
    .destroy = img_mock_destroy,
    .save = img_mock_fail_save,
};

TEST(ImgTest, Width)
{
    struct img_s img;

    img.width = 400;
    ASSERT_THAT(img_width(&img), Eq(400))
            << "img_width should return the width of an image";
}

TEST(ImgTest, Height)
{
    struct img_s img;

    img.height = 600;

    ASSERT_THAT(img_height(&img), Eq(600))
            << "img_height should return the height of an image";
}

TEST(ImgTest, Destroy)
{
    struct img_mock img = {
        .super = { .ops = &success_ops },
        .destroy_called = false,
        .save_called = false,
    };

    img_destroy((struct img_s *)&img);
    ASSERT_FALSE(img.save_called);
    ASSERT_TRUE(img.destroy_called);
}

TEST(ImgTest, Save)
{
    struct img_mock img = {
        .super = { .ops = &success_ops },
        .destroy_called = false,
        .save_called = false,
    };

    ASSERT_THAT(img_save((const struct img_s *)&img, "img-save-test"), Eq(0))
            << "img_save should return 0 on success";
    ASSERT_TRUE(img.save_called);
    ASSERT_FALSE(img.destroy_called);
}

TEST(ImgTest, SaveFailure)
{
    struct img_mock img = {
        .super = { .ops = &save_fail_ops },
        .destroy_called = false,
        .save_called = false,
    };

    ASSERT_THAT(img_save((const struct img_s *)&img, "img-save-fail-test"),
                Lt(0))
            << "img_save should return a negative number on failure";
    ASSERT_TRUE(img.save_called);
    ASSERT_FALSE(img.destroy_called);
}

TEST(ImgTest, NotExistingFile)
{
    ASSERT_THAT(img_from_file("not-existing"), IsNull())
            << "img_from_file should return NULL if the filename does not exist";
}

TEST(ImgTest, EmptyFile)
{
    ASSERT_THAT(img_from_file("empty"), IsNull())
            << "img_from_file should return NULL if the filename does not contain "
               "the magic number";
}

TEST(ImgTest, UnsupportedFileFormat)
{
    ASSERT_THAT(img_from_file("unsupported-format"), IsNull())
            << "img_from_file should return NULL if the magic number is not valid";
}

TEST(ImgTest, Iterator)
{
    struct img_mock img;
    struct img_it *it = img_iterator((struct img_s *)&img);

    ASSERT_THAT(it, NotNull());
    img_it_destroy(it);
}

TEST(ImgTest, IteratorNext)
{
    struct img_mock img = {
      .super = {
        .ops = &success_ops,
        .width = 10,
        .height = 10,
        .pixel_size = 1,
      },
      .destroy_called = false,
      .save_called = false,
    };
    struct img_it *it = img_iterator((struct img_s *)&img);

    ASSERT_THAT(it, Not(IsNull()));
    img_it_next(it);
    img_it_destroy(it);
}

TEST(ImgTest, IteratorHasNext)
{
    struct img_mock img = {
      .super = {
        .ops = &success_ops,
        .width = 10,
        .height = 10,
        .pixel_size = 1,
      },
      .destroy_called = false,
      .save_called = false,
    };
    struct img_it *it = img_iterator((struct img_s *)&img);

    ASSERT_THAT(it, Not(IsNull()));
    ASSERT_THAT(img_it_has_next(it), Not(Eq(0)));
    img_it_destroy(it);
}

TEST(ImgTest, IteratorNotHasNext)
{
    struct img_mock img = {
      .super = {
        .ops = &success_ops,
        .width = 1,
        .height = 1,
        .pixel_size = 1,
      },
      .destroy_called = false,
      .save_called = false,
    };
    struct img_it *it = img_iterator((struct img_s *)&img);

    ASSERT_THAT(it, Not(IsNull()));
    ASSERT_THAT(img_it_has_next(it), Not(Eq(0)));
    img_it_next(it);
    ASSERT_THAT(img_it_has_next(it), Eq(0));
    img_it_destroy(it);
}

TEST(ImgTest, IteratorDeref)
{
    struct img_mock img = {
      .super = {
        .ops = &success_ops,
        .width = 3,
        .height = 2,
        .pixel_size = 1,
      },
      .destroy_called = false,
      .save_called = false,
      .row = 0,
      .col = 0,
    };
    struct img_it *it = img_iterator((struct img_s *)&img);
    std::vector<std::pair<size_t, size_t> > v{ { 0, 0 }, { 0, 1 }, { 0, 2 },
                                               { 1, 0 }, { 1, 1 }, { 1, 2 } };

    ASSERT_THAT(it, Not(IsNull()));

    for (auto i = v.begin(); i != v.end(); ++i) {
        ASSERT_THAT(img_it_has_next(it), Not(Eq(0)));
        img_it_deref(it);
        ASSERT_THAT(img.row, Eq(i->first));
        ASSERT_THAT(img.col, Eq(i->second));
        img_it_next(it);
    }

    ASSERT_THAT(img_it_has_next(it), Eq(0));
    img_it_destroy(it);
}
