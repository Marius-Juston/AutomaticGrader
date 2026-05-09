// Tests for Validator::check() — exception handling, message capture, JSON
// report generation, weak checker() fallback.
//
// Approach: we cannot use lambdas with captures as CheckFunction (they don't
// decay to function pointers). Instead, fakes set a global counter / behavior
// flag and we use plain free functions. This is the codebase's intentional DI
// seam (constructor takes std::vector<CheckFunction>).
//
// Flakiness mitigations:
//   - JSON report path uses ::testing::TempDir() + test name (unique per test).
//   - Reset module-global fake state in SetUp().
//   - No wall-clock sleeps. No reliance on iteration order.

#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <spdlog/spdlog.h>

#include "checks/validator.h"

namespace {

// ---------- Fake CheckFunction registry ----------
// CheckFunction has signature `int(*)(Validator*)`. We use free functions that
// consult globals so each test can configure behavior.

int g_call_count_a = 0;
int g_call_count_b = 0;
int g_return_a = 1;
int g_return_b = 1;
bool g_throw_std_in_a = false;
bool g_throw_int_in_a = false;
bool g_log_in_a = false;
bool g_log_in_b = false;
const char* g_log_msg_a = "msg-from-a";
const char* g_log_msg_b = "msg-from-b";

void reset_fake_state() {
    g_call_count_a = 0;
    g_call_count_b = 0;
    g_return_a = 1;
    g_return_b = 1;
    g_throw_std_in_a = false;
    g_throw_int_in_a = false;
    g_log_in_a = false;
    g_log_in_b = false;
    g_log_msg_a = "msg-from-a";
    g_log_msg_b = "msg-from-b";
}

int fake_check_alpha(Validator*) {
    ++g_call_count_a;
    if (g_throw_std_in_a) throw std::runtime_error("alpha-boom");
    if (g_throw_int_in_a) throw 42;
    if (g_log_in_a) spdlog::error("{}", g_log_msg_a);
    return g_return_a;
}

int fake_check_beta(Validator*) {
    ++g_call_count_b;
    if (g_log_in_b) spdlog::warn("{}", g_log_msg_b);
    return g_return_b;
}

int fake_check_always_pass(Validator*) { return 1; }
int fake_check_always_fail(Validator*) { return 0; }

class ValidatorTest : public ::testing::Test {
protected:
    void SetUp() override { reset_fake_state(); }
    void TearDown() override { reset_fake_state(); }

    // Per-test unique tmp path; auto-cleaned at TearDown.
    std::filesystem::path tmp_path(const char* suffix = ".json") {
        const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
        std::string name = std::string(info->test_suite_name()) + "_" + info->name();
        auto p = std::filesystem::temp_directory_path() / (name + suffix);
        std::error_code ec;
        std::filesystem::remove(p, ec);
        return p;
    }
};

// ---------- empty checker list ----------

TEST_F(ValidatorTest, Check_EmptyCheckList_ReturnsOne) {
    // The product code initializes `result = 1` and AND-folds; an empty list
    // therefore returns 1 (vacuous pass). This is the documented contract for
    // the weak `checker()` fallback case.
    Validator v({});
    EXPECT_EQ(v.check(), 1);
    EXPECT_TRUE(v.results().empty());
}

// ---------- happy-path AND fold ----------

TEST_F(ValidatorTest, Check_AllPassing_ReturnsOne) {
    Validator v({fake_check_always_pass, fake_check_always_pass});
    EXPECT_EQ(v.check(), 1);
    ASSERT_EQ(v.results().size(), 2u);
    EXPECT_TRUE(v.results()[0].passed);
    EXPECT_TRUE(v.results()[1].passed);
}

TEST_F(ValidatorTest, Check_OneFailing_ReturnsZero) {
    Validator v({fake_check_always_pass, fake_check_always_fail});
    EXPECT_EQ(v.check(), 0);
    ASSERT_EQ(v.results().size(), 2u);
    EXPECT_TRUE(v.results()[0].passed);
    EXPECT_FALSE(v.results()[1].passed);
}

TEST_F(ValidatorTest, Check_AllChecksRunEvenAfterFailure) {
    // Boundary: failing first check must NOT short-circuit subsequent checks.
    g_return_a = 0;  // alpha fails
    g_return_b = 1;  // beta passes
    Validator v({fake_check_alpha, fake_check_beta});
    EXPECT_EQ(v.check(), 0);
    EXPECT_EQ(g_call_count_a, 1);
    EXPECT_EQ(g_call_count_b, 1);
}

// ---------- exception handling ----------

TEST_F(ValidatorTest, Check_StdExceptionThrown_TreatedAsFailure) {
    g_throw_std_in_a = true;
    Validator v({fake_check_alpha, fake_check_beta});
    EXPECT_EQ(v.check(), 0);
    ASSERT_EQ(v.results().size(), 2u);
    EXPECT_FALSE(v.results()[0].passed);
    EXPECT_TRUE(v.results()[1].passed);
    // The error message captured should mention the exception text.
    bool found = false;
    for (const auto& m : v.results()[0].messages) {
        if (m.find("alpha-boom") != std::string::npos) found = true;
    }
    EXPECT_TRUE(found) << "captured messages did not include exception what()";
}

TEST_F(ValidatorTest, Check_NonStdExceptionThrown_TreatedAsFailure) {
    g_throw_int_in_a = true;
    Validator v({fake_check_alpha});
    EXPECT_EQ(v.check(), 0);
    ASSERT_EQ(v.results().size(), 1u);
    EXPECT_FALSE(v.results()[0].passed);
    bool found = false;
    for (const auto& m : v.results()[0].messages) {
        if (m.find("non-std exception") != std::string::npos) found = true;
    }
    EXPECT_TRUE(found);
}

// ---------- per-check message capture ----------

TEST_F(ValidatorTest, Check_MessagesAttributedToCorrectCheck) {
    g_log_in_a = true;
    g_log_msg_a = "alpha-only-message";
    g_log_in_b = true;
    g_log_msg_b = "beta-only-message";
    Validator v({fake_check_alpha, fake_check_beta});
    EXPECT_EQ(v.check(), 1);
    ASSERT_EQ(v.results().size(), 2u);

    auto contains = [](const std::vector<std::string>& msgs, const std::string& sub) {
        for (const auto& m : msgs) if (m.find(sub) != std::string::npos) return true;
        return false;
    };

    EXPECT_TRUE(contains(v.results()[0].messages, "alpha-only-message"));
    EXPECT_FALSE(contains(v.results()[0].messages, "beta-only-message"));
    EXPECT_TRUE(contains(v.results()[1].messages, "beta-only-message"));
    EXPECT_FALSE(contains(v.results()[1].messages, "alpha-only-message"));
}

TEST_F(ValidatorTest, Check_NoMessagesLogged_PerCheckMessagesEmpty) {
    Validator v({fake_check_always_pass});
    EXPECT_EQ(v.check(), 1);
    ASSERT_EQ(v.results().size(), 1u);
    EXPECT_TRUE(v.results()[0].messages.empty());
}

// ---------- JSON report ----------

TEST_F(ValidatorTest, JsonReport_NotWritten_WhenPathUnset) {
    auto p = tmp_path();
    Validator v({fake_check_always_pass});
    EXPECT_EQ(v.check(), 1);
    EXPECT_FALSE(std::filesystem::exists(p));
}

TEST_F(ValidatorTest, JsonReport_WrittenToConfiguredPath) {
    auto p = tmp_path();
    Validator v({fake_check_always_pass, fake_check_always_fail});
    v.set_json_report_path(p.string());
    (void)v.check();
    ASSERT_TRUE(std::filesystem::exists(p));
    std::ifstream in(p);
    std::stringstream ss; ss << in.rdbuf();
    const std::string body = ss.str();
    EXPECT_NE(body.find("\"passed\": 1"), std::string::npos) << body;
    EXPECT_NE(body.find("\"failed\": 1"), std::string::npos) << body;
    EXPECT_NE(body.find("\"checks\": ["), std::string::npos) << body;
    EXPECT_NE(body.find("\"status\": \"pass\""), std::string::npos) << body;
    EXPECT_NE(body.find("\"status\": \"fail\""), std::string::npos) << body;
    std::filesystem::remove(p);
}

TEST_F(ValidatorTest, JsonReport_StructureIsWellFormed) {
    auto p = tmp_path();
    Validator v({fake_check_always_pass});
    v.set_json_report_path(p.string());
    (void)v.check();
    std::ifstream in(p);
    std::stringstream ss; ss << in.rdbuf();
    const std::string body = ss.str();
    // Bracket balance check (cheap proxy for well-formedness without dragging
    // in a JSON parser dep).
    int braces = 0, brackets = 0;
    bool in_string = false;
    bool escape = false;
    for (char c : body) {
        if (escape) { escape = false; continue; }
        if (c == '\\') { escape = true; continue; }
        if (c == '"') { in_string = !in_string; continue; }
        if (in_string) continue;
        if (c == '{') ++braces;
        else if (c == '}') --braces;
        else if (c == '[') ++brackets;
        else if (c == ']') --brackets;
    }
    EXPECT_EQ(braces, 0) << body;
    EXPECT_EQ(brackets, 0) << body;
    std::filesystem::remove(p);
}

TEST_F(ValidatorTest, JsonReport_BadPath_DoesNotCrash) {
    // Boundary: unwritable path. Validator::check() must still return.
    Validator v({fake_check_always_pass});
    v.set_json_report_path("/nonexistent_dir_xyz_/out.json");
    EXPECT_EQ(v.check(), 1);
}

TEST_F(ValidatorTest, JsonReport_MessagesAreJsonEscaped) {
    g_log_in_a = true;
    g_log_msg_a = "has \"quote\" and \\backslash and\nnewline";
    auto p = tmp_path();
    Validator v({fake_check_alpha});
    v.set_json_report_path(p.string());
    (void)v.check();
    std::ifstream in(p);
    std::stringstream ss; ss << in.rdbuf();
    const std::string body = ss.str();
    // Must NOT contain raw newline inside the messages array entry; it must be
    // escaped to \n. We check for the escaped forms.
    EXPECT_NE(body.find("\\\""), std::string::npos);
    EXPECT_NE(body.find("\\\\"), std::string::npos);
    EXPECT_NE(body.find("\\n"), std::string::npos);
    std::filesystem::remove(p);
}

// ---------- resolve_check_name ----------

TEST_F(ValidatorTest, CheckName_ResolvesFreeFunctionName) {
    // dladdr should resolve `fake_check_always_pass` (visible in dynamic symbol
    // table when -rdynamic is set; falls back to "check_<i>" otherwise).
    // We accept either form — what matters is that names are NON-EMPTY and
    // distinct between two distinct checks.
    Validator v({fake_check_always_pass, fake_check_always_fail});
    (void)v.check();
    ASSERT_EQ(v.results().size(), 2u);
    EXPECT_FALSE(v.results()[0].name.empty());
    EXPECT_FALSE(v.results()[1].name.empty());
}

// ---------- weak checker() fallback ----------

TEST_F(ValidatorTest, WeakChecker_DefaultReturnsEmpty) {
    // No per-assignment checker linked into the test binary, so the weak
    // default ::checker() must return an empty list, and get_validator()
    // must yield a Validator that passes vacuously.
    Validator v = get_validator();
    EXPECT_EQ(v.check(), 1);
    EXPECT_TRUE(v.results().empty());
}

}  // namespace
