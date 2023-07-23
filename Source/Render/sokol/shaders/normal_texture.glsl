@ctype mat4 Mat4f

@vs vs
//Uniforms
uniform normal_texture_vs_params {
    mat4 un_mvp;
};

//Vertex Buffer inputs
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord0;

//Fragment shader outputs
layout(location=0) out vec3 fs_normal;
layout(location=1) out vec2 fs_uv0;

void main() {
    gl_Position = un_mvp * vec4(vs_position, 1.0f);
    fs_normal = vs_normal;
    fs_uv0 = vs_texcoord0;
}
@end

@fs fs
//Uniforms
uniform normal_texture_fs_params {
    float un_alpha_test;
};
uniform sampler2D un_tex0;

//Fragment shader inputs from Vertex shader
layout(location=0) in vec3 fs_normal;
layout(location=1) in vec2 fs_uv0;

//Fragment shader outputs
out vec4 frag_color;

void main() {
    frag_color = texture(un_tex0, fs_uv0);
    if (un_alpha_test >= frag_color.a) discard;
}
@end

@program program vs fs