#include <cstdlib>
#include <cstring>

#include "checks/validator.h"
#include "spdlog/spdlog.h"

namespace grader {
int run_selftest();
}

int main(int argc, char** argv) {
    bool selftest = false;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--selftest") == 0) {
            selftest = true;
        }
    }
    if (!selftest) {
        if (const char* env = std::getenv("GRADER_SELFTEST"); env && env[0] != '\0' && env[0] != '0') {
            selftest = true;
        }
    }
    if (selftest) {
        return grader::run_selftest();
    }

    Validator validator = get_validator();
    int result = validator.check();

    spdlog::info("Homework: {}", HW);

    if (result == 1) {
        spdlog::info("Succeeded all tests!");
        return 0;
    }
    spdlog::warn("Did not succeed all checks");
    return 1;
}
