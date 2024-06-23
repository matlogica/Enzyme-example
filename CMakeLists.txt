cmake_minimum_required(VERSION 3.10) # Specify the minimum version of CMake required

# Set the project name and version
project(Enzyme-OO-Example VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add an executable with the given sources
add_executable(base-code base-code.cpp)

# Configure include directories
target_include_directories(base-code PRIVATE ${PROJECT_SOURCE_DIR})
