cmake_minimum_required(VERSION 3.15)
project(DSLToCpp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Flex and Bison paths
set(FL_LIBRARY "")
set(FLEX_LIBRARIES "")
set(FLEX_EXECUTABLE "C:/Users/dell/Downloads/win_flex_bison-2.5.25/win_flex.exe")

find_package(BISON REQUIRED)

# Generate Parser with UNIQUE file names to avoid Windows header conflict
BISON_TARGET(MyParser
    ${CMAKE_SOURCE_DIR}/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/dsl_parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/dsl_parser.hpp
)

# Flex manual generation (scanner.cpp)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp
    COMMAND ${FLEX_EXECUTABLE} -o ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp ${CMAKE_SOURCE_DIR}/scanner.l
    DEPENDS ${CMAKE_SOURCE_DIR}/scanner.l
    COMMENT "Running Flex manually"
)

add_custom_target(ScannerGen DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Prevent Flex from trying to include unistd.h on Windows
add_definitions(-DYY_NO_UNISTD_H)

# Add WIN32_LEAN_AND_MEAN to reduce Windows header conflicts
add_definitions(-DWIN32_LEAN_AND_MEAN)

# Gather source files
file(GLOB_RECURSE PROJECT_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.h"
)

# Build executable
add_executable(dsl_to_cpp
    ${PROJECT_SOURCES}
    ${BISON_MyParser_OUTPUTS}
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp
)

# Ensure scanner is generated first
add_dependencies(dsl_to_cpp ScannerGen)