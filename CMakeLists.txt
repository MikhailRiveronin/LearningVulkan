cmake_minimum_required(VERSION 3.24)

project(LearningVulkan LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)
add_subdirectory(ThirdParty)
add_subdirectory(Samples)
