#pragma once

#include "Defines.h"
#include "Structures.h"

class Swapchain {
public:
    void create(Globals& globals);
    void destroy(Globals const& globals);
    void recreate(Globals& globals);
};
