project(
  HELIX_EXEC
  VERSION ${HELIX_VERSION}
  DESCRIPTION "A simple sprite editor")

add_executable(Helix Helix.cpp)
set_target_properties(Helix PROPERTIES OUTPUT_NAME Helix_v${HELIX_VERSION})
target_link_libraries(Helix HELIX_LIB)
target_link_directories(Helix PRIVATE ${HELIX_ROOT}/src)
