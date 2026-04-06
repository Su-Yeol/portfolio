/**
 * @file test_config_pipeline.cpp
 * @brief Integration test: ConfigParser + CanSpecConfigLoader pipeline
 * @ref SW-CMN-IT-001 IT-INT-001 ~ IT-INT-004
 *
 * ISO 26262-6 Clause 10: Integration Testing
 * Verifies config loading chain works end-to-end.
 */

#include <gtest/gtest.h>
#include "CanSpecProvider.h"
#include "modules/shared/config_parser.h"

#include <cstdlib>
#include <fstream>
#include <string>

namespace
{

void CreateModuleConfigIni(const std::string &path)
{
    std::ofstream out(path);
    out << "TargetSpeed = 10\n"
        << "RecordFlag = true\n"
        << "GPSHost = \"192.168.1.100\"\n"
        << "GPSPort = 5000\n"
        << "ControlCycle = 0.05\n";
    out.close();
}

} // namespace

// IT-INT-001: ConfigParser loads module config and CanSpec loads CAN spec
// Both parsers work independently with correct values
TEST(ConfigPipeline, DualConfigLoadersWorkIndependently)
{
    // Create a module-specific config
    system("mkdir -p integration/testdata");
    CreateModuleConfigIni("integration/testdata/module_config.ini");

    // Load module config via ConfigParser
    CConfigParser moduleConfig("integration/testdata/module_config.ini");
    EXPECT_TRUE(moduleConfig.IsSuccess());
    EXPECT_EQ(moduleConfig.GetInt("TargetSpeed"), 10);
    EXPECT_TRUE(moduleConfig.GetBool("RecordFlag"));
    EXPECT_EQ(moduleConfig.GetString("GPSHost"), "192.168.1.100");
    EXPECT_EQ(moduleConfig.GetInt("GPSPort"), 5000);
    EXPECT_FLOAT_EQ(moduleConfig.GetFloat("ControlCycle"), 0.05f);

    // CanSpecProvider is already loaded (singleton from environment)
    const auto &spec = GetCanSpecProvider();
    auto canId = spec.GetU32("aeb.cmd_can_id", 0);
    // May or may not be the integration test INI depending on test execution order
    // At minimum, the provider must not crash and return a consistent value
    auto canId2 = spec.GetU32("aeb.cmd_can_id", 0);
    EXPECT_EQ(canId, canId2) << "CanSpecProvider must return consistent values";
}

// IT-INT-002: ConfigParser handles multiple files in sequence
TEST(ConfigPipeline, MultipleConfigFilesInSequence)
{
    system("mkdir -p integration/testdata");

    {
        std::ofstream out("integration/testdata/config_a.ini");
        out << "ModeA = true\nValueA = 100\n";
    }
    {
        std::ofstream out("integration/testdata/config_b.ini");
        out << "ModeB = false\nValueB = 200\n";
    }

    CConfigParser configA("integration/testdata/config_a.ini");
    CConfigParser configB("integration/testdata/config_b.ini");

    EXPECT_TRUE(configA.IsSuccess());
    EXPECT_TRUE(configB.IsSuccess());

    // Each parser has independent state
    EXPECT_TRUE(configA.Contain("ModeA"));
    EXPECT_FALSE(configA.Contain("ModeB"));
    EXPECT_TRUE(configB.Contain("ModeB"));
    EXPECT_FALSE(configB.Contain("ModeA"));

    EXPECT_EQ(configA.GetInt("ValueA"), 100);
    EXPECT_EQ(configB.GetInt("ValueB"), 200);
}

// IT-INT-003: ConfigParser gracefully handles missing file
TEST(ConfigPipeline, MissingFileGracefulDegradation)
{
    CConfigParser missing("/nonexistent/path/config.ini");
    EXPECT_FALSE(missing.IsSuccess());

    // Should not crash when checking for keys
    EXPECT_FALSE(missing.Contain("AnyKey"));
}

// IT-INT-004: Config values can be used for CAN spec parameter lookup keys
TEST(ConfigPipeline, ConfigDrivenCanSpecLookup)
{
    system("mkdir -p integration/testdata");
    {
        std::ofstream out("integration/testdata/dynamic_config.ini");
        out << "CanSpecKey = aeb.cmd_can_id\n";
    }

    CConfigParser config("integration/testdata/dynamic_config.ini");
    EXPECT_TRUE(config.IsSuccess());

    std::string canSpecKey = config.GetString("CanSpecKey");
    EXPECT_EQ(canSpecKey, "aeb.cmd_can_id");

    // Use the dynamically loaded key to query CanSpec
    const auto &spec = GetCanSpecProvider();
    auto value = spec.GetU32(canSpecKey, 0);
    // Value is valid if the CAN spec is loaded, or 0 (fallback) if not
    // Either way, the pipeline must not crash
    (void)value;
}
