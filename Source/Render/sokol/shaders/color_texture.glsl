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
    switch (un_mode) {
        default:
        case 0: {
            //Modulate each other, default
            frag_color = texture(un_tex1, fs_uv1) * texture(un_tex0, fs_uv0) * fs_color;
            break;
        }
        case 1: {
            //"color" var channels in this mode
            //R: RGB mod, G: A mod, B: Tex1 RGB mod, A: Tex1 RGB add
            frag_color = (
                texture(un_tex1, fs_uv1)
                * vec4(fs_color.b, fs_color.b, fs_color.b, 1.0)
                + vec4(fs_color.a, fs_color.a, fs_color.a, 0.0)
            )
            * texture(un_tex0, fs_uv0)
            * vec4(fs_color.r, fs_color.r, fs_color.r, fs_color.g);
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