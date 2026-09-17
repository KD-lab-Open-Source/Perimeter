#version 450

layout(set = 0, binding = 2) uniform sampler2D un_tex0;

layout(location = 0) in vec2 fs_uv0;
layout(location = 0) out vec4 frag_color;

void main() {
    frag_color = texture(un_tex0, fs_uv0);
}
