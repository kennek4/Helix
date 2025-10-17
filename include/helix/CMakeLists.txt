project(HELIX_INCLUDE_CMAKE)

include(core/CMakeLists.txt)
include(events/CMakeLists.txt)
include(gui/CMakeLists.txt)
include(utility/CMakeLists.txt)

set(HELIX_HEADER ${HELIX_CORE_HEADER} ${HELIX_EVENTS_HEADER}
                 ${HELIX_GUI_HEADER} ${HELIX_UTILITY_HEADER})
set(HELIX_SRC ${HELIX_CORE_SRC} ${HELIX_EVENTS_SRC} ${HELIX_GUI_SRC})
list(APPEND HELIX_SRC ${HELIX_HEADER})

# SDL
cmake_policy(SET CMP0072 NEW)

# Create an option to switch between a system sdl library and a vendored SDL
# library
option(SDL_VENDORED "Use vendored libraries" OFF)
if(SDL_VENDORED)
  # This assumes you have added SDL as a submodule in vendored/SDL
  add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
  # 1. Look for a SDL3 package,
  # 2. look for the SDL3-shared component, and
  # 3. fail if the shared component cannot be found.
  find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

set(SDL_SHARED OFF)
set(SDL_STATIC ON)

add_library(HELIX_LIB STATIC ${HELIX_SRC})

if(UNIX AND NOT APPLE)
  target_compile_definitions(HELIX_LIB PUBLIC HELIX_PLATFORM_LINUX)
elseif(WIN32)
  target_compile_definitions(HELIX_LIB PUBLIC HELIX_PLATFORM_WINDOWS)
endif()

target_precompile_headers(HELIX_LIB PRIVATE HLX_pch.h)

target_include_directories(
  HELIX_LIB
  PUBLIC ${HELIX_ROOT}/include/helix/core
  PUBLIC ${HELIX_ROOT}/include/helix/core/pixelgrid
  PUBLIC ${HELIX_ROOT}/include/helix/core/tools
  PUBLIC ${HELIX_ROOT}/include/helix/core/window
  PUBLIC ${HELIX_ROOT}/include/helix/core/renderer
  PUBLIC ${HELIX_ROOT}/include/helix/events
  PUBLIC ${HELIX_ROOT}/include/helix/gui
  PUBLIC ${HELIX_ROOT}/include/helix/utility)
target_link_libraries(HELIX_LIB imgui SDL3::SDL3 stb_image)
