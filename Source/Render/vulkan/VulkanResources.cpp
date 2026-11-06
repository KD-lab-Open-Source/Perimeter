#include "StdAfxRD.h"
#include "VulkanResources.h"
#include <cstring>
#include <cstdio>

// ---- Helper: find memory type ------------------------------------------

static VkPhysicalDeviceMemoryProperties s_memProps;
static bool s_memPropsCached = false;

static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice,
                                uint32_t typeFilter,
                                VkMemoryPropertyFlags properties) {
    if (!s_memPropsCached) {
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &s_memProps);
        s_memPropsCached = true;
    }
    for (uint32_t i = 0; i < s_memProps.memoryTypeCount; i++) {
        if ((typeFilter & (1u << i)) &&
            (s_memProps.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    fprintf(stderr, "VulkanResources: failed to find suitable memory type\n");
    return UINT32_MAX;
}

// ---- VulkanBufferResource ----------------------------------------------

void VulkanBufferResource::Destroy() {
    if (device != VK_NULL_HANDLE) {
        if (buffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(device, buffer, nullptr);
            buffer = VK_NULL_HANDLE;
        }
        if (memory != VK_NULL_HANDLE) {
            vkFreeMemory(device, memory, nullptr);
            memory = VK_NULL_HANDLE;
        }
    }
    size = 0;
}

// ---- VulkanBuffer ------------------------------------------------------

VulkanBuffer::VulkanBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
                           VkDeviceSize size, VkBufferUsageFlags usage,
                           VkMemoryPropertyFlags memProps) {
    res = new VulkanBufferResource();
    res->device = device;
    res->size   = size;

    VkBufferCreateInfo bufInfo = {};
    bufInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufInfo.size        = size;
    bufInfo.usage       = usage;
    bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(device, &bufInfo, nullptr, &res->buffer) != VK_SUCCESS) {
        fprintf(stderr, "VulkanBuffer: vkCreateBuffer failed\n");
        res->buffer = VK_NULL_HANDLE;
        return;
    }

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(device, res->buffer, &memReq);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memReq.size;
    allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memReq.memoryTypeBits, memProps);
    if (allocInfo.memoryTypeIndex == UINT32_MAX ||
        vkAllocateMemory(device, &allocInfo, nullptr, &res->memory) != VK_SUCCESS) {
        fprintf(stderr, "VulkanBuffer: vkAllocateMemory failed (size=%llu)\n",
                (unsigned long long)size);
        // Tear down the buffer too, so callers that check GetBuffer() see a
        // proper failure rather than a buffer with no memory backing - using
        // such a buffer in a draw triggers GPU faults / DEVICE_LOST.
        vkDestroyBuffer(device, res->buffer, nullptr);
        res->buffer = VK_NULL_HANDLE;
        res->memory = VK_NULL_HANDLE;
        return;
    }
    if (vkBindBufferMemory(device, res->buffer, res->memory, 0) != VK_SUCCESS) {
        fprintf(stderr, "VulkanBuffer: vkBindBufferMemory failed\n");
        vkDestroyBuffer(device, res->buffer, nullptr);
        vkFreeMemory(device, res->memory, nullptr);
        res->buffer = VK_NULL_HANDLE;
        res->memory = VK_NULL_HANDLE;
        return;
    }
}

VulkanBuffer::~VulkanBuffer() {
    if (res) {
        res->Destroy();
        delete res;
        res = nullptr;
    }
}

VkBuffer VulkanBuffer::GetBuffer() const {
    return res ? res->buffer : VK_NULL_HANDLE;
}

VkDeviceSize VulkanBuffer::GetSize() const {
    return res ? res->size : 0;
}

void VulkanBuffer::Update(const void* data, size_t size, size_t offset) {
    if (!res || res->memory == VK_NULL_HANDLE) return;
    void* mapped = nullptr;
    if (vkMapMemory(res->device, res->memory, offset, size, 0, &mapped) == VK_SUCCESS) {
        memcpy(mapped, data, size);
        vkUnmapMemory(res->device, res->memory);
    }
}

// ---- VulkanImageResource -----------------------------------------------

void VulkanImageResource::Destroy() {
    if (device != VK_NULL_HANDLE) {
        if (view != VK_NULL_HANDLE) {
            vkDestroyImageView(device, view, nullptr);
            view = VK_NULL_HANDLE;
        }
        if (image != VK_NULL_HANDLE) {
            vkDestroyImage(device, image, nullptr);
            image = VK_NULL_HANDLE;
        }
        if (memory != VK_NULL_HANDLE) {
            vkFreeMemory(device, memory, nullptr);
            memory = VK_NULL_HANDLE;
        }
    }
}

// ---- VulkanTexture2D ---------------------------------------------------

VulkanTexture2D::VulkanTexture2D(VkFormat fmt, int w, int h,
                                 bool render_target_, bool is_depth_, int mips)
    : format(fmt), render_target(render_target_), is_depth(is_depth_),
      width(w), height(h), mip_levels(mips) {
}

VulkanTexture2D::~VulkanTexture2D() {
    if (image_res) {
        image_res->Destroy();
        delete image_res;
        image_res = nullptr;
    }
    if (staging_data) {
        free(staging_data);
        staging_data = nullptr;
    }
}
