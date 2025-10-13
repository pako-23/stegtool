extern "C" {
#include <img/img.h>
#include <stegtool.h>
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
#include <cstdint>

using namespace testing;

class StegtoolTest : public TestWithParam<const char *> {};

TEST_P(StegtoolTest, EmbedValidMsg)
{
    const char *msg = "some message to embed";
    struct img_s *img = img_from_file(GetParam());

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(sg_embed(img, msg, strlen(msg)), Eq(0));

    img_destroy(img);
}

TEST_P(StegtoolTest, MsgTooLong)
{
    struct img_s *img = img_from_file(GetParam());
    uint32_t len = img_height(img) * img_width(img) * img_pixel_size(img);
    std::string msg(len, 0);

    std::generate_n(msg.begin(), len, []() -> char { return 'a'; });

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(sg_embed(img, msg.c_str(), len), Lt(0));

    img_destroy(img);
}

TEST_P(StegtoolTest, ExtractLength)
{
    const char *msg = "some message to embed";
    struct img_s *img = img_from_file(GetParam());
    uint32_t len;

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(sg_embed(img, msg, strlen(msg)), Eq(0));

    ASSERT_THAT(sg_extract_len(img, &len), Eq(0));
    ASSERT_THAT(len, Eq(strlen(msg)));

    img_destroy(img);
}

TEST_P(StegtoolTest, ExtractMsg)
{
    const char *msg = "some message to embed";
    struct img_s *img = img_from_file(GetParam());
    char *extracted;
    uint32_t len;

    ASSERT_THAT(img, Not(IsNull()));
    ASSERT_THAT(sg_embed(img, msg, strlen(msg)), Eq(0));

    ASSERT_THAT(sg_extract_len(img, &len), Eq(0));
    ASSERT_THAT(len, Eq(strlen(msg)));

    extracted = new char[len + 1];

    ASSERT_THAT(sg_extract_msg(img, extracted, len), Eq(0));
    extracted[len] = '\0';
    ASSERT_THAT(extracted, StrEq(msg));

    delete[] extracted;
    img_destroy(img);
}

INSTANTIATE_TEST_SUITE_P(Stegtool, StegtoolTest, Values("cat.png", "cat.jpg"));
