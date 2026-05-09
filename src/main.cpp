#include <cstdlib>

#include "checks/cli_args.h"
#include "checks/validator.h"
#include "spdlog/spdlog.h"

namespace grader {
int run_selftest();
}

int main(int argc, char** argv) {
    auto args = grader_main::parse_args(argc, argv);
    if (!args.selftest && grader_main::selftest_env_active(std::getenv("GRADER_SELFTEST"))) {
        args.selftest = true;
    }
    if (args.selftest) {
        return grader::run_selftest();
    }

    Validator validator = get_validator();
    if (args.report_json_set) {
        validator.set_json_report_path(args.report_json_path);
    }
    const int result = validator.check();

    spdlog::info("Homework: {}", HW);

    if (result == 1) {
        spdlog::info("Succeeded all tests!");
        return 0;
    }
    spdlog::warn("Did not succeed all checks");
    return 1;
}
