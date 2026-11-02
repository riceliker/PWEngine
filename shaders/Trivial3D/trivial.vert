#version 450

layout(set = 0, binding = 0, std430) readonly buffer ModelUniform {
    mat4 model[];
} model;

layout(set = 1, binding = 0) uniform LightUniform {
    vec3 light_dir;
    vec3 light_color;
    vec3 ambient;
} light;

layout(set = 2, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 proj;
} camera;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec3 in_normal;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 frag_uv;
layout(location = 2) out vec3 frag_world_pos;
layout(location = 3) out vec3 frag_world_normal;

void main() {
    mat4 m = model.model[gl_InstanceIndex];
    vec4 world_pos = m * vec4(in_position, 1.0);
    gl_Position = camera.proj * camera.view * world_pos;

    mat3 normal_mat = transpose(inverse(mat3(m)));
    vec3 world_n = normalize(normal_mat * in_normal);

    frag_color = in_color;
    frag_uv = in_uv;
    frag_world_pos = world_pos.xyz;
    frag_world_normal = world_n;
}