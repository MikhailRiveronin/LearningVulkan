#include "buffer_manager.h"

#include "vertex_buffer.h"
#include "vulkan_struct_initializers.h"

Buffer_Shared_Ptr Buffer_Manager::create_vertex_buffer(u32 vertex_count, u32 vertex_size)
{
    return std::make_shared<Vertex_Buffer>(vertex_count, vertex_size);
}
