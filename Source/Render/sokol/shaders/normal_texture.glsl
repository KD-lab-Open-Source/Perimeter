@ctype mat4 Mat4f

@vs vs
//Uniforms
uniform vs_params {
    mat4 un_mvp;
};

//Vertex Buffer inputs
in vec4 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord0;
#if defined(SHADER_DOT3)
in vec3 vs_dot3_0;
in vec3 vs_dot3_1;
in vec3 vs_dot3_2;
#endif

//Fragment shader outputs
layout(location=0) out vec3 fs_normal;
layout(location=1) out vec2 fs_uv0;

void main() {
    gl_Position = un_mvp * vs_position;
    fs_normal = vs_normal;
    
    #if defined(SOKOL_GLSL)
    fs_uv0 = vec2(vs_texcoord0.x, 1.0f - vs_texcoord0.y);
    #else //SOKOL_GLSL
    fs_uv0 = vs_texcoord0;
    #endif //SOKOL_GLSL
}
@end

@fs fs
//Uniforms
uniform sampler2D un_tex0;

//Fragment shader inputs from Vertex shader
layout(location=0) in vec3 fs_normal;
layout(location=1) in vec2 fs_uv0;

//Fragment shader outputs
out vec4 frag_color;

void main() {
    frag_color = texture(un_tex0, fs_uv0);
}
@end

@program program vs fs