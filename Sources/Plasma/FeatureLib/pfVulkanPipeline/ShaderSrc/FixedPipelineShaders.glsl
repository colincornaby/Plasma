#version 450

// Include shared definitions
#include "ShaderTypes.h"

// ----- Vertex Shader -----
#ifdef VERTEX_SHADER
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inColor;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec4 fragColor;

layout(binding = 0) uniform GlobalUniformBuffer {
    GlobalUniforms globals;
};

void main() {
    vec4 worldPos = globals.modelMatrix * vec4(inPosition, 1.0);
    fragPosition = worldPos.xyz;
    fragNormal = mat3(globals.modelMatrix) * inNormal;
    fragTexCoord = inTexCoord;
    fragColor = inColor;
    
    gl_Position = globals.projMatrix * globals.viewMatrix * worldPos;
}
#endif

// ----- Fragment Shader -----
#ifdef FRAGMENT_SHADER
layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform GlobalUniformBuffer {
    GlobalUniforms globals;
};

layout(binding = 1) uniform MaterialUniformBuffer {
    MaterialUniforms material;
};

layout(binding = 2) uniform LightingUniformBuffer {
    LightingUniforms lighting;
};

layout(binding = 3) uniform sampler2D texSampler;

void main() {
    vec4 baseColor = fragColor;
    
    if (material.useTexture > 0) {
        baseColor *= texture(texSampler, fragTexCoord);
    }
    
    // Apply lighting
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(globals.cameraPos.xyz - fragPosition);
    
    vec3 ambientLight = lighting.ambientColor.rgb * lighting.ambientIntensity;
    vec3 diffuseLight = vec3(0.0);
    vec3 specularLight = vec3(0.0);
    
    // Apply lights
    for (int i = 0; i < lighting.lightCount; i++) {
        LightData light = lighting.lights[i];
        
        vec3 lightDir;
        float attenuation = 1.0;
        
        if (light.type == 0) {
            // Directional light
            lightDir = -light.direction.xyz;
        } else {
            // Point or spot light
            vec3 lightVec = light.position.xyz - fragPosition;
            float distance = length(lightVec);
            lightDir = normalize(lightVec);
            
            // Distance attenuation
            if (light.range > 0.0) {
                attenuation = max(0.0, 1.0 - distance / light.range);
                attenuation = attenuation * attenuation;
            }
            
            // Spot light cone attenuation
            if (light.type == 2) {
                float spotDot = dot(-lightDir, normalize(light.direction.xyz));
                float spotCutoff = cos(radians(light.spotAngle));
                
                if (spotDot > spotCutoff) {
                    float spotFactor = pow(max(0.0, spotDot), light.spotExponent);
                    attenuation *= spotFactor;
                } else {
                    attenuation = 0.0;
                }
            }
        }
        
        float diff = max(dot(normal, lightDir), 0.0);
        diffuseLight += light.color.rgb * diff * light.intensity * attenuation;
        
        // Specular (Blinn-Phong)
        if (material.specularIntensity > 0.0 && diff > 0.0) {
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(normal, halfwayDir), 0.0), material.specularPower);
            specularLight += light.color.rgb * spec * material.specularIntensity * attenuation;
        }
    }
    
    // Combine lighting
    vec3 finalLight = ambientLight + diffuseLight;
    vec3 finalColor = baseColor.rgb * finalLight + specularLight;
    
    // Apply opacity
    outColor = vec4(finalColor, baseColor.a * material.opacity);
}
#endif