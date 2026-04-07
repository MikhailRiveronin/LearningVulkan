#pragma once

#include "defines.h"

#include "device.h"
#include "command_buffer.h"

namespace engine
{

class Command_Pool
{
public:
    VkCommandPool handle;
    Device& device;
    u32 thread_index = 0;

public:
    Command_Pool(Device const& device, u32 queue_family_index, u32 thread_index = 0);
    ~Command_Pool();

    Command_Buffer const& request_command_buffer(Command_Pool const& command_pool, VkCommandBufferLevel level);


private:
    std::vector<Command_Buffer> primary_command_buffers;
    std::vector<Command_Buffer> secondary_command_buffers;
};

} // namespace engine
