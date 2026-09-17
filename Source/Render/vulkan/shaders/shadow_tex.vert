#version 450

layout(set = 0, binding = 0) uniform VS_Params {
    mat4 un_mvp;
} vs_params;

layout(location = 0) in vec3 vs_position;
#ifdef HAS_NORMAL
layout(location = 1) in vec3 vs_normal;
#endif
layout(location = 3) in vec2 vs_texcoord0;
#ifdef USE_INSTANCING
layout(location = 8)  in vec4 inst_col0;
layout(location = 9)  in vec4 inst_col1;
layout(location = 10) in vec4 inst_col2;
layout(location = 11) in vec4 inst_col3;
#endif

layout(location = 0) out vec2 fs_uv0;

void main() {
#ifdef USE_INSTANCING
    mat4 inst_world = mat4(inst_col0, inst_col1, inst_col2, inst_col3);
    gl_Position = vs_params.un_mvp * inst_world * vec4(vs_position, 1.0);
#else
    gl_Position = vs_params.un_mvp * vec4(vs_position, 1.0);
#endif
    fs_uv0 = vs_texcoord0;
}
