file(GLOB IMGUI_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_dx11.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)

add_library(ImGui STATIC ${IMGUI_SRC})

target_include_directories(ImGui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/backends
        ${CMAKE_SOURCE_DIR}/vendor/GLFW/include
        ${CMAKE_SOURCE_DIR}/vendor/GLAD/include
)

target_link_libraries(ImGui
    PUBLIC
        glfw
        GLAD
        opengl32
)
