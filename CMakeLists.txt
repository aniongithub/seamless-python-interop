cmake_minimum_required(VERSION 3.13)

project(nativelib VERSION 0.1 DESCRIPTION "Shared native C library")

# Find all the tools we need to generate bindings here
find_program(CTYPESGEN_PATH ctypesgen REQUIRED)
find_program(CYTHON3 cython3 REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(Boost COMPONENTS python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR} REQUIRED)

# Configure our shell script that does all the bindings generation
configure_file(generate-bindings.sh.in generate-bindings.sh @ONLY)
configure_file(callbacks.py callbacks.py)

# Add a custom command and a target that ALL depends on
message("${Python3_SITELIB}")
add_custom_command(OUTPUT 
    ${PROJECT_NAME}.py
    COMMAND ./generate-bindings.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(python_bindings ALL DEPENDS ${PROJECT_NAME}.py)

# Add top level native target
add_executable(${PROJECT_NAME} main.cpp)

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
        ${Python3_LIBRARIES}
        ${Boost_LIBRARIES})