cmake_minimum_required(VERSION 3.13.4)

project(LearningVulkan LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
include(AddDependencies)

add_executable(${PROJECT_NAME} main.cpp)
