import re


def generate_hashmap_initializer(header_text):
    """
    Parses a C++ header file containing 'extern volatile struct' declarations
    and generates C++ code to populate a std::unordered_map for zero-checking.
    """
    # Matches: extern volatile struct [STRUCT_TYPE] [VAR_NAME];
    pattern = re.compile(
        r'^\s*extern\s+volatile\s+struct\s+([A-Za-z0-9_]+)\s+([A-Za-z0-9_]+)\s*;',
        re.MULTILINE
    )

    cpp_code = "void HardwareStateValidator::populate_map() {\n"

    for match in pattern.finditer(header_text):
        struct_type = match.group(1)
        var_name = match.group(2)

        # Generate the lambda function that binds the specific struct to check_zero
        cpp_code += f'    tracker_["{var_name}"] = []() {{ \n'
        cpp_code += f'        return check_zero(const_cast<const {struct_type}&>({var_name}), "{var_name}"); \n'
        cpp_code += f'    }};\n\n'

    cpp_code += "}\n"
    return cpp_code


if __name__ == "__main__":
    path = 'include/global_defs.h'

    with open(path, 'r') as f:
        header_content = f.read()

    print(generate_hashmap_initializer(header_content))
