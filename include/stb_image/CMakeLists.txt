project(HELIX_STB_IMAGE)

set(HELIX_STB_IMAGE_HEADERS stb_image.h stb_image_write.h)

add_library(stb_image INTERFACE ${HELIX_STB_IMAGE_HEADERS})
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
