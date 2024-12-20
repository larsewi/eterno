cmake_minimum_required(VERSION 3.6)
project(Eterno VERSION 0.1.0 DESCRIPTION "A basic game")

# Set additional package information
set(PACKAGE_BUGREPORT "https://github.com/larsewi/eterno/issues")
set(PACKAGE_URL "https://github.com/larsewi/eterno")

# Configure a header file to pass some settings to the source code
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in  # Template file
    ${CMAKE_CURRENT_BINARY_DIR}/src/config.h # Output file
)

# Find SDL3 package
find_package(SDL3 REQUIRED)

# Add the include directory to your project
include_directories(${RSYNC_INCLUDE_DIR})

# List sources
set(SOURCES
    src/main.c
    src/utils.c
    src/logger.c
    src/game.c
    src/game_object.c
    src/player.c
)

# Add your executable or library
add_executable(eterno ${SOURCES})

# Link SDL3 to your target
target_link_libraries(eterno PRIVATE SDL3::SDL3)
