#version 450

layout(set = 0, binding = 1) uniform FS_Params {
    int un_color_mode;
    float un_tex2_lerp;
    float un_alpha_test;
    float _pad;
} fs_params;

layout(set = 0, binding = 2) uniform sampler2D un_tex0;
#ifdef HAS_TEX2
layout(set = 0, binding = 3) uniform sampler2D un_tex1;
#endif

layout(location = 0) in vec4 fs_color;
layout(location = 1) in vec2 fs_uv0;
#ifdef HAS_TEX2
layout(location = 2) in vec2 fs_uv1;
#endif

layout(location = 0) out vec4 frag_color;

void main() {
#ifdef HAS_TEX2
    vec4 tex0 = texture(un_tex0, fs_uv0);
    if (0.0 <= fs_params.un_tex2_lerp) {
        tex0 = tex0 * vec4(fs_params.un_tex2_lerp)
             + vec4(1.0 - fs_params.un_tex2_lerp, 1.0 - fs_params.un_tex2_lerp,
                    1.0 - fs_params.un_tex2_lerp, 0.0);
    }
    vec4 tex1 = texture(un_tex1, fs_uv1);
    switch (fs_params.un_color_mode) {
        default:
        case 0: frag_color = tex1 * tex0 * fs_color; break;      // COLOR_MOD
        case 1: frag_color = (tex1 + tex0) * fs_color; break;    // COLOR_ADD
        case 2: frag_color = tex1 * tex0 * 2.0 * fs_color; break;// COLOR_MOD2
        case 3: frag_color = tex1 * tex0 * 4.0 * fs_color; break;// COLOR_MOD4
    }
#else
    frag_color = texture(un_tex0, fs_uv0) * fs_color;
#endif
    if (fs_params.un_alpha_test >= frag_color.a) discard;
}
