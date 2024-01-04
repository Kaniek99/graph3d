#version 460

layout(location=0) out vec4 vFragColor;
layout(location=2) in vec4 a_vertex_color_out;

layout(std140, binding = 0) uniform Modifier {
    float strength;
    vec3  color;
};

void main() {
    vFragColor = vec4(a_vertex_color_out[0]*color[0]*strength, a_vertex_color_out[1]*color[1]*strength, a_vertex_color_out[2]*color[2]*strength, a_vertex_color_out[3]*1.0);
}
