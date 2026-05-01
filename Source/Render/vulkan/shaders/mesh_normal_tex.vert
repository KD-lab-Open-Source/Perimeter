#version 450

layout(set = 0, binding = 0) uniform VS_Params {
    mat4 un_mvp;
    mat4 model;
    mat4 tex0_mat;
} vs_params;

layout(location = 0) in vec3 vs_position;
layout(location = 1) in vec3 vs_normal;
layout(location = 3) in vec2 vs_texcoord0;
#ifdef USE_INSTANCING
layout(location = 8)  in vec4 inst_col0;
layout(location = 9)  in vec4 inst_col1;
layout(location = 10) in vec4 inst_col2;
layout(location = 11) in vec4 inst_col3;
#endif

layout(location = 0) out vec3 fs_normal;
layout(location = 1) out vec2 fs_uv0;
layout(location = 2) out vec3 fs_position;

void main() {
#ifdef USE_INSTANCING
    mat4 inst_world = mat4(inst_col0, inst_col1, inst_col2, inst_col3);
    gl_Position = vs_params.un_mvp * inst_world * vec4(vs_position, 1.0);
    fs_normal = (inst_world * vec4(vs_normal, 0.0)).xyz;
    fs_position = (inst_world * vec4(vs_position, 1.0)).xyz;
#else
    gl_Position = vs_params.un_mvp * vec4(vs_position, 1.0);
    fs_normal = (vs_params.model * vec4(vs_normal, 0.0)).xyz;
    fs_position = (vs_params.model * vec4(vs_position, 1.0)).xyz;
#endif
    fs_uv0 = (vs_params.tex0_mat * vec4(vs_texcoord0, 1.0, 1.0)).xy;
}
