#pragma once

#include "defines.h"

#include "pass.h"
#include "renderable.h"

enum class Render_Queue_Group_Type
{
    SKYBOX,
    MAIN,
    UI,
    ENUM_COUNT
};

class Render_Queue_Group
{
public:
    void add_renderable(Renderable* renderable);

};

class Render_Queue
{
public:
    Render_Queue_Group groups[static_cast<std::size_t>(Render_Queue_Group_Type::ENUM_COUNT)];

    void add_renderable(Renderable* renderable, Render_Queue_Group_Type group_type);


};

class Opaque_Renderable_Collection;
class Transparent_Renderable_Collection;

class Renderable_Visitor
{
public:
    virtual void visit(Opaque_Renderable_Collection* collection) = 0;
    virtual void visit(Transparent_Renderable_Collection* collection) = 0;
};

class Renderable_Collection
{
public:
    virtual void accept_visitor(Renderable_Visitor* visitor) = 0;

};

// Grouped by render pass
class Opaque_Renderable_Collection : public Renderable_Collection
{
public:
    std::unordered_map<Pass*, std::vector<Renderable*>> renderables;

    void accept_visitor(Renderable_Visitor* visitor) override;

};

// Sorted by distance from camera
class Transparent_Renderable_Collection : public Renderable_Collection
{
public:
    std::unordered_map<Pass*, std::map<float, Renderable*>> renderables;

    void accept_visitor(Renderable_Visitor* visitor) override;
};
