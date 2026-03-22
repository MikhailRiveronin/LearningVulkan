#pragma once

#include "defines.h"

#include "resource_manager.h"
#include "mesh.h"

class Mesh_Manager : public Resource_Manager
{
public:
    static Mesh_Manager* get_instance();

    std::shared_ptr<Mesh> load(std::string const& name);

private:

};
