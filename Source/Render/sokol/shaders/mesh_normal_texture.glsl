@ctype mat4 Mat4f

@vs vs
//Uniforms
uniform mesh_normal_texture_vs_params {
    mat4 un_mvp;
    mat4 model;
    mat4 tex0_mat;
};

//Vertex Buffer inputs
layout(location=0) in vec3 vs_position;
layout(location=1) in vec3 vs_normal;
layout(location=2) in vec2 vs_texcoord0;

//Fragment shader outputs
layout(location=0) out vec3 fs_normal;
layout(location=1) out vec2 fs_uv0;
layout(location=2) out vec3 fs_position;

void main() {
    gl_Position = un_mvp * vec4(vs_position, 1.0f);
    fs_normal = (model * vec4(vs_normal, 0)).xyz;
    fs_uv0 = (tex0_mat * vec4(vs_texcoord0, 1, 1)).xy;
    fs_position = (model * vec4(vs_position, 0)).xyz;
}
@end
 
@fs fs
//Uniforms
uniform mesh_normal_texture_fs_params {
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    vec3 light_dir;
    vec3 light_diffuse;
    vec3 light_ambient;
    vec3 light_specular;
    float spec_power; // specularPower
    float un_alpha_test;
    int material; // 0 - NONE; 1 - MAT_LIGHT
};
uniform sampler un_sampler0;
uniform texture2D un_tex0;

//Fragment shader inputs from Vertex shader
layout(location=0) in vec3 fs_normal;
layout(location=1) in vec2 fs_uv0;
layout(location=2) in vec3 fs_position;

//Fragment shader outputs
out vec4 frag_color;


const vec3 eye_pos = vec3(0, 0, 0);
const vec3 gamma_power = vec3(1.0/2.2);
const vec3 ambient_k = vec3(1);

vec4 gamma(vec4 c) {
    return vec4(pow(abs(c.xyz), gamma_power), c.w);
}

vec4 phong(vec3 pos, vec3 nrm, vec3 l, vec3 eye,
            vec3 l_ambient, vec3 l_diffuse, vec3 l_specular,
            vec3 ambient, vec3 diffuse, vec3 specular, float spec_power) {
    vec3 n = normalize(nrm);
    vec3 v = normalize(eye - pos);
    float n_dot_l = max(dot(n, l), 0.0);
    vec3 r = reflect(-l, n);
    float r_dot_v = max(dot(r, v), 0.0);
    float diff = n_dot_l;
    float spec = pow(abs(r_dot_v), spec_power) * n_dot_l;
    vec3 ambient_v = l_ambient * ambient * ambient_k;
    vec3 diffuse_v = l_diffuse * diffuse * diff;
    vec3 specular_v = l_specular * specular * spec;
    return vec4(ambient_v + diffuse_v + specular_v, 1.0);
}

void main() {
    frag_color = texture(sampler2D(un_tex0, un_sampler0), fs_uv0);
    if (material > 0) {
        frag_color = frag_color * gamma(
            phong(fs_position, fs_normal, -light_dir, eye_pos,
                    light_ambient, light_diffuse, light_specular,
                    ambient.xyz, diffuse.xyz, specular.xyz, spec_power
        ));
        frag_color.a *= diffuse.a;
    } else {
        frag_color.xyz = frag_color.xyz * ambient.xyz * ambient_k;
    }
    if (un_alpha_test >= frag_color.a) {
        discard;
    }
}
@end

@program program vs fs