cmake_minimum_required(VERSION 3.5)
project(galaga
  LANGUAGES C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Use below for Analysis
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FORTIFY_SOURCE=2 -g3 -O2 -Wall -Werror -std=c17 -pedantic")
# Use below for Debuggin
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -g3 -fno-omit-frame-pointer -fsanitize=address")
# TODO Testing, Acceptance Testing, Release flags

# Create an option to switch between a system sdl library and a vendored SDL library
option(GALAGA_VENDORED "Use vendored libraries" OFF)
if(GALAGA_VENDORED)
    # This assumes you have added SDL as a submodule in vendored/SDL
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL3 package,
    # 2. look for the SDL3-shared component, and
    # 3. fail if the shared component cannot be found.
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
    find_package(SDL3_image REQUIRED CONFIG REQUIRED COMPONENTS SDL3_image-shared)
endif()

# Create your game executable target as usual
add_executable(galaga WIN32 main.c)

# Link to the actual SDL3 library.
target_link_libraries(galaga PRIVATE
  SDL3::SDL3
  SDL3_image::SDL3_image
)
