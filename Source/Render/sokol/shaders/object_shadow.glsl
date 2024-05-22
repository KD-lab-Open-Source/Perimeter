@ctype mat4 Mat4f
@ctype vec4 Vect4f

@vs vs

@glsl_options fixup_clipspace // important: map clipspace z from -1..+1 to 0..+1 on GL

//Uniforms
uniform object_shadow_vs_params {
    mat4 un_mvp;
};

//Vertex Buffer inputs
layout(location=0) in vec3 vs_position;

void main() {
    gl_Position = un_mvp * vec4(vs_position, 1.0f);
}
@end

@fs fs

void main() {
}
@end

@program program vs fs
