file(GLOB_RECURSE boilerplate
    ../../Boilerplate/*.cpp
    ../../build/_deps/imgui-src/imgui.cpp
    ../../build/_deps/imgui-src/imgui_demo.cpp
    ../../build/_deps/imgui-src/imgui_draw.cpp
    ../../build/_deps/imgui-src/imgui_tables.cpp
    ../../build/_deps/imgui-src/imgui_widgets.cpp
    ../../build/_deps/imgui-src/imgui_impl_win32.cpp
    ../../build/_deps/imgui-src/imgui_impl_vulkan.cpp)
list(REMOVE_ITEM boilerplate D:/Projects/LearningVulkan/Samples/Boxes/../../Boilerplate/GltfModel.cpp D:/Projects/LearningVulkan/Samples/Boxes/../../Boilerplate/tiny_gltf.cpp)
message(${boilerplate})

set(sample_name Boxes)
message(${sample_name})

add_executable(${sample_name} Boxes.cpp ${boilerplate})
target_include_directories(${sample_name} PRIVATE
    $ENV{VULKAN_SDK}/Include
    ../../
    ../../build/_deps/stb-src
    ../../build/_deps/tinygltf-src
    ../../build/_deps/imgui-src)
target_link_libraries(${sample_name} PRIVATE Vulkan::Vulkan Vulkan::shaderc_combined glm-header-only)
target_compile_definitions(${sample_name} PRIVATE VK_USE_PLATFORM_WIN32_KHR)

set(shader_spv_dir ${CMAKE_BINARY_DIR}/Shaders)
message(${shader_spv_dir})
file(MAKE_DIRECTORY ${shader_spv_dir}/${sample_name})
message(${shader_spv_dir}/${sample_name})

add_custom_command(TARGET ${sample_name} POST_BUILD
    COMMAND $ENV{VULKAN_SDK}/Bin/glslc.exe Shaders/Cube.vert -o ${shader_spv_dir}/${sample_name}/CubeVertex.spv
    COMMAND $ENV{VULKAN_SDK}/Bin/glslc.exe Shaders/Cube.frag -o ${shader_spv_dir}/${sample_name}/CubeFragment.spv
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    BYPRODUCTS ${shader_spv_dir}/${sample_name}/CubeVertex.spv ${shader_spv_dir}/${sample_name}/CubeFragment.spv
)

add_custom_command(TARGET ${sample_name} POST_BUILD
    COMMAND $ENV{VULKAN_SDK}/Bin/glslc.exe Shaders/LightCube.vert -o ${shader_spv_dir}/${sample_name}/LightCubeVertex.spv
    COMMAND $ENV{VULKAN_SDK}/Bin/glslc.exe Shaders/LightCube.frag -o ${shader_spv_dir}/${sample_name}/LightCubeFragment.spv
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    BYPRODUCTS ${shader_spv_dir}/${sample_name}/LightCubeVertex.spv ${shader_spv_dir}/${sample_name}/LightCubeFragment.spv)

add_custom_command(TARGET ${sample_name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${shader_spv_dir}/${sample_name} ${CMAKE_BINARY_DIR}/Samples/${sample_name}/Shaders)

add_custom_command(TARGET ${sample_name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Textures ${CMAKE_BINARY_DIR}/Samples/${sample_name}/Textures)

add_custom_command(TARGET ${sample_name} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Textures ${CMAKE_BINARY_DIR}/Textures)
