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
        // Boundary: defensive nullptr argv
        auto out = grader_main::parse_args(5, nullptr);
        EXPECT_FALSE(out.selftest);
        EXPECT_FALSE(out.report_json_set);
    }

    TEST(ParseArgs, OnlyProgramName_ReturnsDefaults) {
        Argv a{"prog"};
        auto out = grader_main::parse_args(a.argc(), a.argv());
        EXPECT_FALSE(out.selftest);
        EXPECT_FALSE(out.report_json_set);
    }

    TEST(ParseArgs, UnknownFlags_AreIgnored) {
        // Equivalence partition: unknown args
        Argv a{"prog", "--frobnicate", "-x", "positional"};
        auto out = grader_main::parse_args(a.argc(), a.argv());
        EXPECT_FALSE(out.selftest);
        EXPECT_FALSE(out.report_json_set);
    }

    // ---------- --selftest ----------

    TEST(ParseArgs, Selftest_LongFlag_SetsSelftestTrue) {
        Argv a{"prog", "--selftest"};
        auto out = grader_main::parse_args(a.argc(), a.argv());
        EXPECT_TRUE(out.selftest);
    }

    TEST(ParseArgs, Selftest_AmongOtherFlags_StillRecognized) {
        Argv a{"prog", "--unknown", "--selftest", "garbage"};
        auto out = grader_main::parse_args(a.argc(), a.argv());
        EXPECT_TRUE(out.selftest);
    }

    TEST(ParseArgs, Selftest_PrefixOnly_NotMatched) {
        // Boundary: --selftest must match exactly, --selftesty should NOT match.
        Argv a{"prog", "--selftesty"};
        auto out = grader_main::parse_args(a.argc(), a.argv());
        EXPECT_FALSE(out.selftest);
    }

    // ---------- --report-json space form ----------

    TEST(ParseArgs, ReportJsonSpaceForm_SetsPath) {
        Argv a{"prog", "--report-json", "/tmp/out.json"};
        auto out = grader_main::parse_args(a.argc(), a.argv());
        EXPECT_TRUE(out.report_json_set);
        EXPECT_EQ(out.report_json_path, "/tmp/out.json");
    }

    TEST(ParseArgs, ReportJsonSpaceForm_MissingValueAtEnd_NotSet) {
        // Boundary: --report-json with no following arg must NOT set the path.
        Argv a{"prog", "--report-json"};
        auto out = grader_main::parse_args(a.argc(), a.argv());
        EXPECT_FALSE(out.report_json_set);
    }

    // ---------- --report-json=value form ----------

    TEST(ParseArgs, ReportJsonEqualsForm_SetsPath) {
        Argv a{"prog", "--report-json=/tmp/x.json"};
        auto out = grader_main::parse_args(a.argc(), a.argv());
        EXPECT_TRUE(out.report_json_set);
        EXPECT_EQ(out.report_json_path, "/tmp/x.json");
    }

    TEST(ParseArgs, ReportJsonEqualsForm_EmptyValue_StillSetsButPathEmpty) {
        // Boundary: "--report-json=" has empty value; the parser sets the flag
        // (path == ""). The grader will then no-op (write_json_report fails to
        // open ""), but argv parsing itself accepts it.
        Argv a{"prog", "--report-json="};
        auto out = grader_main::parse_args(a.argc(), a.argv());
        EXPECT_TRUE(out.report_json_set);
        EXPECT_EQ(out.report_json_path, "");
    }

    TEST(ParseArgs, ReportJsonEqualsForm_PathWithEqualsInIt_Preserved) {
        Argv a{"prog", "--report-json=/tmp/key=val.json"};
        auto out = grader_main::parse_args(a.argc(), a.argv());
        EXPECT_TRUE(out.report_json_set);
        EXPECT_EQ(out.report_json_path, "/tmp/key=val.json");
    }

    // ---------- combined ----------

    TEST(ParseArgs, SelftestAndReportJson_BothSet) {
        Argv a{"prog", "--selftest", "--report-json", "/tmp/r.json"};
        auto out = grader_main::parse_args(a.argc(), a.argv());
        EXPECT_TRUE(out.selftest);
        EXPECT_TRUE(out.report_json_set);
        EXPECT_EQ(out.report_json_path, "/tmp/r.json");
    }

    TEST(ParseArgs, ReportJsonRepeated_LastWins) {
        Argv a{"prog", "--report-json", "/tmp/a", "--report-json=/tmp/b"};
        auto out = grader_main::parse_args(a.argc(), a.argv());
        EXPECT_TRUE(out.report_json_set);
        EXPECT_EQ(out.report_json_path, "/tmp/b");
    }

    // ---------- selftest_env_active ----------

    TEST(SelftestEnvActive, Nullptr_ReturnsFalse) {
        EXPECT_FALSE(grader_main::selftest_env_active(nullptr));
    }

    TEST(SelftestEnvActive, EmptyString_ReturnsFalse) {
        EXPECT_FALSE(grader_main::selftest_env_active(""));
    }

    TEST(SelftestEnvActive, ZeroFirstByte_ReturnsFalse) {
        // Equivalence partition: starts with '0' => disabled
        EXPECT_FALSE(grader_main::selftest_env_active("0"));
        EXPECT_FALSE(grader_main::selftest_env_active("0xdeadbeef"));
    }

    TEST(SelftestEnvActive, OneFirstByte_ReturnsTrue) {
        EXPECT_TRUE(grader_main::selftest_env_active("1"));
    }

    TEST(SelftestEnvActive, NonZeroNonEmpty_ReturnsTrue) {
        EXPECT_TRUE(grader_main::selftest_env_active("yes"));
        EXPECT_TRUE(grader_main::selftest_env_active("true"));
        EXPECT_TRUE(grader_main::selftest_env_active(" 0")); // leading space, not '0'
    }
} // namespace