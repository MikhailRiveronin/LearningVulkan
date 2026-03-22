#include "renderer.h"

#include "vulkan_struct_initializers.h"

void Renderer::begin_frame()
{
    VK_CHECK(vkWaitForFences(device, 1, &per_frame_resources[current_frame_index].in_flight_fence, VK_TRUE, UINT64_MAX));
    VK_CHECK(vkResetFences(device, 1, &per_frame_resources[current_frame_index].in_flight_fence));

    VK_CHECK(vkResetCommandBuffer(per_frame_resources[current_frame_index].command_buffer, 0));

    auto command_buffer_begin_info = Vulkan_Struct_Initializers::command_buffer_begin_info();
    VK_CHECK(vkBeginCommandBuffer(per_frame_resources[current_frame_index].command_buffer, &command_buffer_begin_info));

    auto viewport = Vulkan_Struct_Initializers::viewport(swapchain.extent);
    auto scissor = Vulkan_Struct_Initializers::scissor(swapchain.extent);
    vkCmdSetViewport(per_frame_resources[current_frame_index].command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(per_frame_resources[current_frame_index].command_buffer, 0, 1, &scissor);

    VK_CHECK(vkAcquireNextImageKHR(device, swapchain.handle, UINT64_MAX, per_frame_resources[current_frame_index].image_available_semaphore, VK_NULL_HANDLE, &swapchain.next_image_index));
}

void Renderer::end_frame()
{
    VK_CHECK(vkEndCommandBuffer(per_frame_resources[current_frame_index].command_buffer));

    auto wait_dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    auto submit_info = Vulkan_Struct_Initializers::submit_info(&per_frame_resources[current_frame_index].image_available_semaphore, &wait_dst_stage_mask, &per_frame_resources[current_frame_index].command_buffer, &per_frame_resources[current_frame_index].render_finished_semaphore);
    VK_CHECK(vkQueueSubmit(queues.graphics.handle, 1, &submit_info, per_frame_resources[current_frame_index].in_flight_fence));

    auto present_info = Vulkan_Struct_Initializers::present_info(&per_frame_resources[current_frame_index].render_finished_semaphore, &swapchain.handle, &swapchain.next_image_index);
    VK_CHECK(vkQueuePresentKHR(queues.present.handle, &present_info));

    current_frame_index = (current_frame_index + 1) % frames_in_flight;
}
