@ctype mat4 Mat4f

@vs vs
//Uniforms
uniform vs_params {
    mat4 un_mvp;
};

//Vertex Buffer inputs
in vec3 vs_position;
#if defined(SHADER_COLOR)
in vec4 vs_color;
#endif
#if defined(SHADER_TEX_1) || defined(SHADER_TEX_2)
in vec2 vs_texcoord0;
#endif
#if defined(SHADER_TEX_2)
in vec2 vs_texcoord1;
#endif

//Fragment shader outputs
#if defined(SHADER_COLOR)
layout(location=0) out vec4 fs_color;
#endif
#if defined(SHADER_TEX_1) || defined(SHADER_TEX_2)
layout(location=1) out vec2 fs_uv0;
#endif
#if defined(SHADER_TEX_2)
layout(location=2) out vec2 fs_uv1;
#endif

void main() {
    gl_Position = un_mvp * vec4(vs_position, 1.0f);

    #if defined(SHADER_COLOR)
    fs_color = vs_color;
    #endif
    #if defined(SHADER_TEX_1) || defined(SHADER_TEX_2)
    fs_uv0 = vs_texcoord0;
    #endif  
    #if defined(SHADER_TEX_2)
    fs_uv1 = vs_texcoord1;
    #endif
}
@end

@fs fs
//Uniforms
#if defined(SHADER_TEX_1) || defined(SHADER_TEX_2)
uniform sampler2D un_tex0;
#endif
#if defined(SHADER_TEX_2)
uniform sampler2D un_tex1;
#endif

//Fragment shader inputs from Vertex shader
#if defined(SHADER_COLOR)
layout(location=0) in vec4 fs_color;
#endif
#if defined(SHADER_TEX_1) || defined(SHADER_TEX_2)
layout(location=1) in vec2 fs_uv0;
#endif
#if defined(SHADER_TEX_2)
layout(location=2) in vec2 fs_uv1;
#endif

//Fragment shader outputs
out vec4 frag_color;

void main() {
    #if defined(SHADER_COLOR)
    vec4 color = fs_color;
    #else
    vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
    #endif
    #if defined(SHADER_TEX_2)
    color = texture(un_tex1, fs_uv1) * color;
    #endif
    #if defined(SHADER_TEX_1) || defined(SHADER_TEX_2)
    color = texture(un_tex0, fs_uv0) * color;
    #endif
    frag_color = color;
}
@end

@program program vs fs