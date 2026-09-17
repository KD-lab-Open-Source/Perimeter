#version 450

// un_tile_color lives in the VS UBO (binding 0) so the FS can read it
// without needing a separate FS UBO binding.
layout(set = 0, binding = 0) uniform VS_Params {
    mat4 un_mvp;
    mat4 un_shadow;
    vec2 un_inv_world_size;
    vec2 _pad;
    vec4 un_tile_color;
} vs_params;

// Use sampler2D (not sampler2DShadow) so we can fall back to emptyTexture
// (white 1x1 colour) when no real shadow map is available. Manual comparison
// gives the same result as the hardware path; white emptyTexture returns
// r=1.0, which means fully-lit (no shadow), which is correct for that case.
layout(set = 0, binding = 2) uniform sampler2D un_shadow_tex;
layout(set = 0, binding = 3) uniform sampler2D un_diffuse_tex;
layout(set = 0, binding = 4) uniform sampler2D un_lightmap_tex;

layout(location = 0) in vec3 fs_uv0;
layout(location = 1) in vec3 fs_uv1;
layout(location = 2) in vec3 fs_uv2;
layout(location = 3) in vec3 fs_uv3;
layout(location = 4) in vec2 fs_uv4;
layout(location = 5) in vec2 fs_uv5;

layout(location = 0) out vec4 frag_color;

#define SHADE  0.5
#define MSHADE 0.5

float shadowSample(vec3 projCoord) {
    float depth = texture(un_shadow_tex, projCoord.xy).r;
    return (projCoord.z <= depth) ? 1.0 : 0.0;
}

void main() {

    float shadow = shadowSample(fs_uv0)
                 + shadowSample(fs_uv1)
                 + shadowSample(fs_uv2)
                 + shadowSample(fs_uv3);
    shadow *= 0.25;

    vec4 diffuse  = texture(un_diffuse_tex,  fs_uv4);
    vec4 lightmap = texture(un_lightmap_tex, fs_uv5);

    float ambientFactor = shadow * MSHADE + SHADE;
    frag_color = vec4(diffuse.xyz * lightmap.xyz * ambientFactor, diffuse.a) * vs_params.un_tile_color;
}
