@ctype mat4 Mat4f

@vs vs
//Uniforms
uniform vs_params {
    mat4 un_mvp;
    vec2 un_map_pos_start;
    vec2 un_map_uv_step;
    vec2 un_map_uv_base;
};

//Vertex Buffer inputs
in vec3 vs_position;

//Fragment shader outputs
layout(location=0) out vec2 fs_uv0;

void main() {
    gl_Position = un_mvp * vec4(vs_position, 1.0f);
    fs_uv0 = vs_position.xy / un_map_pos_start * un_map_uv_step + un_map_uv_base;
}
@end

@fs fs
//Uniforms
uniform sampler2D un_tex0;

//Fragment shader inputs from Vertex shader
layout(location=0) in vec2 fs_uv0;

//Fragment shader outputs
out vec4 frag_color;

void main() {
    frag_color = texture(un_tex0, fs_uv0);
}
@end

@program program vs fs