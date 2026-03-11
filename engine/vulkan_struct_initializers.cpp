#include "vulkan_struct_initializers.h"

VkDebugUtilsMessengerCreateInfoEXT Vulkan_Struct_Initializers::debug_utils_messenger_create_info(PFN_vkDebugUtilsMessengerCallbackEXT user_callback)
{
    VkDebugUtilsMessengerCreateInfoEXT create_info;
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    create_info.pfnUserCallback = user_callback;
    create_info.pUserData = nullptr;
    return create_info;
}

VkApplicationInfo Vulkan_Struct_Initializers::application_info()
{
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;
    app_info.pApplicationName = "Application";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;
    return app_info;
}

VkInstanceCreateInfo Vulkan_Struct_Initializers::instance_create_info(VkApplicationInfo const* app_info, std::vector<char const*> const& enabled_layer_names, std::vector<char const*> const& enabled_extension_names, void const* next)
{
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pNext = next;
    create_info.pApplicationInfo = app_info;
    create_info.enabledLayerCount = enabled_layer_names.size();
    create_info.ppEnabledLayerNames = enabled_layer_names.data();
    create_info.enabledExtensionCount = enabled_extension_names.size();
    create_info.ppEnabledExtensionNames = enabled_extension_names.data();
    return create_info;
}

VkDeviceQueueCreateInfo Vulkan_Struct_Initializers::device_queue_create_info(u32 queue_family_index)
{
    float queue_priority = 1.f;
    VkDeviceQueueCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.queueFamilyIndex = queue_family_index;
    create_info.queueCount = 1;
    create_info.pQueuePriorities = &queue_priority;
    return create_info;
}

VkWin32SurfaceCreateInfoKHR Vulkan_Struct_Initializers::win32_surface_create_info(HWND h_wnd)
{
    VkWin32SurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.hinstance = GetModuleHandle(nullptr);
    create_info.hwnd = h_wnd;
    return create_info;
}

VkSwapchainCreateInfoKHR Vulkan_Struct_Initializers::swapchain_create_info(VkSurfaceKHR surface, u32 min_image_count, VkFormat image_format, VkColorSpaceKHR image_color_space, VkExtent2D image_extent, std::vector<u32> const& queue_family_indices, VkSurfaceTransformFlagBitsKHR pre_transform, VkPresentModeKHR present_mode)
{
    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.surface = surface;
    create_info.minImageCount = min_image_count;
    create_info.imageFormat = image_format;
    create_info.imageColorSpace = image_color_space;
    create_info.imageExtent = image_extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.imageSharingMode = queue_family_indices.size() == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = create_info.imageSharingMode == VK_SHARING_MODE_CONCURRENT ? queue_family_indices.size() : 0;
    create_info.pQueueFamilyIndices = create_info.imageSharingMode == VK_SHARING_MODE_CONCURRENT ? queue_family_indices.data() : nullptr;
    create_info.preTransform = pre_transform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;
    return create_info;
}

VkImageCreateInfo Vulkan_Struct_Initializers::image_create_info(VkFormat format, u32 width, u32 height, VkImageUsageFlags usage)
{
    VkImageCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.imageType = VK_IMAGE_TYPE_2D;
    create_info.format = format;
    create_info.extent.width = width;
    create_info.extent.height = height;
    create_info.extent.depth = 1;
    create_info.mipLevels = 1;
    create_info.arrayLayers = 1;
    create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
    create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    return create_info;
}

VkImageViewCreateInfo Vulkan_Struct_Initializers::image_view_create_info(VkImage image, VkFormat format)
{
    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.image = image;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = format;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;
    return create_info;
}

VkSamplerCreateInfo Vulkan_Struct_Initializers::sampler_create_info()
{
    VkSamplerCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.magFilter = VK_FILTER_LINEAR;
    create_info.minFilter = VK_FILTER_LINEAR;
    create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    create_info.mipLodBias = 0.f;
    create_info.anisotropyEnable = VK_TRUE;
    create_info.maxAnisotropy = 1.f;
    create_info.compareEnable = VK_FALSE;
    create_info.compareOp = VK_COMPARE_OP_NEVER;
    create_info.minLod = 0.f;
    create_info.maxLod = 0.f;
    create_info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    create_info.unnormalizedCoordinates = VK_FALSE;
    return create_info;
}

VkFramebufferCreateInfo Vulkan_Struct_Initializers::framebufferCreateInfo(
    VkRenderPass renderPass,
    std::vector<VkImageView> const& attachments,
    VkExtent2D const& extent,
    u32 layers)
{
    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.renderPass = renderPass;
    createInfo.attachmentCount = attachments.size();
    createInfo.pAttachments = attachments.data();
    createInfo.width = extent.width;
    createInfo.height = extent.height;
    createInfo.layers = layers;
    return createInfo;
}

VkBufferCreateInfo Vulkan_Struct_Initializers::buffer_create_info(VkDeviceSize size, VkBufferUsageFlags usage)
{
    VkBufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.size = size;
    create_info.usage = usage;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
    return create_info;
}

VkAttachmentDescription Vulkan_Struct_Initializers::attachmentDescription(
    VkFormat format,
    VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp,
    VkImageLayout initialLayout, VkImageLayout finalLayout,
    VkAttachmentDescriptionFlags flags,
    VkSampleCountFlagBits samples,
    VkAttachmentLoadOp stencilLoadOp,
    VkAttachmentStoreOp stencilStoreOp)
{
    VkAttachmentDescription description = {};
    description.flags = flags;
    description.format = format;
    description.samples = samples;
    description.loadOp = loadOp;
    description.storeOp = storeOp;
    description.stencilLoadOp = stencilLoadOp;
    description.stencilStoreOp = stencilStoreOp;
    description.initialLayout = initialLayout;
    description.finalLayout = finalLayout;
    return description;
}

VkAttachmentReference Vulkan_Struct_Initializers::attachmentReference(u32 attachment, VkImageLayout layout)
{
    VkAttachmentReference reference = {};
    reference.attachment = 0;
    reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    return reference;
}

VkSubpassDescription Vulkan_Struct_Initializers::subpassDescription(
    std::vector<VkAttachmentReference> const& colorAttachments,
    VkAttachmentReference const* depthStencilAttachment,
    VkSubpassDescriptionFlags flags,
    VkPipelineBindPoint pipelineBindPoint,
    std::vector<VkAttachmentReference> const& inputAttachments,
    std::vector<VkAttachmentReference> const& resolveAttachments,
    std::vector<u32> const& preserveAttachments)
{
    VkSubpassDescription description = {};
    description.flags = flags;
    description.pipelineBindPoint = pipelineBindPoint;
    description.inputAttachmentCount = inputAttachments.size();
    description.pInputAttachments = inputAttachments.data();
    description.colorAttachmentCount = colorAttachments.size();
    description.pColorAttachments = colorAttachments.data();
    description.pResolveAttachments = resolveAttachments.data();
    description.pDepthStencilAttachment = depthStencilAttachment;
    description.preserveAttachmentCount = preserveAttachments.size();
    description.pPreserveAttachments = preserveAttachments.data();
    return description;
}

VkSubpassDependency Vulkan_Struct_Initializers::subpassDependency(
    u32 srcSubpass, u32 dstSubpass,
    VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
    VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
    VkDependencyFlags dependencyFlags)
{
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = srcSubpass;
    dependency.dstSubpass = dstSubpass;
    dependency.srcStageMask = srcStageMask;
    dependency.dstStageMask = dstStageMask;
    dependency.srcAccessMask = srcAccessMask;
    dependency.dstAccessMask = dstAccessMask;
    dependency.dependencyFlags = dependencyFlags;
    return dependency;
}

VkRenderPassCreateInfo Vulkan_Struct_Initializers::renderPassCreateInfo(
    std::vector<VkAttachmentDescription> const& attachments,
    std::vector<VkSubpassDescription> const& subpasses,
    std::vector<VkSubpassDependency> const& dependencies)
{
    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.attachmentCount = attachments.size();
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = subpasses.size();
    createInfo.pSubpasses = subpasses.data();
    createInfo.dependencyCount = dependencies.size();
    createInfo.pDependencies = dependencies.data();
    return createInfo;
}



VkShaderModuleCreateInfo Vulkan_Struct_Initializers::shaderModuleCreateInfo(std::vector<u32> const& code)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.codeSize = code.size() * sizeof(code[0]);
    createInfo.pCode = code.data();
    return createInfo;
}

VkVertexInputBindingDescription Vulkan_Struct_Initializers::vertexInputBindingDescription(u32 binding, u32 stride, VkVertexInputRate inputRate)
{
    VkVertexInputBindingDescription description = {};
    description.binding = binding;
    description.stride = stride;
    description.inputRate = inputRate;
    return description;
}

VkVertexInputAttributeDescription Vulkan_Struct_Initializers::vertexInputAttributeDescription(u32 location, u32 binding, VkFormat format, u32 offset)
{
    VkVertexInputAttributeDescription description = {};
    description.location = location;
    description.binding = binding;
    description.format = format;
    description.offset = offset;
    return description;
}

VkViewport Vulkan_Struct_Initializers::viewport(VkExtent2D extent)
{
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
    return viewport;
}

VkRect2D Vulkan_Struct_Initializers::scissor(VkExtent2D extent)
{
    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;
    return scissor;
}

VkDescriptorSetLayoutBinding Vulkan_Struct_Initializers::descriptorSetLayoutBinding(
    u32 binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags)
{
    VkDescriptorSetLayoutBinding layoutBinding = {};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = stageFlags;
    layoutBinding.pImmutableSamplers = nullptr;
    return layoutBinding;
}

VkDescriptorPoolSize Vulkan_Struct_Initializers::descriptorPoolSize(VkDescriptorType type, u32 descriptorCount)
{
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = type;
    poolSize.descriptorCount = descriptorCount;
    return poolSize;
}

VkDescriptorPoolCreateInfo Vulkan_Struct_Initializers::descriptorPoolCreateInfo(u32 maxSets, std::vector<VkDescriptorPoolSize> const& poolSizes)
{
    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.maxSets = maxSets;
    createInfo.poolSizeCount = poolSizes.size();
    createInfo.pPoolSizes = poolSizes.data();
    return createInfo;
}

VkDescriptorSetLayoutBindingFlagsCreateInfo Vulkan_Struct_Initializers::descriptorSetLayoutBindingFlagsCreateInfo(std::vector<VkDescriptorBindingFlags> const& bindingFlags)
{
    VkDescriptorSetLayoutBindingFlagsCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.bindingCount = bindingFlags.size();
    createInfo.pBindingFlags = bindingFlags.data();
    return createInfo;
}

VkDescriptorSetLayoutCreateInfo Vulkan_Struct_Initializers::descriptorSetLayoutCreateInfo(std::vector<VkDescriptorSetLayoutBinding> const& bindings, void const* next)
{
    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext = next;
    createInfo.flags = 0;
    createInfo.bindingCount = bindings.size();
    createInfo.pBindings = bindings.data();
    return createInfo;
}

VkDescriptorSetVariableDescriptorCountAllocateInfo Vulkan_Struct_Initializers::descriptorSetVariableDescriptorCountAllocateInfo(std::vector<u32> const& descriptorCounts)
{
    VkDescriptorSetVariableDescriptorCountAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.descriptorSetCount = descriptorCounts.size();
    allocateInfo.pDescriptorCounts = descriptorCounts.data();
    return allocateInfo;
}

VkDescriptorSetAllocateInfo Vulkan_Struct_Initializers::descriptorSetAllocateInfo(
    VkDescriptorPool descriptorPool,
    u32 descriptorSetCount,
    std::vector<VkDescriptorSetLayout> const& setLayouts,
    void const* next)
{
    VkDescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.pNext = next;
    allocateInfo.descriptorPool = descriptorPool;
    allocateInfo.descriptorSetCount = descriptorSetCount;
    allocateInfo.pSetLayouts = setLayouts.data();
    return allocateInfo;
}

VkDescriptorBufferInfo Vulkan_Struct_Initializers::descriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
{
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = buffer;
    bufferInfo.offset = offset;
    bufferInfo.range = range;
    return bufferInfo;
}

VkDescriptorImageInfo Vulkan_Struct_Initializers::descriptorImageInfo(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout)
{
    VkDescriptorImageInfo imageInfo = {};
    imageInfo.sampler = sampler;
    imageInfo.imageView = imageView;
    imageInfo.imageLayout = imageLayout;
    return imageInfo;
}

VkWriteDescriptorSet Vulkan_Struct_Initializers::writeDescriptorSet(
    VkDescriptorSet dstSet,
    u32 dstBinding,
    VkDescriptorType descriptorType,
    VkDescriptorImageInfo const* imageInfo,
    VkDescriptorBufferInfo const* bufferInfo)
{
    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.pNext = nullptr;
    descriptorWrite.dstSet = dstSet;
    descriptorWrite.dstBinding = dstBinding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.descriptorType = descriptorType;
    descriptorWrite.pImageInfo = imageInfo;
    descriptorWrite.pBufferInfo = bufferInfo;
    descriptorWrite.pTexelBufferView = nullptr;
    return descriptorWrite;
}

VkPipelineShaderStageCreateInfo Vulkan_Struct_Initializers::pipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module)
{
    VkPipelineShaderStageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.stage = stage;
    createInfo.module = module;
    createInfo.pName = "main";
    createInfo.pSpecializationInfo = nullptr;
    return createInfo;
}

VkPipelineVertexInputStateCreateInfo Vulkan_Struct_Initializers::pipelineVertexInputStateCreateInfo(
    std::vector<VkVertexInputBindingDescription> const& vertexBindingDescriptions,
    std::vector<VkVertexInputAttributeDescription> const& vertexAttributeDescriptions)
{
    VkPipelineVertexInputStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.vertexBindingDescriptionCount = vertexBindingDescriptions.size();
    createInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
    createInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
    createInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
    return createInfo;
}

VkPipelineInputAssemblyStateCreateInfo Vulkan_Struct_Initializers::pipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable)
{
    VkPipelineInputAssemblyStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.topology = topology;
    createInfo.primitiveRestartEnable = primitiveRestartEnable;
    return createInfo;
}

VkPipelineTessellationStateCreateInfo Vulkan_Struct_Initializers::pipelineTessellationStateCreateInfo()
{
    VkPipelineTessellationStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.patchControlPoints = 0;
    return createInfo;
}

VkPipelineViewportStateCreateInfo Vulkan_Struct_Initializers::pipelineViewportStateCreateInfo(std::vector<VkViewport> const& viewports, std::vector<VkRect2D> const& scissors)
{
    VkPipelineViewportStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.viewportCount = viewports.size();
    createInfo.pViewports = viewports.data();
    createInfo.scissorCount = scissors.size();
    createInfo.pScissors = scissors.data();
    return createInfo;
}

VkPipelineRasterizationStateCreateInfo Vulkan_Struct_Initializers::pipelineRasterizationStateCreateInfo(
    VkBool32 depthClampEnable,
    VkBool32 rasterizerDiscardEnable,
    VkPolygonMode polygonMode,
    VkCullModeFlags cullMode,
    VkFrontFace frontFace,
    VkBool32 depthBiasEnable,
    float depthBiasConstantFactor,
    float depthBiasClamp,
    float depthBiasSlopeFactor,
    float lineWidth)
{
    VkPipelineRasterizationStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.depthClampEnable = depthClampEnable;
    createInfo.rasterizerDiscardEnable = rasterizerDiscardEnable;
    createInfo.polygonMode = polygonMode;
    createInfo.cullMode = cullMode;
    createInfo.frontFace = frontFace;
    createInfo.depthBiasEnable = depthBiasEnable;
    createInfo.depthBiasConstantFactor = depthBiasConstantFactor;
    createInfo.depthBiasClamp = depthBiasClamp;
    createInfo.depthBiasSlopeFactor = depthBiasSlopeFactor;
    createInfo.lineWidth = lineWidth;
    return createInfo;
}

VkPipelineMultisampleStateCreateInfo Vulkan_Struct_Initializers::pipelineMultisampleStateCreateInfo(
    VkSampleCountFlagBits rasterizationSamples,
    VkBool32 sampleShadingEnable,
    float minSampleShading,
    VkSampleMask const* sampleMask,
    VkBool32 alphaToCoverageEnable,
    VkBool32 alphaToOneEnable)
{
    VkPipelineMultisampleStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.rasterizationSamples = rasterizationSamples;
    createInfo.sampleShadingEnable = sampleShadingEnable;
    createInfo.minSampleShading = minSampleShading;
    createInfo.pSampleMask = sampleMask;
    createInfo.alphaToCoverageEnable = alphaToCoverageEnable;
    createInfo.alphaToOneEnable = alphaToOneEnable;
    return createInfo;
}

VkPipelineDepthStencilStateCreateInfo Vulkan_Struct_Initializers::pipelineDepthStencilStateCreateInfo(
    VkBool32 depthTestEnable,
    VkBool32 depthWriteEnable,
    VkCompareOp depthCompareOp,
    VkBool32 depthBoundsTestEnable,
    VkBool32 stencilTestEnable,
    VkStencilOpState front,
    VkStencilOpState back)
{
    VkPipelineDepthStencilStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.depthTestEnable = depthTestEnable;
    createInfo.depthWriteEnable = depthWriteEnable;
    createInfo.depthCompareOp = depthCompareOp;
    createInfo.depthBoundsTestEnable = depthBoundsTestEnable;
    createInfo.stencilTestEnable = stencilTestEnable;
    createInfo.front = front;
    createInfo.back = back;
    createInfo.minDepthBounds = 0.f;
    createInfo.maxDepthBounds = 1.f;
    return createInfo;
}

VkPipelineColorBlendAttachmentState Vulkan_Struct_Initializers::pipelineColorBlendAttachmentState(
    VkBool32 blendEnable,
    VkBlendFactor srcColorBlendFactor,
    VkBlendFactor dstColorBlendFactor,
    VkBlendOp colorBlendOp,
    VkBlendFactor srcAlphaBlendFactor,
    VkBlendFactor dstAlphaBlendFactor,
    VkBlendOp alphaBlendOp,
    VkColorComponentFlags colorWriteMask)
{
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.blendEnable = blendEnable;
    colorBlendAttachment.srcColorBlendFactor = srcColorBlendFactor;
    colorBlendAttachment.dstColorBlendFactor = dstColorBlendFactor;
    colorBlendAttachment.colorBlendOp = colorBlendOp;
    colorBlendAttachment.srcAlphaBlendFactor = srcAlphaBlendFactor;
    colorBlendAttachment.dstAlphaBlendFactor = dstAlphaBlendFactor;
    colorBlendAttachment.alphaBlendOp = alphaBlendOp;
    colorBlendAttachment.colorWriteMask = colorWriteMask;
    return colorBlendAttachment;
}

VkPipelineColorBlendStateCreateInfo Vulkan_Struct_Initializers::pipelineColorBlendStateCreateInfo(std::vector<VkPipelineColorBlendAttachmentState> const& colorBlendAttachments)
{
    VkPipelineColorBlendStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.logicOp = VK_LOGIC_OP_CLEAR;
    createInfo.attachmentCount = colorBlendAttachments.size();
    createInfo.pAttachments = colorBlendAttachments.data();
    createInfo.blendConstants[0] = { 0.f };
    createInfo.blendConstants[1] = { 0.f };
    createInfo.blendConstants[2] = { 0.f };
    createInfo.blendConstants[3] = { 0.f };
    return createInfo;
}

VkPipelineDynamicStateCreateInfo Vulkan_Struct_Initializers::pipelineDynamicStateCreateInfo(std::vector<VkDynamicState> const& dynamicStates)
{
    VkPipelineDynamicStateCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.dynamicStateCount = dynamicStates.size();
    createInfo.pDynamicStates = dynamicStates.data();
    return createInfo;
}

VkPipelineLayoutCreateInfo Vulkan_Struct_Initializers::pipelineLayoutCreateInfo(
    std::vector<VkDescriptorSetLayout> const& setLayouts,
    std::vector<VkPushConstantRange> const& pushConstantRanges)
{
    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.setLayoutCount = setLayouts.size();
    createInfo.pSetLayouts = setLayouts.data();
    createInfo.pushConstantRangeCount = pushConstantRanges.size();
    createInfo.pPushConstantRanges = pushConstantRanges.data();
    return createInfo;
}

VkGraphicsPipelineCreateInfo Vulkan_Struct_Initializers::graphicsPipelineCreateInfo(
    std::vector<VkPipelineShaderStageCreateInfo> const& stages,
    VkPipelineVertexInputStateCreateInfo const* vertexInputState,
    VkPipelineInputAssemblyStateCreateInfo const* inputAssemblyState,
    VkPipelineTessellationStateCreateInfo const* tessellationState,
    VkPipelineViewportStateCreateInfo const* viewportState,
    VkPipelineRasterizationStateCreateInfo const* rasterizationState,
    VkPipelineMultisampleStateCreateInfo const* multisampleState,
    VkPipelineDepthStencilStateCreateInfo const* depthStencilState,
    VkPipelineColorBlendStateCreateInfo const* colorBlendState,
    VkPipelineDynamicStateCreateInfo const* dynamicState,
    VkPipelineLayout layout,
    VkRenderPass renderPass,
    u32 subpass,
    VkPipeline basePipelineHandle,
    i32 basePipelineIndex)
{
    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.stageCount = stages.size();
    createInfo.pStages = stages.data();
    createInfo.pVertexInputState = vertexInputState;
    createInfo.pInputAssemblyState = inputAssemblyState;
    createInfo.pTessellationState = tessellationState;
    createInfo.pViewportState = viewportState;
    createInfo.pRasterizationState = rasterizationState;
    createInfo.pMultisampleState = multisampleState;
    createInfo.pDepthStencilState = depthStencilState;
    createInfo.pColorBlendState = colorBlendState;
    createInfo.pDynamicState = dynamicState;
    createInfo.layout = layout;
    createInfo.renderPass = renderPass;
    createInfo.subpass = subpass;
    createInfo.basePipelineHandle = basePipelineHandle;
    createInfo.basePipelineIndex = basePipelineIndex;
    return createInfo;
}

VkCommandBufferBeginInfo Vulkan_Struct_Initializers::commandBufferBeginInfo()
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;
    return beginInfo;
}

VkRenderPassBeginInfo Vulkan_Struct_Initializers::renderPassBeginInfo(
    VkRenderPass renderPass,
    VkFramebuffer framebuffer,
    VkRect2D renderArea,
    std::vector<VkClearValue> const& clearValues)
{
    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.renderPass = renderPass;
    beginInfo.framebuffer = framebuffer;
    beginInfo.renderArea = renderArea;
    beginInfo.clearValueCount = clearValues.size();
    beginInfo.pClearValues = clearValues.data();
    return beginInfo;
}

VmaAllocationCreateInfo Vulkan_Struct_Initializers::allocation_create_info(VmaAllocationCreateFlags flags)
{
    VmaAllocationCreateInfo create_info = {};
    create_info.flags = flags;
    create_info.usage = VMA_MEMORY_USAGE_AUTO;
    create_info.requiredFlags = 0;
    create_info.preferredFlags = 0;
    create_info.memoryTypeBits = 0;
    create_info.pool = VK_NULL_HANDLE;
    create_info.pUserData = nullptr;
    create_info.priority = 1.f;
    return create_info;
}
