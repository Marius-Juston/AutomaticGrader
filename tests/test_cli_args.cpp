// Tests for grader_main::parse_args + selftest_env_active.
//
// Input selection strategy:
//   - Equivalence partitions: {no flags}, {--selftest}, {--report-json <path>},
//     {--report-json=<path>}, {unknown flags}.
//   - Boundary values for argv: argc=0, argc=1 (no args), nullptr argv,
//     missing trailing value for --report-json.
//   - Boundary values for env: nullptr, "", "0", "1", "yes".

#include <gtest/gtest.h>

#include <vector>

#include "checks/cli_args.h"

namespace {
    // Helper: build a non-const argv pointer array from a vector of literals.
    // Lifetime: caller-owned vector must outlive the returned argv.
    struct Argv {
        std::vector<std::string> storage;
        std::vector<char *> ptrs;

        explicit Argv(std::initializer_list<const char *> args) {
            storage.reserve(args.size());
            for (auto *a: args) storage.emplace_back(a);
            ptrs.reserve(storage.size() + 1);
            for (auto &s: storage) ptrs.push_back(s.data());
            ptrs.push_back(nullptr);
        }

        int argc() const { return static_cast<int>(storage.size()); }
        char **argv() { return ptrs.data(); }
    };

    // ---------- Equivalence partition: empty / no-flag ----------

    TEST(ParseArgs, NoArgs_argcZero_ReturnsDefaults) {
        // Arrange
        char *fake[1] = {nullptr};
        // Act
        auto out = grader_main::parse_args(0, fake);
        // Assert
        EXPECT_FALSE(out.selftest);
        EXPECT_FALSE(out.report_json_set);
        EXPECT_TRUE(out.report_json_path.empty());
    }

    TEST(ParseArgs, NullArgv_ReturnsDefaults) {
        // Arrange: defensive nullptr argv
        // Act
        auto out = grader_main::parse_args(5, nullptr);
        // Assert
        EXPECT_FALSE(out.selftest);
        EXPECT_FALSE(out.report_json_set);
    }

    TEST(ParseArgs, OnlyProgramName_ReturnsDefaults) {
        // Arrange
        Argv a{"prog"};
        // Act
        auto out = grader_main::parse_args(a.argc(), a.argv());
        // Assert
        EXPECT_FALSE(out.selftest);
        EXPECT_FALSE(out.report_json_set);
    }

    TEST(ParseArgs, UnknownFlags_AreIgnored) {
        // Arrange: equivalence partition — unknown args
        Argv a{"prog", "--frobnicate", "-x", "positional"};
        // Act
        auto out = grader_main::parse_args(a.argc(), a.argv());
        // Assert
        EXPECT_FALSE(out.selftest);
        EXPECT_FALSE(out.report_json_set);
    }

    // ---------- --selftest ----------

    TEST(ParseArgs, Selftest_LongFlag_SetsSelftestTrue) {
        // Arrange
        Argv a{"prog", "--selftest"};
        // Act
        auto out = grader_main::parse_args(a.argc(), a.argv());
        // Assert
        EXPECT_TRUE(out.selftest);
    }

    TEST(ParseArgs, Selftest_AmongOtherFlags_StillRecognized) {
        // Arrange
        Argv a{"prog", "--unknown", "--selftest", "garbage"};
        // Act
        auto out = grader_main::parse_args(a.argc(), a.argv());
        // Assert
        EXPECT_TRUE(out.selftest);
    }

    TEST(ParseArgs, Selftest_PrefixOnly_NotMatched) {
        // Arrange: --selftest must match exactly, --selftesty should NOT match.
        Argv a{"prog", "--selftesty"};
        // Act
        auto out = grader_main::parse_args(a.argc(), a.argv());
        // Assert
        EXPECT_FALSE(out.selftest);
    }

    // ---------- --report-json space form ----------

    TEST(ParseArgs, ReportJsonSpaceForm_SetsPath) {
        // Arrange
        Argv a{"prog", "--report-json", "/tmp/out.json"};
        // Act
        auto out = grader_main::parse_args(a.argc(), a.argv());
        // Assert
        EXPECT_TRUE(out.report_json_set);
        EXPECT_EQ(out.report_json_path, "/tmp/out.json");
    }

    TEST(ParseArgs, ReportJsonSpaceForm_MissingValueAtEnd_NotSet) {
        // Arrange: --report-json with no following arg must NOT set the path.
        Argv a{"prog", "--report-json"};
        // Act
        auto out = grader_main::parse_args(a.argc(), a.argv());
        // Assert
        EXPECT_FALSE(out.report_json_set);
    }

    // ---------- --report-json=value form ----------

    TEST(ParseArgs, ReportJsonEqualsForm_SetsPath) {
        // Arrange
        Argv a{"prog", "--report-json=/tmp/x.json"};
        // Act
        auto out = grader_main::parse_args(a.argc(), a.argv());
        // Assert
        EXPECT_TRUE(out.report_json_set);
        EXPECT_EQ(out.report_json_path, "/tmp/x.json");
    }

    TEST(ParseArgs, ReportJsonEqualsForm_EmptyValue_StillSetsButPathEmpty) {
        // Arrange: "--report-json=" has empty value; the parser sets the flag.
        Argv a{"prog", "--report-json="};
        // Act
        auto out = grader_main::parse_args(a.argc(), a.argv());
        // Assert
        EXPECT_TRUE(out.report_json_set);
        EXPECT_EQ(out.report_json_path, "");
    }

    TEST(ParseArgs, ReportJsonEqualsForm_PathWithEqualsInIt_Preserved) {
        // Arrange
        Argv a{"prog", "--report-json=/tmp/key=val.json"};
        // Act
        auto out = grader_main::parse_args(a.argc(), a.argv());
        // Assert
        EXPECT_TRUE(out.report_json_set);
        EXPECT_EQ(out.report_json_path, "/tmp/key=val.json");
    }

    // ---------- combined ----------

    TEST(ParseArgs, SelftestAndReportJson_BothSet) {
        // Arrange
        Argv a{"prog", "--selftest", "--report-json", "/tmp/r.json"};
        // Act
        auto out = grader_main::parse_args(a.argc(), a.argv());
        // Assert
        EXPECT_TRUE(out.selftest);
        EXPECT_TRUE(out.report_json_set);
        EXPECT_EQ(out.report_json_path, "/tmp/r.json");
    }

    TEST(ParseArgs, ReportJsonRepeated_LastWins) {
        // Arrange
        Argv a{"prog", "--report-json", "/tmp/a", "--report-json=/tmp/b"};
        // Act
        auto out = grader_main::parse_args(a.argc(), a.argv());
        // Assert
        EXPECT_TRUE(out.report_json_set);
        EXPECT_EQ(out.report_json_path, "/tmp/b");
    }

    // ---------- selftest_env_active ----------

    TEST(SelftestEnvActive, Nullptr_ReturnsFalse) {
        // Arrange
        // Act
        const bool active = grader_main::selftest_env_active(nullptr);
        // Assert
        EXPECT_FALSE(active);
    }

    TEST(SelftestEnvActive, EmptyString_ReturnsFalse) {
        // Arrange
        // Act
        const bool active = grader_main::selftest_env_active("");
        // Assert
        EXPECT_FALSE(active);
    }

    TEST(SelftestEnvActive, ZeroFirstByte_ReturnsFalse) {
        // Arrange: equivalence partition — starts with '0' => disabled.
        // Act + Assert (one logical partition)
        EXPECT_FALSE(grader_main::selftest_env_active("0"));
        EXPECT_FALSE(grader_main::selftest_env_active("0xdeadbeef"));
    }

    TEST(SelftestEnvActive, OneFirstByte_ReturnsTrue) {
        // Arrange
        // Act
        const bool active = grader_main::selftest_env_active("1");
        // Assert
        EXPECT_TRUE(active);
    }

    TEST(SelftestEnvActive, NonZeroNonEmpty_ReturnsTrue) {
        // Arrange: equivalence partition — non-empty, non-'0'-prefix strings.
        // Act + Assert
        EXPECT_TRUE(grader_main::selftest_env_active("yes"));
        EXPECT_TRUE(grader_main::selftest_env_active("true"));
        EXPECT_TRUE(grader_main::selftest_env_active(" 0")); // leading space, not '0'
    }
} // namespace