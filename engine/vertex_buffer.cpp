#include "vertex_buffer.h"

#include "render_system.h"
#include "vulkan_struct_initializers.h"
#include "glm/glm.hpp"

Vertex_Element::Vertex_Element(Buffer_Shared_Ptr buffer, u32 offset, Semantics semantics) : buffer(buffer), offset(offset), semantics(semantics)
{
}

Buffer_Shared_Ptr Vertex_Element::get_buffer() const
{
    return buffer;
}

u32 Vertex_Element::get_offset() const
{
    return offset;
}

Vertex_Element::Semantics Vertex_Element::get_semantics() const
{
    return semantics;
}

u32 Vertex_Element::get_size() const
{
    switch (semantics)
    {
        case Semantics::POSITION:
            return sizeof(glm::vec3);

        case Semantics::NORMAL:
            return sizeof(glm::vec3);

        case Semantics::TEX_COORD:
            return sizeof(glm::vec2);

        case Semantics::COLOR:
            return sizeof(glm::vec4);

        case Semantics::POSITION_U:
            return sizeof(glm::vec4);

        default:
            break;
    }

    return 0;
}

size_t Vertex_Declaration::get_element_count() const
{
    return elements.size();
}

Vertex_Element const& Vertex_Declaration::push_element(Vertex_Element const& elem)
{
    elements.push_back(elem);
    return elements.back();
}

Vertex_Buffer::Vertex_Buffer(u32 vertex_count, u32 vertex_size) : declaration(Vertex_Declaration()), vertex_count(vertex_count), vertex_size(vertex_size)
{
    auto create_info = Vulkan_Struct_Initializers::buffer_create_info(vertex_count * vertex_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    auto allocation_create_info = Vulkan_Struct_Initializers::allocation_create_info(VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
    VK_CHECK(vmaCreateBuffer(Render_System::get_instance()->context.allocator, &create_info, &allocation_create_info, &handle, &allocation, nullptr));
}

u32 Vertex_Buffer::get_vertex_count() const
{
    return vertex_count;
}

u32 Vertex_Buffer::get_vertex_size() const
{
    return vertex_size;
}
