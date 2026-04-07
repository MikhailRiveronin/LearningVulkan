#include "command_buffer.h"

#include "Utils.h"
#include "vulkan_struct_initializers.h"


namespace engine
{

Command_Buffer::Command_Buffer(Command_Pool const& command_pool, VkCommandBufferLevel level) : command_pool(command_pool)
{
    auto allocate_info = Vulkan_Struct_Initializers::command_buffer_allocate_info((this->command_pool).handle, level);
    handle = device.allocate_command_buffer(allocate_info);
}

Command_Buffer::~Command_Buffer()
{
    if (handle != VK_NULL_HANDLE)
    {
        device.free_command_buffer(command_pool.handle, handle);
    }
}



void CommandBufferManager::Init(Context const& context)
{
    commandPools.resize(context.taskThreadCount);
    for (auto commandPool : commandPools) {
        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // TODO: check if needed
        createInfo.queueFamilyIndex = context.device.queues.graphics.familyIndex;
        VK_CHECK(vkCreateCommandPool(context.device.handle, &createInfo, &context.allocationCallbacks, &commandPool));
    }

    commandBuffers.resize(commandPools.size() * context.framesInFlight);
    for (u32 i = 0; i < commandBuffers.size(); i += context.framesInFlight) {
        u32 poolIndex = i / context.framesInFlight;
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.pNext = nullptr;
        allocateInfo.commandPool = commandPools[poolIndex];
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = FRAMES_IN_FLIGHT;
        VK_CHECK(vkAllocateCommandBuffers(context.device.handle, &allocateInfo, &commandBuffers[i]));
    }
}

}
