#include "mesh_manager.h"

#include "mesh.h"

std::shared_ptr<Resource> Mesh_Manager::create_resource()
{
    return std::make_shared<Mesh>();
}
