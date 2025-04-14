file(GLOB_RECURSE boilerplate ../../Boilerplate/*.cpp)
message(${boilerplate})

add_executable(MainProject Cubes.cpp ${boilerplate})
target_include_directories(MainProject PRIVATE $ENV{VULKAN_SDK}/Include ../../ ../../build/_deps/stb-src)
target_link_libraries(MainProject PRIVATE Vulkan::Vulkan glm-header-only)
target_compile_definitions(MainProject PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_RADIANS VK_USE_PLATFORM_WIN32_KHR)
