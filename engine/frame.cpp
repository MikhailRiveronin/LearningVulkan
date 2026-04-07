#include "frame.h"

namespace engine
{

Frame(Device const& device, u32 thread_count) : device(device), thread_count(thread_count), descriptor_pools(thread_count), descriptor_sets(thread_count)
{
}

Command_Pool const& Frame::get_command_pool(Queue const& queue, u32 thread_index = 0)
{
    assert(thread_index < thread_count);

    auto& command_pools = get_command_pools(queue);
    auto it = std::find(command_pools.begin(), command_pools.end(), [&thread_index](Command_Pool const& command_pool)
    {
        return command_pool.thread_index == thread_index;
    });
    assert(it != command_pools.end());

    return *it;
}

std::vector<Command_Pool> const& Frame::get_command_pools(Queue const& queue)
{
    auto it = command_pools.find(queue.family_index);
    if (it != command_pools.end())
    {
        assert(!it->second.empty());
        return it->second;
    }

    bool inserted = false;
    std::tie(it, inserted) = command_pools.emplace(queue.family_index, std::vector<Command_Pool>());
    if (!inserted)
    {
        throw std::runtime_error("Failed to insert command pool");
    }

    for (u32 i = 0; i < thread_count; ++i)
    {
        it->second.emplace_back(device, queue.family_index, i);
    }

    return it->second;
}

} // namespace engine
