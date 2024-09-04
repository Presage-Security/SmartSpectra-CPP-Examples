set(PROJECT_NAME SmartSpectraExamples)

# === Project basic configuration & Language Standards ===
cmake_minimum_required(VERSION 3.22.1)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to build with")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# === Dependencies ===
find_package(SmartSpectra 0.4.2 REQUIRED)

# === Subdirectories ===
add_subdirectory(image_file_folder_continuous_example)
add_subdirectory(grpc_continuous_example)
add_subdirectory(rest_spot_example)
add_subdirectory(minimal_rest_spot_example)


