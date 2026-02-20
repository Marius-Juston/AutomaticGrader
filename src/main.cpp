#include <iostream>
#include "checks/validator.h"



int main() {
    const Validator validator = get_validator();
    int result = validator.check();

    std::cout << "Homework: " << HW << std::endl;

    std::cout << result << std::endl;

    return 0;
}
