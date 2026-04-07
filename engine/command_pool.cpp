#include "command_pool.h"

#include "vulkan_struct_initializers.h"

namespace engine
{

Command_Pool::Command_Pool(Device const& device, u32 queue_family_index, u32 thread_index) : device(device), thread_index(thread_index)
{
    auto create_info = Vulkan_Struct_Initializers::command_pool_create_info(queue_family_index);
    handle = this->device.create_command_pool(create_info);
}

Command_Pool::~Command_Pool()
{
    primary_command_buffers.clear();
    secondary_command_buffers.clear();

    if (handle != VK_NULL_HANDLE)
    {
        device.destroy_command_pool(handle);
    }
}

Command_Buffer const& Command_Pool::request_command_buffer(Command_Pool const& command_pool, VkCommandBufferLevel level)
{
    switch (level)
    {
        case VK_COMMAND_BUFFER_LEVEL_PRIMARY:
            primary_command_buffers.emplace_back(command_pool, level);
            return primary_command_buffers.back();

        case VK_COMMAND_BUFFER_LEVEL_SECONDARY:
            secondary_command_buffers.emplace_back(command_pool, level);
            return secondary_command_buffers.back();

        default:
            assert(false);
    }
}

} // namespace engine
