/**
 * @file test_config_parser.cpp
 * @brief Unit tests for INI configuration parser (ASIL D)
 * @ref SW-CMN-REQ-CFG-001 ~ CFG-002
 *
 * ISO 26262-6 Clause 9: Unit Testing
 */

#include <gtest/gtest.h>
#include "modules/shared/config_parser.h"

#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>

namespace
{

// Helper to create temporary INI files for testing
class TempIniFile
{
public:
    explicit TempIniFile(const std::string &filename, const std::string &content)
        : path_("common/testdata/" + filename)
    {
        std::ofstream out(path_);
        out << content;
        out.close();
    }
    ~TempIniFile() = default;

    const std::string &Path() const { return path_; }

private:
    std::string path_;
};

} // namespace

// ==========================================================================
// UT-PARSE-001: Normal parsing with space-delimited key=value
// ==========================================================================

TEST(ConfigParser, ParsesSpaceDelimitedEntries)
{
    TempIniFile ini("valid.ini",
                    "TargetSpeed = 10\n"
                    "RecordFlag = true\n"
                    "GPSHost = \"192.168.1.1\"\n"
                    "GainFactor = 0.75\n");

    CConfigParser parser(ini.Path());
    EXPECT_TRUE(parser.IsSuccess());
    EXPECT_EQ(parser.GetInt("TargetSpeed"), 10);
    EXPECT_TRUE(parser.GetBool("RecordFlag"));
    EXPECT_EQ(parser.GetString("GPSHost"), "192.168.1.1");
    EXPECT_FLOAT_EQ(parser.GetFloat("GainFactor"), 0.75f);
}

// UT-PARSE-002: Parsing with equals-only delimiter (no spaces)
TEST(ConfigParser, ParsesEqualsOnlyDelimiter)
{
    TempIniFile ini("equals_only.ini",
                    "Key1=Value1\n"
                    "Number=42\n");

    CConfigParser parser(ini.Path());
    EXPECT_TRUE(parser.IsSuccess());
    EXPECT_EQ(parser.GetString("Key1"), "Value1");
    EXPECT_EQ(parser.GetInt("Number"), 42);
}

// UT-PARSE-003: Contain returns false for missing key
TEST(ConfigParser, ContainReturnsFalseForMissingKey)
{
    TempIniFile ini("contain_test.ini", "ExistingKey = hello\n");
    CConfigParser parser(ini.Path());
    EXPECT_TRUE(parser.Contain("ExistingKey"));
    EXPECT_FALSE(parser.Contain("NonExistentKey"));
}

// UT-PARSE-004: Missing key throws exception
TEST(ConfigParser, ThrowsOnMissingKey)
{
    TempIniFile ini("throw_test.ini", "OnlyKey = value\n");
    CConfigParser parser(ini.Path());

    EXPECT_THROW(parser.GetBool("Missing"), std::invalid_argument);
    EXPECT_THROW(parser.GetString("Missing"), std::invalid_argument);
    EXPECT_THROW(parser.GetFloat("Missing"), std::invalid_argument);
    EXPECT_THROW(parser.GetInt("Missing"), std::invalid_argument);
}

// UT-PARSE-005: Empty file
TEST(ConfigParser, EmptyFileReturnsNoSuccess)
{
    TempIniFile ini("empty.ini", "");
    CConfigParser parser(ini.Path());
    EXPECT_FALSE(parser.IsSuccess());
}

// UT-PARSE-006: Non-existent file
TEST(ConfigParser, NonExistentFileReturnsNoSuccess)
{
    CConfigParser parser("/nonexistent/path/config.ini");
    EXPECT_FALSE(parser.IsSuccess());
}

// UT-PARSE-007: GetBool with various true/false representations
TEST(ConfigParser, GetBoolVariants)
{
    TempIniFile ini("bool_test.ini",
                    "flag_true = true\n"
                    "flag_True = True\n"
                    "flag_false = false\n"
                    "flag_zero = 0\n");

    CConfigParser parser(ini.Path());
    EXPECT_TRUE(parser.GetBool("flag_true"));
    EXPECT_TRUE(parser.GetBool("flag_True"));
    EXPECT_FALSE(parser.GetBool("flag_false"));
    EXPECT_FALSE(parser.GetBool("flag_zero"));
}

// UT-PARSE-008: GetString with quoted values
TEST(ConfigParser, GetStringStripsQuotes)
{
    TempIniFile ini("quoted.ini", "host = \"10.0.0.1\"\n");
    CConfigParser parser(ini.Path());
    EXPECT_EQ(parser.GetString("host"), "10.0.0.1");
}

// UT-PARSE-009: GetString without quotes
TEST(ConfigParser, GetStringNoQuotes)
{
    TempIniFile ini("unquoted.ini", "name = plain_value\n");
    CConfigParser parser(ini.Path());
    EXPECT_EQ(parser.GetString("name"), "plain_value");
}

// UT-PARSE-010: Integer boundary values
TEST(ConfigParser, IntBoundaryValues)
{
    TempIniFile ini("int_boundary.ini",
                    "zero = 0\n"
                    "negative = -100\n"
                    "large = 2147483647\n");

    CConfigParser parser(ini.Path());
    EXPECT_EQ(parser.GetInt("zero"), 0);
    EXPECT_EQ(parser.GetInt("negative"), -100);
    EXPECT_EQ(parser.GetInt("large"), 2147483647);
}

// --- Branch coverage enhancement tests ---

// UT-PARSE-011: Multiple entries parsed correctly
TEST(ConfigParser, MultipleEntriesAllAccessible)
{
    TempIniFile ini("multi.ini",
                    "a = 1\n"
                    "b = 2\n"
                    "c = 3\n"
                    "d = 4\n"
                    "e = 5\n");

    CConfigParser parser(ini.Path());
    EXPECT_TRUE(parser.IsSuccess());
    EXPECT_EQ(parser.GetInt("a"), 1);
    EXPECT_EQ(parser.GetInt("e"), 5);
    EXPECT_TRUE(parser.Contain("c"));
    EXPECT_FALSE(parser.Contain("f"));
}

// UT-PARSE-012: Float precision
TEST(ConfigParser, FloatPrecision)
{
    TempIniFile ini("float_precision.ini",
                    "pi = 3.14159\n"
                    "neg = -0.001\n"
                    "zero_f = 0.0\n");

    CConfigParser parser(ini.Path());
    EXPECT_FLOAT_EQ(parser.GetFloat("pi"), 3.14159f);
    EXPECT_FLOAT_EQ(parser.GetFloat("neg"), -0.001f);
    EXPECT_FLOAT_EQ(parser.GetFloat("zero_f"), 0.0f);
}

// UT-PARSE-013: GetBool with non-t/T first char returns false
TEST(ConfigParser, GetBoolNonTruthy)
{
    TempIniFile ini("bool_edge.ini",
                    "yes = yes\n"
                    "one = 1\n"
                    "False = False\n");

    CConfigParser parser(ini.Path());
    EXPECT_FALSE(parser.GetBool("yes"));    // 'y' != 't'/'T'
    EXPECT_FALSE(parser.GetBool("one"));    // '1' != 't'/'T'
    EXPECT_FALSE(parser.GetBool("False"));  // 'F' != 't'/'T'
}

// UT-PARSE-014: Line with only delimiter
TEST(ConfigParser, LineWithOnlyDelimiter)
{
    TempIniFile ini("delimiter_only.ini",
                    "=value_no_key\n"
                    "key_no_value=\n"
                    "normal = ok\n");

    CConfigParser parser(ini.Path());
    EXPECT_TRUE(parser.IsSuccess());
    EXPECT_EQ(parser.GetString("normal"), "ok");
}
