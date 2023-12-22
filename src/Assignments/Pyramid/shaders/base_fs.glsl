#version 460

layout(location=0) out vec4 vFragColor;
layout(location=2) in vec4 a_vertex_color_out;

layout(std140, binding = 0) uniform Modifier {
    float strength;
    vec3  color;
};

void main() {
    vFragColor = vec4(color[0]*strength, color[1]*strength, color[2]*strength, 1.0);
}
