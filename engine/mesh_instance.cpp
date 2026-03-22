#include "mesh_instance.h"

#include "mesh_manager.h"

Mesh_Instance::Mesh_Instance(std::string const& name, std::string const& mesh_name) : name(name)
{
    std::shared_ptr<Mesh> mesh;
    auto it = Mesh_Manager::get_instance()->resources.find(mesh_name);
    if (it != Mesh_Manager::get_instance()->resources.end())
    {
        mesh = std::dynamic_pointer_cast<Mesh>(it->second);
    }
    else
    {
        // TODO: try to load mesh
    }

    submesh_instances.resize(mesh->sub_meshes.size());
    for (u32 i = 0; i < mesh->sub_meshes.size(); ++i)
    {
        submesh_instances[i] = Submesh_Instance(mesh->sub_meshes[i]);
    }
}
