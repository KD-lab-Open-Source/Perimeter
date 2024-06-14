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
out vec3 fs_uv1;
out vec3 fs_uv2;
out vec3 fs_uv3;
out vec2 fs_uv4;
out vec2 fs_uv5;

const float offset = 1.0f / 1024.0f;

void main() {
    gl_Position = un_mvp * vec4(vs_position, 1.0f);

    const vec4 p = un_shadow * vec4(vs_position, 1.0f);
    fs_uv0 = (p + vec4(-offset, +offset*0.5f, 0, 0)).xyz;
    fs_uv1 = (p + vec4(offset, -offset*0.5f, 0, 0)).xyz;
    fs_uv2 = (p + vec4(0, -offset, 0, 0)).xyz;
    fs_uv3 = (p + vec4(0, offset, 0, 0)).xyz;
#if SOKOL_GLSL
    fs_uv0.y = 1.0f - fs_uv0.y;
    fs_uv1.y = 1.0f - fs_uv1.y;
    fs_uv2.y = 1.0f - fs_uv2.y;
    fs_uv3.y = 1.0f - fs_uv3.y;
#endif

    fs_uv4 = vs_texcoord0;
    fs_uv5 = vs_position.xy * un_inv_world_size;
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
in vec3 fs_uv0; // shadow map 1
in vec3 fs_uv1; // shadow map 2
in vec3 fs_uv2; // shadow map 3
in vec3 fs_uv3; // shadow map 4
in vec2 fs_uv4; // diffuse
in vec2 fs_uv5; // light map

//Fragment shader outputs
out vec4 frag_color;

//SHADE - коэффициент, на который умножается цвет объекта в тени.
#define SHADE 0.5f

//MSHADE=1-SHADE
#define MSHADE 0.5f

// ambient color
const vec3 c0 = vec3(SHADE, MSHADE, 0.25f);
const vec3 c4 = vec3(0.25f, 0.25f, 0.25f);

void main() {
    // shadow
    float shadow = texture(sampler2DShadow(un_tex0, un_sampler1), fs_uv0).r;
    shadow += texture(sampler2DShadow(un_tex0, un_sampler1), fs_uv1).r;
    shadow += texture(sampler2DShadow(un_tex0, un_sampler1), fs_uv2).r;
    shadow += texture(sampler2DShadow(un_tex0, un_sampler1), fs_uv3).r;
    vec3 result = c4 * shadow * c0.y + c0.x;

    // diffuse
    result *= texture(sampler2D(un_tex1, un_sampler0), fs_uv4).rgb;

    // tile color
    result *= un_tile_color.rgb;

    // light map
    result *= texture(sampler2D(un_tex2, un_sampler0), fs_uv5).rgb;

    frag_color = vec4(result, un_tile_color.a);
}
@end

@program program vs fs
