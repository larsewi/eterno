cmake_minimum_required(VERSION 3.6)
project(eterno C)

# Find SDL3 package
find_package(SDL3 REQUIRED)

# Add the include directory to your project
include_directories(${RSYNC_INCLUDE_DIR})

# Add your executable or library
add_executable(eterno src/main.c)

# Link SDL3 to your target
target_link_libraries(eterno PRIVATE SDL3::SDL3)
