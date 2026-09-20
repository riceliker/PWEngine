#version 450

layout(set = 0, binding = 0) uniform ModelUniform {
    mat4 model;
} model;

layout(set = 2, binding = 0) uniform CameraUniform {
    mat4 view;
    mat4 proj;
} camera;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 frag_uv;

void main() {
    gl_Position = camera.proj * camera.view * model.model * vec4(in_position, 1.0);
    frag_color = in_color;
    frag_uv = in_uv;
}