#pragma once

#include "Types.h"

#include <glfw/glfw3.h>

#include <vulkan/vulkan.h>

class Swapchain {
public:
    void create(Context& context, GLFWwindow* window);
    void destroy(Context& context);

    void recreate(Context& context, GLFWwindow* window);

private:
};
