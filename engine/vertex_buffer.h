#pragma once

#include "defines.h"
#include "buffer.h"

class Vertex_Element
{
public:
    enum class Semantics
    {
        POSITION, // vec3
        NORMAL, // vec3
        TEX_COORD, // vec2
        COLOR, // vec4
        POSITION_U // vec4: x, y, z - position, w - u-texture coordinate
    };

    Vertex_Element(Buffer_Shared_Ptr buffer, u32 offset, Semantics semantics);

    Buffer_Shared_Ptr get_buffer() const;
    u32 get_offset() const;
    Semantics get_semantics() const;
    u32 get_size() const;

private:
    Buffer_Shared_Ptr buffer;
    u32 offset; // in vertex structure
    Semantics semantics;
};

class Vertex_Declaration
{
public:
    using Vertex_Element_List = std::list<Vertex_Element>;

    size_t get_element_count() const;
    Vertex_Element const& push_element(Vertex_Element const& elem);

private:
    Vertex_Element_List elements;
};

class Vertex_Buffer : public Buffer
{
public:
    Vertex_Buffer(u32 vertex_count, u32 vertex_size);

    u32 get_vertex_count() const;
    u32 get_vertex_size() const;

private:
    Vertex_Declaration declaration;
    u32 vertex_count;
    u32 vertex_size;
};
