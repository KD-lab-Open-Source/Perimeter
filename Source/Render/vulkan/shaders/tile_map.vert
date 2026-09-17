#version 450

layout(set = 0, binding = 0) uniform VS_Params {
    mat4 un_mvp;
    mat4 un_shadow;
    vec2 un_inv_world_size;
    vec2 _pad;
} vs_params;

layout(location = 0) in vec3 vs_position;
layout(location = 3) in vec2 vs_texcoord0;

layout(location = 0) out vec3 fs_uv0;
layout(location = 1) out vec3 fs_uv1;
layout(location = 2) out vec3 fs_uv2;
layout(location = 3) out vec3 fs_uv3;
layout(location = 4) out vec2 fs_uv4;
layout(location = 5) out vec2 fs_uv5;

const float offset = 1.0 / 1024.0;

void main() {
    gl_Position = vs_params.un_mvp * vec4(vs_position, 1.0);
    vec4 p = vs_params.un_shadow * vec4(vs_position, 1.0);
    fs_uv0 = (p + vec4(-offset, +offset * 0.5, 0.0, 0.0)).xyz;
    fs_uv1 = (p + vec4(+offset, -offset * 0.5, 0.0, 0.0)).xyz;
    fs_uv2 = (p + vec4(0.0, -offset, 0.0, 0.0)).xyz;
    fs_uv3 = (p + vec4(0.0, +offset, 0.0, 0.0)).xyz;
    fs_uv4 = vs_texcoord0;
    fs_uv5 = vs_position.xy * vs_params.un_inv_world_size;
}
