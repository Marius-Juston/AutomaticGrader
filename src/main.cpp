#include "checks/validator.h"
#include "spdlog/spdlog.h"

int main() {
    Validator validator = get_validator();
    int result = validator.check();

    spdlog::info("Homework: {}",  HW );

    if (result != 0) {
        spdlog::warn("Did not sucessed all checks");
    }else {
        spdlog::info("Succeeded all tests!", HW, result);
    }

    return 0;
}