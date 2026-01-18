#version 330 core

// Blood Drip Fragment Shader for OpenGL
// Creates realistic blood dripping effects with physics-based simulation

// Uniforms
uniform float uTime;                    // Animation time
uniform vec2 uResolution;              // Screen resolution
uniform vec3 uBloodColor;              // Base blood color
uniform vec3 uBloodColorDark;          // Darker blood color for depth
uniform vec3 uBloodColorLight;         // Lighter blood color for highlights
uniform float uIntensity;             // Blood intensity/brightness
uniform float uFlowSpeed;             // Blood flow animation speed
uniform float uThickness;             // Blood thickness
uniform float uRoughness;             // Surface roughness for specular
uniform vec2 uUVScale;                // UV scaling factor
uniform vec2 uUVOffset;               // UV offset for animation
uniform vec4 uDripParams;              // Drip parameters (speed, length, frequency, viscosity)
uniform vec4 uCoagulationParams;       // Coagulation (darkening, edge effects, drying time)
uniform vec4 uNoiseParams;             // Noise parameters (scale, octaves, persistence, lacunarity)
uniform vec3 uLightDirection;          // Light direction
uniform vec3 uLightColor;              // Light color
uniform vec3 uAmbientColor;            // Ambient light color
uniform float uAmbientIntensity;       // Ambient light intensity
uniform float uSpecularIntensity;       // Specular highlight intensity
uniform float uShininess;               // Specular shininess factor
uniform float uRimLightIntensity;       // Rim lighting intensity
uniform vec3 uViewDirection;           // Camera/view direction

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
    vec2 BloodFlowUV;                   // Blood flow UV coordinates
    vec3 ViewDirection;                  // View direction
    vec4 ScreenPosition;                 // Screen space position
} fs_in;

// Output
out vec4 FragColor;

// Utility functions
vec3 GenerateBloodNormal(vec2 uv, float time)
{
    // Generate dynamic normal map for blood surface
    vec2 noiseUV = uv * uNoiseParams.x + uUVOffset * uFlowSpeed * time;
    float noise1 = texture(uNoiseTexture, noiseUV).r;
    float noise2 = texture(uNoiseTexture, noiseUV * 2.0).r;
    float noise3 = texture(uNoiseTexture, noiseUV * 4.0).r;
    
    // Combine noise for surface variation
    float combinedNoise = (noise1 + noise2 * 0.5 + noise3 * 0.25) * uNoiseParams.y;
    
    // Create normal from noise
    vec3 normal = vec3(0.0, 0.0, 1.0);
    normal.x = sin(combinedNoise * 6.28318) * 0.1;
    normal.y = cos(combinedNoise * 6.28318) * 0.1;
    normal = normalize(normal);
    
    return normal;
}

float CalculateBloodThickness(vec2 uv, float time)
{
    // Calculate blood thickness variation
    vec2 thicknessUV = uv * uUVScale * 2.0 + uUVOffset * uFlowSpeed * time * 0.5;
    float thicknessNoise = texture(uNoiseTexture, thicknessUV).r;
    
    // Apply thickness parameters
    float thickness = uThickness + thicknessNoise * 0.3;
    
    // Add edge thinning
    vec2 centerDist = abs(uv - 0.5);
    float edgeFactor = 1.0 - max(centerDist.x, centerDist.y) * 2.0;
    thickness *= clamp(edgeFactor, 0.0, 1.0);
    
    return thickness;
}

vec3 CalculateBloodColor(vec2 uv, float thickness, float time)
{
    // Sample blood texture
    vec4 bloodTex = texture(uBloodTexture, uv);
    
    // Generate dynamic color variation
    vec2 colorNoiseUV = uv * uNoiseParams.x * 3.0 + uUVOffset * uFlowSpeed * time * 0.3;
    float colorNoise = texture(uNoiseTexture, colorNoiseUV).r;
    
    // Blend between blood colors based on thickness and noise
    vec3 baseColor = mix(uBloodColor, uBloodColorDark, thickness * 0.5);
    baseColor = mix(baseColor, uBloodColorLight, colorNoise * 0.3);
    
    // Apply texture influence
    baseColor *= bloodTex.rgb;
    
    // Add coagulation effect (darkening over time)
    float coagulationFactor = clamp(time * uCoagulationParams.z * 0.1, 0.0, 1.0);
    baseColor *= (1.0 - coagulationFactor * uCoagulationParams.x);
    
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
    // Calculate rim lighting for blood edges
    float rimFactor = 1.0 - clamp(dot(normal, viewDir), 0.0, 1.0);
    rimFactor = pow(rimFactor, 2.0);
    rimFactor *= uRimLightIntensity;
    rimFactor *= (1.0 - thickness * 0.5); // Thinner areas have more rim lighting
    
    return rimFactor;
}

float GenerateDrippingEffect(vec2 uv, float time)
{
    // Generate blood dripping effect
    float drip = 0.0;
    
    for (int i = 0; i < 3; i++)
    {
        // Drip position and movement
        float dripY = uv.y + time * uDripParams.x * (1.0 + i * 0.3);
        dripY = fract(dripY);
        
        // Drip shape
        float dripShape = 1.0 - abs(dripY - 0.5) * 2.0;
        dripShape = clamp(dripShape, 0.0, 1.0);
        
        // Drip width variation
        float dripWidth = 0.1 + 0.05 * sin(time * 2.0 + i * 123.456);
        float dripX = abs(uv.x - 0.5);
        dripX = 1.0 - clamp(dripX / dripWidth, 0.0, 1.0);
        
        drip += dripShape * dripX * uDripParams.y * (1.0 - i * 0.3);
    }
    
    return clamp(drip, 0.0, 1.0);
}

float GenerateDripTrail(vec2 uv, float time)
{
    // Generate trailing effect for drips
    float trail = 0.0;
    
    // Sample multiple points along the drip path
    for (int i = 0; i < 5; i++)
    {
        float trailOffset = float(i) * 0.05;
        vec2 trailUV = uv;
        trailUV.y += trailOffset;
        
        // Fade out trail
        float fade = 1.0 - trailOffset * 2.0;
        fade = clamp(fade, 0.0, 1.0);
        
        // Sample noise for trail variation
        float noise = texture(uNoiseTexture, trailUV * 3.0 + time).r;
        trail += noise * fade * 0.3;
    }
    
    return clamp(trail, 0.0, 1.0);
}

float CalculateDripPhysics(vec2 uv, float time, vec2 worldPos)
{
    // Physics-based drip simulation
    float physics = 0.0;
    
    // Gravity effect
    float gravity = 9.81;
    float dripHeight = uDripParams.y;
    float dripTime = sqrt(2.0 * dripHeight / gravity);
    
    // Viscosity effect
    float viscosity = uDripParams.w;
    float viscosityFactor = 1.0 / (1.0 + viscosity * 10.0);
    
    // Calculate drip position based on physics
    float dripPos = uv.y + time * uDripParams.x * viscosityFactor;
    dripPos = fract(dripPos);
    
    // Create drip shape with physics-based width
    float dripWidth = 0.1 + 0.05 * sin(worldPos.x * 10.0 + time * 5.0);
    float dripX = abs(uv.x - 0.5);
    float dripShape = 1.0 - clamp(dripX / dripWidth, 0.0, 1.0);
    
    // Apply physics to shape
    physics = dripShape * (1.0 - dripPos);
    physics = clamp(physics, 0.0, 1.0);
    
    return physics;
}

float CalculateDripCoagulation(float drip, float time, vec2 uv)
{
    // Calculate coagulation effect for drips
    float coagulation = drip;
    
    // Drips coagulate faster at the bottom
    float coagulationRate = uCoagulationParams.x * 2.0;
    float coagulationFactor = 1.0 - exp(-time * coagulationRate);
    
    // Edge coagulation
    vec2 edgeDist = abs(uv - 0.5);
    float edgeFactor = max(edgeDist.x, edgeDist.y);
    edgeFactor = clamp(edgeFactor * 2.0, 0.0, 1.0);
    
    coagulation *= (1.0 - edgeFactor * 0.5);
    coagulation *= coagulationFactor;
    
    return coagulation;
}

vec3 CalculateDripColor(vec3 baseColor, float drip, float coagulation, float time)
{
    // Calculate drip color with coagulation
    vec3 dripColor = baseColor;
    
    // Darken drips over time due to coagulation
    dripColor *= (1.0 - coagulation * 0.7);
    
    // Add slight color variation for drips
    float colorVariation = sin(time * 3.0 + time * 0.5) * 0.1;
    dripColor *= (1.0 + colorVariation);
    
    return dripColor;
}

// Main fragment shader
void main()
{
    // Generate dynamic normal
    vec3 bloodNormal = GenerateBloodNormal(fs_in.TexCoord, uTime);
    
    // Calculate thickness
    float thickness = CalculateBloodThickness(fs_in.TexCoord, uTime);
    
    // Calculate blood color
    vec3 bloodColor = CalculateBloodColor(fs_in.TexCoord, thickness, uTime);
    
    // Generate dripping effect
    float drip = GenerateDrippingEffect(fs_in.TexCoord * uUVScale, uTime);
    
    // Generate drip trail
    float trail = GenerateDripTrail(fs_in.TexCoord * uUVScale, uTime);
    
    // Generate physics-based drip
    float physicsDrip = CalculateDripPhysics(fs_in.TexCoord, uTime, fs_in.WorldPosition.xy);
    
    // Calculate coagulation
    float coagulation = CalculateDripCoagulation(drip, uTime, fs_in.TexCoord);
    
    // Combine drip effects
    float finalDrip = max(drip, trail) * 0.7 + physicsDrip * 0.3;
    finalDrip = clamp(finalDrip, 0.0, 1.0);
    
    // Calculate drip color with coagulation
    vec3 dripColor = CalculateDripColor(bloodColor, finalDrip, coagulation, uTime);
    
    // Combine with base color
    vec3 finalColor = mix(bloodColor, dripColor, finalDrip);
    
    // Calculate lighting
    vec3 normal = normalize(fs_in.Normal + bloodNormal * 0.5);
    vec3 viewDir = normalize(fs_in.ViewDirection);
    vec3 lightDir = normalize(-uLightDirection);
    
    // Diffuse lighting
    float NdotL = clamp(dot(normal, lightDir), 0.0, 1.0);
    vec3 diffuse = finalColor * NdotL;
    
    // Ambient lighting
    vec3 ambient = finalColor * uAmbientColor * uAmbientIntensity;
    
    // Specular lighting
    float specular = CalculateSpecular(normal, viewDir, lightDir, uRoughness);
    
    // Rim lighting
    float rim = CalculateRimLighting(normal, viewDir, thickness);
    
    // Combine lighting
    finalColor = ambient + diffuse + specular + rim;
    
    // Apply alpha based on thickness and drip effects
    float alpha = thickness + finalDrip * 0.8;
    alpha = clamp(alpha, 0.0, 1.0);
    
    // Add some transparency for thin areas
    alpha = mix(alpha * 0.3, alpha, thickness);
    
    // Apply vertex color
    finalColor *= fs_in.Color.rgb;
    
    FragColor = vec4(finalColor, alpha);
}