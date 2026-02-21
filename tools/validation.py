import re


def generate_zero_checks(header_text):
    """
    Parses a C/C++ header file containing register structure definitions
    and generates C++ templates to verify all registers are zero.
    """
    # 1. Base C++ template blocks
    cpp_code = """#include "checks/generated.hpp"
#include <iostream>

template<typename T>
std::enable_if_t<std::is_arithmetic_v<T>, bool>
check_zero(const T &value, const std::string &name) {
    if (value != 0) {
        std::cout << name << " is non-zero"<< std::endl ;
        return false;
    }
    return true;
}

template<typename T, std::size_t N>
bool check_zero(const T (&arr)[N], const std::string &name) {
    bool all_zero = true;
    for (std::size_t i = 0; i < N; ++i) {
        all_zero &= check_zero(arr[i], name + "[" + std::to_string(i) + "]");
    }
    return all_zero;
}

bool check_zero(const PINT &obj, const std::string &name) {
    if (obj != nullptr) {
        std::cout << name << " is non-zero" << std::endl;
        return false;
    }
    return true;
}
"""

    header_code = """
#pragma once
    
#include <string>
#include <type_traits>
#include <cstdint>
template<typename T>
std::enable_if_t<std::is_arithmetic_v<T>, bool>
check_zero(const T &value, const std::string &name);

template<typename T, std::size_t N>
bool check_zero(const T (&arr)[N], const std::string &name);

bool check_zero(const PINT &obj, const std::string &name);
"""

    # 2. Regular Expressions for struct and member parsing
    # Captures the struct body. Example: struct MEM_CFG_REGS { ... };
    struct_pattern = re.compile(r'(?:struct|union)\s+([A-Za-z0-9_]+)\s*\{([^}]+)};', re.MULTILINE)

    # Captures member variables inside the struct block.
    # Group 1: union/struct (optional)
    # Group 2: Type
    # Group 3: Variable Name
    # Group 4: Array size (optional)
    # Group 5: Comment/Description (optional)
    member_pattern = re.compile(
        r'^\s*(?:volatile\s+)?(union|struct)?\s*([A-Za-z0-9_]+)\s+([A-Za-z0-9_]+)(?:\[(\d+)])?(?:[:0-9\s]*)?;(?:\s*//\s*(.*))?',
        re.MULTILINE
    )

    # 3. Iterate over all structures found in the header
    for match in struct_pattern.finditer(header_text):
        struct_name = match.group(1)
        members_text = match.group(2)

        # Skip bitfield structure definitions (e.g., DxLOCK_BITS).
        # We validate the entire register simultaneously using the union's .all member.
        # if struct_name.endswith('_BITS') or struct_name.endswith('_REG'):
        #     continue

        cpp_code += f"\nbool check_zero(const {struct_name} &obj, const std::string &name) {{\n"
        cpp_code += "    bool all_zero = true;\n\n"

        header_code += f"\nbool check_zero(const {struct_name} &obj, const std::string &name);"

        # 4. Process each member of the struct
        for mem_match in member_pattern.finditer(members_text):
            # is_union_or_struct = mem_match.group(1)
            # mem_type = mem_match.group(2)
            mem_name = mem_match.group(3)
            comment = mem_match.group(5)

            # Clean up the description string for the C++ printout
            desc = comment.strip() if comment else mem_name
            # Strip out bit-range prefixes often found in TI headers (e.g., "15:0 Reserved")
            # desc = re.sub(r'^[\d:\s]+\s', '', desc)
            desc = desc.replace('"', '\\"')  # Escape quotes

            # Determine accessor method. If it's a union, target the `.all` field.
            accessor = f"obj.{mem_name}"
            # if is_union_or_struct == 'union':
            #     accessor += ".all"

            cpp_code += f'    all_zero &= check_zero({accessor}, name + " " + "{desc}");\n'

        cpp_code += "\n    return all_zero;\n}\n"

    return cpp_code, header_code


if __name__ == "__main__":
    files = ["lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_memconfig.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_adc.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_analogsubsys.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_cla.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_clb.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_clbxbar.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_sysctrl.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_cmpss.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_cputimer.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_sysctrl.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_dac.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_dcsm.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_dma.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_ecap.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_emif.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_epwm.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_epwm_xbar.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_eqep.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_flash.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_gpio.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_i2c.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_mcbsp.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_nmiintrupt.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_piectrl.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_pievect.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_sci.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_sdfm.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_sdfm.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_spi.h",
             "lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include/F2837xD_xint.h",
             ]
    files = sorted(set(files))

    header_content = []

    for file in files:
        with open(file, 'r') as f:
            header_content.append(f.read())

    header_content = "\n".join(header_content)

    generated_cpp, generated_hpp = generate_zero_checks(header_content)

    # Output to standard out or write to a target file
    print(generated_cpp)

    with open("src/checks/generated.cpp", 'w') as f:
        f.write(generated_cpp)

    with open("include/checks/generated.hpp", 'w') as f:
        f.write(generated_hpp)
