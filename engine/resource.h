#pragma once

#include "defines.h"

#include "device.h"
#include "resource_manager.h"

class Resource
{
public:

Resource();


protected:
    Resource_Manager* creator;


private:
    enum class Loading_State
    {
        UNLOADED,
        LOADING,
        LOADED,
        PREPARING,
        PREPARED
    };

    std::shared_ptr<Loader> loader;

    virtual void load_impl() = 0;

};
