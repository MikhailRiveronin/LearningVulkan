#pragma once

#include "defines.h"

#include "math_utils.h"
#include "resource.h"


struct Sub_Mesh
{
    i32 vertex_offset;
    u32 material_index;

    struct LOD
    {
        u32 first_index;
        u32 index_count;
    };
    std::vector<LOD> lods;
};

class Mesh : public Resource
{
public:
    std::string name;
    std::vector<Sub_Mesh> sub_meshes;

    // std::vector<glm::vec3> positions;
    // std::vector<glm::vec3> normals;
    // std::vector<glm::vec4> tangents;
    // std::vector<glm::vec2> tex_coord;
    // std::vector<glm::vec4> colors;
    // std::vector<glm::u16vec4> joints;
    // std::vector<glm::vec4> weights;
    // std::vector<u32> indices;

    




    // void load();

    // void create_sub_mesh(std::string const& name, );



private:


};

enum class Material_Flags
{
    CAST_SHADOW = 0x1,
    RECEIVE_SHADOW = 0x2,
    TRANSPARENT = 0x4
};

struct Material
{
    glm::vec4 emissive_factor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 base_color_factor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float roughness = 1.0f;
    float transparency_factor = 1.0f;
    float alpha_test = 0.0f;
    float metallic_factor = 0.0f;

    // index into MeshData::textureFiles
    i32 base_color_texture = -1;
    i32 emissive_texture = -1;
    i32 normal_texture = -1;
    i32 opacity_texture = -1;
    u32 flags = Material_Flags::CAST_SHADOW | Material_Flags::RECEIVE_SHADOW;
};



class Mesh_Data
{
public:
    struct
    {
        u32 mesh_count = 0;
        u32 vertex_data_size_in_bytes = 0;
        u32 index_data_size_in_bytes = 0;
    } header;

    std::vector<u32> index_data;
    std::vector<u8> vertex_data;
    std::vector<Mesh> meshes;

    std::vector<Material> materials;
    std::vector<std::string> texture_file_names;

    void save_to_file(std::string const& filename);

    void save_materials_to_file(char const* filename);
    void load_materials_from_file(char const* filename);
};

void load_mesh_data(std::string const& filename, Mesh_Data& mesh_data);
