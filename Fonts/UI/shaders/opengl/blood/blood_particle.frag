#version 330 core

// Blood Particle Fragment Shader for OpenGL
// Creates realistic blood particle effects with physics-based simulation

// Uniforms
uniform float uTime;                    // Animation time
uniform vec2 uResolution;              // Screen resolution
uniform vec3 uBloodColor;              // Base blood color
uniform vec3 uBloodColorDark;          // Darker blood color for depth
uniform vec3 uBloodColorLight;         // Lighter blood color for highlights
uniform float uIntensity;             // Blood intensity/brightness
uniform float uParticleSize;           // Particle size
uniform float uParticleLife;           // Particle lifetime
uniform float uParticleFade;           // Particle fade rate
uniform vec4 uParticleParams;          // Particle parameters (count, size, spread, randomness)
uniform vec4 uPhysicsParams;           // Physics parameters (gravity, drag, bounce, turbulence)
uniform vec4 uNoiseParams;             // Noise parameters (scale, octaves, persistence, lacunarity)
uniform vec3 uLightDirection;          // Light direction
uniform vec3 uLightColor;              // Light color
uniform vec3 uAmbientColor;            // Ambient light color
uniform float uAmbientIntensity;       // Ambient light intensity
uniform float uSpecularIntensity;       // Specular highlight intensity
uniform float uShininess;               // Specular shininess factor
uniform float uRimLightIntensity;       // Rim lighting intensity
uniform vec3 uViewDirection;           // Camera/view direction
uniform vec3 uEmitterPosition;         // Particle emitter position
uniform float uEmitterRadius;          // Emitter radius
uniform vec4 uEmitterParams;            // Emitter parameters (rate, spread, velocity, lifetime)

// Textures
uniform sampler2D uBloodTexture;        // Blood texture
uniform sampler2D uNoiseTexture;        // Noise texture
uniform sampler2D uNormalMap;          // Normal map
uniform sampler2D uRoughnessMap;        // Roughness map
uniform sampler2D uThicknessMap;        // Thickness map

// Input from vertex shader
in VS_OUT {
    vec4 Position;                     // Clip space position
    vec2 TexCoord;                      // Texture coordinates
    vec3 Normal;                        // Normal vector
    vec3 Tangent;                       // Tangent vector
    vec3 Bitangent;                     // Bitangent vector
    vec3 WorldPosition;                 // World space position
    vec4 Color;                         // Vertex color
    vec2 ParticleUV;                    // Particle UV coordinates
    vec3 ViewDirection;                  // View direction
    vec4 ScreenPosition;                 // Screen space position
    float ParticleAge;                   // Particle age
    float ParticleLife;                  // Particle lifetime
    vec3 ParticleVelocity;              // Particle velocity
    vec4 InstanceData;                  // Instance data
    float FogFactor;                     // Fog factor
} fs_in;

// Output
out vec4 FragColor;

// Utility functions
vec3 GenerateParticleNormal(vec2 uv, float time, float particleAge)
{
    // Generate dynamic normal map for particle surface
    vec2 noiseUV = uv * uNoiseParams.x + time * 2.0 + particleAge * 0.5;
    float noise1 = texture(uNoiseTexture, noiseUV).r;
    float noise2 = texture(uNoiseTexture, noiseUV * 2.0).r;
    float noise3 = texture(uNoiseTexture, noiseUV * 4.0).r;
    
    // Combine noise for surface variation
    float combinedNoise = (noise1 + noise2 * 0.5 + noise3 * 0.25) * uNoiseParams.y;
    
    // Create normal from noise
    vec3 normal = vec3(0.0, 0.0, 1.0);
    normal.x = sin(combinedNoise * 6.28318) * 0.2;
    normal.y = cos(combinedNoise * 6.28318) * 0.2;
    normal = normalize(normal);
    
    return normal;
}

float CalculateParticleThickness(vec2 uv, float time, float particleAge)
{
    // Calculate particle thickness variation
    vec2 thicknessUV = uv * uNoiseParams.x * 3.0 + time * 1.5 + particleAge * 0.3;
    float thicknessNoise = texture(uNoiseTexture, thicknessUV).r;
    
    // Apply thickness parameters
    float thickness = uParticleSize + thicknessNoise * 0.5;
    
    // Add age-based thinning
    thickness *= (1.0 - particleAge * uParticleFade * 0.5);
    
    // Add edge thinning
    vec2 centerDist = abs(uv - 0.5);
    float edgeFactor = 1.0 - max(centerDist.x, centerDist.y) * 2.0;
    thickness *= clamp(edgeFactor, 0.0, 1.0);
    
    return thickness;
}

vec3 CalculateParticleColor(vec2 uv, float thickness, float time, float particleAge)
{
    // Sample blood texture
    vec4 bloodTex = texture(uBloodTexture, uv);
    
    // Generate dynamic color variation
    vec2 colorNoiseUV = uv * uNoiseParams.x * 4.0 + time * 3.0 + particleAge * 0.2;
    float colorNoise = texture(uNoiseTexture, colorNoiseUV).r;
    
    // Blend between blood colors based on thickness and noise
    vec3 baseColor = mix(uBloodColor, uBloodColorDark, thickness * 0.5);
    baseColor = mix(baseColor, uBloodColorLight, colorNoise * 0.3);
    
    // Apply texture influence
    baseColor *= bloodTex.rgb;
    
    // Add age-based color change (darkening over time)
    float ageFactor = clamp(particleAge * 0.5, 0.0, 1.0);
    baseColor *= (1.0 - ageFactor * 0.7);
    
    return baseColor * uIntensity;
}

float CalculateSpecular(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
    // Calculate Blinn-Phong specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float NdotH = clamp(dot(normal, halfwayDir), 0.0, 1.0);
    
    float specular = pow(NdotH, uShininess);
    specular *= uSpecularIntensity;
    specular *= (1.0 - roughness); // Rougher surfaces have less specular
    
    return specular;
}

float CalculateRimLighting(vec3 normal, vec3 viewDir, float thickness)
{
    // Calculate rim lighting for particle edges
    float rimFactor = 1.0 - clamp(dot(normal, viewDir), 0.0, 1.0);
    rimFactor = pow(rimFactor, 2.0);
    rimFactor *= uRimLightIntensity;
    rimFactor *= (1.0 - thickness * 0.5); // Thinner areas have more rim lighting
    
    return rimFactor;
}

float GenerateParticleShape(vec2 uv, float time, float particleAge)
{
    // Generate particle shape with variation
    float shape = 0.0;
    
    // Base circular shape
    vec2 centerDist = uv - 0.5;
    float distance = length(centerDist);
    float radius = 0.5;
    
    // Add shape variation
    float shapeNoise = texture(uNoiseTexture, uv * 3.0 + time + particleAge).r;
    radius += shapeNoise * 0.1;
    
    // Create soft edges
    shape = 1.0 - smoothstep(radius - 0.1, radius + 0.1, distance);
    
    // Add irregularity
    float irregularity = sin(distance * 20.0 + time * 5.0 + particleAge * 2.0) * 0.1;
    shape += irregularity * (1.0 - shape);
    
    return clamp(shape, 0.0, 1.0);
}

float GenerateParticleSplatter(vec2 uv, float time, float particleAge)
{
    // Generate splatter effect for particles
    float splatter = 0.0;
    
    // Create splatter pattern
    for (int i = 0; i < 5; i++)
    {
        // Random splatter position
        vec2 splatterUV = uv + vec2(sin(time * 3.0 + i * 123.456), cos(time * 2.0 + i * 789.012)) * 0.1;
        
        // Random splatter size
        float splatterSize = 0.05 + 0.02 * sin(time * 4.0 + i * 345.678);
        
        // Calculate splatter shape
        vec2 splatterDist = abs(splatterUV - 0.5);
        float splatterShape = max(splatterDist.x, splatterDist.y);
        splatterShape = 1.0 - clamp(splatterShape / splatterSize, 0.0, 1.0);
        
        // Add age-based fading
        splatterShape *= (1.0 - particleAge * 0.5);
        
        splatter += splatterShape;
    }
    
    return clamp(splatter, 0.0, 1.0);
}

float CalculateParticlePhysics(vec2 uv, float time, float particleAge, vec3 worldPos)
{
    // Physics-based particle simulation
    float physics = 0.0;
    
    // Gravity effect
    float gravity = uPhysicsParams.x;
    float gravityEffect = gravity * particleAge * 0.1;
    
    // Drag effect
    float drag = uPhysicsParams.y;
    float dragEffect = 1.0 - drag * particleAge * 0.1;
    
    // Bounce effect
    float bounce = uPhysicsParams.z;
    float bounceEffect = sin(particleAge * 10.0) * bounce * 0.1;
    
    // Turbulence effect
    float turbulence = uPhysicsParams.w;
    vec2 turbulenceUV = uv * 5.0 + time * 2.0 + particleAge;
    float turbulenceNoise = texture(uNoiseTexture, turbulenceUV).r;
    float turbulenceEffect = turbulenceNoise * turbulence * 0.2;
    
    // Combine physics effects
    physics = gravityEffect + bounceEffect + turbulenceEffect;
    physics *= dragEffect;
    
    return clamp(physics, 0.0, 1.0);
}

float CalculateParticleOcclusion(vec3 worldPos, float particleAge)
{
    // Calculate particle occlusion based on age and position
    float occlusion = 1.0;
    
    // Age-based occlusion
    occlusion *= (1.0 - particleAge * 0.3);
    
    // Position-based occlusion (particles further away are more occluded)
    float distance = length(worldPos - uEmitterPosition);
    float distanceOcclusion = clamp(distance / 10.0, 0.0, 1.0);
    occlusion *= (1.0 - distanceOcclusion * 0.5);
    
    return occlusion;
}

vec3 CalculateParticleLighting(vec3 baseColor, vec3 normal, vec3 worldPos, float thickness, float particleAge)
{
    // Calculate lighting for particles
    vec3 litColor = baseColor;
    
    // View and light directions
    vec3 viewDir = normalize(uViewDirection);
    vec3 lightDir = normalize(-uLightDirection);
    
    // Diffuse lighting
    float NdotL = clamp(dot(normal, lightDir), 0.0, 1.0);
    vec3 diffuse = baseColor * NdotL * uLightColor;
    
    // Ambient lighting
    vec3 ambient = baseColor * uAmbientColor * uAmbientIntensity;
    
    // Specular lighting
    float specular = CalculateSpecular(normal, viewDir, lightDir, 0.5);
    
    // Rim lighting
    float rim = CalculateRimLighting(normal, viewDir, thickness);
    
    // Combine lighting
    litColor = ambient + diffuse + specular + rim;
    
    // Apply occlusion
    float occlusion = CalculateParticleOcclusion(worldPos, particleAge);
    litColor *= occlusion;
    
    return litColor;
}

float CalculateParticleAlpha(float thickness, float shape, float splatter, float particleAge)
{
    // Calculate particle alpha
    float alpha = thickness * shape;
    
    // Add splatter contribution
    alpha += splatter * 0.5;
    
    // Apply age-based fading
    alpha *= (1.0 - particleAge * uParticleFade);
    
    // Ensure minimum visibility
    alpha = max(alpha, 0.1);
    
    return clamp(alpha, 0.0, 1.0);
}

// Main fragment shader
void main()
{
    // Generate dynamic normal
    vec3 particleNormal = GenerateParticleNormal(fs_in.TexCoord, uTime, fs_in.ParticleAge);
    
    // Calculate thickness
    float thickness = CalculateParticleThickness(fs_in.TexCoord, uTime, fs_in.ParticleAge);
    
    // Calculate particle color
    vec3 particleColor = CalculateParticleColor(fs_in.TexCoord, thickness, uTime, fs_in.ParticleAge);
    
    // Generate particle shape
    float shape = GenerateParticleShape(fs_in.TexCoord, uTime, fs_in.ParticleAge);
    
    // Generate splatter effect
    float splatter = GenerateParticleSplatter(fs_in.TexCoord, uTime, fs_in.ParticleAge);
    
    // Calculate physics effects
    float physics = CalculateParticlePhysics(fs_in.TexCoord, uTime, fs_in.ParticleAge, fs_in.WorldPosition);
    
    // Calculate lighting
    vec3 litColor = CalculateParticleLighting(particleColor, particleNormal, fs_in.WorldPosition, thickness, fs_in.ParticleAge);
    
    // Combine effects
    vec3 finalColor = litColor;
    
    // Add physics-based color variation
    finalColor *= (1.0 + physics * 0.2);
    
    // Add splatter color
    vec3 splatterColor = mix(particleColor, particleColor * 0.7, splatter);
    finalColor = mix(finalColor, splatterColor, splatter * 0.3);
    
    // Apply vertex color
    finalColor *= fs_in.Color.rgb;
    
    // Calculate alpha
    float alpha = CalculateParticleAlpha(thickness, shape, splatter, fs_in.ParticleAge);
    
    // Apply fog
    finalColor = mix(finalColor, vec3(0.0, 0.0, 0.0), fs_in.FogFactor);
    alpha *= (1.0 - fs_in.FogFactor);
    
    FragColor = vec4(finalColor, alpha);
}