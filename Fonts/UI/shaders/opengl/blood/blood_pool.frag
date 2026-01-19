#version 330 core

// Blood Pool Fragment Shader for OpenGL
// Creates realistic blood pool effects with spreading and coagulation

// Uniforms
uniform float uTime;                    // Animation time
uniform vec2 uResolution;              // Screen resolution
uniform vec3 uBloodColor;              // Base blood color
uniform vec3 uBloodColorDark;          // Darker blood color for depth
uniform vec3 uBloodColorLight;         // Lighter blood color for highlights
uniform float uIntensity;             // Blood intensity/brightness
uniform float uPoolSize;              // Blood pool size
uniform float uSpreadRate;             // Blood spreading rate
uniform float uCoagulationRate;        // Blood coagulation rate
uniform vec2 uUVScale;                // UV scaling factor
uniform vec2 uUVOffset;               // UV offset for animation
uniform vec4 uPoolParams;              // Pool parameters (size, depth, viscosity, surface tension)
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
uniform vec3 uPoolCenter;              // Pool center position
uniform float uPoolRadius;             // Pool radius

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
    vec2 PoolUV;                        // Pool UV coordinates
    vec3 ViewDirection;                  // View direction
    vec4 ScreenPosition;                 // Screen space position
    float DistanceFromCenter;           // Distance from pool center
    float PoolAge;                      // Pool age
    vec4 InstanceData;                  // Instance data
    float FogFactor;                     // Fog factor
} fs_in;

// Output
out vec4 FragColor;

// Utility functions
vec3 GeneratePoolNormal(vec2 uv, float time, float poolAge)
{
    // Generate dynamic normal map for blood pool surface
    vec2 noiseUV = uv * uNoiseParams.x + time * 0.5 + poolAge * 0.1;
    float noise1 = texture(uNoiseTexture, noiseUV).r;
    float noise2 = texture(uNoiseTexture, noiseUV * 2.0).r;
    float noise3 = texture(uNoiseTexture, noiseUV * 4.0).r;
    
    // Combine noise for surface variation
    float combinedNoise = (noise1 + noise2 * 0.5 + noise3 * 0.25) * uNoiseParams.y;
    
    // Create normal from noise with pool-specific characteristics
    vec3 normal = vec3(0.0, 0.0, 1.0);
    normal.x = sin(combinedNoise * 6.28318) * 0.05; // Subtle surface variation
    normal.y = cos(combinedNoise * 6.28318) * 0.05;
    normal = normalize(normal);
    
    return normal;
}

float CalculatePoolThickness(vec2 uv, float time, float poolAge, float distanceFromCenter)
{
    // Calculate blood pool thickness variation
    vec2 thicknessUV = uv * uNoiseParams.x * 2.0 + time * 0.3 + poolAge * 0.05;
    float thicknessNoise = texture(uNoiseTexture, thicknessUV).r;
    
    // Base thickness from pool parameters
    float thickness = uPoolParams.y + thicknessNoise * 0.3;
    
    // Add distance-based thickness (thicker in center)
    float distanceFactor = 1.0 - clamp(distanceFromCenter / uPoolRadius, 0.0, 1.0);
    thickness *= (1.0 + distanceFactor * 2.0); // Center is 3x thicker
    
    // Add coagulation-based thickening over time
    float coagulationFactor = clamp(poolAge * uCoagulationRate * 0.1, 0.0, 1.0);
    thickness *= (1.0 + coagulationFactor * 0.5);
    
    // Add edge thinning
    vec2 centerDist = abs(uv - 0.5);
    float edgeFactor = 1.0 - max(centerDist.x, centerDist.y) * 2.0;
    thickness *= clamp(edgeFactor, 0.0, 1.0);
    
    return thickness;
}

vec3 CalculatePoolColor(vec2 uv, float thickness, float time, float poolAge, float distanceFromCenter)
{
    // Sample blood texture
    vec4 bloodTex = texture(uBloodTexture, uv);
    
    // Generate dynamic color variation
    vec2 colorNoiseUV = uv * uNoiseParams.x * 3.0 + time * 0.2 + poolAge * 0.03;
    float colorNoise = texture(uNoiseTexture, colorNoiseUV).r;
    
    // Blend between blood colors based on thickness and distance
    vec3 baseColor = mix(uBloodColor, uBloodColorDark, thickness * 0.3);
    baseColor = mix(baseColor, uBloodColorLight, colorNoise * 0.2);
    
    // Apply texture influence
    baseColor *= bloodTex.rgb;
    
    // Add distance-based color variation (darker at edges)
    float distanceFactor = clamp(distanceFromCenter / uPoolRadius, 0.0, 1.0);
    baseColor *= (1.0 - distanceFactor * 0.3);
    
    // Add coagulation effect (darkening over time)
    float coagulationFactor = clamp(poolAge * uCoagulationRate * 0.05, 0.0, 1.0);
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
    // Calculate rim lighting for pool edges
    float rimFactor = 1.0 - clamp(dot(normal, viewDir), 0.0, 1.0);
    rimFactor = pow(rimFactor, 2.0);
    rimFactor *= uRimLightIntensity;
    rimFactor *= (1.0 - thickness * 0.3); // Thinner areas have more rim lighting
    
    return rimFactor;
}

float GeneratePoolShape(vec2 uv, float time, float poolAge, float distanceFromCenter)
{
    // Generate blood pool shape with spreading effect
    float shape = 0.0;
    
    // Base circular pool shape
    float poolRadius = uPoolRadius * (1.0 + poolAge * uSpreadRate * 0.1);
    float distance = distanceFromCenter;
    
    // Create soft edges
    shape = 1.0 - smoothstep(poolRadius - 0.2, poolRadius + 0.2, distance);
    
    // Add spreading animation
    float spreadFactor = clamp(poolAge * uSpreadRate * 0.2, 0.0, 1.0);
    shape *= spreadFactor;
    
    // Add irregularity to pool edges
    float edgeNoise = texture(uNoiseTexture, uv * 5.0 + time * 0.5 + poolAge * 0.1).r;
    float irregularity = edgeNoise * 0.1;
    shape += irregularity * (1.0 - shape) * 0.5;
    
    return clamp(shape, 0.0, 1.0);
}

float GeneratePoolSpreading(vec2 uv, float time, float poolAge)
{
    // Generate blood pool spreading effect
    float spreading = 0.0;
    
    // Calculate spreading based on time and pool parameters
    float spreadRadius = uPoolRadius * (1.0 + poolAge * uSpreadRate * 0.3);
    float distance = length(uv - 0.5);
    
    // Create spreading gradient
    spreading = 1.0 - smoothstep(spreadRadius - 0.5, spreadRadius + 0.5, distance);
    
    // Add viscosity effect (slower spreading)
    float viscosity = uPoolParams.z;
    spreading *= (1.0 - viscosity * 0.3);
    
    // Add surface tension effect
    float surfaceTension = uPoolParams.w;
    spreading *= (1.0 - surfaceTension * 0.2);
    
    return clamp(spreading, 0.0, 1.0);
}

float GeneratePoolCoagulation(vec2 uv, float time, float poolAge, float distanceFromCenter)
{
    // Generate blood pool coagulation effect
    float coagulation = 0.0;
    
    // Base coagulation based on age
    float baseCoagulation = clamp(poolAge * uCoagulationRate * 0.1, 0.0, 1.0);
    
    // Edge coagulation (edges coagulate faster)
    float edgeFactor = clamp(distanceFromCenter / uPoolRadius, 0.0, 1.0);
    float edgeCoagulation = edgeFactor * uCoagulationParams.y;
    
    // Surface coagulation patterns
    vec2 coagulationUV = uv * uNoiseParams.x * 4.0 + time * 0.1 + poolAge * 0.02;
    float coagulationNoise = texture(uNoiseTexture, coagulationUV).r;
    float surfaceCoagulation = coagulationNoise * uCoagulationParams.z;
    
    // Combine coagulation effects
    coagulation = baseCoagulation + edgeCoagulation + surfaceCoagulation;
    coagulation = clamp(coagulation, 0.0, 1.0);
    
    return coagulation;
}

float GeneratePoolDepth(vec2 uv, float time, float poolAge, float distanceFromCenter)
{
    // Generate blood pool depth variation
    float depth = 0.0;
    
    // Base depth from pool parameters
    depth = uPoolParams.y;
    
    // Add distance-based depth (deeper in center)
    float distanceFactor = 1.0 - clamp(distanceFromCenter / uPoolRadius, 0.0, 1.0);
    depth *= (1.0 + distanceFactor * 3.0); // Center is 4x deeper
    
    // Add depth variation over time (pool gets deeper as it spreads)
    float timeDepth = poolAge * uSpreadRate * 0.05;
    depth += timeDepth;
    
    // Add noise-based depth variation
    vec2 depthUV = uv * uNoiseParams.x * 2.0 + time * 0.2 + poolAge * 0.03;
    float depthNoise = texture(uNoiseTexture, depthUV).r;
    depth += depthNoise * 0.2;
    
    return depth;
}

vec3 CalculatePoolLighting(vec3 baseColor, vec3 normal, vec3 worldPos, float thickness, float poolAge)
{
    // Calculate lighting for blood pool
    vec3 litColor = baseColor;
    
    // View and light directions
    vec3 viewDir = normalize(uViewDirection);
    vec3 lightDir = normalize(-uLightDirection);
    
    // Diffuse lighting
    float NdotL = clamp(dot(normal, lightDir), 0.0, 1.0);
    vec3 diffuse = baseColor * NdotL * uLightColor;
    
    // Ambient lighting
    vec3 ambient = baseColor * uAmbientColor * uAmbientIntensity;
    
    // Specular lighting (reduced for liquid)
    float roughness = 0.8; // Blood pools are rough
    float specular = CalculateSpecular(normal, viewDir, lightDir, roughness) * 0.3;
    
    // Rim lighting
    float rim = CalculateRimLighting(normal, viewDir, thickness) * 0.5;
    
    // Combine lighting
    litColor = ambient + diffuse + specular + rim;
    
    // Add age-based lighting changes (darker as it coagulates)
    float ageFactor = clamp(poolAge * uCoagulationRate * 0.02, 0.0, 1.0);
    litColor *= (1.0 - ageFactor * 0.3);
    
    return litColor;
}

float CalculatePoolAlpha(float thickness, float shape, float spreading, float poolAge)
{
    // Calculate blood pool alpha
    float alpha = thickness * shape * spreading;
    
    // Apply age-based opacity changes
    float ageFactor = clamp(poolAge * uCoagulationRate * 0.1, 0.0, 1.0);
    alpha *= (1.0 + ageFactor * 0.2); // Becomes more opaque as it coagulates
    
    // Ensure minimum visibility
    alpha = max(alpha, 0.3);
    
    return clamp(alpha, 0.0, 1.0);
}

// Main fragment shader
void main()
{
    // Generate dynamic normal
    vec3 poolNormal = GeneratePoolNormal(fs_in.TexCoord, uTime, fs_in.PoolAge);
    
    // Calculate thickness
    float thickness = CalculatePoolThickness(fs_in.TexCoord, uTime, fs_in.PoolAge, fs_in.DistanceFromCenter);
    
    // Calculate pool color
    vec3 poolColor = CalculatePoolColor(fs_in.TexCoord, thickness, uTime, fs_in.PoolAge, fs_in.DistanceFromCenter);
    
    // Generate pool shape
    float shape = GeneratePoolShape(fs_in.TexCoord, uTime, fs_in.PoolAge, fs_in.DistanceFromCenter);
    
    // Generate spreading effect
    float spreading = GeneratePoolSpreading(fs_in.TexCoord, uTime, fs_in.PoolAge);
    
    // Generate coagulation effect
    float coagulation = GeneratePoolCoagulation(fs_in.TexCoord, uTime, fs_in.PoolAge, fs_in.DistanceFromCenter);
    
    // Generate depth variation
    float depth = GeneratePoolDepth(fs_in.TexCoord, uTime, fs_in.PoolAge, fs_in.DistanceFromCenter);
    
    // Calculate lighting
    vec3 litColor = CalculatePoolLighting(poolColor, poolNormal, fs_in.WorldPosition, thickness, fs_in.PoolAge);
    
    // Combine effects
    vec3 finalColor = litColor;
    
    // Add depth-based color variation
    finalColor *= (1.0 + depth * 0.1);
    
    // Add coagulation color changes
    vec3 coagulatedColor = mix(poolColor, poolColor * 0.6, coagulation);
    finalColor = mix(finalColor, coagulatedColor, coagulation * 0.7);
    
    // Apply vertex color
    finalColor *= fs_in.Color.rgb;
    
    // Calculate alpha
    float alpha = CalculatePoolAlpha(thickness, shape, spreading, fs_in.PoolAge);
    
    // Apply fog
    finalColor = mix(finalColor, vec3(0.0, 0.0, 0.0), fs_in.FogFactor);
    alpha *= (1.0 - fs_in.FogFactor);
    
    FragColor = vec4(finalColor, alpha);
}