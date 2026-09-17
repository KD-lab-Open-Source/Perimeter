#version 450

layout(set = 0, binding = 1) uniform FS_Params {
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    vec3 light_dir;
    float _pad0;
    vec3 light_diffuse;
    float _pad1;
    vec3 light_ambient;
    float _pad2;
    vec3 light_specular;
    float spec_power;
    float un_alpha_test;
    int material;
    // Two scalar slots to push eye_pos to its 16-byte-aligned offset (144).
    // A `float _pad[2]` array would be 32 bytes in std140 - do not use one.
    float _pad_align0;
    float _pad_align1;
    vec3 eye_pos;
    float _pad3;
} fs_params;

layout(set = 0, binding = 2) uniform sampler2D un_tex0;

layout(location = 0) in vec3 fs_normal;
layout(location = 1) in vec2 fs_uv0;
layout(location = 2) in vec3 fs_position;

layout(location = 0) out vec4 frag_color;

const vec3 gamma_power = vec3(1.0 / 2.2);
const vec3 ambient_k = vec3(1.0);

vec4 gamma_correct(vec4 c) {
    return vec4(pow(abs(c.xyz), gamma_power), c.w);
}

vec4 phong(vec3 pos, vec3 nrm, vec3 l, vec3 eye,
           vec3 l_ambient, vec3 l_diffuse, vec3 l_specular,
           vec3 amb, vec3 diff, vec3 spec, float sp) {
    vec3 n = normalize(nrm);
    vec3 v = normalize(eye - pos);
    float n_dot_l = max(dot(n, l), 0.0);
    vec3 r = reflect(-l, n);
    float r_dot_v = max(dot(r, v), 0.0);
    float d = n_dot_l;
    float s = pow(abs(r_dot_v), sp);
    return vec4(l_ambient * amb * ambient_k + l_diffuse * diff * d + l_specular * spec * s, 1.0);
}

void main() {
    frag_color = texture(un_tex0, fs_uv0);
    if (fs_params.material > 0) {
        frag_color = frag_color * gamma_correct(
            phong(fs_position, fs_normal, -fs_params.light_dir, fs_params.eye_pos,
                  fs_params.light_ambient, fs_params.light_diffuse, fs_params.light_specular,
                  fs_params.ambient.xyz, fs_params.diffuse.xyz, fs_params.specular.xyz,
                  fs_params.spec_power));
        frag_color.a *= fs_params.diffuse.a;
    } else {
        frag_color.xyz = frag_color.xyz * fs_params.ambient.xyz * ambient_k;
    }
    if (fs_params.un_alpha_test >= frag_color.a) discard;
}
