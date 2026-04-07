#pragma once

#include "defines.h"

#include "device.h
#include "command_pool.h"
#include "descriptor_pool.h"
#include "descriptor_set.h"
#include "queue.h"

namespace engine
{

class Frame
{
public:
    Device& device;

public:
    Frame(Device const& device, u32 frame_count);

    Command_Pool const& get_command_pool(Queue const& queue, u32 thread_index = 0);


private:

    u32 thread_count = 0;

    std::unordered_map<u32, std::vector<Command_Pool>> command_pools; // per queue per thread
    std::vector<Descriptor_Pool> descriptor_pools; // per thread
    std::vector<Descriptor_Set> descriptor_sets; // per thread

private:
    std::vector<Command_Pool> const& get_command_pools(Queue const& queue);


};

} // namespace engine
