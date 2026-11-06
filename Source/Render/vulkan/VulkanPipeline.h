#pragma once

#include "VulkanTypes.h"
#include "VertexFormat.h"
#include <vulkan/vulkan.h>

// ---- VulkanPipeline ----------------------------------------------------

struct VulkanPipeline {
    VULKAN_SHADER_ID       shader_id      = VULKAN_SHADER_ID_NONE;
    VULKAN_PIPELINE_TARGET target         = VULKAN_PIPELINE_TARGET_SWAPCHAIN;
    VULKAN_PIPELINE_TYPE   pipeline_type  = VULKAN_PIPELINE_TYPE_MESH;
    VulkanPipelineMode     mode           = {};
    vertex_fmt_t           vertex_fmt     = 0;
    ePrimitiveType         primitive      = PT_TRIANGLES;

    VkPipeline pipeline = VK_NULL_HANDLE;
    VkDevice   device   = VK_NULL_HANDLE;
    bool       instanced = false; // supports gl_InstanceIndex via per-instance VB at binding 1

    ~VulkanPipeline() {
        if (pipeline != VK_NULL_HANDLE && device != VK_NULL_HANDLE) {
            vkDestroyPipeline(device, pipeline, nullptr);
            pipeline = VK_NULL_HANDLE;
        }
    }
};

// ---- VulkanCommand -----------------------------------------------------

struct VulkanCommand {
    VulkanPipeline* pipeline     = nullptr;
    VkBuffer        vertexBuffer = VK_NULL_HANDLE;
    VkBuffer        indexBuffer  = VK_NULL_HANDLE;
    size_t          vertexCount  = 0;
    size_t          indexCount   = 0;
    size_t          baseElement  = 0;        // vertex buffer byte offset
    uint32_t        firstIndex   = 0;        // first index in index buffer (DrawRange offset)
    VkDeviceSize    indexByteOffset = 0;     // byte offset into indexBuffer (ring-buffer draws)
    VkDescriptorSet descriptorSet  = VK_NULL_HANDLE;
    uint32_t        dynUboOffsets[3] = {};   // [0]=VS [1]=FS [2]=unused
    uint32_t        instanceCount     = 1;   // 1 = non-instanced
    VkDeviceSize    instanceBufOffset = 0;   // byte offset into instance ring buffer

    // Viewport & clip
    Vect2i viewport[2] = {};
    Vect2i clip[2]     = {};
    bool   hasViewport = false;
    bool   hasClip     = false;

    // Depth-clear sentinel: when true, clears depth in the viewport region instead of drawing.
    bool   clearDepth  = false;

    ~VulkanCommand() = default;
};

// ---- VulkanRenderTarget ------------------------------------------------

struct VulkanRenderTarget {
    cTexture*       texture     = nullptr;  // colour texture (may be null for shadow)
    VkRenderPass    renderPass  = VK_NULL_HANDLE;
    VkFramebuffer   framebuffer = VK_NULL_HANDLE;
    VkExtent2D      extent      = {};
    bool            depthOnly   = false;    // true for shadow map

    std::vector<VulkanCommand> commands;

    ~VulkanRenderTarget();
};
