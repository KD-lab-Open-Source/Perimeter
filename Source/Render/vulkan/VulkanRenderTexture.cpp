#include "StdAfxRD.h"
#include "VulkanRender.h"
#include "VulkanResources.h"
#include "Texture.h"
#include "FileImage.h"
#include "RenderTracker.h"
#include <cstring>
#include <cstdio>
#include <vector>
#include <algorithm>

// ---- Format helpers ------------------------------------------------------

static VkFormat SurfmtToVkFormat(eSurfaceFormat fmt) {
    switch (fmt) {
        case SURFMT_COLOR:
        case SURFMT_COLORALPHA:
        case SURFMT_COLOR32:
        case SURFMT_COLORALPHA32:
        case SURFMT_BUMP:
        case SURFMT_GRAYALPHA:
        case SURFMT_UV:
        case SURFMT_U16V16:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case SURFMT_RENDERMAP16:
        case SURFMT_RENDERMAP32:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case SURFMT_RENDERMAP_DEPTH:
            return VK_FORMAT_D32_SFLOAT;
        default:
            return VK_FORMAT_R8G8B8A8_UNORM;
    }
}

static bool IsDepthVkFormat(VkFormat fmt) {
    return fmt == VK_FORMAT_D32_SFLOAT
        || fmt == VK_FORMAT_D32_SFLOAT_S8_UINT
        || fmt == VK_FORMAT_D24_UNORM_S8_UINT;
}

static bool IsRenderTarget(eSurfaceFormat fmt) {
    return fmt == SURFMT_RENDERMAP16
        || fmt == SURFMT_RENDERMAP32
        || fmt == SURFMT_RENDERMAP_DEPTH;
}

// ---- CreateTexture -------------------------------------------------------

int cVulkanRender::CreateTexture(cTexture* Texture, cFileImage* FileImage, bool enable_assert) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    RenderSubmitEvent(RenderEvent::CREATE_TEXTURE, "", Texture);
#endif

    eSurfaceFormat surfFmt = Texture->GetFmt();
    VkFormat vkFmt   = SurfmtToVkFormat(surfFmt);
    bool isDepth     = IsDepthVkFormat(vkFmt);
    bool isRT        = IsRenderTarget(surfFmt);
    int dx           = Texture->GetWidth();
    int dy           = Texture->GetHeight();
    int mipCount     = std::min(Texture->GetNumberMipMap(), (int)16);

    size_t tex_len = dx * dy * 4;

    bool is_alpha_test  = false;
    bool is_alpha_blend = false;

    for (int i = 0; i < Texture->GetNumberFrame(); i++) {
        TextureImage* fi = Texture->GetFrameImage(i);
        if (!fi) continue;

        // Delete any previous handle
        if (fi->vk) {
            for (int j = 0; j < VULKAN_TEXTURES; j++) {
                if (activeTextures[j] == fi->vk) activeTextures[j] = nullptr;
            }
            delete fi->vk;
            fi->vk = nullptr;
        }

        VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
        if (isDepth) {
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        } else if (isRT) {
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        } else {
            usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            if (mipCount > 1) usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }

        VkImage img = VK_NULL_HANDLE;
        VkDeviceMemory mem = VK_NULL_HANDLE;
        if (!CreateImage(dx, dy, mipCount, vkFmt, VK_IMAGE_TILING_OPTIMAL, usage,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, img, mem)) {
            if (enable_assert) {
                xassert(0);
            }
            return -1;
        }

        VkImageAspectFlags aspect = isDepth ? VK_IMAGE_ASPECT_DEPTH_BIT
                                            : VK_IMAGE_ASPECT_COLOR_BIT;
        VkImageView view = CreateImageView(img, vkFmt, aspect, mipCount);

        auto* imgRes    = new VulkanImageResource();
        imgRes->device  = device;
        imgRes->image   = img;
        imgRes->memory  = mem;
        imgRes->view    = view;
        imgRes->format  = vkFmt;
        imgRes->width   = dx;
        imgRes->height  = dy;
        imgRes->is_depth= isDepth;

        auto* vkTex       = new VulkanTexture2D(vkFmt, dx, dy, isRT, isDepth, mipCount);
        vkTex->image_res  = imgRes;

        fi->vk = vkTex;

        if (!isRT && FileImage) {
            // --- CPU: generate all mip level pixel data first ---
            // Each entry owns its pixel buffer; mip N+1 is downsampled from mip N.
            struct MipLevel { std::vector<uint8_t> data; int w, h; };
            std::vector<MipLevel> mips(mipCount);

            mips[0].data.resize(tex_len);
            mips[0].w = dx; mips[0].h = dy;
            uint8_t* buf0 = mips[0].data.data();
            memset(buf0, 0xFF, tex_len);
            FileImage->GetTextureRGB(buf0, i * Texture->GetTimePerFrame(), 4, 4 * dx,
                                     8, 8, 8, 0, 8, 16, dx, dy);

            if (Texture->IsAlpha() || Texture->IsAlphaTest()) {
                FileImage->GetTextureAlpha(buf0, i * Texture->GetTimePerFrame(), 4, 4 * dx,
                                           8, 24, dx, dy);
                bool early = false;
                for (size_t n = 3; n < tex_len; n += 4) {
                    uint8_t a = buf0[n];
                    if (a != 255 && a != 0) { early = true; break; }
                }
                if (early) is_alpha_blend = true;
                else        is_alpha_test  = true;
            }

            if (Texture->GetAttribute(TEXTURE_BUMP) && !Texture->GetAttribute(TEXTURE_NORMAL))
                Texture->ConvertBumpToNormal(buf0);

            for (int nMip = 1; nMip < mipCount; nMip++) {
                int mw = std::max(1, dx >> nMip);
                int mh = std::max(1, dy >> nMip);
                mips[nMip].data.resize(mw * mh * 4);
                mips[nMip].w = mw; mips[nMip].h = mh;
                BuildMipMap(mw, mh, 4,
                            4 * mips[nMip-1].w, mips[nMip-1].data.data(),
                            4 * mw,              mips[nMip].data.data(),
                            8, 8, 8, 8, 0, 8, 16, 24,
                            Texture->GetAttribute(TEXTURE_MIPMAP_POINT |
                                                  TEXTURE_MIPMAPBLUR   |
                                                  TEXTURE_BLURWHITE));
            }

            // --- GPU: upload all mips ---
            // Always defer into the frame CB. DoVulkanRendering processes
            // pendingTextureUploads before any render pass, so the texture
            // is visible to the next frame regardless of activeScene state.
            {
                PendingTextureUpload upload;
                upload.image  = img;
                upload.aspect = aspect;
                for (int nMip = 0; nMip < mipCount; nMip++) {
                    PendingTextureUpload::Mip m;
                    m.w    = mips[nMip].w;
                    m.h    = mips[nMip].h;
                    m.data = std::move(mips[nMip].data);
                    upload.mips.push_back(std::move(m));
                }
                pendingTextureUploads.push_back(std::move(upload));
            }
        } else if (!isRT) {
            PendingTextureUpload upload;
            upload.image      = img;
            upload.aspect     = aspect;
            upload.mipCount   = mipCount;
            upload.layoutOnly = true;
            pendingTextureUploads.push_back(std::move(upload));
        }
    }

    if (is_alpha_test && !is_alpha_blend) {
        Texture->ClearAttribute(TEXTURE_BLURWHITE | TEXTURE_MIPMAPBLUR | TEXTURE_ALPHA_BLEND);
        Texture->SetAttribute(TEXTURE_MIPMAP_POINT | TEXTURE_ALPHA_TEST);
    }
    return 0;
}

// ---- DeleteVertexBuffer / DeleteIndexBuffer ------------------------------
// Without these overrides the GPU buffer (vb.vk / ib.vk) is leaked every time
// the engine destroys a VertexBuffer or IndexBuffer, which is frequent during
// gameplay (tile-mesh churn, transient draw buffers, etc).  We defer the
// VulkanBuffer destruction by FRAMES_IN_FLIGHT so an in-flight CB that still
// references the buffer can finish on the GPU first.

void cVulkanRender::DeleteVertexBuffer(VertexBuffer& vb) {
    if (vb.vk) {
        pendingBufferDeletions.push_back({ vb.vk,
            globalFrameIndex + VULKAN_FRAMES_IN_FLIGHT });
        vb.vk = nullptr;
    }
    cInterfaceRenderDevice::DeleteVertexBuffer(vb);
}

void cVulkanRender::DeleteIndexBuffer(IndexBuffer& ib) {
    if (ib.vk) {
        pendingBufferDeletions.push_back({ ib.vk,
            globalFrameIndex + VULKAN_FRAMES_IN_FLIGHT });
        ib.vk = nullptr;
    }
    cInterfaceRenderDevice::DeleteIndexBuffer(ib);
}

// ---- DeleteTexture -------------------------------------------------------

int cVulkanRender::DeleteTexture(cTexture* Texture) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    RenderSubmitEvent(RenderEvent::DELETE_TEXTURE, "", Texture);
#endif
    for (auto& fi : Texture->frames) {
        if (!fi.vk) continue;
        for (int i = 0; i < VULKAN_TEXTURES; i++) {
            if (activeTextures[i] == fi.vk) activeTextures[i] = nullptr;
        }
        // Defer GPU resource destruction: both in-flight frame CBs may still reference
        // this texture. BeginScene processes the queue after the per-slot fence wait.
        pendingTextureDeletions.push_back({ fi.vk,
            globalFrameIndex + VULKAN_FRAMES_IN_FLIGHT });
        fi.vk = nullptr;
    }
    return 0;
}

// ---- LockTexture ---------------------------------------------------------

void* cVulkanRender::LockTexture(cTexture* Texture, int& Pitch) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::LOCK_TEXTURE);
#endif
    TextureImage* fi = Texture->GetFrameImage(0);
    if (!fi || !fi->vk) return nullptr;
    VulkanTexture2D* tex = fi->vk;
    if (!tex->staging_data) {
        tex->staging_size = tex->width * tex->height * 4;
        tex->staging_data = calloc(tex->staging_size, 1);
    }
    tex->locked_mip = 0;
    Pitch = tex->width * 4;
    tex->dirty_staging = true;
    return tex->staging_data;
}

void* cVulkanRender::LockTextureRect(cTexture* Texture, int& Pitch, Vect2i pos, Vect2i size) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::LOCK_TEXTURE_RECT);
#endif
    TextureImage* fi = Texture->GetFrameImage(0);
    if (!fi || !fi->vk) return nullptr;
    VulkanTexture2D* tex = fi->vk;
    if (!tex->staging_data) {
        tex->staging_size = tex->width * tex->height * 4;
        tex->staging_data = calloc(tex->staging_size, 1);
    }
    tex->locked_mip    = 0;
    Pitch              = tex->width * 4;
    tex->dirty_staging = true;
    size_t start = (pos.y * tex->width + pos.x) * 4;
    return static_cast<uint8_t*>(tex->staging_data) + start;
}

// ---- UnlockTexture -------------------------------------------------------

void cVulkanRender::UnlockTexture(cTexture* Texture) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::UNLOCK_TEXTURE);
#endif
    TextureImage* fi = Texture->GetFrameImage(0);
    if (!fi || !fi->vk) return;
    VulkanTexture2D* tex = fi->vk;
    if (!tex->staging_data || !tex->dirty_staging) return;
    tex->dirty_staging = false;

    if (tex->image_res) {
        // Always defer into the frame CB - avoids vkQueueWaitIdle stall regardless
        // of activeScene state. DoVulkanRendering records the copy before the first
        // render pass, so the GPU sees the new data before sampling it.
        PendingTextureUpload upload;
        upload.image  = tex->image_res->image;
        upload.aspect = tex->is_depth ? VK_IMAGE_ASPECT_DEPTH_BIT
                                      : VK_IMAGE_ASPECT_COLOR_BIT;
        PendingTextureUpload::Mip mip;
        mip.w    = tex->width  >> tex->locked_mip;
        mip.h    = tex->height >> tex->locked_mip;
        mip.data.assign(static_cast<uint8_t*>(tex->staging_data),
                        static_cast<uint8_t*>(tex->staging_data) + tex->staging_size);
        upload.mips.push_back(std::move(mip));
        pendingTextureUploads.push_back(std::move(upload));
    }
}

// ---- UploadTexture -------------------------------------------------------

void cVulkanRender::UploadTexture(VulkanTexture2D* tex,
                                   const void* data, size_t dataSize) {
    UploadTextureMip(tex, 0, data, dataSize, tex->width, tex->height);
}

// ---- UploadTextureMip ----------------------------------------------------

void cVulkanRender::UploadTextureMip(VulkanTexture2D* tex, int mip,
                                      const void* data, size_t dataSize,
                                      int mipW, int mipH)
{
    if (!tex || !tex->image_res || !data || dataSize == 0) return;

    // Create staging buffer
    VkBufferCreateInfo bi = {};
    bi.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bi.size        = dataSize;
    bi.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer stagingBuf = VK_NULL_HANDLE;
    VkDeviceMemory stagingMem = VK_NULL_HANDLE;
    if (vkCreateBuffer(device, &bi, nullptr, &stagingBuf) != VK_SUCCESS) return;

    VkMemoryRequirements mr;
    vkGetBufferMemoryRequirements(device, stagingBuf, &mr);
    VkMemoryAllocateInfo ai = {};
    ai.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    ai.allocationSize  = mr.size;
    ai.memoryTypeIndex = FindMemoryType(mr.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (vkAllocateMemory(device, &ai, nullptr, &stagingMem) != VK_SUCCESS) {
        vkDestroyBuffer(device, stagingBuf, nullptr);
        return;
    }
    VkResult bindResult = vkBindBufferMemory(device, stagingBuf, stagingMem, 0);
    if (bindResult != VK_SUCCESS) {
        vkFreeMemory(device, stagingMem, nullptr);
        vkDestroyBuffer(device, stagingBuf, nullptr);
        return;
    }

    void* mapped;
    if (vkMapMemory(device, stagingMem, 0, dataSize, 0, &mapped) != VK_SUCCESS) {
        vkFreeMemory(device, stagingMem, nullptr);
        vkDestroyBuffer(device, stagingBuf, nullptr);
        return;
    }
    memcpy(mapped, data, dataSize);
    vkUnmapMemory(device, stagingMem);

    auto cb = BeginSingleTimeCommands();

    VkImageAspectFlags aspect = tex->is_depth ? VK_IMAGE_ASPECT_DEPTH_BIT
                                              : VK_IMAGE_ASPECT_COLOR_BIT;
    TransitionImageLayout(cb, tex->image_res->image,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aspect, 1, mip);

    VkBufferImageCopy region = {};
    region.imageSubresource.aspectMask     = aspect;
    region.imageSubresource.mipLevel       = mip;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount     = 1;
    region.imageExtent = { (uint32_t)mipW, (uint32_t)mipH, 1 };

    vkCmdCopyBufferToImage(cb, stagingBuf, tex->image_res->image,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    TransitionImageLayout(cb, tex->image_res->image,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, aspect, 1, mip);

    EndSingleTimeCommands(cb);

    vkDestroyBuffer(device, stagingBuf, nullptr);
    vkFreeMemory(device, stagingMem, nullptr);
}
