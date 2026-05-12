#include "StdAfxRD.h"
#include "VulkanRender.h"
#include "VulkanPipeline.h"
#include "VulkanResources.h"
#include "Texture.h"
#include "DrawBuffer.h"
#include "ObjMesh.h"
#include "RenderTracker.h"
#include "TexLibrary.h"
#include "cCamera.h"
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <chrono>

using hrc = std::chrono::high_resolution_clock;

static const size_t STAGE_ALIGN = 16;

struct ProfilingState {
    hrc::time_point t0;
    hrc::time_point beginSceneEnd{};  // Set at end of BeginScene for logic-time calc
    int    frameCount = 0;

    double fenceUs   = 0;
    double cleanupUs = 0;
    double logicUs   = 0;
    double texUpUs   = 0;
    double shadowUs  = 0;
    double lightUs   = 0;
    double mainUs    = 0;
    double acquireUs = 0;
    double submitUs  = 0;
    double presentUs = 0;

    size_t texUpCount    = 0;
    size_t shadowDraws   = 0;
    size_t lightDraws    = 0;
    size_t mainDraws     = 0;
    size_t descAllocs    = 0;
    size_t pipeCreates   = 0;
    size_t drawCalls     = 0;

    // Per-draw phase breakdown (accumulated us over sampling interval)
    double cmdUBOus  = 0;
    double cmdDescUs = 0;
    double cmdPipeUs = 0;
    double cmdVBUs   = 0;
    double cmdIBUs   = 0;

    // FlushDeferredBatch phase breakdown
    double   batchGroupUs  = 0; // hash-map grouping
    double   batchUploadUs = 0; // VB/IB PrepareStaticBuffer
    double   batchUboAllocUs = 0; // UBO alloc + FillUBOData
    double   batchDescUs   = 0; // BuildDescriptorSet
    double   batchPipeUs    = 0; // GetOrCreatePipeline
    double   batchInstUs    = 0; // instance data ring write
    double   batchCmdUs     = 0; // VulkanCommand assembly + push_back
    size_t   batchGroups    = 0;
    size_t   batchInstanced = 0;
    size_t   batchFallback  = 0;

    void reset() {
        fenceUs = cleanupUs = logicUs = texUpUs = shadowUs = lightUs = 0;
        mainUs = acquireUs = submitUs = presentUs = 0;
        texUpCount = shadowDraws = lightDraws = mainDraws = 0;
        descAllocs = pipeCreates = drawCalls = 0;
        cmdUBOus = cmdDescUs = cmdPipeUs = cmdVBUs = cmdIBUs = 0;
        batchGroupUs = batchUploadUs = batchUboAllocUs = batchDescUs = 0;
        batchPipeUs = batchInstUs = batchCmdUs = 0;
        batchGroups = batchInstanced = batchFallback = 0;
    }
};
static ProfilingState s_ps;

// Called from VulkanRenderDraw.cpp
void cVulkanRender::ProfRecordDescAlloc()  { if (profilingEnabled) s_ps.descAllocs++;  }
void cVulkanRender::ProfRecordPipeCreate() { if (profilingEnabled) s_ps.pipeCreates++; }
void cVulkanRender::ProfRecordDrawCall()   { if (profilingEnabled) s_ps.drawCalls++;   }
void cVulkanRender::ProfRecordBatchPhase(size_t groups, size_t instanced, size_t fallback,
                                          double groupUs, double uploadUs, double uboUs,
                                          double descUs, double pipeUs, double instUs, double cmdUs) {
    if (profilingEnabled) {
        s_ps.batchGroups    += groups;
        s_ps.batchInstanced += instanced;
        s_ps.batchFallback  += fallback;
        s_ps.batchGroupUs   += groupUs;
        s_ps.batchUploadUs  += uploadUs;
        s_ps.batchUboAllocUs += uboUs;
        s_ps.batchDescUs    += descUs;
        s_ps.batchPipeUs    += pipeUs;
        s_ps.batchInstUs    += instUs;
        s_ps.batchCmdUs     += cmdUs;
    }
}
void cVulkanRender::ProfRecordCmdPhase(double ubo, double desc, double pipe, double vb, double ib) {
    if (profilingEnabled) {
        s_ps.cmdUBOus  += ubo;
        s_ps.cmdDescUs += desc;
        s_ps.cmdPipeUs += pipe;
        s_ps.cmdVBUs   += vb;
        s_ps.cmdIBUs   += ib;
    }
}

static double us_since(hrc::time_point start) {
    return std::chrono::duration<double, std::micro>(hrc::now() - start).count();
}

void cVulkanRender::ProfReport() {
    auto now = hrc::now();
    double elapsedSec = std::chrono::duration<double>(now - s_ps.t0).count();
    int n = s_ps.frameCount ? s_ps.frameCount : 1;

    double fps = n / elapsedSec;
    double msPerFrame = 1000.0 * elapsedSec / n;
    double inv = 1.0 / n; // per-frame fraction

    fprintf(stderr,
        "[PROF] %.0f fps  %zu draws  %.2f ms/frame  (%d frames)\n"
        "  fence=%.0fus  cleanup=%.0fus  logic=%.0fus  texUp/%zu=%.0fus\n"
        "  acquire=%.0fus  sh/%zu=%.0fus  lt/%zu=%.0fus  main/%zu=%.0fus\n"
        "  submit=%.0fus  present=%.0fus  descAlloc=%zu  pipeNew=%zu\n"
        "  cmd-breakdown: ubo=%.0fus  desc=%.0fus  pipe=%.0fus  vb=%.0fus  ib=%.0fus\n"
        "  batch: groups=%zu  instanced=%zu  fallback=%zu  group=%.0fus  upload=%.0fus\n"
        "         ubo=%.0fus  desc=%.0fus  pipe=%.0fus  inst=%.0fus  cmd=%.0fus\n",
        fps, s_ps.drawCalls, msPerFrame, n,
        s_ps.fenceUs * inv, s_ps.cleanupUs * inv, s_ps.logicUs * inv,
        s_ps.texUpCount, s_ps.texUpUs * inv,
        s_ps.acquireUs * inv,
        s_ps.shadowDraws, s_ps.shadowUs * inv,
        s_ps.lightDraws, s_ps.lightUs * inv,
        s_ps.mainDraws, s_ps.mainUs * inv,
        s_ps.submitUs * inv, s_ps.presentUs * inv,
        s_ps.descAllocs, s_ps.pipeCreates,
        s_ps.cmdUBOus * inv, s_ps.cmdDescUs * inv, s_ps.cmdPipeUs * inv,
        s_ps.cmdVBUs * inv, s_ps.cmdIBUs * inv,
        s_ps.batchGroups, s_ps.batchInstanced, s_ps.batchFallback,
        s_ps.batchGroupUs * inv, s_ps.batchUploadUs * inv,
        s_ps.batchUboAllocUs * inv, s_ps.batchDescUs * inv,
        s_ps.batchPipeUs * inv, s_ps.batchInstUs * inv, s_ps.batchCmdUs * inv);

    s_ps.t0 = now;
    s_ps.frameCount = 0;
    s_ps.reset();
}

// Track BeginScene fence-wait and cleanup separately from the rest of logic.
void cVulkanRender::ProfBeginSceneFence(double us)  { if (profilingEnabled) s_ps.fenceUs   += us; }
void cVulkanRender::ProfBeginSceneCleanup(double us) { if (profilingEnabled) s_ps.cleanupUs += us; }
void cVulkanRender::ProfLogicTime(double us)         { if (profilingEnabled) s_ps.logicUs   += us; }

// ---- BeginScene / EndScene ---------------------------------------------

int cVulkanRender::BeginScene() {
    RenderSubmitEvent(RenderEvent::BEGIN_SCENE, "");
    if (activeScene) { xassert(0); return 1; }
    activeScene = true;
    needClearColor = false;
    needClearDepth = false;

    auto& frame = frames[currentFrame];
    hrc::time_point t0{}, t1{};

    if (profilingEnabled) t0 = hrc::now();

    VkResult fenceStatus = vkGetFenceStatus(device, frame.fence);
    if (fenceStatus == VK_NOT_READY) {
        VkResult waitResult = vkWaitForFences(device, 1, &frame.fence, VK_TRUE, 1'000'000'000u);
        if (waitResult == VK_TIMEOUT) {
            fprintf(stderr, "Vulkan: GPU fence timeout (1s) for frame slot %u - possible hang\n",
                    currentFrame);
        }
    }

    if (profilingEnabled) { t1 = hrc::now(); s_ps.fenceUs += us_since(t0); t0 = t1; }

    for (auto* b : frame.dynamicBuffers) delete b;
    frame.dynamicBuffers.clear();

    for (auto& [sbuf, smem] : frame.stagingBuffers) {
        vkDestroyBuffer(device, sbuf, nullptr);
        vkFreeMemory(device, smem, nullptr);
    }
    frame.stagingBuffers.clear();

    vkResetDescriptorPool(device, frame.descriptorPool, 0);
    frame.texDescCache.clear();

    globalFrameIndex++;

    {
        auto it = std::remove_if(pendingTextureDeletions.begin(), pendingTextureDeletions.end(),
            [this](const DeferredTextureDeletion& d) {
                if (globalFrameIndex >= d.safeAfterFrame) { delete d.tex; return true; }
                return false;
            });
        pendingTextureDeletions.erase(it, pendingTextureDeletions.end());
    }
    {
        auto it = std::remove_if(pendingBufferDeletions.begin(), pendingBufferDeletions.end(),
            [this](const DeferredBufferDeletion& d) {
                if (globalFrameIndex >= d.safeAfterFrame) { delete d.buf; return true; }
                return false;
            });
        pendingBufferDeletions.erase(it, pendingBufferDeletions.end());
    }

    frame.uboOffset    = 0;
    frame.dynVtxOffset = 0;
    frame.dynIdxOffset = 0;
    frame.stageOffset  = 0;
    frame.instOffset   = 0;
    frame.vtxCache.clear();
    frame.idxCache.clear();
    cachedMvpDirty     = true;

    if (profilingEnabled) { s_ps.cleanupUs += us_since(t0); s_ps.beginSceneEnd = hrc::now(); }

    return cInterfaceRenderDevice::BeginScene();
}

int cVulkanRender::EndScene() {
    RenderSubmitEvent(RenderEvent::END_SCENE, "");
    if (!activeScene) { xassert(0); return 1; }

    FlushActiveCommand();
    activeScene = false;

    // Capture logic time (BeginScene->EndScene minus fence+cleanup) before render.
    // DoVulkanRendering will use this to print the breakdown.
    hrc::time_point t1 = hrc::now();
    if (profilingEnabled && s_ps.beginSceneEnd != hrc::time_point{}) {
        s_ps.logicUs += us_since(s_ps.beginSceneEnd);
        s_ps.beginSceneEnd = hrc::time_point{};
    }

    DoVulkanRendering();
    return cInterfaceRenderDevice::EndScene();
}

// ---- DoVulkanRendering (submit frame) -----------------------------------

void cVulkanRender::DoVulkanRendering() {
    auto& frame = frames[currentFrame];

    hrc::time_point t0{};
    if (profilingEnabled) t0 = hrc::now();

    VkResult result = vkAcquireNextImageKHR(device, swapchain, 2'000'000'000u,
                                              frame.imageAvailSemaphore,
                                              VK_NULL_HANDLE, &currentImageIndex);
    if (result == VK_TIMEOUT) {
        fprintf(stderr, "Vulkan: vkAcquireNextImageKHR timeout (2s)\n");
        currentFrame = (currentFrame + 1) % VULKAN_FRAMES_IN_FLIGHT;
        RecreateSwapchain();
        return;
    }
    if (profilingEnabled) s_ps.acquireUs += us_since(t0);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        ClearAllCommands();
        // Keep pending upload queues: CreateVulkanCommand already cleared
        // the source VB/IB dirty flags, so PrepareStaticBuffer won't fire
        // again.  The uploads will be drained onto the next non-OOD frame's
        // command buffer.  (Textures created during a resize-storm are rare;
        // they drain on the first good frame - no unbounded growth.)
        currentFrame = (currentFrame + 1) % VULKAN_FRAMES_IN_FLIGHT;
        RecreateSwapchain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        ClearAllCommands();
        // Keep pending uploads for the same reason as OOD above.
        currentFrame = (currentFrame + 1) % VULKAN_FRAMES_IN_FLIGHT;
        return;
    }

    vkResetFences(device, 1, &frame.fence);
    vkResetCommandBuffer(frame.commandBuffer, 0);

    VkCommandBufferBeginInfo bi = {};
    bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VkResult beginRes = vkBeginCommandBuffer(frame.commandBuffer, &bi);
    if (beginRes != VK_SUCCESS) {
        fprintf(stderr, "Vulkan: vkBeginCommandBuffer failed: %d\n", (int)beginRes);
        currentFrame = (currentFrame + 1) % VULKAN_FRAMES_IN_FLIGHT;
        ClearAllCommands();
        return;
    }

    // Flush deferred texture uploads
    if (!pendingTextureUploads.empty()) {
        if (profilingEnabled) t0 = hrc::now();

        VkCommandBuffer cb = frame.commandBuffer;
        for (auto& upload : pendingTextureUploads) {
            if (upload.layoutOnly) {
                TransitionImageLayout(cb, upload.image, VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                      upload.aspect, upload.mipCount);
                continue;
            }
            const int mipCount = (int)upload.mips.size();

            // Phase 1: copy all mip data to the staging ring (or fallback
            // buffers if the ring is exhausted).  Collect per-mip offsets.
            VkDeviceSize mipOffsets[16];   // max mipCount bounded at 16
            VkBuffer     mipSrcBufs[16];
            bool useFallback = false;
            for (int nMip = 0; nMip < mipCount; nMip++) {
                const auto& mip = upload.mips[nMip];
                size_t mipSize = mip.data.size();
                size_t alignedSize = (mipSize + STAGE_ALIGN - 1) & ~(STAGE_ALIGN - 1);
                if (!useFallback && frame.stageOffset + alignedSize <= VULKAN_STAGE_POOL_SIZE) {
                    mipOffsets[nMip] = (VkDeviceSize)frame.stageOffset;
                    frame.stageOffset += alignedSize;
                    memcpy(static_cast<uint8_t*>(frame.stageMapped) + mipOffsets[nMip],
                           mip.data.data(), mipSize);
                    mipSrcBufs[nMip] = frame.stageBuffer;
                } else {
                    useFallback = true;
                    // Fallback: create a dedicated staging buffer for this mip.
                    VkBufferCreateInfo bi2 = {};
                    bi2.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                    bi2.size        = mipSize;
                    bi2.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                    bi2.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    VkBuffer tmpBuf = VK_NULL_HANDLE;
                    VkDeviceMemory tmpMem = VK_NULL_HANDLE;
                    if (vkCreateBuffer(device, &bi2, nullptr, &tmpBuf) != VK_SUCCESS)
                        goto next_upload;
                    VkMemoryRequirements mr2;
                    vkGetBufferMemoryRequirements(device, tmpBuf, &mr2);
                    VkMemoryAllocateInfo ai2 = {};
                    ai2.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                    ai2.allocationSize  = mr2.size;
                    ai2.memoryTypeIndex = FindMemoryType(mr2.memoryTypeBits,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                    if (vkAllocateMemory(device, &ai2, nullptr, &tmpMem) != VK_SUCCESS) {
                        vkDestroyBuffer(device, tmpBuf, nullptr);
                        goto next_upload;
                    }
                    if (vkBindBufferMemory(device, tmpBuf, tmpMem, 0) != VK_SUCCESS) {
                        vkFreeMemory(device, tmpMem, nullptr);
                        vkDestroyBuffer(device, tmpBuf, nullptr);
                        goto next_upload;
                    }
                    void* mp;
                    if (vkMapMemory(device, tmpMem, 0, mipSize, 0, &mp) != VK_SUCCESS) {
                        vkFreeMemory(device, tmpMem, nullptr);
                        vkDestroyBuffer(device, tmpBuf, nullptr);
                        goto next_upload;
                    }
                    memcpy(mp, mip.data.data(), mipSize);
                    vkUnmapMemory(device, tmpMem);
                    frame.stagingBuffers.push_back({tmpBuf, tmpMem});
                    mipOffsets[nMip] = 0;
                    mipSrcBufs[nMip] = tmpBuf;
                }
            }

            // Phase 2: one pre-copy barrier for ALL mips.
            TransitionImageLayout(cb, upload.image, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  upload.aspect, mipCount, 0);

            // Phase 3: copy all mips to the image.
            for (int nMip = 0; nMip < mipCount; nMip++) {
                VkBufferImageCopy region = {};
                region.bufferOffset                    = mipOffsets[nMip];
                region.imageSubresource.aspectMask     = upload.aspect;
                region.imageSubresource.mipLevel       = nMip;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount     = 1;
                region.imageExtent = { (uint32_t)upload.mips[nMip].w,
                                       (uint32_t)upload.mips[nMip].h, 1 };
                vkCmdCopyBufferToImage(cb, mipSrcBufs[nMip], upload.image,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
            }

            // Phase 4: one post-copy barrier for ALL mips.
            TransitionImageLayout(cb, upload.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                  upload.aspect, mipCount, 0);
            next_upload: ;
        }
        if (profilingEnabled) { s_ps.texUpCount += pendingTextureUploads.size(); s_ps.texUpUs += us_since(t0); }
        pendingTextureUploads.clear();
    }

    // Flush deferred VB/IB uploads (staging->DEVICE_LOCAL) onto the frame CB.
    // The staging buffers are kept alive until BeginScene drains
    // frame.stagingBuffers after the per-slot fence - that guarantees the GPU
    // is done with them.
    if (!pendingBufferUploads.empty()) {
        VkCommandBuffer cb = frame.commandBuffer;
        for (auto& up : pendingBufferUploads) {
            const VkDeviceSize len = (VkDeviceSize)up.data.size();
            if (len == 0 || up.dstBuffer == VK_NULL_HANDLE) continue;

            // Try to suballocate from the per-frame staging ring; fall back to
            // a dedicated staging buffer only if the ring is full this frame.
            VkBuffer     srcBuf = VK_NULL_HANDLE;
            VkDeviceSize srcOff = 0;
            const size_t alignedSize = (size_t)((len + STAGE_ALIGN - 1) & ~(STAGE_ALIGN - 1));
            if (frame.stageOffset + alignedSize <= VULKAN_STAGE_POOL_SIZE) {
                srcOff = (VkDeviceSize)frame.stageOffset;
                memcpy(static_cast<uint8_t*>(frame.stageMapped) + srcOff,
                       up.data.data(), (size_t)len);
                frame.stageOffset += alignedSize;
                srcBuf = frame.stageBuffer;
            } else {
                VkBufferCreateInfo bi2 = {};
                bi2.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bi2.size        = len;
                bi2.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                bi2.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                VkBuffer tmpBuf = VK_NULL_HANDLE;
                VkDeviceMemory tmpMem = VK_NULL_HANDLE;
                if (vkCreateBuffer(device, &bi2, nullptr, &tmpBuf) != VK_SUCCESS) continue;
                VkMemoryRequirements mr2;
                vkGetBufferMemoryRequirements(device, tmpBuf, &mr2);
                VkMemoryAllocateInfo ai2 = {};
                ai2.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                ai2.allocationSize  = mr2.size;
                ai2.memoryTypeIndex = FindMemoryType(mr2.memoryTypeBits,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                if (vkAllocateMemory(device, &ai2, nullptr, &tmpMem) != VK_SUCCESS) {
                    vkDestroyBuffer(device, tmpBuf, nullptr);
                    continue;
                }
                if (vkBindBufferMemory(device, tmpBuf, tmpMem, 0) != VK_SUCCESS) {
                    vkFreeMemory(device, tmpMem, nullptr);
                    vkDestroyBuffer(device, tmpBuf, nullptr);
                    continue;
                }
                void* mp;
                if (vkMapMemory(device, tmpMem, 0, len, 0, &mp) != VK_SUCCESS) {
                    vkFreeMemory(device, tmpMem, nullptr);
                    vkDestroyBuffer(device, tmpBuf, nullptr);
                    continue;
                }
                memcpy(mp, up.data.data(), (size_t)len);
                vkUnmapMemory(device, tmpMem);
                frame.stagingBuffers.push_back({tmpBuf, tmpMem});
                srcBuf = tmpBuf;
                srcOff = 0;
            }

            VkBufferCopy region = { srcOff, 0, len };
            vkCmdCopyBuffer(cb, srcBuf, up.dstBuffer, 1, &region);

            // Transfer->vertex/index read barrier.
            VkBufferMemoryBarrier b = {};
            b.sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            b.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
            b.dstAccessMask       = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
                                  | VK_ACCESS_INDEX_READ_BIT;
            b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            b.buffer              = up.dstBuffer;
            b.offset              = 0;
            b.size                = VK_WHOLE_SIZE;
            vkCmdPipelineBarrier(cb,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                0, 0, nullptr, 1, &b, 0, nullptr);
        }
        pendingBufferUploads.clear();
    }

    // Shadow map pass
    if (shadowMapRenderTarget) {
        if (profilingEnabled) t0 = hrc::now();
        ProcessRenderPass(shadowRenderPass, shadowMapRenderTarget->framebuffer,
                          shadowMapRenderTarget->extent, false, true,
                          shadowMapRenderTarget->commands);
        if (profilingEnabled) { s_ps.shadowUs += us_since(t0); s_ps.shadowDraws += shadowMapRenderTarget->commands.size(); }
    }
    // Light map pass
    if (lightMapRenderTarget) {
        if (profilingEnabled) t0 = hrc::now();
        ProcessRenderPass(lightmapRenderPass, lightMapRenderTarget->framebuffer,
                          lightMapRenderTarget->extent, true, false,
                          lightMapRenderTarget->commands);
        if (profilingEnabled) { s_ps.lightUs += us_since(t0); s_ps.lightDraws += lightMapRenderTarget->commands.size(); }
    }
    // Main swapchain pass
    VkExtent2D ext = swapchainExtent;
    if (profilingEnabled) t0 = hrc::now();
    ProcessRenderPass(mainRenderPass, swapchainFramebuffers[currentImageIndex],
                      ext, needClearColor, true, swapchainCommands);
    if (profilingEnabled) { s_ps.mainUs += us_since(t0); s_ps.mainDraws += swapchainCommands.size(); }

    VkResult endResult = vkEndCommandBuffer(frame.commandBuffer);
    if (endResult != VK_SUCCESS) {
        fprintf(stderr, "Vulkan: vkEndCommandBuffer failed: %d\n", (int)endResult);
        lastSubmitSucceeded = false;
        currentFrame = (currentFrame + 1) % VULKAN_FRAMES_IN_FLIGHT;
        ClearAllCommands();
        return;
    }

    // Submit
    if (profilingEnabled) t0 = hrc::now();
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo si = {};
    si.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.waitSemaphoreCount   = 1;
    si.pWaitSemaphores      = &frame.imageAvailSemaphore;
    si.pWaitDstStageMask    = &waitStage;
    si.commandBufferCount   = 1;
    si.pCommandBuffers      = &frame.commandBuffer;
    si.signalSemaphoreCount = 1;
    si.pSignalSemaphores    = &frame.renderDoneSemaphore;
    VkResult submitResult = vkQueueSubmit(graphicsQueue, 1, &si, frame.fence);
    if (submitResult != VK_SUCCESS) {
        fprintf(stderr, "Vulkan: vkQueueSubmit failed: %d\n", (int)submitResult);
        ClearAllCommands();
        currentFrame = (currentFrame + 1) % VULKAN_FRAMES_IN_FLIGHT;
        return;
    }
    lastSubmitSucceeded = true;
    if (profilingEnabled) s_ps.submitUs += us_since(t0);

    s_ps.frameCount++;
    if (profilingEnabled) {
        // Lazy-init t0 on first profiled frame
        if (s_ps.t0 == hrc::time_point{}) s_ps.t0 = hrc::now();
        double elapsedSec = std::chrono::duration<double>(hrc::now() - s_ps.t0).count();
        if (elapsedSec >= 5.0) {
            ProfReport();
        }
    }

    currentFrame = (currentFrame + 1) % VULKAN_FRAMES_IN_FLIGHT;
    ClearAllCommands();
}

// ---- ProcessRenderPass --------------------------------------------------

void cVulkanRender::ProcessRenderPass(VkRenderPass renderPass,
                                       VkFramebuffer framebuffer,
                                       VkExtent2D extent,
                                       bool clearColor, bool clearDepth,
                                       std::vector<VulkanCommand>& commands) {
    if (renderPass == VK_NULL_HANDLE || framebuffer == VK_NULL_HANDLE) return;
    // ProcessRenderPass is always called from DoVulkanRendering before currentFrame
    // is incremented, so frames[currentFrame].commandBuffer is the active CB.

    VkClearValue clearValues[2];
    uint32_t clearCount = 0;
    if (clearColor) {
        if (renderPass == lightmapRenderPass) {
            clearValues[clearCount].color = { 1.0f, 1.0f, 1.0f, 1.0f };
        } else {
            clearValues[clearCount].color = { fillColor.r, fillColor.g, fillColor.b, fillColor.a };
        }
        clearCount++;
    } else if (renderPass == mainRenderPass || renderPass == lightmapRenderPass) {
        clearValues[clearCount].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearCount++;
    }
    if (clearDepth || renderPass == shadowRenderPass) {
        clearValues[clearCount].depthStencil = { 1.0f, 0 };
        clearCount++;
    } else if (renderPass == mainRenderPass) {
        clearValues[clearCount].depthStencil = { 1.0f, 0 };
        clearCount++;
    }

    VkRenderPassBeginInfo rpbi = {};
    rpbi.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpbi.renderPass        = renderPass;
    rpbi.framebuffer       = framebuffer;
    rpbi.renderArea.offset = { 0, 0 };
    rpbi.renderArea.extent = extent;
    rpbi.clearValueCount   = clearCount;
    rpbi.pClearValues      = clearValues;

    // Re-get the active command buffer (recording was started before this call)
    VkCommandBuffer activeCB = frames[currentFrame].commandBuffer;

    vkCmdBeginRenderPass(activeCB, &rpbi, VK_SUBPASS_CONTENTS_INLINE);

    // Default viewport and scissor
    VkViewport vp = {};
    vp.x = 0.0f; vp.y = 0.0f;
    vp.width  = (float)extent.width;
    vp.height = (float)extent.height;
    vp.minDepth = 0.0f; vp.maxDepth = 1.0f;
    vkCmdSetViewport(activeCB, 0, 1, &vp);

    VkRect2D scissor = { {0, 0}, extent };
    vkCmdSetScissor(activeCB, 0, 1, &scissor);

    VkPipeline     lastPipeline      = VK_NULL_HANDLE;
    VkDescriptorSet lastDescSet      = VK_NULL_HANDLE;
    uint32_t        lastUboOffs[2]   = { UINT32_MAX, UINT32_MAX };
    VkBuffer        lastVtxBuf       = VK_NULL_HANDLE;
    VkDeviceSize    lastVtxByteOff   = UINT64_MAX;
    VkBuffer        lastIdxBuf       = VK_NULL_HANDLE;
    VkDeviceSize    lastIdxOff       = UINT64_MAX;
    VkBuffer        lastInstBuf      = VK_NULL_HANDLE;
    VkDeviceSize    lastInstOff      = UINT64_MAX;
    bool lastViewportWasCustom = false;
    bool lastScissorWasClip    = false;

    for (auto& cmd : commands) {

        if (cmd.clearDepth) {
            // Depth-clear sentinel emitted by ClearZBuffer(). Only valid for
            // passes that have a depth attachment (main swapchain pass).
            if (renderPass == mainRenderPass) {
                VkClearRect cr = {};
                if (cmd.hasViewport) {
                    cr.rect.offset = { cmd.viewport[0].x, cmd.viewport[0].y };
                    cr.rect.extent = { (uint32_t)cmd.viewport[1].x,
                                       (uint32_t)cmd.viewport[1].y };
                } else {
                    cr.rect = { {0, 0}, extent };
                }
                cr.baseArrayLayer = 0;
                cr.layerCount     = 1;
                VkClearAttachment att = {};
                att.aspectMask              = VK_IMAGE_ASPECT_DEPTH_BIT;
                att.clearValue.depthStencil = { 1.0f, 0 };
                vkCmdClearAttachments(activeCB, 1, &att, 1, &cr);
            }
            continue;
        }

        if (cmd.vertexCount < 3 && cmd.indexCount < 3) continue;
        if (!cmd.pipeline || cmd.pipeline->pipeline == VK_NULL_HANDLE) continue;
        if (cmd.vertexBuffer == VK_NULL_HANDLE) continue;

        // Apply viewport
        if (cmd.hasViewport) {
            VkViewport cmdVp = {};
            cmdVp.x = (float)cmd.viewport[0].x;
            cmdVp.y = (float)cmd.viewport[0].y;
            cmdVp.width  = (float)cmd.viewport[1].x;
            cmdVp.height = (float)cmd.viewport[1].y;
            cmdVp.minDepth = 0.0f; cmdVp.maxDepth = 1.0f;
            vkCmdSetViewport(activeCB, 0, 1, &cmdVp);
            lastViewportWasCustom = true;
        } else if (lastViewportWasCustom) {
            VkViewport cmdVp = {};
            cmdVp.x = 0.0f; cmdVp.y = 0.0f;
            cmdVp.width  = (float)extent.width;
            cmdVp.height = (float)extent.height;
            cmdVp.minDepth = 0.0f; cmdVp.maxDepth = 1.0f;
            vkCmdSetViewport(activeCB, 0, 1, &cmdVp);
            lastViewportWasCustom = false;
        }
        if (cmd.hasClip) {
            VkRect2D sc = { { cmd.clip[0].x, cmd.clip[0].y },
                            { (uint32_t)cmd.clip[1].x, (uint32_t)cmd.clip[1].y } };
            vkCmdSetScissor(activeCB, 0, 1, &sc);
            lastScissorWasClip = true;
        } else if (lastScissorWasClip) {
            VkRect2D sc = { {0, 0}, extent };
            vkCmdSetScissor(activeCB, 0, 1, &sc);
            lastScissorWasClip = false;
        }

        // Bind pipeline
        if (cmd.pipeline->pipeline != lastPipeline) {
            vkCmdBindPipeline(activeCB, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              cmd.pipeline->pipeline);
            lastPipeline = cmd.pipeline->pipeline;
        }

        // Bind descriptor set with dynamic UBO offsets (skip when unchanged)
        if (cmd.descriptorSet != VK_NULL_HANDLE) {
            bool descChanged = cmd.descriptorSet != lastDescSet
                            || cmd.dynUboOffsets[0] != lastUboOffs[0]
                            || cmd.dynUboOffsets[1] != lastUboOffs[1];
            if (descChanged) {
                vkCmdBindDescriptorSets(activeCB, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pipelineLayout, 0, 1, &cmd.descriptorSet,
                                        2, cmd.dynUboOffsets);
                lastDescSet    = cmd.descriptorSet;
                lastUboOffs[0] = cmd.dynUboOffsets[0];
                lastUboOffs[1] = cmd.dynUboOffsets[1];
            }
        }

        // Bind vertex buffer (skip when unchanged)
        if (cmd.vertexBuffer != lastVtxBuf || cmd.baseElement != lastVtxByteOff) {
            VkDeviceSize offsets[] = { cmd.baseElement };
            vkCmdBindVertexBuffers(activeCB, 0, 1, &cmd.vertexBuffer, offsets);
            lastVtxBuf     = cmd.vertexBuffer;
            lastVtxByteOff = cmd.baseElement;
        }

        // Bind instance data buffer (binding 1, instance rate) for instanced draws
        if (cmd.instanceCount > 1) {
            VkBuffer instBuf = frames[currentFrame].instBuffer;
            if (instBuf != VK_NULL_HANDLE) {
                if (instBuf != lastInstBuf || cmd.instanceBufOffset != lastInstOff) {
                    VkDeviceSize iOffsets[] = { cmd.instanceBufOffset };
                    vkCmdBindVertexBuffers(activeCB, 1, 1, &instBuf, iOffsets);
                    lastInstBuf = instBuf;
                    lastInstOff = cmd.instanceBufOffset;
                }
            }
        }

        if (cmd.indexBuffer != VK_NULL_HANDLE && cmd.indexCount > 0) {
            if (cmd.indexBuffer != lastIdxBuf || cmd.indexByteOffset != lastIdxOff) {
                vkCmdBindIndexBuffer(activeCB, cmd.indexBuffer,
                                     cmd.indexByteOffset, VK_INDEX_TYPE_UINT16);
                lastIdxBuf = cmd.indexBuffer;
                lastIdxOff = cmd.indexByteOffset;
            }
            vkCmdDrawIndexed(activeCB, (uint32_t)cmd.indexCount, cmd.instanceCount,
                             cmd.firstIndex, 0, 0);
        } else {
            vkCmdDraw(activeCB, (uint32_t)cmd.vertexCount, cmd.instanceCount, 0, 0);
        }
    }

    vkCmdEndRenderPass(activeCB);
}

// ---- Flush (present) ----------------------------------------------------

int cVulkanRender::Flush(bool wnd) {
    if (activeScene) EndScene();
    if (!lastSubmitSucceeded) {
        return 0;
    }
    lastSubmitSucceeded = false;
    int presentFrame = (currentFrame - 1 + VULKAN_FRAMES_IN_FLIGHT) % VULKAN_FRAMES_IN_FLIGHT;
    VkPresentInfoKHR pi = {};
    pi.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    pi.waitSemaphoreCount = 1;
    pi.pWaitSemaphores    = &frames[presentFrame].renderDoneSemaphore;
    pi.swapchainCount     = 1;
    pi.pSwapchains        = &swapchain;
    pi.pImageIndices      = &currentImageIndex;

    hrc::time_point tp0;
    if (profilingEnabled) tp0 = hrc::now();
    VkResult result = vkQueuePresentKHR(presentQueue, &pi);
    if (profilingEnabled) s_ps.presentUs += us_since(tp0);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RecreateSwapchain();
    }
    return 0;
}

// ---- State management --------------------------------------------------

int cVulkanRender::SetRenderState(eRenderStateOption option, uint32_t value) {
    switch (option) {
        case RS_ZENABLE:
            // handled per-pipeline
            break;
        case RS_ZWRITEENABLE:
            if (activePipelineMode.depth_write != (bool)value) {
                FlushActiveCommand();
                activePipelineMode.depth_write = (bool)value;
            }
            break;
        case RS_ZFUNC: {
            auto dc = static_cast<eCMPFUNC>(value);
            if (activePipelineMode.depth_cmp != dc) {
                FlushActiveCommand();
                activePipelineMode.depth_cmp = dc;
            }
            break;
        }
        case RS_CULLMODE: {
            auto cm = static_cast<eCullMode>(value);
            if (activePipelineMode.cull != cm) {
                FlushActiveCommand();
                activePipelineMode.cull = cm;
            }
            break;
        }
        case RS_ALPHABLENDENABLE:
            break;
        case RS_WIREFRAME: {
            bool wf = (bool)value;
            if (activePipelineMode.wireframe != wf) {
                FlushActiveCommand();
                activePipelineMode.wireframe = wf;
            }
            break;
        }
        case RS_ALPHA_TEST_MODE:
            if (activeAlphaTest != static_cast<eAlphaTestMode>(value)) {
                FlushActiveCommand();
                activeAlphaTest = static_cast<eAlphaTestMode>(value);
            }
            break;
        default:
            break;
    }
    return 0;
}

uint32_t cVulkanRender::GetRenderState(eRenderStateOption option) {
    switch (option) {
        case RS_ZWRITEENABLE:  return activePipelineMode.depth_write ? 1 : 0;
        case RS_ZFUNC:         return activePipelineMode.depth_cmp;
        case RS_CULLMODE:      return activePipelineMode.cull;
        case RS_WIREFRAME:     return activePipelineMode.wireframe ? 1 : 0;
        case RS_ALPHA_TEST_MODE: return activeAlphaTest;
        default:               return 0;
    }
}

// ---- SetDrawNode --------------------------------------------------------

void cVulkanRender::SetDrawNode(cCamera* pDrawNode) {
    // No short-circuit on (DrawNode == pDrawNode): the camera pointer can
    // repeat across frames while its matViewProj has been updated.
    // SetDrawTransform already skips a flush when matrices compare equal.
    if (DrawNode != pDrawNode) {
        FlushActiveCommand();
        if (pDrawNode && pDrawNode->GetRenderTarget()) {
            SetRenderTarget(pDrawNode->GetRenderTarget(), pDrawNode->GetZBuffer());
        } else {
            RestoreRenderTarget();
        }
    }
    cInterfaceRenderDevice::SetDrawNode(pDrawNode);
    SetDrawTransform(pDrawNode);
}

// ---- SetDrawTransform ---------------------------------------------------

void cVulkanRender::SetDrawTransform(cCamera* pDrawNode) {
    if (!pDrawNode) return;
    const Mat4f& newVP = pDrawNode->matViewProj;
    if (isOrthographicProjSet || !activeVP.eq(newVP, 0) || !activeWorld.eq(Mat4f::ID, 0)) {
        FlushActiveCommand();
    }
    isOrthographicProjSet = false;
    activeVP  = newVP;
    activeVPVk = newVP * GetVkClipFix();
    activeEyePos = pDrawNode->GetPos();
    activeWorld = Mat4f::ID;
    activePipelineMode.cull = (pDrawNode->GetAttribute(ATTRCAMERA_REFLECTION) == 0)
                              ? CULL_CW : CULL_CCW;
    CameraCullMode = activePipelineMode.cull;
    pShadowCamera = pDrawNode->FindCildCamera(ATTRCAMERA_SHADOWMAP);

    activeViewportPos.set(pDrawNode->vp.X, pDrawNode->vp.Y);
    activeViewportSize.set(pDrawNode->vp.Width, pDrawNode->vp.Height);
    hasViewport = true;
    // Mirror Sokol: reset clip to the camera's viewport so stale UI clip rects
    // from the previous frame don't leak into 3D rendering or the next HUD pass.
    activeClipPos.set(pDrawNode->vp.X, pDrawNode->vp.Y);
    activeClipSize.set(pDrawNode->vp.Width, pDrawNode->vp.Height);
    hasClip = false; // let the viewport act as the implicit scissor; only set true on explicit SetClipRect
}

// ---- SetBlendState ------------------------------------------------------

void cVulkanRender::SetBlendState(eBlendMode blend) {
    if (activePipelineMode.blend != blend) {
        FlushActiveCommand();
        activePipelineMode.blend = blend;
    }
}

// ---- Command helpers ---------------------------------------------------

void cVulkanRender::FlushActiveCommand() {
    FlushDeferredBatch();
    FinishActiveDrawBuffer();
    // Within a flush window, activeWorld/activeVP/ortho-flag are immutable
    // (their setters call FlushActiveCommand on change), so the MVP is stable.
    // Invalidate on flush so the next draw recomputes once.
    cachedMvpDirty = true;
}

void cVulkanRender::ClearCommands(std::vector<VulkanCommand>& cmds) {
    cmds.clear();
}

void cVulkanRender::ClearAllCommands() {
    ClearCommands(swapchainCommands);
    if (shadowMapRenderTarget) ClearCommands(shadowMapRenderTarget->commands);
    if (lightMapRenderTarget)  ClearCommands(lightMapRenderTarget->commands);
}

std::vector<VulkanCommand>& cVulkanRender::GetActiveCommands() {
    if (activeRenderTarget) return activeRenderTarget->commands;
    return swapchainCommands;
}



// ---- Shadow texture creation -------------------------------------------

bool cVulkanRender::CreateShadowTexture(int xysize) {
    if (shadowMapRenderTarget) { DeleteShadowTexture(); }

    // Use TexLibrary to create shadow depth texture (calls CreateTexture internally)
    cTexture* depthTex = GetTexLibrary()->CreateRenderTexture(xysize, xysize, TEXTURE_RENDER_DEPTH, false);
    if (!depthTex) return false;

    TextureImage* fi = depthTex->GetFrameImage(0);
    if (!fi || !fi->vk) { depthTex->Release(); return false; }
    VulkanTexture2D* vkTex = fi->vk;
    VulkanImageResource* imgRes = vkTex->image_res;
    if (!imgRes) { depthTex->Release(); return false; }

    // Transition to depth attachment layout
    auto cb = BeginSingleTimeCommands();
    TransitionImageLayout(cb, imgRes->image, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                          VK_IMAGE_ASPECT_DEPTH_BIT);
    EndSingleTimeCommands(cb);

    // Create framebuffer
    VkFramebufferCreateInfo fci = {};
    fci.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fci.renderPass      = shadowRenderPass;
    fci.attachmentCount = 1;
    fci.pAttachments    = &imgRes->view;
    fci.width           = xysize;
    fci.height          = xysize;
    fci.layers          = 1;
    VkFramebuffer fb;
    if (vkCreateFramebuffer(device, &fci, nullptr, &fb) != VK_SUCCESS) {
        depthTex->Release(); return false;
    }

    shadowMapRenderTarget = new VulkanRenderTarget();
    shadowMapRenderTarget->texture     = depthTex;
    shadowMapRenderTarget->renderPass  = shadowRenderPass;
    shadowMapRenderTarget->framebuffer = fb;
    shadowMapRenderTarget->extent      = { (uint32_t)xysize, (uint32_t)xysize };
    shadowMapRenderTarget->depthOnly   = true;
    shadowMapRenderTarget->commands.reserve(16384);

    // Create lightmap render target (256x256 RGBA color, used by tile_map shader slot 2)
    cTexture* lightTex = GetTexLibrary()->CreateRenderTexture(256, 256, TEXTURE_RENDER32, false);
    if (!lightTex) { DeleteShadowTexture(); return false; }

    TextureImage* lfi = lightTex->GetFrameImage(0);
    if (!lfi || !lfi->vk) { lightTex->Release(); DeleteShadowTexture(); return false; }
    VulkanTexture2D* lkTex = lfi->vk;
    VulkanImageResource* lmgRes = lkTex->image_res;
    if (!lmgRes) { lightTex->Release(); DeleteShadowTexture(); return false; }

    // Transition to shader-read layout initially so it can be safely sampled
    // before the first lightmap render pass runs.
    auto lcb = BeginSingleTimeCommands();
    TransitionImageLayout(lcb, lmgRes->image, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                          VK_IMAGE_ASPECT_COLOR_BIT);
    EndSingleTimeCommands(lcb);

    VkFramebufferCreateInfo lfci = {};
    lfci.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    lfci.renderPass      = lightmapRenderPass;
    lfci.attachmentCount = 1;
    lfci.pAttachments    = &lmgRes->view;
    lfci.width           = 256;
    lfci.height          = 256;
    lfci.layers          = 1;
    VkFramebuffer lfb;
    if (vkCreateFramebuffer(device, &lfci, nullptr, &lfb) != VK_SUCCESS) {
        lightTex->Release(); DeleteShadowTexture(); return false;
    }

    lightMapRenderTarget = new VulkanRenderTarget();
    lightMapRenderTarget->texture     = lightTex;
    lightMapRenderTarget->renderPass  = lightmapRenderPass;
    lightMapRenderTarget->framebuffer = lfb;
    lightMapRenderTarget->extent      = { 256, 256 };
    lightMapRenderTarget->depthOnly   = false;
    lightMapRenderTarget->commands.reserve(16384);

    return true;
}

void cVulkanRender::DeleteShadowTexture() {
    // Guard against being called after the device has been destroyed (e.g. on
    // a second pass through Done() via the destructor + Release path): the
    // graphicsQueue handle is stale at that point and any Vulkan call on it
    // is undefined behaviour.
    if (device == VK_NULL_HANDLE) {
        delete lightMapRenderTarget;  lightMapRenderTarget  = nullptr;
        delete shadowMapRenderTarget; shadowMapRenderTarget = nullptr;
        return;
    }
    vkQueueWaitIdle(graphicsQueue);
    // Clear any activeTexture slots that may still reference the shadow/light textures
    // to avoid dangling VulkanTexture2D pointers.
    for (auto& slot : activeTextures) slot = nullptr;
    activeRenderTarget = nullptr;

    if (lightMapRenderTarget) {
        if (lightMapRenderTarget->framebuffer != VK_NULL_HANDLE)
            vkDestroyFramebuffer(device, lightMapRenderTarget->framebuffer, nullptr);
        delete lightMapRenderTarget;
        lightMapRenderTarget = nullptr;
    }
    if (shadowMapRenderTarget) {
        if (shadowMapRenderTarget->framebuffer != VK_NULL_HANDLE)
            vkDestroyFramebuffer(device, shadowMapRenderTarget->framebuffer, nullptr);
        delete shadowMapRenderTarget;
        shadowMapRenderTarget = nullptr;
    }
}
