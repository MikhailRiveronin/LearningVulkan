#pragma once

#include "defines.h"

#include "mesh.h"
#include "renderable.h"

class Submesh_Instance : public Renderable
{
public:
    Submesh_Instance(Mesh::Sub_Mesh const& submesh);

    float get_view_depth() const override;

};

class Mesh_Instance
{
public:
    std::string name;
    std::shared_ptr<Mesh> mesh;
    std::vector<Submesh_Instance> submesh_instances;

    Mesh_Instance(std::string const& name, std::string const& mesh_name);

};
