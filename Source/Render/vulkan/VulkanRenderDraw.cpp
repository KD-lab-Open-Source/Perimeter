#include "StdAfxRD.h"
#include "VulkanRender.h"
#include "VulkanPipeline.h"
#include "VulkanResources.h"
#include "VulkanTypes.h"
#include "Texture.h"
#include "DrawBuffer.h"
#include "ObjMesh.h"
#include "MeshTri.h"
#include "RenderTracker.h"
#include "RenderUtils.h"
#include "cCamera.h"
#include "VertexFormat.h"
#include "TileMap.h"
#include "Font.h"
#include <cstring>
#include <cstdio>
#include <chrono>

static const Mat4f VK_CLIP_FIX(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f,-1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);

// ---- Upload static VB/IB to DEVICE_LOCAL memory ------------------------
// Always allocates a DEVICE_LOCAL buffer and uploads via staging.  The staging
// path differs based on whether we are inside a frame:
//   - Outside a frame: synchronous one-time upload (one queue stall - only at
//     load time, never during gameplay).
//   - Inside a frame:  the staging copy is recorded onto the frame command
//     buffer in DoVulkanRendering, avoiding a vkQueueWaitIdle stall.
bool cVulkanRender::PrepareStaticBuffer(VulkanBuffer*& vkBuf,
                                         const void* data, size_t dataLen,
                                         VkBufferUsageFlags usage)
{
    if (!data || dataLen == 0) return false;

    if (vkBuf == nullptr) {
        vkBuf = new VulkanBuffer(device, physicalDevice, (VkDeviceSize)dataLen,
                                  usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (vkBuf->GetBuffer() == VK_NULL_HANDLE) {
            delete vkBuf; vkBuf = nullptr;
            return false;
        }
    }

    if (activeScene) {
        // Defer the staging->DEVICE_LOCAL copy onto the frame CB.
        PendingBufferUpload up;
        up.dstBuffer = vkBuf->GetBuffer();
        up.data.assign(static_cast<const uint8_t*>(data),
                       static_cast<const uint8_t*>(data) + dataLen);
        pendingBufferUploads.push_back(std::move(up));
        return true;
    }

    // Outside a scene: synchronous upload via a one-shot staging buffer.
    VkBufferCreateInfo sbi = {};
    sbi.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    sbi.size        = (VkDeviceSize)dataLen;
    sbi.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    sbi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkBuffer stagingBuf = VK_NULL_HANDLE;
    if (vkCreateBuffer(device, &sbi, nullptr, &stagingBuf) != VK_SUCCESS)
        return false;

    VkMemoryRequirements mr;
    vkGetBufferMemoryRequirements(device, stagingBuf, &mr);
    VkMemoryAllocateInfo smai = {};
    smai.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    smai.allocationSize  = mr.size;
    smai.memoryTypeIndex = FindMemoryType(mr.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VkDeviceMemory stagingMem = VK_NULL_HANDLE;
    if (vkAllocateMemory(device, &smai, nullptr, &stagingMem) != VK_SUCCESS) {
        vkDestroyBuffer(device, stagingBuf, nullptr);
        return false;
    }
    vkBindBufferMemory(device, stagingBuf, stagingMem, 0);

    void* mapped = nullptr;
    vkMapMemory(device, stagingMem, 0, dataLen, 0, &mapped);
    memcpy(mapped, data, dataLen);
    vkUnmapMemory(device, stagingMem);

    VkCommandBuffer cb = BeginSingleTimeCommands();
    VkBufferCopy region = { 0, 0, (VkDeviceSize)dataLen };
    vkCmdCopyBuffer(cb, stagingBuf, vkBuf->GetBuffer(), 1, &region);
    EndSingleTimeCommands(cb); // submits + waits - one-time cost per unique buffer

    vkDestroyBuffer(device, stagingBuf, nullptr);
    vkFreeMemory(device, stagingMem, nullptr);
    return true;
}

// ---- Build descriptor set -----------------------------------------------
// UBO bindings (0, 1) use UNIFORM_BUFFER_DYNAMIC - the actual per-draw offsets
// are passed at vkCmdBindDescriptorSets time, not stored in the descriptor.
// This lets us cache the descriptor set by texture combo and reuse it for all
// draws that share the same textures (e.g. every unit of the same type).

VkDescriptorSet cVulkanRender::BuildDescriptorSet(
        uint32_t frame,
        VulkanTexture2D* tex0, VulkanTexture2D* tex1,
        VulkanTexture2D* tex2, VulkanTexture2D* tex3)
{
    VulkanTexCacheKey key;
    key.t[0] = tex0; key.t[1] = tex1; key.t[2] = tex2; key.t[3] = tex3;

    auto& cache = frames[frame].texDescCache;
    auto it = cache.find(key);
    if (it != cache.end()) return it->second;

    VkDescriptorSet ds = AllocDescriptorSet(frame);
    if (ds == VK_NULL_HANDLE) return VK_NULL_HANDLE;
    cVulkanRender::ProfRecordDescAlloc();

    // Dynamic UBO bindings: offset=0, range=max struct size.
    // The real offset is supplied per-draw via vkCmdBindDescriptorSets pDynamicOffsets.
    VkDescriptorBufferInfo bufInfos[2] = {};
    bufInfos[0] = { frames[frame].uboBuffer, 0, VULKAN_UBO_MAX_VS_RANGE };
    bufInfos[1] = { frames[frame].uboBuffer, 0, VULKAN_UBO_MAX_FS_RANGE };

    VulkanTexture2D* texSlots[4] = { tex0, tex1, tex2, tex3 };
    VkDescriptorImageInfo imgInfos[4] = {};
    for (int i = 0; i < 4; i++) {
        VulkanTexture2D* t = texSlots[i];
        VkImageView view = (t && t->image_res) ? t->image_res->view
                         : (emptyTexture && emptyTexture->image_res
                            ? emptyTexture->image_res->view
                            : VK_NULL_HANDLE);
        if (view == VK_NULL_HANDLE) continue;
        imgInfos[i].sampler     = sampler;
        imgInfos[i].imageView   = view;
        imgInfos[i].imageLayout = (t && t->is_depth)
                                 ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
                                 : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    VkWriteDescriptorSet writes[6] = {};
    int writeCount = 0;
    for (int i = 0; i < 2; i++) {
        writes[writeCount].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[writeCount].dstSet          = ds;
        writes[writeCount].dstBinding      = (uint32_t)i;
        writes[writeCount].descriptorCount = 1;
        writes[writeCount].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        writes[writeCount].pBufferInfo     = &bufInfos[i];
        writeCount++;
    }
    for (int i = 0; i < 4; i++) {
        if (imgInfos[i].imageView == VK_NULL_HANDLE) continue;
        writes[writeCount].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[writeCount].dstSet          = ds;
        writes[writeCount].dstBinding      = (uint32_t)(2 + i);
        writes[writeCount].descriptorCount = 1;
        writes[writeCount].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[writeCount].pImageInfo      = &imgInfos[i];
        writeCount++;
    }

    vkUpdateDescriptorSets(device, writeCount, writes, 0, nullptr);
    cache[key] = ds;
    return ds;
}

// ---- FinishActiveDrawBuffer (Vulkan equivalent of Sokol) ---------------

void cVulkanRender::FinishActiveDrawBuffer() {
    if (!activeDrawBuffer || !activeDrawBuffer->written_vertices) return;
    xassert(!activeDrawBuffer->IsLocked());

    // When SubmitDrawBuffer was called with a DrawRange (static mesh bank sub-range),
    // lock_written_indices holds the range length and activeDrawRangeOffset holds the
    // first index.  For plain dynamic buffers, lock_written_indices == 0 after Unlock.
    size_t drawIdxCount = activeDrawBuffer->lock_written_indices; // 0 = use full written_indices
    uint32_t firstIndex  = activeDrawRangeOffset;
    activeDrawRangeOffset = 0;

    CreateVulkanCommand(&activeDrawBuffer->vb, activeDrawBuffer->written_vertices,
                        &activeDrawBuffer->ib, activeDrawBuffer->written_indices,
                        firstIndex, drawIdxCount);

    activeDrawBuffer->PostDraw();
    activeDrawBuffer = nullptr;
}

// ---- CreateVulkanCommand ------------------------------------------------

void cVulkanRender::CreateVulkanCommand(VertexBuffer* vb, size_t vertices,
                                         IndexBuffer* ib, size_t indices,
                                         uint32_t firstIndex, size_t drawIdxCount)
{
    if (!vb || !vertices) return;

    VULKAN_PIPELINE_TARGET target = VULKAN_PIPELINE_TARGET_SWAPCHAIN;
    VulkanPipelineMode mode = activePipelineMode;
    if (activeRenderTarget != nullptr && activeRenderTarget == shadowMapRenderTarget) {
        target = VULKAN_PIPELINE_TARGET_SHADOWMAP;
        mode.cull        = CULL_CCW;
        mode.depth_cmp   = CMP_LESSEQUAL;
        mode.depth_write = true;
    } else if (activeRenderTarget != nullptr && activeRenderTarget == lightMapRenderTarget) {
        target = VULKAN_PIPELINE_TARGET_LIGHTMAP;
        mode.depth_cmp   = CMP_ALWAYS;
        mode.depth_write = false;
    }

    VULKAN_SHADER_ID shaderId = ChooseShader(vb->fmt, activePipelineType, target);
    size_t vsSize = 64, fsSize = 16;
    GetUBOSizes(shaderId, vsSize, fsSize);

    // ---- VTX/IDX ring buffer allocation (must come first) ---------------
    // Allocate ring-buffer space before UBO/descriptor/pipeline so we don't
    // leak ring space on draws that are dropped due to VTX/IDX overflow.
    // Dynamic VBs are suballocated from the per-frame ring buffer.
    // Static VBs are uploaded once to DEVICE_LOCAL; GPU reads from VRAM every frame.
    size_t vtxDataLen = vertices * vb->VertexSize;
    VkBuffer vtxBuffer = VK_NULL_HANDLE;
    size_t   vtxByteOffset = 0;
    std::vector<struct VulkanBuffer*> localFallbacks; // freed below on early-exit paths

    if (vb->dynamic) {
        if (!vb->data || !frames[currentFrame].dynVtxMapped) return;
        auto it = frames[currentFrame].vtxCache.find(vb);
        if (vb->dirty && it != frames[currentFrame].vtxCache.end())
            frames[currentFrame].vtxCache.erase(it);
        if (!vb->dirty && it != frames[currentFrame].vtxCache.end()) {
            vtxByteOffset = it->second;
            vtxBuffer = frames[currentFrame].dynVtxBuffer;
        } else {
            size_t alignment = vb->VertexSize > 0 ? vb->VertexSize : 16;
            vtxByteOffset = AllocDynVtx(currentFrame, vtxDataLen, alignment);
            if (vtxByteOffset != SIZE_MAX) {
                memcpy(static_cast<uint8_t*>(frames[currentFrame].dynVtxMapped) + vtxByteOffset,
                       vb->data, vtxDataLen);
                frames[currentFrame].vtxCache[vb] = vtxByteOffset;
                vtxBuffer = frames[currentFrame].dynVtxBuffer;
            } else {
                auto* fallback = new VulkanBuffer(device, physicalDevice,
                    (VkDeviceSize)vtxDataLen,
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                if (!fallback || fallback->GetBuffer() == VK_NULL_HANDLE) {
                    delete fallback;
                    return; // <--- drop draw, no ring resource leaked yet
                }
                fallback->Update(vb->data, vtxDataLen);
                localFallbacks.push_back(fallback);
                vtxBuffer = fallback->GetBuffer();
                vtxByteOffset = 0;
            }
        }
    } else {
        if (vb->vk == nullptr || vb->dirty) {
            PrepareStaticBuffer(vb->vk, vb->data, vtxDataLen,
                                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        }
        if (!vb->vk) return; // <--- drop draw, no ring resource leaked yet
        vtxBuffer = vb->vk->GetBuffer();
    }

    // Index buffer
    VkBuffer idxBuffer = VK_NULL_HANDLE;
    VkDeviceSize idxByteOffset = 0;
    if (ib && indices > 0) {
        size_t idxDataLen = indices * sizeof(indices_t);
        if (ib->dynamic) {
            if (ib->data && frames[currentFrame].dynIdxMapped) {
                auto it = frames[currentFrame].idxCache.find(ib);
                if (ib->dirty && it != frames[currentFrame].idxCache.end())
                    frames[currentFrame].idxCache.erase(it);
                if (!ib->dirty && it != frames[currentFrame].idxCache.end()) {
                    idxByteOffset = (VkDeviceSize)it->second;
                    idxBuffer = frames[currentFrame].dynIdxBuffer;
                } else {
                    size_t off = AllocDynIdx(currentFrame, idxDataLen);
                    if (off != SIZE_MAX) {
                        memcpy(static_cast<uint8_t*>(frames[currentFrame].dynIdxMapped) + off,
                               ib->data, idxDataLen);
                        frames[currentFrame].idxCache[ib] = off;
                        idxBuffer = frames[currentFrame].dynIdxBuffer;
                        idxByteOffset = (VkDeviceSize)off;
                    } else {
                        auto* fallback = new VulkanBuffer(device, physicalDevice,
                            (VkDeviceSize)idxDataLen,
                            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                        if (fallback && fallback->GetBuffer() != VK_NULL_HANDLE) {
                            fallback->Update(ib->data, idxDataLen);
                            localFallbacks.push_back(fallback);
                            idxBuffer = fallback->GetBuffer();
                            idxByteOffset = 0;
                        } else {
                            delete fallback;
                            // Drop draw: free any already-allocated VTX fallback.
                            for (auto* fb : localFallbacks) delete fb;
                            return; // <--- drop draw, no ring resource leaked yet
                        }
                    }
                }
            }
        } else {
            if (ib->vk == nullptr || ib->dirty) {
                PrepareStaticBuffer(ib->vk, ib->data, idxDataLen,
                                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
            }
            if (!ib->vk) {
                for (auto* fb : localFallbacks) delete fb;
                return;
            }
            idxBuffer = ib->vk->GetBuffer();
        }
    }

    // ---- UBO / descriptor / pipeline (only after VTX/IDX succeeded) -----

    using hrc = std::chrono::high_resolution_clock;
    hrc::time_point pt0;
    if (cVulkanRender::profilingEnabled) pt0 = hrc::now();

    // Combine VS+FS UBO into a single ring allocation.
    // fsOff must be aligned to uboAlignment (minUniformBufferOffsetAlignment),
    // so pad vsSize to the alignment boundary.
    size_t vsAligned = (vsSize + uboAlignment - 1) & ~(size_t)(uboAlignment - 1);
    size_t totalUbo = vsAligned + fsSize;
    size_t vsOff = AllocUBO(currentFrame, totalUbo);
    if (vsOff == SIZE_MAX) {
        for (auto* fb : localFallbacks) delete fb;
        return;
    }
    size_t fsOff = (fsSize > 0) ? vsOff + vsAligned : 0;
    FillUBOData(shaderId, vsOff, vsSize, fsOff, fsSize);
    double uboUs = 0;
    if (cVulkanRender::profilingEnabled) {
        uboUs = std::chrono::duration<double, std::micro>(hrc::now() - pt0).count();
        pt0 = hrc::now();
    }

    VkDescriptorSet ds = BuildDescriptorSet(currentFrame,
        activeTextures[0], activeTextures[1],
        activeTextures[2], activeTextures[3]);

    if (ds == VK_NULL_HANDLE) {
        for (auto* fb : localFallbacks) delete fb;
        return;
    }

    double descUs = 0;
    if (cVulkanRender::profilingEnabled) {
        descUs = std::chrono::duration<double, std::micro>(hrc::now() - pt0).count();
        pt0 = hrc::now();
    }

    ePrimitiveType prim = activeDrawBuffer ? activeDrawBuffer->primitive : PT_TRIANGLES;
    VulkanPipeline* pipeline = GetOrCreatePipeline(
        shaderId, target, activePipelineType, mode, vb->fmt, prim, false);

    double pipeUs = 0;
    if (cVulkanRender::profilingEnabled) {
        pipeUs = std::chrono::duration<double, std::micro>(hrc::now() - pt0).count();
        pt0 = hrc::now();
    }

    // Transfer local fallback buffers to the frame's dynamicBuffers list so
    // BeginScene cleans them up after the GPU fence fires.
    for (auto* fb : localFallbacks)
        frames[currentFrame].dynamicBuffers.push_back(fb);
    localFallbacks.clear(); // ownership transferred

    // Mark source buffers non-dirty now that uploads have been recorded/queued.
    // Clear dirty immediately after each buffer's section so the per-frame
    // cache (vtxCache / idxCache) can satisfy repeat draws of the same
    // buffer within the same frame.
    vb->dirty = false;
    if (ib) ib->dirty = false;

    double vbUs = 0;
    if (cVulkanRender::profilingEnabled) {
        vbUs = std::chrono::duration<double, std::micro>(hrc::now() - pt0).count();
        pt0 = hrc::now();
    }

    double ibUs = 0;
    if (cVulkanRender::profilingEnabled) {
        ibUs = std::chrono::duration<double, std::micro>(hrc::now() - pt0).count();
    }

    VulkanCommand cmd;
    cmd.pipeline      = pipeline;
    cmd.vertexBuffer  = vtxBuffer;
    cmd.indexBuffer   = idxBuffer;
    cmd.vertexCount   = (uint32_t)vertices;
    size_t actualIdxCount = (drawIdxCount > 0) ? drawIdxCount : indices;
    cmd.indexCount    = (uint32_t)(idxBuffer != VK_NULL_HANDLE ? actualIdxCount : 0);
    cmd.baseElement   = vtxByteOffset;
    cmd.firstIndex    = firstIndex;
    cmd.indexByteOffset = idxByteOffset;
    cmd.descriptorSet    = ds;
    cmd.dynUboOffsets[0] = (uint32_t)vsOff;
    cmd.dynUboOffsets[1] = (uint32_t)fsOff;
    cmd.hasViewport   = hasViewport;
    if (hasViewport) {
        cmd.viewport[0] = activeViewportPos;
        cmd.viewport[1] = activeViewportSize;
    }
    cmd.hasClip       = hasClip;
    if (hasClip) {
        cmd.clip[0] = { activeClipPos.x, activeClipPos.y };
        cmd.clip[1] = { activeClipSize.x, activeClipSize.y };
    }

    auto& activeCmds = GetActiveCommands();
    activeCmds.push_back(std::move(cmd));
    cVulkanRender::ProfRecordDrawCall();
    if (cVulkanRender::profilingEnabled) {
        cVulkanRender::ProfRecordCmdPhase(uboUs, descUs, pipeUs, vbUs, ibUs);
    }
}

// ---- Deferred draw batching (instancing) -------------------------------

static const size_t INST_DATA_SIZE = 64; // 4×4 float = mat4 world matrix per instance

void cVulkanRender::EnterDeferredMode() {
    if (m_deferredMode) return;
    if (!m_batchingEnabled) return;
    FlushActiveCommand();
    m_deferredMode = true;
    m_deferredDraws.clear();
    m_deferredDraws.reserve(512);
    // Reset texture transforms: all meshes in a group share the same transforms
    activeTexTransform[0] = Mat4f::ID;
    activeTexTransform[1] = Mat4f::ID;
}

void cVulkanRender::FlushDeferredBatch() {
    if (!m_deferredMode) return;
    m_deferredMode = false;
    if (m_deferredDraws.empty()) return;

    using hrc = std::chrono::high_resolution_clock;
    hrc::time_point tpGroup{}, tpUpload{}, tpUbo{}, tpDesc{}, tpPipe{};
    hrc::time_point t0;
    if (cVulkanRender::profilingEnabled) t0 = hrc::now();
    double groupUsAcc = 0, uploadUsAcc = 0, uboUsAcc = 0, descUsAcc = 0;
    double pipeUsAcc = 0, instUsAcc = 0, cmdUsAcc = 0;
    size_t groupsEmitted = 0, instancedEmitted = 0, fallbackEmitted = 0;

    auto& frame = frames[currentFrame];
    const size_t instMax = frame.instBuffer != VK_NULL_HANDLE
                         ? (VULKAN_DYN_VTX_POOL_SIZE / 8)
                         : 0;

    // Group by DrawRange (offset, len) - identical meshes get instanced.
    struct GroupKey {
        size_t ibPtr;
        uint32_t dbrOffset;
        size_t dbrLen;
        VULKAN_SHADER_ID shaderId;
        bool operator==(const GroupKey& o) const {
            return ibPtr==o.ibPtr && dbrOffset==o.dbrOffset
                && dbrLen==o.dbrLen && shaderId==o.shaderId;
        }
    };
    struct GroupKeyHash {
        size_t operator()(const GroupKey& k) const {
            return k.ibPtr ^ (k.dbrOffset << 12) ^ (k.dbrLen << 24)
                 ^ ((size_t)k.shaderId << 8);
        }
    };

    std::unordered_map<GroupKey, size_t, GroupKeyHash> groups;
    groups.reserve(128);

    for (size_t i = 0; i < m_deferredDraws.size(); i++) {
        const auto& d = m_deferredDraws[i];
        GroupKey key = {};
        key.ibPtr     = (size_t)(uintptr_t)d.ib;
        key.dbrOffset = d.dbrOffset;
        key.dbrLen    = d.dbrLen;
        key.shaderId  = d.shaderId;
        groups[key]++;  // count instances per key
    }

    if (cVulkanRender::profilingEnabled) {
        groupUsAcc = std::chrono::duration<double, std::micro>(hrc::now() - t0).count();
        tpUpload = hrc::now();
    }

    // Emit draws per group
    for (auto& [key, instanceCount] : groups) {
        if (cVulkanRender::profilingEnabled) tpUpload = hrc::now();

        const auto* first = &m_deferredDraws[0];
        // Find the first draw with this key
        for (size_t i = 0; i < m_deferredDraws.size(); i++) {
            const auto& d = m_deferredDraws[i];
            if ((size_t)(uintptr_t)d.ib == key.ibPtr
                && d.dbrOffset == key.dbrOffset
                && d.dbrLen == key.dbrLen
                && d.shaderId == key.shaderId) {
                first = &d;
                break;
            }
        }

        // Static VB/IB upload (if dirty)
        if (first->vb) {
            size_t vtxLen = first->vertices * first->vb->VertexSize;
            if (first->vb->vk == nullptr || first->vb->dirty)
                PrepareStaticBuffer(first->vb->vk, first->vb->data, vtxLen,
                                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
            first->vb->dirty = false;
            if (!first->vb->vk) continue;
        }
        if (first->ib && first->indices > 0) {
            size_t idxLen = first->indices * sizeof(indices_t);
            if (first->ib->vk == nullptr || first->ib->dirty)
                PrepareStaticBuffer(first->ib->vk, first->ib->data, idxLen,
                                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
            first->ib->dirty = false;
            if (!first->ib->vk) continue;
        }

        if (cVulkanRender::profilingEnabled) {
            uploadUsAcc += std::chrono::duration<double, std::micro>(hrc::now() - tpUpload).count();
            tpUbo = hrc::now();
        }

        // UBO alloc (combined VS+FS)
        size_t vsAligned = (first->vsSize + uboAlignment - 1) & ~(size_t)(uboAlignment - 1);
        size_t totalUbo = vsAligned + first->fsSize;
        size_t vsOff = AllocUBO(currentFrame, totalUbo);
        if (vsOff == SIZE_MAX) continue;
        size_t fsOff = (first->fsSize > 0) ? vsOff + vsAligned : 0;

        const bool instSpvAvailable = (shaderModulesInst[first->shaderId][0] != VK_NULL_HANDLE);
        bool useInstancing = (instanceCount > 1) && (instMax > 0) && instSpvAvailable;

        // Pipeline mode & target (needed for both instanced and fallback paths)
        VULKAN_PIPELINE_TARGET target = VULKAN_PIPELINE_TARGET_SWAPCHAIN;
        VulkanPipelineMode mode = activePipelineMode;
        if (activeRenderTarget != nullptr && activeRenderTarget == shadowMapRenderTarget) {
            target = VULKAN_PIPELINE_TARGET_SHADOWMAP;
            mode.cull = CULL_CCW; mode.depth_cmp = CMP_LESSEQUAL; mode.depth_write = true;
        } else if (activeRenderTarget != nullptr && activeRenderTarget == lightMapRenderTarget) {
            target = VULKAN_PIPELINE_TARGET_LIGHTMAP;
            mode.depth_cmp = CMP_ALWAYS; mode.depth_write = false;
        }
        ePrimitiveType prim = activeDrawBuffer ? activeDrawBuffer->primitive : PT_TRIANGLES;
        vertex_fmt_t vfmt = first->vb ? first->vb->fmt : 0;

        // Instance data ring allocation
        size_t instOff = 0;
        if (useInstancing) {
            size_t instBytes = instanceCount * INST_DATA_SIZE;
            instOff = AllocInstData(currentFrame, instBytes);
            if (instOff != SIZE_MAX) {
                uint8_t* dst = static_cast<uint8_t*>(frame.instMapped) + instOff;
                size_t wi = 0;
                for (size_t j = 0; j < m_deferredDraws.size() && wi < instanceCount; j++) {
                    const auto& dd = m_deferredDraws[j];
                    if ((size_t)(uintptr_t)dd.ib != key.ibPtr
                        || dd.dbrOffset != key.dbrOffset
                        || dd.dbrLen != key.dbrLen
                        || dd.shaderId != key.shaderId)
                        continue;
                    memcpy(dst + wi * INST_DATA_SIZE, &dd.worldMatrix, 64);
                    wi++;
                }
            } else {
                useInstancing = false; // ring overflow
            }
        }

        if (!useInstancing && instanceCount > 1) {
            // Fallback: instanced SPV unavailable or instance ring overflowed.
            // Emit individual non-instanced draws.
            for (size_t j = 0; j < m_deferredDraws.size(); j++) {
                const auto& dd = m_deferredDraws[j];
                if ((size_t)(uintptr_t)dd.ib != key.ibPtr
                    || dd.dbrOffset != key.dbrOffset
                    || dd.dbrLen != key.dbrLen
                    || dd.shaderId != key.shaderId)
                    continue;

                if (dd.vb) {
                    size_t vtxLen = dd.vertices * dd.vb->VertexSize;
                    if (dd.vb->vk == nullptr || dd.vb->dirty)
                        PrepareStaticBuffer(dd.vb->vk, dd.vb->data, vtxLen,
                                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
                    dd.vb->dirty = false;
                    if (!dd.vb->vk) continue;
                }
                if (dd.ib && dd.indices > 0) {
                    size_t idxLen = dd.indices * sizeof(indices_t);
                    if (dd.ib->vk == nullptr || dd.ib->dirty)
                        PrepareStaticBuffer(dd.ib->vk, dd.ib->data, idxLen,
                                            VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
                    dd.ib->dirty = false;
                    if (!dd.ib->vk) continue;
                }

                size_t vsAl = (dd.vsSize + uboAlignment - 1) & ~(size_t)(uboAlignment - 1);
                size_t tot = vsAl + dd.fsSize;
                size_t vo = AllocUBO(currentFrame, tot);
                if (vo == SIZE_MAX) continue;
                size_t fo = (dd.fsSize > 0) ? vo + vsAl : 0;

                Mat4f sw = activeWorld;
                activeWorld = dd.worldMatrix;
                cachedMvpDirty = true;
                FillUBOData(dd.shaderId, vo, dd.vsSize, fo, dd.fsSize);
                activeWorld = sw;
                cachedMvpDirty = true;

                VkDescriptorSet dset = BuildDescriptorSet(currentFrame,
                    activeTextures[0], activeTextures[1],
                    activeTextures[2], activeTextures[3]);
                if (dset == VK_NULL_HANDLE) continue;

                VulkanPipeline* pl = GetOrCreatePipeline(
                    dd.shaderId, target, activePipelineType, mode, vfmt, prim, false);
                if (!pl || pl->pipeline == VK_NULL_HANDLE) continue;

                VkBuffer vb = dd.vb ? dd.vb->vk->GetBuffer() : VK_NULL_HANDLE;
                VkBuffer ib = (dd.ib && dd.indices > 0) ? dd.ib->vk->GetBuffer() : VK_NULL_HANDLE;

                VulkanCommand c;
                c.pipeline = pl;
                c.vertexBuffer = vb;
                c.indexBuffer = ib;
                c.vertexCount = (uint32_t)dd.vertices;
                c.indexCount = (uint32_t)(ib ? dd.dbrLen : 0);
                c.baseElement = 0;
                c.firstIndex = dd.dbrOffset;
                c.indexByteOffset = 0;
                c.descriptorSet = dset;
                c.dynUboOffsets[0] = (uint32_t)vo;
                c.dynUboOffsets[1] = (uint32_t)fo;
                c.instanceCount = 1;
                c.hasViewport = hasViewport;
                if (hasViewport) { c.viewport[0] = activeViewportPos; c.viewport[1] = activeViewportSize; }
                c.hasClip = hasClip;
                if (hasClip) { c.clip[0] = { activeClipPos.x, activeClipPos.y }; c.clip[1] = { activeClipSize.x, activeClipSize.y }; }

                auto& activeCmds = GetActiveCommands();
                activeCmds.push_back(std::move(c));
                cVulkanRender::ProfRecordDrawCall();
            }
            continue; // skip the instanced path below
        }

        // Fill UBO: for instanced, un_mvp = viewProj; for non-instanced, use first draw's world
        Mat4f savedWorld = activeWorld;
        if (useInstancing) {
            activeWorld = Mat4f::ID;  // model matrix from instance VB
        } else {
            activeWorld = first->worldMatrix;
        }
        cachedMvpDirty = true;
        FillUBOData(first->shaderId, vsOff, first->vsSize, fsOff, first->fsSize);
        activeWorld = savedWorld;
        cachedMvpDirty = true;

        if (cVulkanRender::profilingEnabled) {
            uboUsAcc += std::chrono::duration<double, std::micro>(hrc::now() - tpUbo).count();
            tpDesc = hrc::now();
        }

        // Descriptor set
        VkDescriptorSet ds = BuildDescriptorSet(currentFrame,
            activeTextures[0], activeTextures[1],
            activeTextures[2], activeTextures[3]);
        if (ds == VK_NULL_HANDLE) continue;

        if (cVulkanRender::profilingEnabled) {
            descUsAcc += std::chrono::duration<double, std::micro>(hrc::now() - tpDesc).count();
            tpPipe = hrc::now();
        }

        // Pipeline (target/mode/vfmt/prim already set above)
        VulkanPipeline* pipeline = GetOrCreatePipeline(
             first->shaderId, target, activePipelineType, mode, vfmt, prim, useInstancing);
        if (!pipeline || pipeline->pipeline == VK_NULL_HANDLE) continue;

        if (cVulkanRender::profilingEnabled) {
            pipeUsAcc += std::chrono::duration<double, std::micro>(hrc::now() - tpPipe).count();
            tpPipe = hrc::now(); // reused for cmd assembly timer
        }

        VkBuffer vtxBuf = first->vb ? first->vb->vk->GetBuffer() : VK_NULL_HANDLE;
        VkBuffer idxBuf = (first->ib && first->indices > 0) ? first->ib->vk->GetBuffer() : VK_NULL_HANDLE;

        VulkanCommand cmd;
        cmd.pipeline         = pipeline;
        cmd.vertexBuffer     = vtxBuf;
        cmd.indexBuffer      = idxBuf;
        cmd.vertexCount      = (uint32_t)first->vertices;
        cmd.indexCount       = (uint32_t)(idxBuf ? first->dbrLen : 0);
        cmd.baseElement      = 0;
        cmd.firstIndex       = first->dbrOffset;
        cmd.indexByteOffset  = 0;
        cmd.descriptorSet    = ds;
        cmd.dynUboOffsets[0] = (uint32_t)vsOff;
        cmd.dynUboOffsets[1] = (uint32_t)fsOff;
        cmd.instanceBufOffset = instOff;
        cmd.instanceCount    = useInstancing ? (uint32_t)instanceCount : 1;
        cmd.hasViewport = hasViewport;
        if (hasViewport) {
            cmd.viewport[0] = activeViewportPos;
            cmd.viewport[1] = activeViewportSize;
        }
        cmd.hasClip = hasClip;
        if (hasClip) {
            cmd.clip[0] = { activeClipPos.x, activeClipPos.y };
            cmd.clip[1] = { activeClipSize.x, activeClipSize.y };
        }

        auto& activeCmds = GetActiveCommands();
        activeCmds.push_back(std::move(cmd));
        cVulkanRender::ProfRecordDrawCall();

        if (cVulkanRender::profilingEnabled) {
            cmdUsAcc += std::chrono::duration<double, std::micro>(hrc::now() - tpPipe).count();
            if (useInstancing) instancedEmitted++;
            else if (instanceCount == 1) {} // no special counter for singletons
            else fallbackEmitted += instanceCount;
            groupsEmitted++;
        }
    }

    if (cVulkanRender::profilingEnabled) {
        cVulkanRender::ProfRecordBatchPhase(
            groupsEmitted, instancedEmitted, fallbackEmitted,
            groupUsAcc, uploadUsAcc, uboUsAcc, descUsAcc, pipeUsAcc,
            instUsAcc, cmdUsAcc);
    }

    m_deferredDraws.clear();
}

// ---- AllocInstData -------------------------------------------------------

size_t cVulkanRender::AllocInstData(uint32_t frame, size_t byteSize) {
    auto& f = frames[frame];
    if (!f.instMapped || f.instBuffer == VK_NULL_HANDLE) return SIZE_MAX;
    size_t aligned = (byteSize + 15) & ~(size_t)15;
    size_t off = f.instOffset;
    if (off + aligned > (VULKAN_DYN_VTX_POOL_SIZE / 8)) return SIZE_MAX;
    f.instOffset = off + aligned;
    return off;
}

void cVulkanRender::ResetInstRing(uint32_t frame) {
    frames[frame].instOffset = 0;
}

// ---- ChooseShader / GetUBOSizes ----------------------------------------

VULKAN_SHADER_ID cVulkanRender::ChooseShader(vertex_fmt_t vfmt,
                                               VULKAN_PIPELINE_TYPE ptype,
                                               VULKAN_PIPELINE_TARGET target) const
{
    if (target == VULKAN_PIPELINE_TARGET_SHADOWMAP)
        return (vfmt & VERTEX_FMT_NORMAL) ? VULKAN_SHADER_ID_shadow_normal_tex1
                                           : VULKAN_SHADER_ID_shadow_tex1;
    if (ptype == VULKAN_PIPELINE_TYPE_TILE_MAP) return VULKAN_SHADER_ID_tile_map;
    if (vfmt & VERTEX_FMT_NORMAL)              return VULKAN_SHADER_ID_mesh_normal_tex1;
    if ((vfmt & VERTEX_FMT_TEX2) || activeTextures[1]) return VULKAN_SHADER_ID_mesh_color_tex2;
    if (vfmt & VERTEX_FMT_DIFFUSE)             return VULKAN_SHADER_ID_mesh_color_tex1;
    return VULKAN_SHADER_ID_mesh_tex1;
}

void cVulkanRender::GetUBOSizes(VULKAN_SHADER_ID id, size_t& vsSize, size_t& fsSize) const {
    switch (id) {
        case VULKAN_SHADER_ID_mesh_color_tex1:
        case VULKAN_SHADER_ID_mesh_color_tex2:    vsSize = sizeof(VulkanMeshColorTexVsParams);  fsSize = sizeof(VulkanMeshColorTexFsParams);  return;
        case VULKAN_SHADER_ID_mesh_normal_tex1:   vsSize = sizeof(VulkanMeshNormalTexVsParams); fsSize = sizeof(VulkanMeshNormalTexFsParams); return;
        case VULKAN_SHADER_ID_mesh_tex1:          vsSize = sizeof(VulkanMeshTexVsParams);       fsSize = 0; return; // no FS UBO
        case VULKAN_SHADER_ID_shadow_tex1:
        case VULKAN_SHADER_ID_shadow_normal_tex1: vsSize = sizeof(VulkanShadowTexVsParams);     fsSize = sizeof(VulkanShadowTexFsParams); return;
        case VULKAN_SHADER_ID_tile_map:           vsSize = sizeof(VulkanTileMapVsParams);       fsSize = 0; return; // FS reads from binding 0 (VS UBO)
        default: vsSize = 64; fsSize = 0; return;
    }
}

// ---- FillUBOData --------------------------------------------------------

void cVulkanRender::FillUBOData(VULKAN_SHADER_ID id,
                                 size_t vsOff, size_t /*vsSize*/,
                                 size_t fsOff, size_t /*fsSize*/)
{
    uint8_t* ubo = static_cast<uint8_t*>(frames[currentFrame].uboMapped);
    if (!ubo) return;

    // Recompute the MVP once per flush-window. activeWorld/activeVP changes
    // already call FlushActiveCommand, which sets cachedMvpDirty.
    if (cachedMvpDirty) {
        cachedMvp = isOrthographicProjSet ? orthoVPVk : (activeWorld * activeVPVk);
        cachedMvpDirty = false;
    }
    const Mat4f& mvp = cachedMvp;

    switch (id) {
        case VULKAN_SHADER_ID_mesh_color_tex1:
        case VULKAN_SHADER_ID_mesh_color_tex2: {
            memcpy(ubo + vsOff + 0,   &mvp,                  64);
            memcpy(ubo + vsOff + 64,  &activeTexTransform[0], 64);
            memcpy(ubo + vsOff + 128, &activeTexTransform[1], 64);
            VulkanMeshColorTexFsParams fsP = {};
            fsP.un_color_mode = (int32_t)activeColorMode;
            fsP.un_tex2_lerp  = activeTex2Lerp;
            fsP.un_alpha_test = GetAlphaTestValue();
            memcpy(ubo + fsOff, &fsP, sizeof(fsP));
            break;
        }
      case VULKAN_SHADER_ID_mesh_normal_tex1: {
            const Mat4f& modelMat = isOrthographicProjSet ? Mat4f::ID : activeWorld;
            memcpy(ubo + vsOff + 0,   &mvp,      64);
            memcpy(ubo + vsOff + 64,  &modelMat, 64);
            memcpy(ubo + vsOff + 128, &activeTexTransform[0], 64);
            VulkanMeshNormalTexFsParams fsP = {};
            memcpy(fsP.diffuse,  &activeDiffuse,  16);
            memcpy(fsP.ambient,  &activeAmbient,  16);
            memcpy(fsP.specular, &activeSpecular, 16);
            memcpy(fsP.emissive, &activeEmissive, 16);
            fsP.light_dir[0]      = activeLightDir.x;
            fsP.light_dir[1]      = activeLightDir.y;
            fsP.light_dir[2]      = activeLightDir.z;
            fsP.light_diffuse[0]  = activeLightDiffuse.r;
            fsP.light_diffuse[1]  = activeLightDiffuse.g;
            fsP.light_diffuse[2]  = activeLightDiffuse.b;
            fsP.light_ambient[0]  = activeLightAmbient.r;
            fsP.light_ambient[1]  = activeLightAmbient.g;
            fsP.light_ambient[2]  = activeLightAmbient.b;
            fsP.light_specular[0] = activeLightSpecular.r;
            fsP.light_specular[1] = activeLightSpecular.g;
            fsP.light_specular[2] = activeLightSpecular.b;
            fsP.spec_power        = activePower;
            fsP.un_alpha_test     = GetAlphaTestValue();
            fsP.material          = (int32_t)activeMaterial;
            fsP.eye_pos[0]        = activeEyePos.x;
            fsP.eye_pos[1]        = activeEyePos.y;
            fsP.eye_pos[2]        = activeEyePos.z;
            memcpy(ubo + fsOff, &fsP, sizeof(fsP));
            break;
        }
      case VULKAN_SHADER_ID_mesh_tex1: {
            memcpy(ubo + vsOff, &mvp, 64);
            break;
        }
      case VULKAN_SHADER_ID_shadow_tex1:
        case VULKAN_SHADER_ID_shadow_normal_tex1: {
            memcpy(ubo + vsOff, &mvp, 64);
            VulkanShadowTexFsParams fsP = {};
            fsP.un_alpha_test = GetAlphaTestValue();
            memcpy(ubo + fsOff, &fsP, sizeof(fsP));
            break;
        }
      case VULKAN_SHADER_ID_tile_map: {
            memcpy(ubo + vsOff + 0,  &mvp,                 64);
            memcpy(ubo + vsOff + 64, &activeShadowMatrix,  64);
            float inv_world_size[2] = { activeWorldSize.x, activeWorldSize.y };
            memcpy(ubo + vsOff + 128, inv_world_size,       8);
            float tile_color[4] = { activeTileColor.r, activeTileColor.g,
                                    activeTileColor.b, activeTileColor.a };
            memcpy(ubo + vsOff + 144, tile_color,           16);
            break;
        }
        default: break;
    }
}

float cVulkanRender::GetAlphaTestValue() const {
    switch (activeAlphaTest) {
        case ALPHATEST_NONE:    return -1.0f;
        case ALPHATEST_GT_0:    return 0.0f;
        case ALPHATEST_GT_1:    return 1.0f / 255.0f;
        case ALPHATEST_GT_254:  return 254.0f / 255.0f;
        default:                 return -1.0f;
    }
}

// ---- SubmitDrawBuffer ---------------------------------------------------

void cVulkanRender::SubmitDrawBuffer(DrawBuffer* db, DrawRange* range) {
#ifdef PERIMETER_RENDER_TRACKER_DRAW_BUFFER_STATE
    RenderSubmitEvent(RenderEvent::SUBMIT_DRAW_BUFFER, "", db);
#endif
    if (activeDrawBuffer != nullptr && activeDrawBuffer != db) {
        FinishActiveDrawBuffer();
    }
    activeDrawBuffer = db;
    if (activeDrawBuffer) {
        if (range) {
            activeDrawBuffer->lock_written_vertices = 0;
            activeDrawBuffer->lock_written_indices  = range->len;
            activeDrawRangeOffset = range->offset;
        } else {
            activeDrawRangeOffset = 0;
        }
    }
    FinishActiveDrawBuffer();
}

// ---- SubmitBuffers ------------------------------------------------------

void cVulkanRender::SubmitBuffers(ePrimitiveType primitive,
                                   VertexBuffer* vb, size_t vertices,
                                   IndexBuffer*  ib, size_t indices,
                                   DrawRange* range)
{
    static const size_t MAX_VERTICES = 1024 * 20;
    xassert(vertices <= MAX_VERTICES);
    DrawBuffer* db = GetDrawBuffer(vb->fmt, primitive, MAX_VERTICES);
    size_t indices_count = range ? range->len : indices;

    void* vb_dst = nullptr;
    indices_t* ib_dst = nullptr;
    db->LockRaw(vertices, indices_count, vb_dst, ib_dst, false);
    size_t written_vertices = db->written_vertices;
    db->lock_written_vertices = vertices;
    db->lock_written_indices  = indices_count;

    memcpy(vb_dst, vb->data, vertices * vb->VertexSize);

    indices_t* ib_src = static_cast<indices_t*>(ib->data);
    if (range) ib_src += range->offset;
    copy_indices_list(ib_dst, ib_src, indices_count, written_vertices);

    db->Unlock();
    SubmitDrawBuffer(db, nullptr);
}

// ---- SetActiveDrawBuffer ------------------------------------------------

void cVulkanRender::SetActiveDrawBuffer(DrawBuffer* db) {
    if (activeDrawBuffer != nullptr && activeDrawBuffer != db) {
        FinishActiveDrawBuffer();
    }
    cInterfaceRenderDevice::SetActiveDrawBuffer(db);
}

// ---- SetNoMaterial ------------------------------------------------------

void cVulkanRender::SetNoMaterial(eBlendMode blend, float Phase,
                                    cTexture* Tex0, cTexture* Tex1,
                                    eColorMode color_mode)
{
    FlushDeferredBatch();
    if (Tex0) {
        if (blend == ALPHA_NONE && Tex0->IsAlphaTest()) blend = ALPHA_TEST;
        if (blend <= ALPHA_TEST  && Tex0->IsAlpha())    blend = ALPHA_BLEND;
    }
    SetColorMode(color_mode);
    SetBlendState(blend);
    SetTexture(0, Tex0, Phase);
    SetTexture(1, Tex1, Phase);
    // HUD / sprite draws expect an identity texture transform.
    // Reset both slots so a previous mesh's animated transform doesn't leak.
    activeTexTransform[0] = Mat4f::ID;
    activeTexTransform[1] = Mat4f::ID;
    if (activePipelineType != VULKAN_PIPELINE_TYPE_MESH) {
        FinishActiveDrawBuffer();
    }
    activePipelineType = VULKAN_PIPELINE_TYPE_MESH;
}

// ---- SetSimplyMaterialMesh ----------------------------------------------

void cVulkanRender::SetSimplyMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) {
    FlushDeferredBatch();

    bool specular = data->Specular.r >= 1/256.0f ||
                    data->Specular.g >= 1/256.0f ||
                    data->Specular.b >= 1/256.0f;
    if (specular) data->mat |=  MAT_COLOR_ADD_SPECULAR;
    else          data->mat &= ~static_cast<uint32_t>(MAT_COLOR_ADD_SPECULAR);

    cTexture* Texture0 = data->Tex[0];
    eBlendMode blend = ALPHA_NONE;
    if (Texture0) {
        if (Texture0->IsAlphaTest()) blend = ALPHA_TEST;
        else if (Texture0->IsAlpha() || data->Diffuse.a < 0.99f) blend = ALPHA_BLEND;
    }

    if (data->mat & MAT_LIGHT) {
        activeMaterial = VULKAN_MAT_LIGHT;
        activeDiffuse  = data->Diffuse;
        activeAmbient  = { data->Ambient.r,  data->Ambient.g,  data->Ambient.b,  0 };
        activeSpecular = { data->Specular.r, data->Specular.g, data->Specular.b, 0 };
        activeEmissive = { data->Emissive.r, data->Emissive.g, data->Emissive.b, 0 };
        activePower    = data->Power;
    } else {
        activeMaterial = VULKAN_MAT_NONE;
        activeDiffuse  = { 0, 0, 0, data->Diffuse.a };
        activeAmbient  = { data->Diffuse.r, data->Diffuse.g, data->Diffuse.b, 0 };
        activeSpecular = { 0, 0, 0, 0 };
        activeEmissive = { 0, 0, 0, 0 };
        activePower    = 0;
    }

    if (data->mat & MAT_ALPHA_SUBBLEND)           blend = ALPHA_SUBBLEND;
    else if (data->mat & MAT_ALPHA_ADDBLENDALPHA)  blend = ALPHA_ADDBLENDALPHA;
    else if (data->mat & MAT_ALPHA_ADDBLEND)       blend = ALPHA_ADDBLEND;

    SetColorMode(COLOR_MOD);
    SetBlendState(blend);
    SetTexture(0, Texture0, data->MaterialAnimPhase);
    SetTexture(1, data->Tex[1], data->MaterialAnimPhase);

    if (activePipelineType != VULKAN_PIPELINE_TYPE_MESH) {
        FinishActiveDrawBuffer();
    }
    activePipelineType = VULKAN_PIPELINE_TYPE_MESH;

    EnterDeferredMode();
}

// ---- DrawNoMaterialMesh -------------------------------------------------

void cVulkanRender::DrawNoMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) {
    cMeshTri* Tri = mesh->GetTri();
    if (m_deferredMode) {
        // Collect into deferred batch - no flush, no immediate draw.
        Mat4f world;
        Mat4fSetTransposedMatXf(world, mesh->GetGlobalMatrix());
        VULKAN_SHADER_ID shaderId = ChooseShader(
            Tri->db->vb.fmt, activePipelineType, VULKAN_PIPELINE_TARGET_SWAPCHAIN);
        size_t vsSize = 64, fsSize = 16;
        GetUBOSizes(shaderId, vsSize, fsSize);

        DeferredMeshDraw d = {};
        d.vb        = &Tri->db->vb;
        d.vertices  = Tri->db->written_vertices;
        d.ib        = &Tri->db->ib;
        d.indices   = Tri->db->written_indices;
        d.dbrOffset = (uint32_t)Tri->dbr.offset;
        d.dbrLen    = Tri->dbr.len;
        d.worldMatrix = world;
        d.shaderId  = shaderId;
        d.vsSize    = vsSize;
        d.fsSize    = fsSize;
        m_deferredDraws.push_back(d);
        return;
    }

    SetWorldMatXf(mesh->GetGlobalMatrix());
    Mat4f& tex0mat = activeTexTransform[0];
    if (data->mat & MAT_TEXMATRIX_STAGE1) {
        MatXf& m = data->TexMatrix;
        tex0mat.xx = m.rot()[0][0]; tex0mat.xy = m.rot()[0][1];
        tex0mat.yx = m.rot()[1][0]; tex0mat.yy = m.rot()[1][1];
        tex0mat.zx = m.trans().x;   tex0mat.zy = m.trans().y;
    } else {
        tex0mat = Mat4f::ID;
    }
    activeTexTransform[1] = Mat4f::ID;
    SubmitDrawBuffer(Tri->db, &Tri->dbr);
    activeTexTransform[0] = Mat4f::ID;
    activeTexTransform[1] = Mat4f::ID;
}

// ---- BeginDrawMesh / EndDrawMesh ----------------------------------------

void cVulkanRender::BeginDrawMesh(bool obj_mesh, bool use_shadow_) {
    use_shadow = use_shadow_;
    pShadowCamera = GetDrawNode()->FindCildCamera(ATTRCAMERA_SHADOWMAP);
}

void cVulkanRender::EndDrawMesh() {
    FlushDeferredBatch();
}

// ---- BeginDrawShadow / EndDrawShadow ------------------------------------

void cVulkanRender::BeginDrawShadow(bool shadow_map) {
    FlushDeferredBatch();
    cCamera* camera = GetDrawNode()->FindCildCamera(ATTRCAMERA_SHADOWMAP);
    if (pShadowCamera != camera) {
        FinishActiveDrawBuffer();
        pShadowCamera = camera;
    }
    activeRenderTarget = shadowMapRenderTarget;
}

void cVulkanRender::EndDrawShadow() {
    FlushDeferredBatch();
    if (pShadowCamera != nullptr) {
        FinishActiveDrawBuffer();
        pShadowCamera = nullptr;
    }
    activeRenderTarget = nullptr;
}

// ---- SetSimplyMaterialShadow / DrawNoMaterialShadow ---------------------

void cVulkanRender::SetSimplyMaterialShadow(cObjMesh* mesh, cTexture* texture) {
    FlushDeferredBatch();
    SetTexture(0, texture, 0);
    eBlendMode blend = ALPHA_NONE;
    if (texture && texture->IsAlphaTest()) blend = ALPHA_TEST;
    if (texture && texture->IsAlpha())     blend = ALPHA_BLEND;
    SetBlendState(blend);
    if (activePipelineType != VULKAN_PIPELINE_TYPE_OBJECT_SHADOW) {
        FinishActiveDrawBuffer();
    }
    activePipelineType = VULKAN_PIPELINE_TYPE_OBJECT_SHADOW;
    EnterDeferredMode();
}

void cVulkanRender::DrawNoMaterialShadow(cObjMesh* mesh) {
    cMeshTri* Tri = mesh->GetTri();
    if (m_deferredMode) {
        Mat4f world;
        Mat4fSetTransposedMatXf(world, mesh->GetGlobalMatrix());
        VULKAN_SHADER_ID shaderId = ChooseShader(
            Tri->db->vb.fmt, activePipelineType, VULKAN_PIPELINE_TARGET_SHADOWMAP);
        size_t vsSize = 64, fsSize = 16;
        GetUBOSizes(shaderId, vsSize, fsSize);

        DeferredMeshDraw d = {};
        d.vb        = &Tri->db->vb;
        d.vertices  = Tri->db->written_vertices;
        d.ib        = &Tri->db->ib;
        d.indices   = Tri->db->written_indices;
        d.dbrOffset = (uint32_t)Tri->dbr.offset;
        d.dbrLen    = Tri->dbr.len;
        d.worldMatrix = world;
        d.shaderId  = shaderId;
        d.vsSize    = vsSize;
        d.fsSize    = fsSize;
        m_deferredDraws.push_back(d);
        return;
    }

    SetWorldMatXf(mesh->GetGlobalMatrix());
    SubmitDrawBuffer(Tri->db, &Tri->dbr);
}

// ---- SetMaterialTilemap / SetMaterialTilemapShadow ----------------------

void cVulkanRender::SetMaterialTilemap(cTileMap* TileMap) {
    if (activePipelineType == VULKAN_PIPELINE_TYPE_TILE_MAP) return;
    FinishActiveDrawBuffer();
    activePipelineType = VULKAN_PIPELINE_TYPE_TILE_MAP;

    auto* pShadowMap = shadowMapRenderTarget ? shadowMapRenderTarget->texture : nullptr;
    auto* pLightMap  = lightMapRenderTarget  ? lightMapRenderTarget->texture  : nullptr;

    // Bind shadow map (slot 0) and lightmap (slot 2) when available.
    // If either is absent, slot keeps whatever was previously set (emptyTexture
    // is the fallback: its depth=1.0 equivalent means fully-lit, no shadow).
    if (pShadowMap) {
        SetTextureImage(0, pShadowMap->GetFrameImage(0));
    }
    if (pLightMap) {
        SetTextureImage(2, pLightMap->GetFrameImage(0));
    }

    if (pShadowCamera && pShadowMap) {
        float fOffsetX = 0.5f + (0.0f / pShadowMap->GetWidth());
        float fOffsetY = 0.5f + (0.0f / pShadowMap->GetWidth());
        float range = 1.0f;
        float fBias = -0.0001f * range;
        Mat4f matTexAdj(
             0.5f,      0.0f,     0.0f,  0.0f,
             0.0f,     -0.5f,     0.0f,  0.0f,
             0.0f,      0.0f,     range, 0.0f,
             fOffsetX,  fOffsetY, fBias, 1.0f
        );
        activeShadowMatrix = pShadowCamera->matViewProj * matTexAdj;
    }

    if (TileMap) {
        TerraInterface* terra = TileMap->GetTerra();
        if (terra) activeWorldSize = Vect2f(1.0f / terra->SizeX(), 1.0f / terra->SizeY());
    }
}

void cVulkanRender::SetMaterialTilemapShadow() {
    if (activePipelineType != VULKAN_PIPELINE_TYPE_OBJECT_SHADOW) {
        FinishActiveDrawBuffer();
    }
    activePipelineType = VULKAN_PIPELINE_TYPE_OBJECT_SHADOW;
}

void cVulkanRender::SetTileColor(sColor4f color) {
    if (!(activeTileColor == color)) {
        FinishActiveDrawBuffer();
        activeTileColor = color;
    }
}

// ---- SetTexture (helpers) -----------------------------------------------

// SetTexture is handled by the base class: it calls SetTextureImage which sets activeTextures[slot]

void cVulkanRender::SetTextureImage(uint32_t slot, struct TextureImage* ti) {
    if (slot >= VULKAN_TEXTURES) return;
    VulkanTexture2D* tex = ti ? ti->vk : nullptr;
    if (activeTextures[slot] != tex) {
        FinishActiveDrawBuffer();
        activeTextures[slot] = tex;
    }
}

void cVulkanRender::SetTextureTransform(uint32_t slot, const Mat4f& transform) {
    if (slot >= VULKAN_TEXTURES) return;
    if (!activeTexTransform[slot].eq(transform, 0)) {
        FinishActiveDrawBuffer();
        activeTexTransform[slot] = transform;
    }
}

// ---- SetColorMode -------------------------------------------------------

void cVulkanRender::SetColorMode(eColorMode mode) {
    if (activeColorMode != mode) {
        FinishActiveDrawBuffer();
        activeColorMode = mode;
    }
}

// ---- SetTex2Lerp --------------------------------------------------------

void cVulkanRender::SetTex2Lerp(float lerp) {
    if (activeTex2Lerp != lerp) {
        FinishActiveDrawBuffer();
        activeTex2Lerp = lerp;
    }
}

// ---- DrawSprite2 override (must set material before base class draw) ---

void cVulkanRender::DrawSprite2(int x1, int y1, int dx, int dy,
                                 float u0, float v0, float du0, float dv0,
                                 float u1, float v1, float du1, float dv1,
                                 cTexture* Tex1, cTexture* Tex2, float lerp_factor,
                                 float alpha, float phase,
                                 eColorMode mode, eBlendMode blend_mode) {
    if (!Tex1 || !Tex2) return;
    if (blend_mode == ALPHA_NONE && alpha < 1.0f) blend_mode = ALPHA_BLEND;
    SetTex2Lerp(lerp_factor);
    SetNoMaterial(blend_mode, phase, Tex2, Tex1, mode);
    cInterfaceRenderDevice::DrawSprite2(x1, y1, dx, dy,
                                        u0, v0, du0, dv0, u1, v1, du1, dv1,
                                        Tex1, Tex2, lerp_factor, alpha, phase, mode, blend_mode);
    SetTex2Lerp(-1.0f);
}

// ---- OutText overrides (must set font texture before base class draw) ---

void cVulkanRender::OutText(int x, int y, const char* string, const sColor4f& color,
                             int align, eBlendMode blend_mode) {
    if (!CurrentFont) return;
    SetNoMaterial(blend_mode, 0, CurrentFont->GetTexture());
    cInterfaceRenderDevice::OutText(x, y, string, color, align, blend_mode);
}

void cVulkanRender::OutText(int x, int y, const char* string, const sColor4f& color,
                             int align, eBlendMode blend_mode,
                             cTexture* pTexture, eColorMode mode, Vect2f uv, Vect2f duv,
                             float phase, float lerp_factor) {
    if (!CurrentFont) return;
    SetTex2Lerp(lerp_factor);
    SetNoMaterial(blend_mode, phase, pTexture, CurrentFont->GetTexture(), mode);
    cInterfaceRenderDevice::OutText(x, y, string, color, align, blend_mode,
                                    pTexture, mode, uv, duv, phase, lerp_factor);
    SetTex2Lerp(-1.0f);
}
