cmake_minimum_required(VERSION 3.10) # Specify the minimum version of CMake required

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json for use with clangd

# Set the project name and version
project(Enzyme-OO-Example VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add an executable with the given sources
add_executable(base-code base-code.cpp)
add_executable(adept-code adept-code.cpp )

# Configure include directories
target_include_directories(base-code PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(adept-code PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/adept)
