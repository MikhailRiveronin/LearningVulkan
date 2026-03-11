#pragma once

#include "Defines.h"
#include "Context.h"

#include <vulkan/vulkan.h>
#include <vector>

// struct CommandBufferManagerConfig {
//     u32 threadsPerFrame;
// };

class CommandBuffer {
public:
    VkCommandBuffer handle;


private:
};

class CommandBufferManager {
public:
    void Init(Context const& context);


private:
    u32 commandPoolsPerFrame;
    std::vector<VkCommandPool> commandPools;
    std::vector<VkCommandBuffer> commandBuffers;
};
