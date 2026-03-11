#pragma once

#include "defines.h"
#include "resource.h"
#include "glm/glm.hpp"
#include <list>
#include <variant>


class Vertex_Data
{
public:


private:
    std::shared_ptr<Vertex_Declaration> vertex_declaration;


    u32 vertex_start;
    u32 vertex_count;
};

class Mesh_Resource : public Resource
{


private:
    void load_impl() override;
};
