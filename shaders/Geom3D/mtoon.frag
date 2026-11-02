#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vWorldPos;
layout(location = 1) in vec3 vWorldNormal;
layout(location = 2) in vec2 vUv;
layout(location = 3) in vec3 vWorldTangent;
layout(location = 4) in vec3 vWorldBitangent;

layout(location = 0) out vec4 outColor;

// Scene
layout(set = 0, binding = 0, std140) uniform SceneUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPos;
    vec3 lightDir;
    vec3 lightColor;
    float time;
} scene;

// Object
layout(set = 1, binding = 0, std140) uniform ObjectUBO {
    mat4 model;
    mat4 normalMatrix;
    mat4 bones[128];
} obj;

// MToon Material UBO
layout(set = 2, binding = 0, std140) uniform MToonMaterialUBO {
    vec4 baseColorFactor;
    vec3 shadeColorFactor;
    float shadingShiftFactor;

    float shadingToonyFactor;
    float shadingShiftTextureScale;
    float giEqualizationFactor;
    float alphaCutoff;

    vec3 matcapFactor;
    float parametricRimFresnelPowerFactor;

    vec3 parametricRimColorFactor;
    float parametricRimLiftFactor;

    float rimLightingMixFactor;
    float outlineWidthFactor;
    int outlineWidthMode;
    float outlineLightingMixFactor;

    vec3 outlineColorFactor;
    float uvAnimationScrollXSpeedFactor;

    float uvAnimationScrollYSpeedFactor;
    float uvAnimationRotationSpeedFactor;
    uint flags;
} mt;

// Textures
layout(set = 2, binding = 1) uniform sampler2D baseColorTexture;
layout(set = 2, binding = 2) uniform sampler2D shadeMultiplyTexture;
layout(set = 2, binding = 3) uniform sampler2D shadingShiftTexture;
layout(set = 2, binding = 4) uniform sampler2D normalTexture;
layout(set = 2, binding = 5) uniform sampler2D matcapTexture;
layout(set = 2, binding = 6) uniform sampler2D rimMultiplyTexture;
layout(set = 2, binding = 7) uniform sampler2D outlineWidthMultiplyTexture;
layout(set = 2, binding = 8) uniform sampler2D uvAnimationMaskTexture;
layout(set = 2, binding = 9) uniform sampler2D emissiveTexture;

float linearstep(float edge0, float edge1, float x)
{
    return clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
}

vec2 animateUv(vec2 uv)
{
    vec2 mask = texture(uvAnimationMaskTexture, uv).rg;
    float maskVal = mask.r;

    // Scroll
    vec2 scroll = vec2(mt.uvAnimationScrollXSpeedFactor, mt.uvAnimationScrollYSpeedFactor) * scene.time * maskVal;
    uv += scroll;

    // Rotate
    float rotSpeed = mt.uvAnimationRotationSpeedFactor * scene.time * maskVal;
    float c = cos(rotSpeed);
    float s = sin(rotSpeed);
    vec2 center = vec2(0.5);
    uv -= center;
    uv = vec2(uv.x * c - uv.y * s, uv.x * s + uv.y * c);
    uv += center;
    return uv;
}

void main()
{
    vec2 uv = animateUv(vUv);

    // 1. Base / Shade sampler
    vec4 baseTex = texture(baseColorTexture, uv);
    vec4 shadeTex = texture(shadeMultiplyTexture, uv);
    vec3 baseColor = baseTex.rgb * mt.baseColorFactor.rgb;
    vec3 shadeColor = shadeTex.rgb * mt.shadeColorFactor;
    float alpha = baseTex.a * mt.baseColorFactor.a;

    // Alpha mask
    const uint ALPHA_MODE_MASK_BIT = 1u << 0u;
    if ((mt.flags & ALPHA_MODE_MASK_BIT) != 0)
    {
        if (alpha < mt.alphaCutoff)
            discard;
    }

    // 2. Normal map (TBN)
    mat3 TBN = mat3(
        normalize(vWorldTangent),
        normalize(vWorldBitangent),
        normalize(vWorldNormal)
    );
    vec3 normalSample = texture(normalTexture, uv).xyz;
    normalSample = normalSample * 2.0 - 1.0;
    vec3 N = normalize(TBN * normalSample);

    vec3 L = normalize(-scene.lightDir);
    vec3 V = normalize(scene.cameraPos - vWorldPos);

    // 3. Shading shift + toon step（MToon)
    float NdotL = dot(N, L);
    float shiftTex = texture(shadingShiftTexture, uv).r * mt.shadingShiftTextureScale;
    float shadingValue = NdotL + mt.shadingShiftFactor + shiftTex;

    float toony = mt.shadingToonyFactor;
    float lit = linearstep(-1.0 + toony, 1.0 - toony, shadingValue);
    vec3 directLit = mix(shadeColor, baseColor, lit) * scene.lightColor;

    // GI Equalization
    vec3 giColor = baseColor * mt.giEqualizationFactor + shadeColor * (1.0 - mt.giEqualizationFactor);

    // 4. Rim / Matcap
    vec3 rimMultiply = texture(rimMultiplyTexture, uv).rgb;
    float fresnel = 1.0 - dot(N, V);
    fresnel = pow(fresnel, mt.parametricRimFresnelPowerFactor);
    fresnel += mt.parametricRimLiftFactor;
    fresnel = clamp(fresnel, 0.0, 1.0);
    vec3 rim = mt.parametricRimColorFactor * rimMultiply * fresnel;

    // Matcap
    vec2 matcapUv = N.xy * 0.5 + 0.5;
    vec3 matcapCol = texture(matcapTexture, matcapUv).rgb * mt.matcapFactor;
    vec3 rimComposite = rim + matcapCol;

    // LightMix
    rimComposite = mix(rimComposite, rimComposite * scene.lightColor, mt.rimLightingMixFactor);

    // 5. Emissive
    vec3 emissive = texture(emissiveTexture, uv).rgb;

    // 6. Final
    vec3 final = directLit + giColor * 0.0;
    final += rimComposite;
    final += emissive;

    outColor = vec4(final, alpha);
}