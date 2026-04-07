#pragma once

#include "Defines.h"
#include "Context.h"


#include "command_pool.h"
#include "device.h"


#include <vulkan/vulkan.h>
#include <vector>

// struct CommandBufferManagerConfig {
//     u32 threadsPerFrame;
// };

namespace engine
{

class Command_Buffer
{
public:
    VkCommandBuffer handle = VK_NULL_HANDLE;
    Device& device;
    Command_Pool& command_pool;


public:
    Command_Buffer(Command_Pool const& command_pool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    ~Command_Buffer();

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

} // namespace engine
