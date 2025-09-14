cmake_minimum_required(VERSION 3.15)

set(HELIX_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(HELIX_VERSION 0.1.0)
set(HELIX_DESC "A simple sprite editor")

project(
  HELIX_CMAKE
  VERSION ${HELIX_VERSION}
  DESCRIPTION ${HELIX_DESC}
  LANGUAGES CXX)

# C++ Standards
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Directories
add_subdirectory(src)
add_subdirectory(include)
