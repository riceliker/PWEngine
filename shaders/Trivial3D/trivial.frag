#version 450

layout(set = 0, binding = 1) uniform sampler2D tex;

layout(set = 1, binding = 0) uniform LightUniform {
    vec3 light_dir;
    vec3 light_color;
    vec3 ambient;
} light;

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 frag_uv;
layout(location = 2) in vec3 frag_world_pos;
layout(location = 3) in vec3 frag_world_normal;

layout(location = 0) out vec4 out_color;

void main() {
    vec3 albedo = texture(tex, frag_uv).rgb * frag_color;

    vec3 N = normalize(frag_world_normal);
    vec3 L = normalize(-light.light_dir);

    float ndotl = max(dot(N, L), 0.0);
    vec3 diffuse = light.light_color * ndotl;
    vec3 total_light = light.ambient + diffuse;

    out_color = vec4(albedo * total_light, 1.0);
}