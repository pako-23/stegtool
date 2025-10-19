#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <string>
#include <random>

extern "C" {
int run(int argc, const char *argv[]);
}

using namespace testing;

const char *help_msg =
        "Usage: ./stegtool [options] <filename>\n\n"
        "Options:\n"
        "  -d, --decode\n"
        "       Run in decode mode. Extract data embedded into an image.\n"
        "  -m <message>, --message <message>\n"
        "       The message to embedd within the image.\n"
        "  -o <filename>, --output <filename>\n"
        "       Write the program output to the given file.\n"
        "  -h, --help\n"
        "       Display this help and exit\n";

class MainTest : public TestWithParam<const char *> {};
class FlagTest : public TestWithParam<const char *> {};
class HelpFlagTest : public TestWithParam<const char *> {};
class FlagArgumentTest : public TestWithParam<const char *> {};

TEST_P(MainTest, EncodeDecode)
{
    std::string fname = GetParam();
    std::string out = "out-" + fname;
    std::random_device dev;
    std::default_random_engine eng(dev());
    std::bernoulli_distribution dist(0.5);
    const char *argv1[] = {
        "./stegtool",
        dist(eng) ? "-m" : "--message",
        "some message to hide",
        dist(eng) ? "-o" : "--output",
        out.c_str(),
        fname.c_str(),
        NULL,
    };
    const char *argv2[] = {
        "./stegtool",
        dist(eng) ? "-d" : "--decode",
        out.c_str(),
        NULL,
    };
    int res;

    res = run(6, argv1);
    EXPECT_THAT(res, Eq(EXIT_SUCCESS));

    testing::internal::CaptureStdout();
    res = run(3, argv2);
    EXPECT_THAT(res, Eq(EXIT_SUCCESS));
    std::string msg = testing::internal::GetCapturedStdout();
    msg.pop_back();

    EXPECT_THAT(msg, StrEq(argv1[2]));
}

TEST(MainTest, MissingFile)
{
    const char *argv[] = {
        "./stegtool",
        NULL,
    };

    testing::internal::CaptureStderr();
    EXPECT_THAT(run(1, argv), Eq(EXIT_FAILURE));

    std::string msg = testing::internal::GetCapturedStderr();
    EXPECT_THAT(msg, StrEq("Missing input image\n" + std::string{ help_msg }));
}

TEST_P(HelpFlagTest, HelpFlagOnly)
{
    const char *argv[] = {
        "./stegtool",
        GetParam(),
        NULL,
    };

    testing::internal::CaptureStdout();
    EXPECT_THAT(run(2, argv), Eq(EXIT_SUCCESS));

    std::string msg = testing::internal::GetCapturedStdout();
    EXPECT_THAT(msg, StrEq(std::string{ help_msg }));
}

TEST_P(HelpFlagTest, InTheMiddle)
{
    const char *argv[] = {
        "./stegtool",
        GetParam(),
        "file.png",
        NULL,
    };

    testing::internal::CaptureStdout();
    EXPECT_THAT(run(3, argv), Eq(EXIT_SUCCESS));

    std::string msg = testing::internal::GetCapturedStdout();
    EXPECT_THAT(msg, StrEq(std::string{ help_msg }));
}

TEST_P(FlagTest, MultipleFlag)
{
    const char *argv[] = {
        "./stegtool", GetParam(), GetParam(), "file.png", NULL,
    };
    int res;

    testing::internal::CaptureStderr();
    res = run(4, argv);
    EXPECT_THAT(res, Eq(EXIT_FAILURE));

    std::string msg = testing::internal::GetCapturedStderr();
    EXPECT_THAT(msg, StrEq("Flag specified multiple times: " +
                           std::string{ GetParam() } + "\n"));
}

TEST_P(FlagArgumentTest, MultipleFlag)
{
    const char *argv[] = {
        "./stegtool", GetParam(), "value", GetParam(),
        "value",      "file.png", NULL,
    };
    int res;

    testing::internal::CaptureStderr();
    res = run(6, argv);
    EXPECT_THAT(res, Eq(EXIT_FAILURE));

    std::string msg = testing::internal::GetCapturedStderr();
    EXPECT_THAT(msg, StrEq("Flag specified multiple times: " +
                           std::string{ GetParam() } + "\n"));
}

TEST_P(FlagArgumentTest, MissingArgument)
{
    const char *argv[] = {
        "./stegtool",
        GetParam(),
        "file.png",
        NULL,
    };
    int res;

    testing::internal::CaptureStderr();
    res = run(3, argv);
    EXPECT_THAT(res, Eq(EXIT_FAILURE));

    std::string msg = testing::internal::GetCapturedStderr();
    EXPECT_THAT(msg, StrEq("Flag requires argument: " +
                           std::string{ GetParam() } + "\n"));
}

TEST(MainTest, InvalidFlag)
{
    const char *argv[] = {
        "./stegtool",
        "--invalid",
        "file.png",
        NULL,
    };

    testing::internal::CaptureStderr();
    EXPECT_THAT(run(3, argv), Eq(EXIT_FAILURE));

    std::string msg = testing::internal::GetCapturedStderr();
    EXPECT_THAT(msg, StrEq("Unrecognized flag: --invalid\n" +
                           std::string{ help_msg }));
}

TEST(MainTest, MissingOutput)
{
    const char *argv[] = {
        "./stegtool", "--message", "some message", "file.png", NULL,
    };

    testing::internal::CaptureStderr();
    EXPECT_THAT(run(4, argv), Eq(EXIT_FAILURE));

    std::string msg = testing::internal::GetCapturedStderr();
    EXPECT_THAT(msg,
                StrEq("A output file is required when used in encode mode\n" +
                      std::string{ help_msg }));
}

TEST(MainTest, MissingMessage)
{
    const char *argv[] = {
        "./stegtool", "--output", "out.png", "file.png", NULL,
    };

    testing::internal::CaptureStderr();
    EXPECT_THAT(run(4, argv), Eq(EXIT_FAILURE));

    std::string msg = testing::internal::GetCapturedStderr();
    EXPECT_THAT(
            msg,
            StrEq("A message to hide is required when used in encode mode\n" +
                  std::string{ help_msg }));
}

TEST(MainTest, NotExistingFile)
{
    const char *argv[] = {
        "./stegtool", "--output", "out.png", "-m", "some msg", "file.png", NULL,
    };

    testing::internal::CaptureStderr();
    EXPECT_THAT(run(6, argv), Eq(EXIT_FAILURE));

    std::string msg = testing::internal::GetCapturedStderr();
    EXPECT_THAT(msg, StrEq("Failed to read image file: file.png\n"));
}

TEST(MainTest, NoEmbeddingsFile)
{
    const char *argv[] = {
        "./stegtool",
        "-d",
        "single.ascii.ppm",
        NULL,
    };

    testing::internal::CaptureStderr();
    EXPECT_THAT(run(3, argv), Eq(EXIT_FAILURE));

    std::string msg = testing::internal::GetCapturedStderr();
    EXPECT_THAT(msg, StrEq("Failed to extract message length\n"));
}

TEST(MainTest, EmbedTooLong)
{
    const char *argv[] = {
        "./stegtool",       "-m", "really long message", "-o", "result.ppm",
        "single.ascii.ppm", NULL,
    };

    testing::internal::CaptureStderr();
    EXPECT_THAT(run(6, argv), Eq(EXIT_FAILURE));

    std::string msg = testing::internal::GetCapturedStderr();
    EXPECT_THAT(msg, StrEq("Failed to hide message into image\n"));
}

INSTANTIATE_TEST_SUITE_P(
        Stegtool, MainTest,
        Values("basn0g01.png", "basn0g02.png", "basn0g04.png", "basn0g08.png",
               "basn0g16.png", "basn2c08.png", "basn2c16.png", "basn3p01.png",
               "basn3p02.png", "basn3p04.png", "basn3p08.png", "basn4a08.png",
               "basn4a16.png", "basn6a08.png", "basn6a16.png", "cat.png",
               "ftbbn0g01.png", "ftbbn0g02.png", "ftbbn0g04.png",
               "ftbbn2c16.png", "ftbbn3p08.png", "ftbgn2c16.png",
               "ftbgn3p08.png", "ftbrn2c08.png", "ftbwn0g16.png",
               "ftbwn3p08.png", "ftbyn3p08.png", "ftp0n0g08.png",
               "ftp0n2c08.png", "ftp0n3p08.png", "ftp1n3p08.png",
               "ibasn0g08.png", "ibasn0g16.png", "ibasn2c08.png",
               "ibasn2c16.png", "ibasn3p08.png", "ibasn4a08.png",
               "ibasn4a16.png", "ibasn6a08.png", "ibasn6a16.png",
               "iftbbn2c16.png", "iftbbn3p08.png", "iftbgn2c16.png",
               "iftbgn3p08.png", "iftbrn2c08.png", "iftbwn0g16.png",
               "iftbwn3p08.png", "iftbyn3p08.png", "iftp0n0g08.png",
               "iftp0n2c08.png", "iftp0n3p08.png", "iftp1n3p08.png",
               "squares.binary.ppm", "prague.binary.ppm", "blocks.binary.ppm",
               "fruit.binary.ppm"),
        [](const testing::TestParamInfo<const char *> &info) {
            std::string fname = info.param;

            std::replace(fname.begin(), fname.end(), '.', '_');
            return fname;
        });

INSTANTIATE_TEST_SUITE_P(Stegtool, FlagTest, Values("-d", "--decode"),
                         [](const testing::TestParamInfo<const char *> &info) {
                             std::string flag = info.param;
                             std::string name{ "flag_" };

                             for (auto c : flag)
                                 if (c != '-')
                                     name += c;

                             return name;
                         });

INSTANTIATE_TEST_SUITE_P(Stegtool, HelpFlagTest, Values("-h", "--help"),
                         [](const testing::TestParamInfo<const char *> &info) {
                             std::string flag = info.param;
                             std::string name{ "flag_" };

                             for (auto c : flag)
                                 if (c != '-')
                                     name += c;

                             return name;
                         });

INSTANTIATE_TEST_SUITE_P(Stegtool, FlagArgumentTest,
                         Values("-o", "--output", "-m", "--message"),
                         [](const testing::TestParamInfo<const char *> &info) {
                             std::string flag = info.param;
                             std::string name{ "flag_" };

                             for (auto c : flag)
                                 if (c != '-')
                                     name += c;

                             return name;
                         });
