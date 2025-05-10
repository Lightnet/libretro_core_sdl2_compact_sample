cmake_minimum_required(VERSION 3.16)
project(my_libretro_core LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include FetchContent module
include(FetchContent)

# Fetch Libretro common headers
FetchContent_Declare(
    libretro_common
    GIT_REPOSITORY https://github.com/libretro/libretro-common.git
    GIT_TAG master
)
FetchContent_MakeAvailable(libretro_common)

# Fetch SDL3 (required by SDL2-compat)
FetchContent_Declare(
    sdl3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.12
)
FetchContent_MakeAvailable(sdl3)

# Fetch SDL2-compat
FetchContent_Declare(
    sdl2_compat
    GIT_REPOSITORY https://github.com/libsdl-org/sdl2-compat.git
    GIT_TAG release-2.32.56
)
FetchContent_MakeAvailable(sdl2_compat)

# Define the shared library (Libretro core)
add_library(my_libretro_core SHARED src/main.c)

# Include directories
target_include_directories(my_libretro_core PRIVATE
    ${libretro_common_SOURCE_DIR}/include
    ${sdl2_compat_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(my_libretro_core PRIVATE
    SDL2::SDL2
    SDL2::SDL2main  # Use SDL2main for SDL2-compat
)

# Ensure the output is a .dll with no "lib" prefix
set_target_properties(my_libretro_core PROPERTIES
    PREFIX ""
    OUTPUT_NAME "my_libretro_core"
)

# Platform-specific settings for Windows
if(WIN32)
    # Ensure the DLL is compatible with Windows GUI applications
    target_link_libraries(my_libretro_core PRIVATE SDL2::SDL2main)
endif()