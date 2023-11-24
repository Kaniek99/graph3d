#version 410

layout(location=0) in  vec4 a_vertex_position;
layout(location=1) in  vec4 a_vertex_color;
layout(location=2) out  vec4 a_vertex_color_out;

void main() {
    gl_Position = a_vertex_position;
    a_vertex_color_out = a_vertex_color;
}
