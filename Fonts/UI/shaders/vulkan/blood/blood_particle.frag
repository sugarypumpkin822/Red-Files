#version 450 core

// Blood Particle Fragment Shader for Vulkan
// Working blood particle fragment shader

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec4 vColor;
layout(location = 3) in vec4 vProperties;
layout(location = 4) in vec3 vWorldPos;
layout(location = 5) in vec3 vViewPos;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragNormal;
layout(location = 2) out vec4 fragMaterial;
layout(location = 3) out vec4 fragDepth;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;
    vec4 cameraPosition;
    vec4 time;
} camera;

layout(set = 0, binding = 1) uniform LightData {
    vec4 lightDirection;
    vec4 lightColor;
    vec4 lightIntensity;
    vec4 ambientColor;
} light;

layout(set = 0, binding = 2) uniform sampler2D particleTexture;

// Random number generator
float random(vec2 seed) {
    return fract(sin(dot(seed.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// Simple noise function
float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    
    float a = random(i.xy + p.z);
    float b = random(i.xy + vec2(1.0, 0.0) + p.z);
    float c = random(i.xy + vec2(0.0, 1.0) + p.z);
    float d = random(i.xy + vec2(1.0, 1.0) + p.z);
    
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

void main() {
    // Sample texture
    vec4 texColor = texture(particleTexture, vTexCoord);
    
    // Calculate lighting
    vec3 normal = normalize(vNormal);
    float NdotL = max(dot(normal, light.lightDirection.xyz), 0.0);
    
    vec3 ambient = light.ambientColor.rgb * light.ambientColor.a;
    vec3 diffuse = light.lightColor.rgb * light.lightColor.a * NdotL;
    
    // Apply blood color modulation
    vec3 bloodColor = mix(
        vec3(0.8, 0.1, 0.1),  // Fresh blood
        vec3(0.3, 0.05, 0.05), // Old blood
        vProperties.x  // Age factor
    );
    
    // Add some animated noise for fluid effect
    float noiseValue = noise(vWorldPos + camera.time.xxx * 0.5);
    vec3 noiseColor = vec3(noiseValue * 0.1);
    
    // Combine colors
    vec3 finalColor = bloodColor * texColor.rgb * vColor.rgb;
    finalColor += noiseColor;
    finalColor = finalColor * (ambient + diffuse);
    
    // Calculate alpha based on texture and properties
    float alpha = texColor.a * vColor.a * vProperties.y;
    
    // Add glow effect for fresh blood
    if (vProperties.x < 0.5) {
        float glow = 1.0 - vProperties.x * 2.0;
        finalColor += vec3(0.2, 0.05, 0.05) * glow * 0.5;
    }
    
    // Output final color
    fragColor = vec4(finalColor, alpha);
    fragNormal = vec4(normal, 1.0);
    fragMaterial = vec4(vProperties.xyz, 1.0);
    fragDepth = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
}
