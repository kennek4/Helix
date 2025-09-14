project(HELIX_IMGUI)

set(HELIX_IMGUI_HEADERS
    backends/imgui_impl_sdl3.h
    backends/imgui_impl_sdlrenderer3.h
    imconfig.h
    imgui.h
    imgui_internal.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h)

set(HELIX_IMGUI_SRC
    backends/imgui_impl_sdl3.cpp
    backends/imgui_impl_sdlrenderer3.cpp
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp)

list(APPEND HELIX_IMGUI_SRC ${HELIX_IMGUI_HEADERS})

add_library(imgui ${HELIX_IMGUI_SRC})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
