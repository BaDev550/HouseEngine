file(GLOB IMGUI_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp"
)

add_library(ImGui STATIC ${IMGUI_SRC})

target_include_directories(ImGui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/backends
        ${CMAKE_SOURCE_DIR}/vendor/GLFW/include
        ${VULKAN_INCLUDE_DIR}
)

target_link_libraries(ImGui
    PUBLIC
        glfw
        ${VULKAN_LIB}
)
