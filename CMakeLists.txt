cmake_minimum_required(VERSION 3.24)

project(LearningVulkan LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)

find_package(Vulkan COMPONENTS shaderc_combined)
add_subdirectory(ThirdParty)
add_subdirectory(Samples)
