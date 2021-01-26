cmake_minimum_required(VERSION 3.13)

project(nativelib VERSION 0.1 DESCRIPTION "Shared native C library")

# Find all the tools we need to generate bindings here
find_program(CTYPESGEN_PATH ctypesgen REQUIRED)
find_program(CTYPESGEN_TO_PXD_PATH ctypesgen_to_pxd REQUIRED)
find_program(CYTHON3 cython3 REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Configure our shell script that does all the bindings generation
configure_file(generate-bindings.sh.in generate-bindings.sh @ONLY)

# Add a custom command and a target that ALL depends on
add_custom_command(OUTPUT 
    ${PROJECT_NAME}.py
    ${PROJECT_NAME}.pxd
    COMMAND ./generate-bindings.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(python_bindings ALL DEPENDS ${PROJECT_NAME}.py)

# Configure our pyx file with Cython interop functions
configure_file(cythoncallbacks.pyx.in cythoncallbacks.pyx @ONLY)

# Cythonize the configured pyx file
add_custom_command(OUTPUT 
        cythoncallbacks.c 
        cythoncallbacks.h 
    COMMAND ${CYTHON3} --working ${CMAKE_CURRENT_BINARY_DIR} cythoncallbacks.pyx
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(cython_bindings ALL DEPENDS 
    cythoncallbacks.h 
    cythoncallbacks.c)
add_dependencies(cython_bindings python_bindings)

# Add top level native target
add_library(${PROJECT_NAME} SHARED 
    nativelib.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/cythoncallbacks.c) # Include generated Cython source

add_dependencies(${PROJECT_NAME} python_bindings)

# Tell CMake where to find files and libs
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${Python3_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${Python3_LIBRARIES})

add_subdirectory(host)