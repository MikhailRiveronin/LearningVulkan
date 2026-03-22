#pragma once

#include "defines.h"

#include "device.h"
#include "resource.h"

class Resource_Manager
{
public:
    Device* device;
    std::unordered_map<std::string, std::shared_ptr<Resource>> resources;

    Resource_Manager(Device* device);

    void add_resource(std::string const& name, std::shared_ptr<Resource> resource);

protected:

private:

};
