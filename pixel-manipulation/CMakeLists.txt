cmake_minimum_required(version 3.0.2)

# Project name
project(pixel-manip VERSION 1.0)

# Finds and loads the settings from the OPENCV package
find_package(OpenCV REQUIRED)

# Add header file directories to the project
target_include_directories(${OpenCV_INCLUDE_DIRS})
target_include_directories(pixel-manip PUBLIC "${PROJECT_BINARY_DIR}")

# Adding the executable to be created
add_executable(pixel-manip src/pixel-manip.cpp)

# Linking the created executable to the OpenCV libraries.
target_link_libraries(pixel-manip ${OpenCV_LIBS})

# Exporting the compile_commands.json so we can use eglot
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
