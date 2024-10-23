@ctype mat4 Mat4f

@vs vs

@glsl_options fixup_clipspace // important: map clipspace z from -1..+1 to 0..+1 on GL

//Uniforms
uniform shadow_texture_vs_params {
    mat4 un_mvp;
};

//Vertex Buffer inputs
layout(location=0) in vec3 vs_position;
#if defined(SHADER_NORMAL)
layout(location=1) in vec3 vs_normal;
layout(location=2) in vec2 vs_texcoord0;
#else
layout(location=1) in vec2 vs_texcoord0;
#endif

//Fragment shader outputs
layout(location=0) out vec2 fs_uv0;

void main() {
    gl_Position = un_mvp * vec4(vs_position, 1.0f);
    fs_uv0 = vs_texcoord0;
}
@end

@fs fs

//Uniforms
uniform shadow_texture_fs_params {
    float un_alpha_test;
};
uniform sampler un_sampler0;
uniform texture2D un_tex0;

//Fragment shader inputs from Vertex shader
layout(location=0) in vec2 fs_uv0;

//Fragment shader outputs

void main() {
    vec4 color = texture(sampler2D(un_tex0, un_sampler0), fs_uv0);
    if (un_alpha_test >= color.a) discard;
}
@end

@program program vs fs
