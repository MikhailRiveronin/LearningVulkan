#include "mesh.h"

#include "utils.h"

void Mesh_Data::save_to_file(std::string const& filename)
{
    FILE* file = fopen(filename.c_str(), "wb");
    if (!file)
    {
        throw std::runtime_error("Failed to open file");
    }

    header.mesh_count = meshes.size();
    header.vertex_data_size_in_bytes = vertex_data.size();
    header.index_data_size_in_bytes = index_data.size() * sizeof(u32);

    fwrite(&header, 1, sizeof(header), file);
    fwrite(meshes.data(), sizeof(meshes[0]), header.mesh_count, file);
    fwrite(vertex_data.data(), 1, header.vertex_data_size_in_bytes, file);
    fwrite(index_data.data(), 1, header.vertex_data_size_in_bytes, file);
    fclose(file);
}

void Mesh_Data::save_materials_to_file(char const* filename)
{
    FILE* file = fopen(filename, "wb");
    if (!file)
    {
        // TODO: Handle error
    }

    u32 material_count = materials.size();
    u32 materials_size_in_bytes = material_count * sizeof(Material);
    fwrite(&material_count, 1, sizeof(material_count), file);
    fwrite(&materials_size_in_bytes, 1, sizeof(materials_size_in_bytes), file);
    fwrite(materials.data(), sizeof(Material), material_count, file);

    save_strings(file, texture_file_names);
    fclose(file);
}

void Mesh_Data::load_materials_from_file(char const* filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        // TODO: Handle error
    }

    u32 material_count;
    u32 materials_size_in_bytes;
    fread(&material_count, 1, sizeof(material_count), file);
    fread(&materials_size_in_bytes, 1, sizeof(materials_size_in_bytes), file);

    materials.resize(material_count);
    fread(materials.data(), 1, materials_size_in_bytes, file);

    load_strings(file, texture_file_names);
    fclose(file);
}

void load_mesh_data(std::string const& filename, Mesh_Data& mesh_data)
{
    FILE* file = fopen(filename.c_str(), "rb");
    if (!file)
    {
        throw std::runtime_error("Failed to open file");
    }

    if (fread(&mesh_data.header, 1, sizeof(mesh_data.header), file) != sizeof(mesh_data.header))
    {
        throw std::runtime_error("Failed to read mesh data header");
    }

    mesh_data.meshes.resize(mesh_data.header.mesh_count);
    if (fread(mesh_data.meshes.data(), sizeof(Mesh), mesh_data.header.mesh_count, file) != mesh_data.header.mesh_count)
    {
        throw std::runtime_error("Failed to read mesh data meshes");
    }

    mesh_data.vertex_data.resize(mesh_data.header.vertex_data_size_in_bytes);
    if (fread(mesh_data.vertex_data.data(), 1, mesh_data.header.vertex_data_size_in_bytes, file) != mesh_data.header.vertex_data_size_in_bytes)
    {
        throw std::runtime_error("Failed to read mesh data vertex data");
    }

    mesh_data.index_data.resize(mesh_data.header.index_data_size_in_bytes / sizeof(u32));
    if (fread(mesh_data.index_data.data(), 1, mesh_data.header.index_data_size_in_bytes, file) != mesh_data.header.index_data_size_in_bytes)
    {
        throw std::runtime_error("Failed to read mesh data index data");
    }
}
