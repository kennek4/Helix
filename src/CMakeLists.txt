project(
  HELIX_EXEC
  VERSION ${HELIX_VERSION}
  DESCRIPTION "A simple sprite editor")

set(DEBUG_DIR ${HELIX_ROOT}/build/Debug/Helix_Debug)
set(RELEASE_DIR ${HELIX_ROOT}/build/Release/Helix_Release)

add_executable(Helix main.cpp Helix.h)
set_target_properties(
  Helix
  PROPERTIES OUTPUT_NAME Helix_v${HELIX_VERSION}
             RUNTIME_OUTPUT_DIRECTORY_DEBUG ${HELIX_ROOT}/build/Debug/Helix
             RUNTIME_OUTPUT_DIRECTORY_RELEASE ${HELIX_ROOT}/build/Release/Helix)
target_link_libraries(Helix HELIX_LIB)
target_link_directories(Helix PRIVATE ${HELIX_ROOT}/src)

add_custom_command(
  TARGET Helix
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${HELIX_ROOT}/assets/checkerboard.bmp
          ${HELIX_ROOT}/build/Debug/Helix/assets/checkerboard.bmp)
