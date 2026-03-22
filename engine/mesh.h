#pragma once

#include "defines.h"

#include "resource.h"

class Mesh : public Resource
{
public:
    std::string name;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec4> tangents;
    std::vector<glm::vec2> tex_coord;
    std::vector<glm::vec4> colors;
    std::vector<glm::u16vec4> joints;
    std::vector<glm::vec4> weights;
    std::vector<u32> indices;

    struct Sub_Mesh
    {
        std::string material_name;

        u32 index_count;
        u32 first_index;
        i32 vertex_offset;
    };

    std::vector<Sub_Mesh> sub_meshes;



    void load();

    void create_sub_mesh(std::string const& name, );



private:


};
