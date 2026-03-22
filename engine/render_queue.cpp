#include "render_queue.h"



void Render_Queue::add_renderable(Renderable* renderable, Render_Queue_Group_Type group_type)
{
    groups[static_cast<std::size_t>(group_type)].add_renderable(renderable);
}


void Opaque_Renderable_Collection::accept_visitor(Renderable_Visitor* visitor)
{
    visitor->visit(this);
}

void Transparent_Renderable_Collection::accept_visitor(Renderable_Visitor* visitor)
{
    visitor->visit(this);
}


