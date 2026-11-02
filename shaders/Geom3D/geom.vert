#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in uvec4 aJoint;
layout(location = 4) in vec4 aWeight;
layout(location = 5) in vec4 aTangent;

layout(location = 0) out vec3 vWorldPos;
layout(location = 1) out vec3 vWorldNormal;
layout(location = 2) out vec2 vUv;
layout(location = 3) out vec3 vWorldTangent;
layout(location = 4) out vec3 vWorldBitangent;

layout(set = 0, binding = 0, std140) uniform SceneUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPos;
    vec3 lightDir;
    vec3 lightColor;
    float time;
} scene;

layout(set = 1, binding = 0, std140) uniform ObjectUBO {
    mat4 model;
    mat4 normalMatrix;
    mat4 bones[128];
} obj;

layout(set = 1, binding = 1, std140) uniform MorphWeightsUBO
{
    float weights[16];
    uint morphCount;
    uint vertexCount;
    uint pad;
} morph_w;

struct MorphDelta
{
    vec3 pos;
    vec3 normal;
    vec3 tangent;
};

layout(set = 1, binding = 2, std430) buffer MorphDeltaBuffer
{
    MorphDelta deltas[];
} morph_d;


void main()
{
    // value
    vec3 localPos = aPos;
    vec3 localN   = aNormal;
    vec3 localT   = aTangent.xyz;

    // morph
    uint vid = gl_VertexIndex;
    for(uint m = 0; m < morph_w.morphCount; m++)
    {
        float w = morph_w.weights[m];
        if(w <= 0.0001) continue;

        uint idx = m * morph_w.vertexCount + vid;
        MorphDelta d = morph_d.deltas[idx];

        localPos += d.pos * w;
        localN   += d.normal * w;
        localT   += d.tangent * w;
    }

    // bones
    mat4 skin = mat4(0.0);
    skin += aWeight.x * obj.bones[aJoint.x];
    skin += aWeight.y * obj.bones[aJoint.y];
    skin += aWeight.z * obj.bones[aJoint.z];
    skin += aWeight.w * obj.bones[aJoint.w];

    vec4 skinnedPos = skin * vec4(localPos, 1.0);
    vec3 skinnedN = mat3(skin) * localN;
    vec3 skinnedT = mat3(skin) * localT;

    vec4 worldPos = obj.model * skinnedPos;
    vWorldPos = worldPos.xyz;
    vWorldNormal = normalize(mat3(obj.normalMatrix) * skinnedN);
    vWorldTangent = normalize(mat3(obj.normalMatrix) * skinnedT);
    vWorldBitangent = normalize(cross(vWorldNormal, vWorldTangent) * aTangent.w);

    vUv = aUv;
    gl_Position = scene.proj * scene.view * worldPos;
}