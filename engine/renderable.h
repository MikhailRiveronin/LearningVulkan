#pragma once

#include "defines.h"

class Renderable
{
public:
    virtual float get_view_depth() const = 0;

};
