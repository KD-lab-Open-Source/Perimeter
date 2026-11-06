#version 450

layout(set = 0, binding = 1) uniform FS_Params {
    float un_alpha_test;
    float _pad[3];
} fs_params;

layout(set = 0, binding = 2) uniform sampler2D un_tex0;

layout(location = 0) in vec2 fs_uv0;

void main() {
    vec4 color = texture(un_tex0, fs_uv0);
    if (fs_params.un_alpha_test >= color.a) discard;
    // shadow pass: no color output needed, only depth
}
