@ctype mat4 Mat4f

@vs vs
//Uniforms
uniform mesh_texture_vs_params {
    mat4 un_mvp;
};

//Vertex Buffer inputs
in vec3 vs_position;
in vec2 vs_texcoord0;

//Fragment shader outputs
layout(location=0) out vec2 fs_uv0;

void main() {
    gl_Position = un_mvp * vec4(vs_position, 1.0f);
    fs_uv0 = vs_texcoord0;
}
@end

@fs fs
uniform sampler un_sampler0;
uniform texture2D un_tex0;

//Fragment shader inputs from Vertex shader
layout(location=0) in vec2 fs_uv0;

//Fragment shader outputs
out vec4 frag_color;

void main() {
    frag_color = texture(sampler2D(un_tex0, un_sampler0), fs_uv0);
}
@end

@program program vs fs
