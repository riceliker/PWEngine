#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vWorldPos;
layout(location = 1) in vec3 vWorldNormal;
layout(location = 2) in vec2 vUv;
layout(location = 3) in vec3 vWorldTangent;
layout(location = 4) in vec3 vWorldBitangent;

layout(location = 0) out vec4 outColor;

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

// glTF metallic roughness material UBO
layout(set = 2, binding = 0, std140) uniform PbrMaterialUBO {
    vec4 baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
    float alphaCutoff;
    uint flags; // bit0: mask, bit1: transparent, bit2: doubleSided
    vec3 emissiveFactor;
    float pad0;
} mt;

// glTF Standard Texture
layout(set = 2, binding = 1) uniform sampler2D baseColorTexture;
layout(set = 2, binding = 2) uniform sampler2D metallicRoughnessTexture; // B:metallic, G:roughness
layout(set = 2, binding = 3) uniform sampler2D normalTexture;
layout(set = 2, binding = 4) uniform sampler2D occlusionTexture;
layout(set = 2, binding = 5) uniform sampler2D emissiveTexture;

// IBL
layout(set = 0, binding = 2) uniform samplerCube irradianceMap;
layout(set = 0, binding = 3) uniform samplerCube prefilterMap;
layout(set = 0, binding = 4) uniform sampler2D brdfLUT;

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = 3.14159265 * denom * denom;
    return nom / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return geometrySchlickGGX(NdotV, roughness) * geometrySchlickGGX(NdotL, roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    // sample texture
    vec4 baseTex = texture(baseColorTexture, vUv);
    vec3 baseColor = baseTex.rgb * mt.baseColorFactor.rgb;
    float alpha = baseTex.a * mt.baseColorFactor.a;

    vec2 mrSample = texture(metallicRoughnessTexture, vUv).bg;
    float metallic = mrSample.r * mt.metallicFactor;
    float roughness = mrSample.g * mt.roughnessFactor;
    float ao = texture(occlusionTexture, vUv).r;
    vec3 emissive = texture(emissiveTexture, vUv).rgb * mt.emissiveFactor;

    // alpha mask
    const uint ALPHA_MODE_MASK_BIT = 1u << 0u;
    if ((mt.flags & ALPHA_MODE_MASK_BIT) != 0)
    {
        if (alpha < mt.alphaCutoff)
            discard;
    }

    // TBN + world normal
    mat3 TBN = mat3(
        normalize(vWorldTangent),
        normalize(vWorldBitangent),
        normalize(vWorldNormal)
    );
    vec3 normalSample = texture(normalTexture, vUv).xyz * 2.0 - 1.0;
    vec3 N = normalize(TBN * normalSample);

    vec3 V = normalize(scene.cameraPos - vWorldPos);
    vec3 L = normalize(-scene.lightDir);
    vec3 H = normalize(V + L);

    // F0 not metal 0.04
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, baseColor, metallic);

    // Cook-Torrance BRDF
    float NDF = distributionGGX(N, H, roughness);
    float G   = geometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    vec3 direct = (kD * baseColor / 3.14159265 + specular) * scene.lightColor * NdotL;

    // IBL
    vec3 F_ibl = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS_ibl = F_ibl;
    vec3 kD_ibl = 1.0 - kS_ibl;
    kD_ibl *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuseIBL = irradiance * baseColor * kD_ibl;

    vec3 R = reflect(-V, N);
    vec3 prefiltered = textureLod(prefilterMap, R, roughness * 4.0).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V),0.0), roughness)).rg;
    vec3 specIBL = prefiltered * (F_ibl * brdf.x + brdf.y);

    vec3 ambient = (diffuseIBL + specIBL) * ao;

    vec3 color = direct + ambient + emissive;

    //
    outColor = vec4(color, alpha);
}