#include "StdAfxRD.h"
#include "VulkanPipeline.h"
#include "VulkanRender.h"
#include "VulkanResources.h"
#include "VertexFormat.h"
#include "Texture.h"
#include <cstdio>

// ---- VulkanRenderTarget ------------------------------------------------

VulkanRenderTarget::~VulkanRenderTarget() {
    commands.clear();
    RELEASE(texture);
    // framebuffer / renderPass owned by cVulkanRender
}

// ---- Pipeline creation -------------------------------------------------

static VkCompareOp GetVkCompareOp(eCMPFUNC f) {
    switch (f) {
        case CMP_LESSEQUAL:    return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CMP_GREATER:      return VK_COMPARE_OP_GREATER;
        case CMP_GREATEREQUAL: return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CMP_ALWAYS:       return VK_COMPARE_OP_ALWAYS;
        default:               return VK_COMPARE_OP_LESS_OR_EQUAL;
    }
}

// VK_CLIP_FIX negates clip-space Y, which inverts triangle winding in
// framebuffer space relative to D3D.  We compensate by declaring the *opposite*
// front-face from the eCullMode the engine asked for, so culling matches D3D
// behaviour.  In D3D the default front-face is CW; after the
// clip-Y flip the equivalent winding in framebuffer space is CCW, so D3D's
// "cull back-face when front=CW" maps to Vulkan "cull back-face when front=CCW".
//
// Engine convention:
//   CULL_CW   = D3D front-face=CW, cull back-facing -> cull face that
//               isn't CW in screen-space.
//   CULL_CCW  = inverted (used for reflection passes)
//   CULL_NONE = disable culling entirely.
static VkCullModeFlags GetVkCullMode(eCullMode c) {
    switch (c) {
        case CULL_CW:   return VK_CULL_MODE_BACK_BIT;
        case CULL_CCW:  return VK_CULL_MODE_BACK_BIT;
        case CULL_NONE: return VK_CULL_MODE_NONE;
        default:        return VK_CULL_MODE_NONE;
    }
}

static VkFrontFace GetVkFrontFace(eCullMode c) {
    // After Y-flip, D3D's CW front-face becomes CCW in framebuffer space;
    // for the reflection pass (CULL_CCW) the convention inverts again.
    switch (c) {
        case CULL_CW:   return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        case CULL_CCW:  return VK_FRONT_FACE_CLOCKWISE;
        default:        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }
}

// Populate VkPipelineVertexInputStateCreateInfo for a given vertex format
static void FillVertexInput(vertex_fmt_t fmt,
    std::vector<VkVertexInputBindingDescription>&   bindings,
    std::vector<VkVertexInputAttributeDescription>& attrs) {
    VkVertexInputBindingDescription bd = {};
    bd.binding   = 0;
    bd.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // Fixed locations matching GLSL shader layout locations
    // location 0: position, location 1: normal, location 2: diffuse, location 3: tex0, location 4: tex1
    uint32_t off = 0;

    // position always at location 0
    {
        VkVertexInputAttributeDescription a = {};
        a.location = 0; a.binding = 0;
        a.format   = VK_FORMAT_R32G32B32_SFLOAT;
        a.offset   = off;
        attrs.push_back(a);
        off += 12;
    }

    if (fmt & VERTEX_FMT_NORMAL) {
        VkVertexInputAttributeDescription a = {};
        a.location = 1; a.binding = 0;
        a.format   = VK_FORMAT_R32G32B32_SFLOAT;
        a.offset   = off;
        attrs.push_back(a);
        off += 12;
    }
    if (fmt & VERTEX_FMT_DIFFUSE) {
        VkVertexInputAttributeDescription a = {};
        a.location = 2; a.binding = 0;
        a.format   = VK_FORMAT_R8G8B8A8_UNORM;
        a.offset   = off;
        attrs.push_back(a);
        off += 4;
    }
    if (fmt & VERTEX_FMT_TEX1) {
        VkVertexInputAttributeDescription a = {};
        a.location = 3; a.binding = 0;
        a.format   = VK_FORMAT_R32G32_SFLOAT;
        a.offset   = off;
        attrs.push_back(a);
        off += 8;
    }
    if (fmt & VERTEX_FMT_TEX2) {
        VkVertexInputAttributeDescription a = {};
        a.location = 4; a.binding = 0;
        a.format   = VK_FORMAT_R32G32_SFLOAT;
        a.offset   = off;
        attrs.push_back(a);
        off += 8;
    }

    bd.stride = off;
    bindings.push_back(bd);
}

// Build blend attachment state from eBlendMode
static VkPipelineColorBlendAttachmentState MakeBlendAttachment(eBlendMode blend) {
    VkPipelineColorBlendAttachmentState s = {};
    s.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
                     | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    s.blendEnable = (blend != ALPHA_NONE) ? VK_TRUE : VK_FALSE;
    if (!s.blendEnable) return s;

    s.colorBlendOp = s.alphaBlendOp = VK_BLEND_OP_ADD;
    switch (blend) {
        case ALPHA_SUBBLEND:
            s.colorBlendOp = s.alphaBlendOp = VK_BLEND_OP_REVERSE_SUBTRACT;
            s.srcColorBlendFactor = s.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            s.dstColorBlendFactor = s.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            break;
        case ALPHA_ADDBLENDALPHA:
            s.srcColorBlendFactor = s.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            s.dstColorBlendFactor = s.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            break;
        case ALPHA_ADDBLEND:
            s.srcColorBlendFactor = s.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            s.dstColorBlendFactor = s.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            break;
        case ALPHA_MUL:
            s.srcColorBlendFactor = s.srcAlphaBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
            s.dstColorBlendFactor = s.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            break;
        case ALPHA_TEST:
        case ALPHA_BLEND:
        default:
            s.srcColorBlendFactor = s.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            s.dstColorBlendFactor = s.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            break;
    }
    return s;
}

// ---- cVulkanRender pipeline helpers (called from VulkanRender.cpp) -----

static uint32_t MakePipelineKey(VULKAN_SHADER_ID shaderId, VULKAN_PIPELINE_TARGET target,
                                 VULKAN_PIPELINE_TYPE pipeType, const VulkanPipelineMode& mode,
                                 vertex_fmt_t vertFmt, ePrimitiveType primitive, bool instanced) {
    return  ((uint32_t)shaderId         & 0x7u)
         | (((uint32_t)target           & 0x3u) << 3)
         | (((uint32_t)pipeType         & 0x3u) << 5)
         | (((uint32_t)mode.blend       & 0x7u) << 7)
         | (((uint32_t)mode.cull        & 0x3u) << 10)
         | (((uint32_t)mode.depth_cmp   & 0x3u) << 12)
         | ((mode.depth_write ? 1u : 0u)        << 14)
         | ((mode.wireframe   ? 1u : 0u)        << 15)
         | (((uint32_t)vertFmt          & 0x1fu) << 16)
         | (((uint32_t)primitive        & 0x1u) << 21)
         | ((instanced         ? 1u : 0u)        << 22);
}

VulkanPipeline* cVulkanRender::GetOrCreatePipeline(
    VULKAN_SHADER_ID       shaderId,
    VULKAN_PIPELINE_TARGET target,
    VULKAN_PIPELINE_TYPE   pipeType,
    const VulkanPipelineMode& mode,
    vertex_fmt_t vertFmt,
    ePrimitiveType primitive,
    bool instanced) {

    uint32_t key = MakePipelineKey(shaderId, target, pipeType, mode, vertFmt, primitive, instanced);
    auto it = pipelineMap.find(key);
    if (it != pipelineMap.end()) return it->second;

    VkShaderModule vertModule = instanced ? shaderModulesInst[shaderId][0] : shaderModules[shaderId][0];
    VkShaderModule fragModule = instanced ? shaderModulesInst[shaderId][1] : shaderModules[shaderId][1];
    if (vertModule == VK_NULL_HANDLE) {
        fprintf(stderr, "GetOrCreatePipeline: missing vertex shader for id %d\n", shaderId);
        return nullptr;
    }

    // Shader stages
    VkPipelineShaderStageCreateInfo stages[2] = {};
    stages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = vertModule;
    stages[0].pName  = "main";

    uint32_t stageCount = 1;
    if (fragModule != VK_NULL_HANDLE && target != VULKAN_PIPELINE_TARGET_SHADOWMAP) {
        stages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
        stages[1].module = fragModule;
        stages[1].pName  = "main";
        stageCount = 2;
    }

    // Vertex input
    std::vector<VkVertexInputBindingDescription>   bindings;
    std::vector<VkVertexInputAttributeDescription> attrs;
    FillVertexInput(vertFmt, bindings, attrs);

    // Per-instance world matrix: binding 1, locations 8-11 (4× vec4), instance rate
    if (instanced) {
        VkVertexInputBindingDescription ibd = {};
        ibd.binding   = 1;
        ibd.stride    = 64; // 4×4 float matrix
        ibd.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
        bindings.push_back(ibd);

        for (uint32_t loc = 8; loc < 12; loc++) {
            VkVertexInputAttributeDescription iad = {};
            iad.location = loc;
            iad.binding  = 1;
            iad.format   = VK_FORMAT_R32G32B32A32_SFLOAT;
            iad.offset   = (loc - 8) * 16; // 16 bytes per vec4
            attrs.push_back(iad);
        }
    }

    VkPipelineVertexInputStateCreateInfo vertInput = {};
    vertInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertInput.vertexBindingDescriptionCount   = (uint32_t)bindings.size();
    vertInput.pVertexBindingDescriptions      = bindings.data();
    vertInput.vertexAttributeDescriptionCount = (uint32_t)attrs.size();
    vertInput.pVertexAttributeDescriptions    = attrs.data();

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssem = {};
    inputAssem.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    switch (primitive) {
        case PT_TRIANGLES:      inputAssem.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;  break;
        case PT_TRIANGLESTRIP:  inputAssem.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
        default:
            // Anything else (line/point list) is not supported by the current
            // pipeline-key encoding (1 bit) and isn't used by any shader either.
            // Loudly fall back to triangle list rather than silently rendering garbage.
            fprintf(stderr, "GetOrCreatePipeline: unsupported primitive %d, falling back to triangle list\n",
                    (int)primitive);
            xassert(0);
            inputAssem.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
    }

    // Viewport / scissor (dynamic)
    VkPipelineViewportStateCreateInfo vpState = {};
    vpState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vpState.viewportCount = 1;
    vpState.scissorCount  = 1;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rast = {};
    rast.sType       = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rast.polygonMode = mode.wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
    rast.cullMode    = GetVkCullMode(mode.cull);
    rast.frontFace   = GetVkFrontFace(mode.cull);
    rast.lineWidth   = 1.0f;

    // Multisample
    VkPipelineMultisampleStateCreateInfo ms = {};
    ms.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Depth stencil
    VkPipelineDepthStencilStateCreateInfo ds = {};
    ds.sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    bool hasDepth = (target != VULKAN_PIPELINE_TARGET_LIGHTMAP);
    ds.depthTestEnable  = hasDepth ? VK_TRUE : VK_FALSE;
    ds.depthWriteEnable = (hasDepth && mode.depth_write) ? VK_TRUE : VK_FALSE;
    ds.depthCompareOp   = GetVkCompareOp(mode.depth_cmp);

    // Colour blend
    VkPipelineColorBlendAttachmentState blendAtt = MakeBlendAttachment(mode.blend);
    VkPipelineColorBlendStateCreateInfo blend = {};
    blend.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend.attachmentCount = (target == VULKAN_PIPELINE_TARGET_SHADOWMAP) ? 0 : 1;
    blend.pAttachments    = (target == VULKAN_PIPELINE_TARGET_SHADOWMAP) ? nullptr : &blendAtt;

    // Dynamic states
    VkDynamicState dynStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dyn = {};
    dyn.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dyn.dynamicStateCount = 2;
    dyn.pDynamicStates    = dynStates;

    // Choose render pass
    VkRenderPass rp = mainRenderPass;
    if (target == VULKAN_PIPELINE_TARGET_SHADOWMAP)   rp = shadowRenderPass;
    else if (target == VULKAN_PIPELINE_TARGET_LIGHTMAP) rp = lightmapRenderPass;

    VkGraphicsPipelineCreateInfo ci = {};
    ci.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    ci.stageCount          = stageCount;
    ci.pStages             = stages;
    ci.pVertexInputState   = &vertInput;
    ci.pInputAssemblyState = &inputAssem;
    ci.pViewportState      = &vpState;
    ci.pRasterizationState = &rast;
    ci.pMultisampleState   = &ms;
    ci.pDepthStencilState  = &ds;
    ci.pColorBlendState    = &blend;
    ci.pDynamicState       = &dyn;
    ci.layout              = pipelineLayout;
    ci.renderPass          = rp;
    ci.subpass             = 0;

    VkPipeline vkp = VK_NULL_HANDLE;
    if (vkCreateGraphicsPipelines(device, pipelineCache, 1, &ci, nullptr, &vkp) != VK_SUCCESS) {
        fprintf(stderr, "GetOrCreatePipeline: vkCreateGraphicsPipelines failed for shader %d\n", shaderId);
        return nullptr;
    }

    auto* p = new VulkanPipeline();
    p->device        = device;
    p->shader_id     = shaderId;
    p->target        = target;
    p->pipeline_type = pipeType;
    p->mode          = mode;
    p->vertex_fmt    = vertFmt;
    p->primitive     = primitive;
    p->pipeline      = vkp;
    p->instanced     = instanced;
    pipelines.push_back(p);
    pipelineMap[key] = p;
    ProfRecordPipeCreate();
    return p;
}

void cVulkanRender::ClearPipelines() {
    pipelineMap.clear();
    for (auto* p : pipelines) delete p;
    pipelines.clear();
}

void cVulkanRender::WarmupPipelines() {
    // Pre-create common pipeline combinations to eliminate first-use hitches during gameplay.
    // Tilemap pipelines are the primary stutter source during map rotation since every tile
    // triggers GetOrCreatePipeline on the first frame each combination is seen.

    struct WarmEntry {
        VULKAN_SHADER_ID       shaderId;
        VULKAN_PIPELINE_TARGET target;
        VULKAN_PIPELINE_TYPE   pipeType;
        VulkanPipelineMode     mode;
        vertex_fmt_t           vertFmt;
    };

    std::vector<WarmEntry> entries;

    auto add = [&](VULKAN_SHADER_ID id, VULKAN_PIPELINE_TARGET tgt, VULKAN_PIPELINE_TYPE pt,
                   eBlendMode blend, eCullMode cull, eCMPFUNC dcmp, bool dw, vertex_fmt_t fmt) {
        VulkanPipelineMode m;
        m.blend = blend; m.cull = cull; m.depth_cmp = dcmp;
        m.depth_write = dw; m.wireframe = false;
        entries.push_back({ id, tgt, pt, m, fmt });
    };

    // ---- Tile map - swapchain ----
    const vertex_fmt_t tmFmt = sVertexXYZT1::fmt;
    const eCMPFUNC tileCmps[] = { CMP_LESSEQUAL, CMP_GREATEREQUAL, CMP_GREATER, CMP_ALWAYS };
    for (auto cull : { CULL_CW, CULL_CCW, CULL_NONE }) {
        for (auto blend : { ALPHA_NONE, ALPHA_BLEND }) {
            for (auto dcmp : tileCmps) {
                add(VULKAN_SHADER_ID_tile_map, VULKAN_PIPELINE_TARGET_SWAPCHAIN,
                    VULKAN_PIPELINE_TYPE_TILE_MAP, blend, cull, dcmp, true, tmFmt);
            }
        }
    }
    // Tile map - lightmap
    add(VULKAN_SHADER_ID_tile_map, VULKAN_PIPELINE_TARGET_LIGHTMAP,
        VULKAN_PIPELINE_TYPE_TILE_MAP, ALPHA_NONE, CULL_NONE, CMP_ALWAYS, false, tmFmt);

    // ---- Shadow passes ----
    for (auto shaderId : { VULKAN_SHADER_ID_shadow_tex1, VULKAN_SHADER_ID_shadow_normal_tex1 }) {
        vertex_fmt_t fmt = (shaderId == VULKAN_SHADER_ID_shadow_normal_tex1)
                           ? sVertexXYZNT1::fmt : sVertexXYZT1::fmt;
        add(shaderId, VULKAN_PIPELINE_TARGET_SHADOWMAP, VULKAN_PIPELINE_TYPE_OBJECT_SHADOW,
            ALPHA_NONE, CULL_CCW, CMP_LESSEQUAL, true, fmt);
        add(shaderId, VULKAN_PIPELINE_TARGET_SHADOWMAP, VULKAN_PIPELINE_TYPE_OBJECT_SHADOW,
            ALPHA_TEST, CULL_CCW, CMP_LESSEQUAL, true, fmt);
    }

    // ---- Mesh shaders - swapchain ----
    struct { VULKAN_SHADER_ID id; vertex_fmt_t fmt; } meshShaders[] = {
        { VULKAN_SHADER_ID_mesh_color_tex1,   sVertexXYZDT1::fmt },
        { VULKAN_SHADER_ID_mesh_color_tex2,   sVertexXYZDT2::fmt },
        { VULKAN_SHADER_ID_mesh_normal_tex1,  sVertexXYZNT1::fmt },
        { VULKAN_SHADER_ID_mesh_tex1,         sVertexXYZT1::fmt  },
    };
    const eBlendMode meshBlends[] = { ALPHA_NONE, ALPHA_BLEND, ALPHA_TEST,
                                      ALPHA_ADDBLENDALPHA, ALPHA_ADDBLEND };
    const eCullMode  meshCulls[]  = { CULL_CW, CULL_CCW, CULL_NONE };
    const eCMPFUNC   meshCmps[]   = { CMP_LESSEQUAL, CMP_GREATER,
                                      CMP_GREATEREQUAL, CMP_ALWAYS };
    for (auto& ms : meshShaders) {
        for (auto blend : meshBlends) {
            for (auto cull : meshCulls) {
                for (auto dcmp : meshCmps) {
                    add(ms.id, VULKAN_PIPELINE_TARGET_SWAPCHAIN, VULKAN_PIPELINE_TYPE_MESH,
                        blend, cull, dcmp, true,  ms.fmt);
                    add(ms.id, VULKAN_PIPELINE_TARGET_SWAPCHAIN, VULKAN_PIPELINE_TYPE_MESH,
                        blend, cull, dcmp, false, ms.fmt);
                }
            }
        }
    }

    for (auto& e : entries) {
        GetOrCreatePipeline(e.shaderId, e.target, e.pipeType, e.mode, e.vertFmt, PT_TRIANGLES, false);
        GetOrCreatePipeline(e.shaderId, e.target, e.pipeType, e.mode, e.vertFmt, PT_TRIANGLESTRIP, false);
    }
}

// ---- Pipeline cache persistence -----------------------------------------

#include <fstream>

void cVulkanRender::SavePipelineCache() {
    if (pipelineCache == VK_NULL_HANDLE) return;
    size_t sz = 0;
    vkGetPipelineCacheData(device, pipelineCache, &sz, nullptr);
    if (sz == 0) return;

    std::vector<uint8_t> data(sz);
    if (vkGetPipelineCacheData(device, pipelineCache, &sz, data.data()) != VK_SUCCESS)
        return;

    std::ofstream f("vulkan_pipeline_cache.bin", std::ios::binary | std::ios::trunc);
    if (!f.is_open()) return;
    f.write(reinterpret_cast<const char*>(data.data()), sz);
    f.close();
}

