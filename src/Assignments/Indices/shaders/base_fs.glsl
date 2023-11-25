#version 410

layout(location=0) out vec4 vFragColor;
layout(location=2) in vec4 a_vertex_color_out;

void main() {
    vFragColor = a_vertex_color_out;
}
