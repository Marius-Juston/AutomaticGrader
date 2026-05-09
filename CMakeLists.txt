cmake_minimum_required(VERSION 3.21)
project(AutomaticGrader CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)

# Default to Release if not specified — much faster runtime, only marginally slower link.
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif ()

# Ensure relocatable objects so we can shuffle them into static libs / object libs cleanly.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Speed up incremental builds with ccache when available.
find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    message(STATUS "ccache found: ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif ()

# ASSIGNMENT selector — accepts HW1..HW6, Lab1..Lab7-2.
# Back-compat: legacy HW=N env var maps to ASSIGNMENT=HW${N}.
if (DEFINED ENV{ASSIGNMENT})
    set(ASSIGNMENT $ENV{ASSIGNMENT} CACHE STRING "Which assignment to grade" FORCE)
elseif (DEFINED ENV{HW})
    set(ASSIGNMENT "HW$ENV{HW}" CACHE STRING "Which assignment to grade" FORCE)
elseif (NOT DEFINED ASSIGNMENT)
    set(ASSIGNMENT "HW1" CACHE STRING "Which assignment to grade" FORCE)
endif ()

set(GRADER_VALID_ASSIGNMENTS
        HW1 HW2 HW3 HW4 HW5 HW6
        Lab1 Lab2 Lab3 Lab4 Lab5 Lab6 Lab7-1 Lab7-2)
if (NOT ASSIGNMENT IN_LIST GRADER_VALID_ASSIGNMENTS)
    message(FATAL_ERROR
            "Invalid ASSIGNMENT='${ASSIGNMENT}'. Valid values: ${GRADER_VALID_ASSIGNMENTS}")
endif ()

message(STATUS "Building grader for: ${ASSIGNMENT}")

# Decode ASSIGNMENT into kind (HW/Lab) + numeric token, plus the source filename.
if (ASSIGNMENT MATCHES "^HW([1-6])$")
    set(GRADER_KIND HW)
    set(GRADER_NUM ${CMAKE_MATCH_1})
    set(GRADER_FILE "src/checks/hw${GRADER_NUM}.cpp")
elseif (ASSIGNMENT MATCHES "^Lab([1-6])$")
    set(GRADER_KIND LAB)
    set(GRADER_NUM ${CMAKE_MATCH_1})
    set(GRADER_FILE "src/checks/lab${GRADER_NUM}.cpp")
elseif (ASSIGNMENT STREQUAL "Lab7-1")
    set(GRADER_KIND LAB)
    set(GRADER_NUM 71)
    set(GRADER_FILE "src/checks/lab7_1.cpp")
elseif (ASSIGNMENT STREQUAL "Lab7-2")
    set(GRADER_KIND LAB)
    set(GRADER_NUM 72)
    set(GRADER_FILE "src/checks/lab7_2.cpp")
endif ()

# Tolerate not-yet-implemented checkers — fall back to the HW1 file but flag clearly.
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${GRADER_FILE}")
    message(WARNING
            "Checker source ${GRADER_FILE} not yet implemented; falling back to src/checks/hw1.cpp. "
            "The grader will still build but will not validate ${ASSIGNMENT}.")
    set(GRADER_FILE "src/checks/hw1.cpp")
    # Force HW1 selector so validator.cpp picks up the right header.
    set(GRADER_KIND HW)
    set(GRADER_NUM 1)
endif ()

# STUDENT_SRC selector — points the build at the firmware source under test.
# Default: student_work/LABstarter_main.c (the in-repo working copy).
# Override per-assignment by setting STUDENT_SRC env var or -DSTUDENT_SRC=path.
# Common reference fixtures live under context/code_solutions/.
if (DEFINED ENV{STUDENT_SRC})
    set(STUDENT_SRC $ENV{STUDENT_SRC} CACHE STRING "Student firmware source" FORCE)
elseif (NOT DEFINED STUDENT_SRC)
    set(STUDENT_SRC "student_work/LABstarter_main.c" CACHE STRING "Student firmware source" FORCE)
endif ()
if (NOT IS_ABSOLUTE "${STUDENT_SRC}")
    set(STUDENT_SRC_ABS "${CMAKE_CURRENT_SOURCE_DIR}/${STUDENT_SRC}")
else ()
    set(STUDENT_SRC_ABS "${STUDENT_SRC}")
endif ()
if (NOT EXISTS "${STUDENT_SRC_ABS}")
    message(FATAL_ERROR "STUDENT_SRC='${STUDENT_SRC}' does not exist (resolved: ${STUDENT_SRC_ABS}).")
endif ()
message(STATUS "Student firmware source: ${STUDENT_SRC_ABS}")

# Patch the student source so its `while(1)` IDLE loop becomes the harness-controlled
# `GRADER_MAIN_LOOP` macro (defined in include/ti_stubs.h under AUTO_GRADER).
# This lets the cooperative main-loop driver step the student's main() body
# deterministically, eliminating the race between the ISR-driving test thread
# and a detached `temp_main` thread. See ~/.claude/plans/based-on-hw1mj-main-from-resilient-steele.md
set(STUDENT_PATCHED_DIR "${CMAKE_CURRENT_BINARY_DIR}/student_patched")
set(STUDENT_PATCHED "${STUDENT_PATCHED_DIR}/student.c")
find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_custom_command(
        OUTPUT ${STUDENT_PATCHED}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${STUDENT_PATCHED_DIR}"
        COMMAND ${Python3_EXECUTABLE}
        "${PROJECT_SOURCE_DIR}/tools/patch_student_source.py"
        "${STUDENT_SRC_ABS}"
        "${STUDENT_PATCHED}"
        DEPENDS ${STUDENT_SRC_ABS}
        "${PROJECT_SOURCE_DIR}/tools/patch_student_source.py"
        VERBATIM)
add_custom_target(grader_student_patch DEPENDS ${STUDENT_PATCHED})

if (NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif ()

# ---- Object libraries for parallel compile ---------------------------------

add_library(grader_infra OBJECT
        src/checks/format_parser.cpp
        src/checks/synthetic_clock.cpp
        src/checks/printf_capture.cpp
        src/checks/stimulus.cpp
        src/checks/expectations.cpp
        src/checks/selftest.cpp
)
target_include_directories(grader_infra PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/common/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include
        ${PROJECT_SOURCE_DIR}/lib/default
)
target_link_libraries(grader_infra PUBLIC spdlog::spdlog)
target_compile_options(grader_infra PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>
        -include ti_stubs.h
)

# Heavy generated checker code — split out so it can compile in parallel with the rest
# and so we don't pay its cost on every checker tweak.
add_library(grader_state_check OBJECT
        src/checks/generated.cpp
        src/checks/compare_generated.cpp
        src/checks/stat_checker.cpp
        src/checks/validator.cpp
)
target_include_directories(grader_state_check PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/common/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include
        ${PROJECT_SOURCE_DIR}/lib/default
)
target_link_libraries(grader_state_check PUBLIC spdlog::spdlog)
target_compile_options(grader_state_check PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>
        -include ti_stubs.h
)

# Stubs + student support code.
add_library(grader_stubs OBJECT
        src/ti_stubs.cpp
        src/lidar.cpp
        src/LEDPatterns.c
        src/MatrixMath.c
        src/OptiTrack.c
        src/SE423Lib.c
        src/user_xy.c
        src/checks/main_loop_driver.cpp
)
target_include_directories(grader_stubs PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/common/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/driverlib/f2837xd/driverlib
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/libraries/dsp/FPU/c28/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include
        ${PROJECT_SOURCE_DIR}/lib/default
)
target_link_libraries(grader_stubs PUBLIC spdlog::spdlog)
target_compile_options(grader_stubs PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>
        $<$<COMPILE_LANGUAGE:C>:-Wno-pointer-to-int-cast>
        -include ti_stubs.h
)

# Per-assignment checker — recompiled when ASSIGNMENT changes.
add_library(grader_assignment OBJECT
        ${GRADER_FILE}
)
target_include_directories(grader_assignment PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/common/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include
        ${PROJECT_SOURCE_DIR}/lib/default
)
target_link_libraries(grader_assignment PUBLIC spdlog::spdlog)
target_compile_options(grader_assignment PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>
        -include ti_stubs.h
)

# Student firmware (renamed main → temp_main so it coexists with src/main.cpp).
# We compile the *patched* copy where `while(1)` has been rewritten to GRADER_MAIN_LOOP.
add_library(grader_student OBJECT
        ${STUDENT_PATCHED}
)
add_dependencies(grader_student grader_student_patch)
target_include_directories(grader_student PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/common/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/driverlib/f2837xd/driverlib
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/libraries/dsp/FPU/c28/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include
        ${PROJECT_SOURCE_DIR}/lib/default
)
target_compile_options(grader_student PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-Wno-pointer-to-int-cast -Wno-implicit-function-declaration>
        -include ti_stubs.h
)
set_source_files_properties(
        ${STUDENT_PATCHED}
        PROPERTIES
        COMPILE_DEFINITIONS "main=temp_main"
        GENERATED TRUE
)

# ---- Main executable -------------------------------------------------------

add_executable(${PROJECT_NAME}
        src/main.cpp
        $<TARGET_OBJECTS:grader_infra>
        $<TARGET_OBJECTS:grader_state_check>
        $<TARGET_OBJECTS:grader_stubs>
        $<TARGET_OBJECTS:grader_assignment>
        $<TARGET_OBJECTS:grader_student>
)

target_include_directories(${PROJECT_NAME}
        BEFORE PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/common/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/driverlib/f2837xd/driverlib
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/libraries/dsp/FPU/c28/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include
        ${PROJECT_SOURCE_DIR}/lib/default
)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32> ${CMAKE_DL_LIBS})

# Export symbols so dladdr can resolve check_function names in --report-json output.
if (NOT MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE -rdynamic)
endif ()

target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>
        -include ti_stubs.h
)

# Common compile definitions: AUTO_GRADER, HW=, LAB=, ASSIGNMENT_HW/LAB
set(GRADER_COMMON_DEFS AUTO_GRADER)
if (GRADER_KIND STREQUAL "HW")
    list(APPEND GRADER_COMMON_DEFS HW=${GRADER_NUM} ASSIGNMENT_HW)
else ()
    list(APPEND GRADER_COMMON_DEFS LAB=${GRADER_NUM} ASSIGNMENT_LAB
            HW=1)  # validator.cpp legacy fallback
endif ()

foreach (tgt grader_infra grader_state_check grader_stubs grader_assignment grader_student ${PROJECT_NAME})
    target_compile_definitions(${tgt} PRIVATE ${GRADER_COMMON_DEFS})
endforeach ()

# NOTE: precompiled headers play poorly with `-include ti_stubs.h` and the
# generated TUs already redefine many register globals — we rely on ccache +
# object-library parallelism instead. If compile times become a bottleneck,
# revisit a per-target PCH.

# Fast-debug, hard-on-warnings toggles
target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-O0 -g>
        $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG>
        $<$<CONFIG:Release>:-O2 -DNDEBUG>
)
