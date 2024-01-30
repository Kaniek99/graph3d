#version 460

layout(location=0) out vec4 vFragColor;

#if __VERSION__ > 410
layout(std140, binding=0) uniform Modifiers {
#else
    layout(std140) uniform Color {
    #endif
    vec4  Kd;
    bool use_map_Kd;
};

in vec2 vertex_texcoords;
in vec3 vertex_normals_in_vs;
in vec4 vertex_coords_in_vs;
in vec3 vertex_normals;

const int MAX_POINT_LIGHTS=24;

struct PointLight {
    vec3 position_in_world_space;
    vec3 position_in_view_space;
    vec3 color;
    float intensity;
    float radius;
} ;

layout(std140, binding=2) uniform Lights {
    vec3 ambient;
    uint n_p_lights;
    PointLight p_light[MAX_POINT_LIGHTS];
};

uniform sampler2D map_Kd;

void main() {
    if (use_map_Kd){
        vFragColor = Kd*texture(map_Kd, vertex_texcoords);
    } else {
        vec3 normal = normalize(vertex_normals);
        vec3 diffuse;
        for (uint i = 0u; i < n_p_lights; i++) {
            vec3 lightDir = normalize(p_light[i].position_in_view_space - vertex_coords_in_vs.xyz);
            float diff = max(dot(normal, lightDir), 0.0f);
            diffuse += diff * p_light[i].color; //* p_light[i].intensity;
        }

        vFragColor.a = Kd.a;
        vFragColor.rgb = Kd.rgb * (ambient + diffuse);
    }
}