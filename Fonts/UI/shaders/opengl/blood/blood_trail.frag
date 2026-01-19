#version 330 core

// Blood Trail Fragment Shader for OpenGL
// Creates realistic blood trail effects with dynamic movement and coagulation

// Uniforms
uniform float uTime;                    // Animation time
uniform vec2 uResolution;              // Screen resolution
uniform vec3 uBloodColor;              // Base blood color
uniform vec3 uBloodColorDark;          // Darker blood color for depth
uniform vec3 uBloodColorLight;         // Lighter blood color for highlights
uniform float uIntensity;             // Blood intensity/brightness
uniform float uTrailLength;           // Trail length
uniform float uTrailWidth;            // Trail width
uniform float uTrailFadeRate;          // Trail fade rate
uniform float uCoagulationRate;        // Trail coagulation rate
uniform vec2 uUVScale;                // UV scaling factor
uniform vec2 uUVOffset;               // UV offset for animation
uniform vec4 uTrailParams;            // Trail parameters (length, width, direction, persistence)
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
uniform vec3 uTrailStart;             // Trail start position
uniform vec3 uTrailEnd;               // Trail end position
uniform float uTrailAge;              // Trail age

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
    vec2 TrailUV;                       // Trail UV coordinates
    vec3 ViewDirection;                  // View direction
    vec4 ScreenPosition;                 // Screen space position
    float DistanceFromStart;             // Distance from trail start
    float DistanceFromEnd;               // Distance from trail end
    float TrailAge;                      // Trail age
    vec3 TrailVelocity;                  // Trail velocity
    vec4 InstanceData;                  // Instance data
    float FogFactor;                     // Fog factor
} fs_in;

// Output
out vec4 FragColor;

// Utility functions
vec3 GenerateTrailNormal(vec2 uv, float time, float trailAge)
{
    // Generate dynamic normal map for trail surface
    vec2 noiseUV = uv * uNoiseParams.x + time * 2.0 + trailAge * 0.2;
    float noise1 = texture(uNoiseTexture, noiseUV).r;
    float noise2 = texture(uNoiseTexture, noiseUV * 2.0).r;
    float noise3 = texture(uNoiseTexture, noiseUV * 4.0).r;
    
    // Combine noise for surface variation
    float combinedNoise = (noise1 + noise2 * 0.5 + noise3 * 0.25) * uNoiseParams.y;
    
    // Create normal from noise with trail-specific characteristics
    vec3 normal = vec3(0.0, 0.0, 1.0);
    normal.x = sin(combinedNoise * 6.28318) * 0.12; // Moderate variation for trail
    normal.y = cos(combinedNoise * 6.28318) * 0.12;
    normal = normalize(normal);
    
    return normal;
}

float CalculateTrailThickness(vec2 uv, float time, float trailAge, float distanceFromStart, float distanceFromEnd)
{
    // Calculate blood trail thickness variation
    vec2 thicknessUV = uv * uNoiseParams.x * 2.5 + time * 1.5 + trailAge * 0.05;
    float thicknessNoise = texture(uNoiseTexture, thicknessUV).r;
    
    // Base thickness from trail parameters
    float thickness = uTrailWidth + thicknessNoise * 0.4;
    
    // Add distance-based thickness (thicker at start, thinner at end)
    float distanceFactor = distanceFromStart / uTrailLength;
    thickness *= (1.0 + (1.0 - distanceFactor) * 1.5); // Start is 2.5x thicker
    
    // Add age-based thinning (trail gets thinner over time)
    float ageFactor = clamp(trailAge / (uTrailDuration * 2.0), 0.0, 1.0);
    thickness *= (1.0 - ageFactor * 0.6);
    
    // Add edge thinning
    vec2 centerDist = abs(uv - 0.5);
    float edgeFactor = 1.0 - max(centerDist.x, centerDist.y) * 2.0;
    thickness *= clamp(edgeFactor, 0.0, 1.0);
    
    return thickness;
}

vec3 CalculateTrailColor(vec2 uv, float thickness, float time, float trailAge, float distanceFromStart, float distanceFromEnd)
{
    // Sample blood texture
    vec4 bloodTex = texture(uBloodTexture, uv);
    
    // Generate dynamic color variation
    vec2 colorNoiseUV = uv * uNoiseParams.x * 3.5 + time * 1.0 + trailAge * 0.03;
    float colorNoise = texture(uNoiseTexture, colorNoiseUV).r;
    
    // Blend between blood colors based on thickness and distance
    vec3 baseColor = mix(uBloodColor, uBloodColorDark, thickness * 0.3);
    baseColor = mix(baseColor, uBloodColorLight, colorNoise * 0.25);
    
    // Apply texture influence
    baseColor *= bloodTex.rgb;
    
    // Add distance-based color variation (darker towards end)
    float distanceFactor = clamp(distanceFromEnd / uTrailLength, 0.0, 1.0);
    baseColor *= (1.0 - distanceFactor * 0.4);
    
    // Add age-based color change (darkening and fading over time)
    float ageFactor = clamp(trailAge / (uTrailDuration * 2.0), 0.0, 1.0);
    baseColor *= (1.0 - ageFactor * 0.4);
    
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
    // Calculate rim lighting for trail edges
    float rimFactor = 1.0 - clamp(dot(normal, viewDir), 0.0, 1.0);
    rimFactor = pow(rimFactor, 2.0);
    rimFactor *= uRimLightIntensity;
    rimFactor *= (1.0 - thickness * 0.3); // Thinner areas have more rim lighting
    
    return rimFactor;
}

float GenerateTrailShape(vec2 uv, float time, float trailAge, float distanceFromStart, float distanceFromEnd)
{
    // Generate blood trail shape with dynamic fading
    float shape = 0.0;
    
    // Calculate trail position along the trail
    float trailPosition = distanceFromStart / uTrailLength;
    
    // Create trail shape with width variation
    float trailWidth = uTrailWidth * (1.0 - trailPosition * 0.8); // Trail gets thinner towards end
    float distance = abs(uv - 0.5);
    
    // Create soft edges
    shape = 1.0 - smoothstep(trailWidth - 0.05, trailWidth + 0.05, distance);
    
    // Add age-based fading
    float ageFactor = clamp(trailAge / (uTrailDuration * 2.0), 0.0, 1.0);
    shape *= (1.0 - ageFactor * uTrailFadeRate);
    
    // Add irregularity to trail edges
    float edgeNoise = texture(uNoiseTexture, uv * 6.0 + time * 0.8 + trailAge * 0.1).r;
    float irregularity = edgeNoise * 0.12;
    shape += irregularity * (1.0 - shape) * 0.5;
    
    // Add trail direction bias
    vec2 trailDir = normalize(uTrailParams.xy);
    float dirBias = dot(uv - 0.5, trailDir);
    shape *= (1.0 + dirBias * 0.2);
    
    return clamp(shape, 0.0, 1.0);
}

float GenerateTrailDroplets(vec2 uv, float time, float trailAge, float distanceFromStart)
{
    // Generate blood trail droplets
    float droplets = 0.0;
    
    // Create multiple droplets along the trail
    for (int i = 0; i < 6; i++)
    {
        // Random droplet position
        vec2 dropletUV = uv + vec2(sin(time * 3.0 + i * 234.567), cos(time * 2.0 + i * 789.012)) * 0.15;
        
        // Random droplet size
        float dropletSize = 0.015 + 0.008 * sin(time * 4.0 + i * 345.678);
        
        // Calculate droplet shape
        vec2 dropletDist = abs(dropletUV - 0.5);
        float dropletShape = max(dropletDist.x, dropletDist.y);
        dropletShape = 1.0 - clamp(dropletShape / dropletSize, 0.0, 1.0);
        
        // Add age-based fading
        dropletShape *= (1.0 - trailAge * 0.6);
        
        // Add position-based fading (droplets fade towards end of trail)
        dropletShape *= (1.0 - distanceFromStart / uTrailLength * 0.5);
        
        droplets += dropletShape;
    }
    
    return clamp(droplets, 0.0, 1.0);
}

float GenerateTrailSmearing(vec2 uv, float time, float trailAge, float distanceFromStart)
{
    // Generate blood trail smearing effect
    float smearing = 0.0;
    
    // Create smearing along trail direction
    vec2 smearDir = normalize(uTrailParams.xy);
    float smearAmount = 0.1 + 0.05 * sin(time * 2.0 + trailAge * 0.5);
    
    // Calculate smear position
    vec2 smearUV = uv + smearDir * smearAmount * (1.0 - distanceFromStart / uTrailLength);
    
    // Sample multiple points for smearing
    float smearSamples[5];
    vec2 smearOffsets[5] = {
        vec2(-0.02, 0.0), vec2(-0.01, 0.0), vec2(0.0, 0.0), vec2(0.01, 0.0), vec2(0.02, 0.0)
    };
    
    for (int i = 0; i < 5; i++)
    {
        vec2 sampleUV = smearUV + smearOffsets[i];
        float sample = texture(uBloodTexture, sampleUV).r;
        smearSamples[i] = sample;
    }
    
    // Average the samples
    float smearedValue = 0.0;
    for (int i = 0; i < 5; i++)
    {
        smearedValue += smearSamples[i];
    }
    smearedValue /= 5.0;
    
    smearing = smearedValue * (1.0 - trailAge * 0.5);
    
    return clamp(smearing, 0.0, 1.0);
}

float GenerateTrailCoagulation(vec2 uv, float time, float trailAge, float distanceFromStart, float distanceFromEnd)
{
    // Generate blood trail coagulation effect
    float coagulation = 0.0;
    
    // Base coagulation based on age
    float baseCoagulation = clamp(trailAge / (uTrailDuration * 2.0), 0.0, 1.0);
    
    // Edge coagulation (edges coagulate faster)
    float edgeFactor = clamp(distanceFromEnd / uTrailLength, 0.0, 1.0);
    float edgeCoagulation = edgeFactor * uCoagulationParams.y;
    
    // Surface coagulation patterns
    vec2 coagulationUV = uv * uNoiseParams.x * 3.5 + time * 0.8 + trailAge * 0.02;
    float coagulationNoise = texture(uNoiseTexture, coagulationUV).r;
    float surfaceCoagulation = coagulationNoise * uCoagulationParams.z;
    
    // Combine coagulation effects
    coagulation = baseCoagulation + edgeCoagulation + surfaceCoagulation;
    coagulation = clamp(coagulation, 0.0, 1.0);
    
    return coagulation;
}

float GenerateTrailDepth(vec2 uv, float time, float trailAge, float distanceFromStart, float distanceFromEnd)
{
    // Generate blood trail depth variation
    float depth = 0.0;
    
    // Base depth from trail parameters
    depth = uTrailWidth * 0.5;
    
    // Add distance-based depth (deeper at start, shallower at end)
    float distanceFactor = 1.0 - clamp(distanceFromStart / uTrailLength, 0.0, 1.0);
    depth *= (1.0 + distanceFactor * 2.0); // Start is 3x deeper
    
    // Add age-based depth changes (trail gets shallower as it coagulates)
    float timeDepth = trailAge * uCoagulationRate * 0.02;
    depth -= timeDepth * 0.5;
    
    // Add noise-based depth variation
    vec2 depthUV = uv * uNoiseParams.x * 2.0 + time * 0.3 + trailAge * 0.03;
    float depthNoise = texture(uNoiseTexture, depthUV).r;
    depth += depthNoise * 0.15;
    
    return max(depth, 0.1);
}

vec3 CalculateTrailLighting(vec3 baseColor, vec3 normal, vec3 worldPos, float thickness, float trailAge)
{
    // Calculate lighting for blood trail
    vec3 litColor = baseColor;
    
    // View and light directions
    vec3 viewDir = normalize(uViewDirection);
    vec3 lightDir = normalize(-uLightDirection);
    
    // Diffuse lighting
    float NdotL = clamp(dot(normal, lightDir), 0.0, 1.0);
    vec3 diffuse = baseColor * NdotL * uLightColor;
    
    // Ambient lighting
    vec3 ambient = baseColor * uAmbientColor * uAmbientIntensity;
    
    // Specular lighting (reduced for liquid trail)
    float roughness = 0.6; // Blood trails are rough
    float specular = CalculateSpecular(normal, viewDir, lightDir, roughness) * 0.3;
    
    // Rim lighting
    float rim = CalculateRimLighting(normal, viewDir, thickness) * 0.5;
    
    // Combine lighting
    litColor = ambient + diffuse + specular + rim;
    
    // Add age-based lighting changes (darker as it coagulates)
    float ageFactor = clamp(trailAge / (uTrailDuration * 2.0), 0.0, 1.0);
    litColor *= (1.0 - ageFactor * 0.3);
    
    return litColor;
}

float CalculateTrailAlpha(float thickness, float shape, float droplets, float smearing, float coagulation, float trailAge)
{
    // Calculate blood trail alpha
    float alpha = thickness * shape;
    
    // Add droplet contribution
    alpha += droplets * 0.2;
    
    // Add smearing contribution
    alpha += smearing * 0.3;
    
    // Apply coagulation opacity changes
    alpha *= (1.0 + coagulation * 0.1);
    
    // Apply age-based fading
    alpha *= (1.0 - trailAge * uTrailFadeRate);
    
    // Ensure minimum visibility
    alpha = max(alpha, 0.15);
    
    return clamp(alpha, 0.0, 1.0);
}

// Main fragment shader
void main()
{
    // Generate dynamic normal
    vec3 trailNormal = GenerateTrailNormal(fs_in.TexCoord, uTime, fs_in.TrailAge);
    
    // Calculate thickness
    float thickness = CalculateTrailThickness(fs_in.TexCoord, uTime, fs_in.TrailAge, fs_in.DistanceFromStart, fs_in.DistanceFromEnd);
    
    // Calculate trail color
    vec3 trailColor = CalculateTrailColor(fs_in.TexCoord, thickness, uTime, fs_in.TrailAge, fs_in.DistanceFromStart, fs_in.DistanceFromEnd);
    
    // Generate trail shape
    float shape = GenerateTrailShape(fs_in.TexCoord, uTime, fs_in.TrailAge, fs_in.DistanceFromStart, fs_in.DistanceFromEnd);
    
    // Generate droplets
    float droplets = GenerateTrailDroplets(fs_in.TexCoord, uTime, fs_in.TrailAge, fs_in.DistanceFromStart);
    
    // Generate smearing effect
    float smearing = GenerateTrailSmearing(fs_in.TexCoord, uTime, fs_in.TrailAge, fs_in.DistanceFromStart);
    
    // Generate coagulation effect
    float coagulation = GenerateTrailCoagulation(fs_in.TexCoord, uTime, fs_in.TrailAge, fs_in.DistanceFromStart, fs_in.DistanceFromEnd);
    
    // Generate depth variation
    float depth = GenerateTrailDepth(fs_in.TexCoord, uTime, fs_in.TrailAge, fs_in.DistanceFromStart, fs_in.DistanceFromEnd);
    
    // Calculate lighting
    vec3 litColor = CalculateTrailLighting(trailColor, trailNormal, fs_in.WorldPosition, thickness, fs_in.TrailAge);
    
    // Combine effects
    vec3 finalColor = litColor;
    
    // Add depth-based color variation
    finalColor *= (1.0 + depth * 0.2);
    
    // Add coagulation color changes
    vec3 coagulatedColor = mix(trailColor, trailColor * 0.7, coagulation);
    finalColor = mix(finalColor, coagulatedColor, coagulation * 0.8);
    
    // Add smearing effect
    finalColor *= (1.0 + smearing * 0.3);
    
    // Apply vertex color
    finalColor *= fs_in.Color.rgb;
    
    // Calculate alpha
    float alpha = CalculateTrailAlpha(thickness, shape, droplets, smearing, coagulation, fs_in.TrailAge);
    
    // Apply fog
    finalColor = mix(finalColor, vec3(0.0, 0.0, 0.0), fs_in.FogFactor);
    alpha *= (1.0 - fs_in.FogFactor);
    
    FragColor = vec4(finalColor, alpha);
}