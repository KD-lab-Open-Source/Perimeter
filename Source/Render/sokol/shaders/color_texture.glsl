@ctype mat4 Mat4f

@vs vs
//Uniforms
uniform vs_params {
    mat4 un_mvp;
};

//Vertex Buffer inputs
in vec3 vs_position;
in vec4 vs_color;
in vec2 vs_texcoord0;
#if defined(SHADER_TEX_2)
in vec2 vs_texcoord1;
#endif

//Fragment shader outputs
layout(location=0) out vec4 fs_color;
layout(location=1) out vec2 fs_uv0;
#if defined(SHADER_TEX_2)
layout(location=2) out vec2 fs_uv1;
#endif

void main() {
    gl_Position = un_mvp * vec4(vs_position, 1.0f);
    fs_color = vs_color;
    fs_uv0 = vs_texcoord0;
    #if defined(SHADER_TEX_2)
    fs_uv1 = vs_texcoord1;
    #endif
}
@end

@fs fs
//Uniforms
uniform fs_params {
    int un_mode;
};
uniform sampler2D un_tex0;
#if defined(SHADER_TEX_2)
uniform sampler2D un_tex1;
#endif

//Fragment shader inputs from Vertex shader
layout(location=0) in vec4 fs_color;
layout(location=1) in vec2 fs_uv0;
#if defined(SHADER_TEX_2)
layout(location=2) in vec2 fs_uv1;
#endif

//Fragment shader outputs
out vec4 frag_color;

void main() {
    #if defined(SHADER_TEX_2)
    vec4 tex0 = texture(un_tex0, fs_uv0);
    if ((un_mode & (1 << 7)) != 0) { //PERIMETER_SOKOL_COLOR_MODE_MOD_COLOR_ADD_ALPHA
        float lerp = ((un_mode & 0xFF00) >> 8) / 255.0;
        tex0 = (
            tex0
            * vec4(lerp, lerp, lerp, 1.0)
            + vec4(1.0f - lerp, 1.0f - lerp, 1.0f - lerp, 0.0)
        );
    }
    //eColorMode
    switch (un_mode & 0xF) {
        default:
        case 0: { //COLOR_MOD Modulate
            frag_color = texture(un_tex1, fs_uv1) * tex0 * fs_color;
            break;
        }
        case 1: { //COLOR_ADD Add
            frag_color = (texture(un_tex1, fs_uv1) + tex0) * fs_color;
            break;
        }
        case 2: { //COLOR_MOD2 Modulate and mul x2
            frag_color = texture(un_tex1, fs_uv1) * tex0 * 2 * fs_color;
            break;
        }
        case 3: { //COLOR_MOD4 Modulate and mul x4
            frag_color = texture(un_tex1, fs_uv1) * tex0 * 4 * fs_color;
            break;
        }
    };
    #else //SHADER_TEX_2
    //Modulate each other, default
    frag_color = texture(un_tex0, fs_uv0) * fs_color;
    #endif //SHADER_TEX_2
}
@end

@program program vs fs