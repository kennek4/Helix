project(
  HELIX_TESTS
  VERSION ${HELIX_VERSION}
  DESCRIPTION "Test suite for Helix")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/9706f75b8f91c52a3840cf5d878a7f37ea10ef00.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(HelixEventSystemTest HLX_EventSystemTest.cpp)
set_target_properties(HelixEventSystemTest PROPERTIES OUTPUT_NAME
                                                      HelixEventSystemTest)
target_link_libraries(HelixEventSystemTest HELIX_LIB GTest::gtest_main)
target_link_directories(HelixEventSystemTest PRIVATE ${HELIX_ROOT}/tests)

include(GoogleTest)
gtest_add_tests(HelixEventSystemTest SOURCES HLX_EventSystemTest.cpp)
