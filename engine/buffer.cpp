#include "buffer.h"

#include "render_system.h"

Buffer::~Buffer()
{
    vmaDestroyBuffer(Render_System::get_instance()->context.allocator, handle, allocation);
}
