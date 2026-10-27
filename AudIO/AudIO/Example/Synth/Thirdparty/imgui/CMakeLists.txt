cmake_minimum_required(VERSION 3.0.0)
project(Imgui VERSION 0.1.0)
# find_library(glfw required)
# add_subdirectory(glfw-3.3.3)
set(_imgui_Sources
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    imconfig.h
    imgui.h
    imgui_internal.h
    imgui_tables.cpp
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
)
include_directories(Imgui
    ${CMAKE_CURRENT_SOURCE_DIR}
    # examples/libs/glfw/include
    # backends
    # examples/example_glfw_opengl2/
)

add_library(Imgui
# examples/example_glfw_opengl2/main.cpp
# backends/imgui_impl_opengl2.cpp 
# backends/imgui_impl_glfw.cpp
${_imgui_Sources}
)

# target_link_libraries(Imgui glfw gdi32 opengl32 imm32
# )
