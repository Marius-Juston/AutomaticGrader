#include "checks/validator.h"
#include "spdlog/spdlog.h"

int main() {
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
