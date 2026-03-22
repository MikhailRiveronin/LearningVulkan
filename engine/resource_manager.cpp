#include "resource_manager.h"

Resource_Manager::Resource_Manager(Device* device) : device(device)
{
}

void Resource_Manager::add_resource(std::string const& name, std::shared_ptr<Resource> resource)
{
    resources.emplace(name, resource);
}
