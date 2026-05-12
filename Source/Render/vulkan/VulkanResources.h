#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include "MemoryResource.h"

// ---- Forward declarations ----------------------------------------------

struct VulkanBufferResource;
struct VulkanImageResource;

// ---- VulkanBuffer -------------------------------------------------------
// Wrapper around VkBuffer + VkDeviceMemory, used in VertexBuffer/IndexBuffer
// union (analogous to SokolBuffer).

struct VulkanBuffer {
    VulkanBufferResource* res = nullptr;

    explicit VulkanBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
                          VkDeviceSize size, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags memProps);
    ~VulkanBuffer();

    // No copy
    VulkanBuffer(const VulkanBuffer&) = delete;
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;

    VkBuffer GetBuffer() const;
    VkDeviceSize GetSize() const;

    // Upload data to the buffer via mapped memory (buffer must be host-visible)
    void Update(const void* data, size_t size, size_t offset = 0);
};

// ---- VulkanTexture2D ----------------------------------------------------
// Wrapper around VkImage + VkImageView + VkDeviceMemory (analogous to SokolTexture2D).

struct VulkanTexture2D : MemoryResource {
    std::string label;
    VkFormat    format = VK_FORMAT_R8G8B8A8_UNORM;
    bool        render_target = false;
    bool        is_depth = false;
    int         width = 0;
    int         height = 0;
    int         mip_levels = 1;

    // Lazily created on the GPU (set by VulkanRender)
    VulkanImageResource* image_res = nullptr;

    // Staging data for LockTexture/UnlockTexture
    void*  staging_data  = nullptr;
    size_t staging_size  = 0;
    int    locked_mip    = 0;
    bool   dirty_staging = false;

    explicit VulkanTexture2D(VkFormat fmt, int w, int h,
                             bool render_target_ = false, bool is_depth_ = false,
                             int mips = 1);
    ~VulkanTexture2D();

    // No copy
    VulkanTexture2D(const VulkanTexture2D&) = delete;
    VulkanTexture2D& operator=(const VulkanTexture2D&) = delete;
};

// ---- Internal resource structs -----------------------------------------

struct VulkanBufferResource {
    VkDevice       device = VK_NULL_HANDLE;
    VkBuffer       buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize   size   = 0;

    void Destroy();
};

struct VulkanImageResource {
    VkDevice       device      = VK_NULL_HANDLE;
    VkImage        image       = VK_NULL_HANDLE;
    VkImageView    view        = VK_NULL_HANDLE;
    VkDeviceMemory memory      = VK_NULL_HANDLE;
    VkFormat       format      = VK_FORMAT_UNDEFINED;
    uint32_t       width       = 0;
    uint32_t       height      = 0;
    uint32_t       mip_levels  = 1;
    bool           is_depth    = false;

    void Destroy();
};
