@ctype mat4 Mat4f

@vs vs
//Uniforms
uniform mesh_color_texture_vs_params {
    mat4 un_mvp;
    mat4 tex0_mat;
    mat4 tex1_mat;
};

//Vertex Buffer inputs
layout(location=0) in vec3 vs_position;
layout(location=1) in vec4 vs_color;
layout(location=2) in vec2 vs_texcoord0;
#if defined(SHADER_TEX_2)
layout(location=3) in vec2 vs_texcoord1;
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
    fs_uv0 = (tex0_mat * vec4(vs_texcoord0, 1, 1)).xy;
    #if defined(SHADER_TEX_2)
    fs_uv1 = (tex1_mat * vec4(vs_texcoord1, 1, 1)).xy;
    #endif
}
@end

@fs fs
//Uniforms
uniform mesh_color_texture_fs_params {
    int un_color_mode;
    float un_tex2_lerp;
    float un_alpha_test;
};
uniform sampler un_sampler0;
uniform texture2D un_tex0;
#if defined(SHADER_TEX_2)
uniform texture2D un_tex1;
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
    vec4 tex0 = texture(sampler2D(un_tex0, un_sampler0), fs_uv0);
    if (0 <= un_tex2_lerp) { //Modulate color add alpha
        tex0 = (
            tex0
            * vec4(un_tex2_lerp, un_tex2_lerp, un_tex2_lerp, 1.0)
            + vec4(1.0f - un_tex2_lerp, 1.0f - un_tex2_lerp, 1.0f - un_tex2_lerp, 0.0)
        );
    }
    //eColorMode
    vec4 tex1 = texture(sampler2D(un_tex1, un_sampler0), fs_uv1);
    switch (un_color_mode) {
        default:
        case 0: { //COLOR_MOD Modulate
            frag_color = tex1 * tex0 * fs_color;
            break;
        }
        case 1: { //COLOR_ADD Add
            frag_color = (tex1 + tex0) * fs_color;
            break;
        }
        case 2: { //COLOR_MOD2 Modulate and mul x2
            frag_color = tex1 * tex0 * 2 * fs_color;
            break;
        }
        case 3: { //COLOR_MOD4 Modulate and mul x4
            frag_color = tex1 * tex0 * 4 * fs_color;
            break;
        }
    };
    #else //SHADER_TEX_2
    //Modulate each other
    frag_color = texture(sampler2D(un_tex0, un_sampler0), fs_uv0) * fs_color;
    #endif //SHADER_TEX_2
    if (un_alpha_test >= frag_color.a) discard;
}
@end

@program program vs fs
