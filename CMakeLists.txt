cmake_minimum_required(VERSION 4.1)
project(AutomaticGrader)

set(CMAKE_CXX_STANDARD 20)
#
#file(GLOB_RECURSE DRIVER_SOURCES
#        "${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/driverlib/f2837xd/driverlib/*.c"
#)

#file(GLOB_RECURSE DEVICE_SOURCES
#        "${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/common/source/*.c"
#        "${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/source/*.c"
#)

add_executable(${PROJECT_NAME} src/main.cpp student_work/LABstarter_main.c src/ti_stubs.cpp
        #        ${DRIVER_SOURCES}
        #        ${DEVICE_SOURCES}
)


#target_compile_definitions(${PROJECT_NAME} PRIVATE CPU1)
#target_compile_definitions(${PROJECT_NAME} PRIVATE _LAUNCHXL_F28379D)
#target_compile_definitions(${PROJECT_NAME} PRIVATE __TMS320C28XX__)
#target_compile_definitions(${PROJECT_NAME} PRIVATE _DUAL_HEADERS)
#target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
#target_compile_definitions(${PROJECT_NAME} PRIVATE __TI_COMPILER_VERSION__=16006000)
#target_compile_definitions(${PROJECT_NAME} PRIVATE __TMS320C28XX_CLA__)
#target_compile_definitions(${PROJECT_NAME} PRIVATE __TI_FPU_SUPPORT__=1)
#target_compile_definitions(${PROJECT_NAME} PRIVATE __TMS320C28XX_FPU32__=1)

target_include_directories(${PROJECT_NAME}
        BEFORE PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(${PROJECT_NAME}
        PRIVATE
        -include ti_stubs.h
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/common/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/driverlib/f2837xd/driverlib
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/libraries/dsp/FPU/c28/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include
        ${PROJECT_SOURCE_DIR}/lib/default
)

#set_property(TARGET ${PROJECT_NAME} PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/libraries/dsp/FPU/c28/lib/c28x_fpu_dsp_library.lib")
#set_property(TARGET ${PROJECT_NAME} PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/driverlib/f2837xd/driverlib/ccs/Debug/driverlib.lib")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-volatile -Wno-pointer-to-int-cast")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-volatile -Wno-pointer-to-int-cast")

set_source_files_properties(
        student_work/LABstarter_main.c
        PROPERTIES
        COMPILE_DEFINITIONS "main=temp_main"
)