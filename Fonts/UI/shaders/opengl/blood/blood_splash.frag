#version 330 core

// Blood Splash Fragment Shader for OpenGL
// Creates realistic blood splash effects with dynamic particle simulation

// Uniforms
uniform float uTime;                    // Animation time
uniform vec2 uResolution;              // Screen resolution
uniform vec3 uBloodColor;              // Base blood color
uniform vec3 uBloodColorDark;          // Darker blood color for depth
uniform vec3 uBloodColorLight;         // Lighter blood color for highlights
uniform float uIntensity;             // Blood intensity/brightness
uniform float uSplashRadius;           // Splash radius
uniform float uSplashHeight;           // Splash height
uniform float uSplashDuration;         // Splash duration
uniform vec2 uUVScale;                // UV scaling factor
uniform vec2 uUVOffset;               // UV offset for animation
uniform vec4 uSplashParams;            // Splash parameters (intensity, spread, direction, randomness)
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
uniform vec3 uSplashCenter;           // Splash center position
uniform float uSplashAge;              // Splash age

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
    vec2 SplashUV;                      // Splash UV coordinates
    vec3 ViewDirection;                  // View direction
    vec4 ScreenPosition;                 // Screen space position
    float DistanceFromCenter;           // Distance from splash center
    float SplashAge;                    // Splash age
    vec3 SplashVelocity;                // Splash velocity
    vec4 InstanceData;                  // Instance data
    float FogFactor;                     // Fog factor
} fs_in;

// Output
out vec4 FragColor;

// Utility functions
vec3 GenerateSplashNormal(vec2 uv, float time, float splashAge)
{
    // Generate dynamic normal map for splash surface
    vec2 noiseUV = uv * uNoiseParams.x + time * 3.0 + splashAge * 0.5;
    float noise1 = texture(uNoiseTexture, noiseUV).r;
    float noise2 = texture(uNoiseTexture, noiseUV * 2.0).r;
    float noise3 = texture(uNoiseTexture, noiseUV * 4.0).r;
    
    // Combine noise for surface variation
    float combinedNoise = (noise1 + noise2 * 0.5 + noise3 * 0.25) * uNoiseParams.y;
    
    // Create normal from noise with splash-specific characteristics
    vec3 normal = vec3(0.0, 0.0, 1.0);
    normal.x = sin(combinedNoise * 6.28318) * 0.15; // More variation for splash
    normal.y = cos(combinedNoise * 6.28318) * 0.15;
    normal = normalize(normal);
    
    return normal;
}

float CalculateSplashThickness(vec2 uv, float time, float splashAge, float distanceFromCenter)
{
    // Calculate blood splash thickness variation
    vec2 thicknessUV = uv * uNoiseParams.x * 3.0 + time * 2.0 + splashAge * 0.1;
    float thicknessNoise = texture(uNoiseTexture, thicknessUV).r;
    
    // Base thickness from splash parameters
    float thickness = uSplashHeight + thicknessNoise * 0.5;
    
    // Add distance-based thickness (thicker at center, thinner at edges)
    float distanceFactor = 1.0 - clamp(distanceFromCenter / uSplashRadius, 0.0, 1.0);
    thickness *= (1.0 + distanceFactor * 2.0); // Center is 3x thicker
    
    // Add age-based thinning (splash gets thinner over time)
    float ageFactor = clamp(splashAge / uSplashDuration, 0.0, 1.0);
    thickness *= (1.0 - ageFactor * 0.7);
    
    // Add edge thinning
    vec2 centerDist = abs(uv - 0.5);
    float edgeFactor = 1.0 - max(centerDist.x, centerDist.y) * 2.0;
    thickness *= clamp(edgeFactor, 0.0, 1.0);
    
    return thickness;
}

vec3 CalculateSplashColor(vec2 uv, float thickness, float time, float splashAge, float distanceFromCenter)
{
    // Sample blood texture
    vec4 bloodTex = texture(uBloodTexture, uv);
    
    // Generate dynamic color variation
    vec2 colorNoiseUV = uv * uNoiseParams.x * 4.0 + time * 1.5 + splashAge * 0.05;
    float colorNoise = texture(uNoiseTexture, colorNoiseUV).r;
    
    // Blend between blood colors based on thickness and distance
    vec3 baseColor = mix(uBloodColor, uBloodColorDark, thickness * 0.4);
    baseColor = mix(baseColor, uBloodColorLight, colorNoise * 0.3);
    
    // Apply texture influence
    baseColor *= bloodTex.rgb;
    
    // Add distance-based color variation (darker at edges)
    float distanceFactor = clamp(distanceFromCenter / uSplashRadius, 0.0, 1.0);
    baseColor *= (1.0 - distanceFactor * 0.2);
    
    // Add age-based color change (darkening and fading over time)
    float ageFactor = clamp(splashAge / uSplashDuration, 0.0, 1.0);
    baseColor *= (1.0 - ageFactor * 0.5);
    
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
    // Calculate rim lighting for splash edges
    float rimFactor = 1.0 - clamp(dot(normal, viewDir), 0.0, 1.0);
    rimFactor = pow(rimFactor, 2.0);
    rimFactor *= uRimLightIntensity;
    rimFactor *= (1.0 - thickness * 0.4); // Thinner areas have more rim lighting
    
    return rimFactor;
}

float GenerateSplashShape(vec2 uv, float time, float splashAge, float distanceFromCenter)
{
    // Generate blood splash shape with dynamic spreading
    float shape = 0.0;
    
    // Base circular splash shape
    float splashRadius = uSplashRadius * (1.0 + splashAge * uSplashParams.y * 0.2);
    float distance = distanceFromCenter;
    
    // Create soft edges
    shape = 1.0 - smoothstep(splashRadius - 0.1, splashRadius + 0.1, distance);
    
    // Add spreading animation
    float spreadFactor = clamp(splashAge / uSplashDuration, 0.0, 1.0);
    shape *= (1.0 - spreadFactor * 0.8); // Splash fades as it spreads
    
    // Add irregularity to splash edges
    float edgeNoise = texture(uNoiseTexture, uv * 8.0 + time * 1.0 + splashAge * 0.2).r;
    float irregularity = edgeNoise * 0.15;
    shape += irregularity * (1.0 - shape) * 0.5;
    
    return clamp(shape, 0.0, 1.0);
}

float GenerateSplashDroplets(vec2 uv, float time, float splashAge)
{
    // Generate blood splash droplets
    float droplets = 0.0;
    
    // Create multiple droplets
    for (int i = 0; i < 8; i++)
    {
        // Random droplet position
        vec2 dropletUV = uv + vec2(sin(time * 4.0 + i * 123.456), cos(time * 3.0 + i * 789.012)) * 0.2;
        
        // Random droplet size
        float dropletSize = 0.02 + 0.01 * sin(time * 5.0 + i * 345.678);
        
        // Calculate droplet shape
        vec2 dropletDist = abs(dropletUV - 0.5);
        float dropletShape = max(dropletDist.x, dropletDist.y);
        dropletShape = 1.0 - clamp(dropletShape / dropletSize, 0.0, 1.0);
        
        // Add age-based fading
        dropletShape *= (1.0 - splashAge * 0.7);
        
        // Add splash direction bias
        vec2 splashDir = normalize(uSplashParams.xy);
        float dirBias = dot(dropletUV - 0.5, splashDir);
        dropletShape *= (1.0 + dirBias * 0.5);
        
        droplets += dropletShape;
    }
    
    return clamp(droplets, 0.0, 1.0);
}

float GenerateSplashPhysics(vec2 uv, float time, float splashAge, vec3 worldPos)
{
    // Physics-based splash simulation
    float physics = 0.0;
    
    // Gravity effect
    float gravity = uPhysicsParams.x;
    float gravityEffect = gravity * splashAge * 0.2;
    
    // Drag effect
    float drag = uPhysicsParams.y;
    float dragEffect = 1.0 - drag * splashAge * 0.3;
    
    // Bounce effect
    float bounce = uPhysicsParams.z;
    float bounceEffect = sin(splashAge * 15.0) * bounce * 0.1;
    
    // Turbulence effect
    float turbulence = uPhysicsParams.w;
    vec2 turbulenceUV = uv * 6.0 + time * 3.0 + splashAge;
    float turbulenceNoise = texture(uNoiseTexture, turbulenceUV).r;
    float turbulenceEffect = turbulenceNoise * turbulence * 0.3;
    
    // Combine physics effects
    physics = gravityEffect + bounceEffect + turbulenceEffect;
    physics *= dragEffect;
    
    return clamp(physics, 0.0, 1.0);
}

float GenerateSplashSpray(vec2 uv, float time, float splashAge, float distanceFromCenter)
{
    // Generate blood splash spray effect
    float spray = 0.0;
    
    // Create spray particles
    for (int i = 0; i < 12; i++)
    {
        // Random spray position
        vec2 sprayUV = uv + vec2(sin(time * 6.0 + i * 234.567), cos(time * 4.0 + i * 890.123)) * 0.3;
        
        // Random spray size
        float spraySize = 0.01 + 0.005 * sin(time * 7.0 + i * 456.789);
        
        // Calculate spray shape
        vec2 sprayDist = abs(sprayUV - 0.5);
        float sprayShape = max(sprayDist.x, sprayDist.y);
        sprayShape = 1.0 - clamp(sprayShape / spraySize, 0.0, 1.0);
        
        // Add age-based fading
        sprayShape *= (1.0 - splashAge * 0.8);
        
        // Add distance-based fading (spray is more visible at edges)
        float distanceFactor = clamp(distanceFromCenter / uSplashRadius, 0.0, 1.0);
        sprayShape *= distanceFactor;
        
        spray += sprayShape;
    }
    
    return clamp(spray, 0.0, 1.0);
}

float GenerateSplashRipples(vec2 uv, float time, float splashAge)
{
    // Generate blood splash ripples
    float ripples = 0.0;
    
    // Create ripple rings
    for (int i = 0; i < 3; i++)
    {
        float rippleRadius = 0.1 + float(i) * 0.15;
        float rippleSpeed = 2.0 + i * 0.5;
        float rippleAge = splashAge * rippleSpeed;
        
        // Calculate ripple ring
        vec2 centerDist = uv - 0.5;
        float distance = length(centerDist);
        float ripple = 1.0 - smoothstep(rippleRadius - 0.02, rippleRadius + 0.02, abs(distance - rippleAge));
        
        // Add ripple fading
        ripple *= (1.0 - rippleAge * 0.5);
        
        ripples += ripple;
    }
    
    return clamp(ripples, 0.0, 1.0);
}

vec3 CalculateSplashLighting(vec3 baseColor, vec3 normal, vec3 worldPos, float thickness, float splashAge)
{
    // Calculate lighting for blood splash
    vec3 litColor = baseColor;
    
    // View and light directions
    vec3 viewDir = normalize(uViewDirection);
    vec3 lightDir = normalize(-uLightDirection);
    
    // Diffuse lighting
    float NdotL = clamp(dot(normal, lightDir), 0.0, 1.0);
    vec3 diffuse = baseColor * NdotL * uLightColor;
    
    // Ambient lighting
    vec3 ambient = baseColor * uAmbientColor * uAmbientIntensity;
    
    // Specular lighting (reduced for liquid splash)
    float roughness = 0.7; // Blood splashes are rough
    float specular = CalculateSpecular(normal, viewDir, lightDir, roughness) * 0.4;
    
    // Rim lighting
    float rim = CalculateRimLighting(normal, viewDir, thickness) * 0.6;
    
    // Combine lighting
    litColor = ambient + diffuse + specular + rim;
    
    // Add age-based lighting changes (darker as it ages)
    float ageFactor = clamp(splashAge / uSplashDuration, 0.0, 1.0);
    litColor *= (1.0 - ageFactor * 0.4);
    
    return litColor;
}

float CalculateSplashAlpha(float thickness, float shape, float droplets, float spray, float splashAge)
{
    // Calculate blood splash alpha
    float alpha = thickness * shape;
    
    // Add droplet contribution
    alpha += droplets * 0.3;
    
    // Add spray contribution
    alpha += spray * 0.2;
    
    // Apply age-based fading
    alpha *= (1.0 - splashAge * 0.6);
    
    // Ensure minimum visibility
    alpha = max(alpha, 0.2);
    
    return clamp(alpha, 0.0, 1.0);
}

// Main fragment shader
void main()
{
    // Generate dynamic normal
    vec3 splashNormal = GenerateSplashNormal(fs_in.TexCoord, uTime, fs_in.SplashAge);
    
    // Calculate thickness
    float thickness = CalculateSplashThickness(fs_in.TexCoord, uTime, fs_in.SplashAge, fs_in.DistanceFromCenter);
    
    // Calculate splash color
    vec3 splashColor = CalculateSplashColor(fs_in.TexCoord, thickness, uTime, fs_in.SplashAge, fs_in.DistanceFromCenter);
    
    // Generate splash shape
    float shape = GenerateSplashShape(fs_in.TexCoord, uTime, fs_in.SplashAge, fs_in.DistanceFromCenter);
    
    // Generate droplets
    float droplets = GenerateSplashDroplets(fs_in.TexCoord, uTime, fs_in.SplashAge);
    
    // Generate spray effect
    float spray = GenerateSplashSpray(fs_in.TexCoord, uTime, fs_in.SplashAge, fs_in.DistanceFromCenter);
    
    // Generate ripples
    float ripples = GenerateSplashRipples(fs_in.TexCoord, uTime, fs_in.SplashAge);
    
    // Calculate physics effects
    float physics = GenerateSplashPhysics(fs_in.TexCoord, uTime, fs_in.SplashAge, fs_in.WorldPosition);
    
    // Calculate lighting
    vec3 litColor = CalculateSplashLighting(splashColor, splashNormal, fs_in.WorldPosition, thickness, fs_in.SplashAge);
    
    // Combine effects
    vec3 finalColor = litColor;
    
    // Add physics-based color variation
    finalColor *= (1.0 + physics * 0.3);
    
    // Add droplet color
    vec3 dropletColor = mix(splashColor, splashColor * 0.8, droplets);
    finalColor = mix(finalColor, dropletColor, droplets * 0.4);
    
    // Add spray color
    vec3 sprayColor = mix(splashColor, splashColor * 0.6, spray);
    finalColor = mix(finalColor, sprayColor, spray * 0.3);
    
    // Add ripples effect
    finalColor *= (1.0 + ripples * 0.1);
    
    // Apply vertex color
    finalColor *= fs_in.Color.rgb;
    
    // Calculate alpha
    float alpha = CalculateSplashAlpha(thickness, shape, droplets, spray, fs_in.SplashAge);
    
    // Apply fog
    finalColor = mix(finalColor, vec3(0.0, 0.0, 0.0), fs_in.FogFactor);
    alpha *= (1.0 - fs_in.FogFactor);
    
    FragColor = vec4(finalColor, alpha);
}