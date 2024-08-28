set(PROJECT_NAME SmartSpectraExamples)

# === Project basic configuration & Language Standards ===
cmake_minimum_required(VERSION 3.22.1)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to build with")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# === Dependencies ===
find_package(SmartSpectra REQUIRED)

# === Subdirectories ===
add_subdirectory(cpp_file_example)
add_subdirectory(cpp_grpc_example)
add_subdirectory(cpp_web_example)


