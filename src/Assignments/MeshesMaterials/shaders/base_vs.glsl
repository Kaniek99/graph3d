#version 460

layout(location=0) in  vec4 a_vertex_position;
layout(location=1) in  vec4 a_vertex_color;
layout(location=2) out  vec4 a_vertex_color_out;

layout(std140, binding=1) uniform Transformations {
    mat4 PVM;
};

void main() {
    gl_Position =  PVM * a_vertex_position;
    a_vertex_color_out = a_vertex_color;
}
