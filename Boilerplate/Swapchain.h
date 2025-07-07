#pragma once

#include "Defines.h"
#include "Structures.h"

class Swapchain {
public:
    void create(Context& globals);
    void destroy(Context const& globals);
};
