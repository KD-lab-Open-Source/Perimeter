#pragma once

#include <tuple>
#include "RenderTypes.h"

// ---- Shader IDs --------------------------------------------------------

enum VULKAN_SHADER_ID {
    VULKAN_SHADER_ID_NONE = 0,
    VULKAN_SHADER_ID_mesh_color_tex1,     // sVertexXYZDT1
    VULKAN_SHADER_ID_mesh_color_tex2,     // sVertexXYZDT2
    VULKAN_SHADER_ID_mesh_normal_tex1,    // sVertexXYZNT1
    VULKAN_SHADER_ID_mesh_tex1,           // sVertexXYZT1
    VULKAN_SHADER_ID_shadow_tex1,         // sVertexXYZT1  (shadow pass)
    VULKAN_SHADER_ID_shadow_normal_tex1,  // sVertexXYZNT1 (shadow pass)
    VULKAN_SHADER_ID_tile_map,
    VULKAN_SHADER_ID_COUNT,
};

// ---- Pipeline types ----------------------------------------------------

enum VULKAN_PIPELINE_TYPE {
    VULKAN_PIPELINE_TYPE_MESH = 0,
    VULKAN_PIPELINE_TYPE_TILE_MAP,
    VULKAN_PIPELINE_TYPE_OBJECT_SHADOW,
    VULKAN_PIPELINE_TYPE_MAX,
};

const VULKAN_PIPELINE_TYPE VULKAN_PIPELINE_TYPE_DEFAULT = VULKAN_PIPELINE_TYPE_MESH;

enum VULKAN_PIPELINE_TARGET {
    VULKAN_PIPELINE_TARGET_SHADOWMAP = 0,
    VULKAN_PIPELINE_TARGET_LIGHTMAP,
    VULKAN_PIPELINE_TARGET_SWAPCHAIN,
};

enum VULKAN_MATERIAL_TYPE {
    VULKAN_MAT_NONE  = 0,
    VULKAN_MAT_LIGHT = 1,
};

// ---- Pipeline mode (state hash) ----------------------------------------

struct VulkanPipelineMode {
    eBlendMode  blend      = ALPHA_NONE;
    eCullMode   cull       = CULL_NONE;
    eCMPFUNC    depth_cmp  = CMP_LESSEQUAL;
    bool        depth_write = true;
    bool        wireframe  = false;

    bool operator==(const VulkanPipelineMode& o) const {
        return std::tie(blend, cull, depth_cmp, depth_write, wireframe)
            == std::tie(o.blend, o.cull, o.depth_cmp, o.depth_write, o.wireframe);
    }
};

// ---- UBO parameter structs (std140 layout, matching GLSL) --------------

#pragma pack(push, 1)

struct VulkanMeshColorTexVsParams {
    float un_mvp[16];     // mat4
    float tex0_mat[16];   // mat4
    float tex1_mat[16];   // mat4
};
static_assert(sizeof(VulkanMeshColorTexVsParams) == 192, "");

struct VulkanMeshColorTexFsParams {
    int32_t un_color_mode;
    float   un_tex2_lerp;
    float   un_alpha_test;
    float   _pad;
};
static_assert(sizeof(VulkanMeshColorTexFsParams) == 16, "");

struct VulkanMeshNormalTexVsParams {
    float un_mvp[16];     // mat4
    float model[16];      // mat4
    float tex0_mat[16];   // mat4
};
static_assert(sizeof(VulkanMeshNormalTexVsParams) == 192, "");

// std140 layout. Every vec3 sits at a 16-byte boundary and consumes 16 bytes
// (next scalar packs into the trailing 4 bytes).  Avoid `float[N]` padding here
// std140 array stride is 16 bytes per element, which would silently disagree
// with C array stride of 4.
struct VulkanMeshNormalTexFsParams {
    float   diffuse[4];               // 0
    float   ambient[4];               // 16
    float   specular[4];              // 32
    float   emissive[4];              // 48
    float   light_dir[3];    float _pad0;   // 64
    float   light_diffuse[3]; float _pad1;  // 80
    float   light_ambient[3]; float _pad2;  // 96
    float   light_specular[3];               // 112 (12 bytes; spec_power packs into trailing 4)
    float   spec_power;               // 124
    float   un_alpha_test;            // 128
    int32_t material;                 // 132
    float   _pad_align[2];            // 136..144 (align eye_pos to 16)
    float   eye_pos[3];               // 144 (12 bytes; trailing 4 = _pad3)
    float   _pad3;                    // 156
};
static_assert(sizeof(VulkanMeshNormalTexFsParams) == 160, "");

struct VulkanMeshTexVsParams {
    float un_mvp[16];   // mat4
};
static_assert(sizeof(VulkanMeshTexVsParams) == 64, "");

struct VulkanShadowTexVsParams {
    float un_mvp[16];   // mat4
};
static_assert(sizeof(VulkanShadowTexVsParams) == 64, "");

struct VulkanShadowTexFsParams {
    float   un_alpha_test;
    uint8_t _pad[12];
};
static_assert(sizeof(VulkanShadowTexFsParams) == 16, "");

struct VulkanTileMapVsParams {
    float un_mvp[16];          // mat4
    float un_shadow[16];       // mat4
    float un_inv_world_size[2];// vec2
    float _pad[2];
    float un_tile_color[4];    // vec4 - read by both VS and FS from binding 0
};
static_assert(sizeof(VulkanTileMapVsParams) == 160, "");

struct VulkanTileMapFsParams {
    float un_tile_color[4];   // vec4
};
static_assert(sizeof(VulkanTileMapFsParams) == 16, "");

#pragma pack(pop)

// Number of texture bindings in the descriptor set (binding 2..5)
static const int VULKAN_TEXTURES = 4;
