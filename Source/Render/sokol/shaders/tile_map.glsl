@ctype mat4 Mat4f
@ctype vec4 Vect4f

@vs vs
//Uniforms
uniform tile_map_vs_params {
    mat4 un_mvp;
    mat4 un_shadow;
    vec2 un_inv_world_size;
};

//Vertex Buffer inputs
in vec3 vs_position;
in vec2 vs_texcoord0;

//Fragment shader outputs
out vec3 fs_uv0;
out vec2 fs_uv1;
out vec2 fs_uv2;

void main() {
    gl_Position = un_mvp * vec4(vs_position, 1.0f);

    float offset = 1.0f / 1024.0f;
    fs_uv0 = (un_shadow * vec4(vs_position, 1.0f) + vec4(-offset, +offset*0.5f, 0, 0)).xyz;
#if SOKOL_GLSL
    fs_uv0.y = 1.0f - fs_uv0.y;
#endif
    fs_uv1 = vs_texcoord0;
    fs_uv2 = vs_position.xy * un_inv_world_size;
}
@end

@fs fs
//Uniforms
uniform tile_map_fs_params {
    vec4 un_tile_color;
};
uniform sampler un_sampler0;
uniform sampler un_sampler1;
uniform texture2D un_tex0; // shadow map
uniform texture2D un_tex1; // diffuse
uniform texture2D un_tex2; // light map

//Fragment shader inputs from Vertex shader
in vec3 fs_uv0; // shadow map
in vec2 fs_uv1; // diffuse
in vec2 fs_uv2; // light map

//Fragment shader outputs
out vec4 frag_color;

//SHADE - коэффициэнт, не который умножается цвет объекта в тени.
#define SHADE 0.5f

//MSHADE=1-SHADE
#define MSHADE 0.5f

void main() {
    frag_color = texture(sampler2DShadow(un_tex0, un_sampler1), fs_uv0).rrrr;

    // ambient color
    vec4 c0 = vec4(SHADE, MSHADE, 0, 1.0f);
    frag_color = frag_color * c0.y + c0.x;

    // diffuse
    frag_color = frag_color * texture(sampler2D(un_tex1, un_sampler0), fs_uv1);

    // tile color
    frag_color = frag_color * un_tile_color;

    // light map
    frag_color = frag_color * texture(sampler2D(un_tex2, un_sampler0), fs_uv2);

    frag_color.a = un_tile_color.a;
}
@end

@program program vs fs
