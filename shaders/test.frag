#version 450

    layout(location = 0) in vec3 frag_normal;
    layout(location = 1) in vec2 frag_uv;
    layout(location = 2) in vec3 frag_position;

    layout(location = 0) out vec4 out_color;

    layout(set = 2, binding = 0) uniform sampler2D texture_sampler;

    layout(set = 3, binding = 0) uniform DirectionLight {
        vec3 color;
        vec3 position;
    } environment_light;

    void main() {
        float ambient_strength = 0.1;
        vec3 ambient = ambient_strength * vec3(1, 1, 1);

        vec3 normal = normalize(frag_normal);
        vec3 light_direction = normalize(environment_light.position - frag_position);
        vec3 diffuse = max(dot(normal, light_direction), 0.0) * environment_light.color;

        vec3 color = (ambient + diffuse) * texture(texture_sampler, frag_uv).xyz;
        out_color = vec4(color, 1.0);
    }