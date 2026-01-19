// Blood Effect Pixel Shader for DirectX
// Creates a realistic blood splatter effect with dynamic properties

cbuffer BloodConstants : register(b0)
{
    float4 BloodColor;          // Base blood color (RGBA)
    float4 BloodColorDark;      // Darker blood color for depth
    float4 BloodColorLight;     // Lighter blood color for highlights
    float Time;                 // Animation time
    float Intensity;            // Blood intensity/brightness
    float FlowSpeed;            // Blood flow animation speed
    float Thickness;            // Blood thickness/thickness variation
    float Roughness;            // Surface roughness for specular
    float2 UVScale;             // UV scaling for texture coordinates
    float2 UVOffset;            // UV offset for animation
    float4 SplatterParams;       // Splatter parameters (count, size, spread, randomness)
    float4 DripParams;          // Drip parameters (speed, length, frequency, viscosity)
    float4 CoagulationParams;   // Coagulation (darkening, edge effects, drying time)
    float4 NoiseParams;         // Noise parameters (scale, octaves, persistence, lacunarity)
};

cbuffer LightingConstants : register(b1)
{
    float3 LightDirection;      // Main light direction
    float3 LightColor;          // Main light color
    float3 AmbientColor;        // Ambient light color
    float AmbientIntensity;     // Ambient light intensity
    float SpecularIntensity;    // Specular highlight intensity
    float Shininess;            // Specular shininess factor
    float RimLightIntensity;    // Rim lighting intensity
    float3 ViewDirection;       // Camera/view direction
};

// Textures
Texture2D BloodTexture : register(t0);
Texture2D NoiseTexture : register(t1);
Texture2D NormalMap : register(t2);
Texture2D RoughnessMap : register(t3);
Texture2D ThicknessMap : register(t4);

// Samplers
SamplerState TextureSampler : register(s0);
SamplerState LinearSampler : register(s1);

// Input structure from vertex shader
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float3 WorldPosition : WORLD_POSITION;
    float4 Color : COLOR;
    float2 BloodFlowUV : TEXCOORD1;
    float3 ViewDirection : TEXCOORD2;
    float4 ScreenPosition : TEXCOORD3;
};

// Utility functions
float3 GenerateBloodNormal(float2 uv, float time)
{
    // Generate dynamic normal map for blood surface
    float2 noiseUV = uv * NoiseParams.x + UVOffset * FlowSpeed * time;
    float noise1 = NoiseTexture.Sample(TextureSampler, noiseUV).r;
    float noise2 = NoiseTexture.Sample(TextureSampler, noiseUV * 2.0).r;
    float noise3 = NoiseTexture.Sample(TextureSampler, noiseUV * 4.0).r;
    
    // Combine noise for surface variation
    float combinedNoise = (noise1 + noise2 * 0.5 + noise3 * 0.25) * NoiseParams.y;
    
    // Create normal from noise
    float3 normal = float3(0, 0, 1);
    normal.x = sin(combinedNoise * 6.28318) * 0.1;
    normal.y = cos(combinedNoise * 6.28318) * 0.1;
    normal = normalize(normal);
    
    return normal;
}

float CalculateBloodThickness(float2 uv, float time)
{
    // Calculate blood thickness variation
    float2 thicknessUV = uv * UVScale * 2.0 + UVOffset * FlowSpeed * time * 0.5;
    float thicknessNoise = NoiseTexture.Sample(TextureSampler, thicknessUV).r;
    
    // Apply thickness parameters
    float thickness = Thickness + thicknessNoise * 0.3;
    
    // Add edge thinning
    float2 centerDist = abs(uv - 0.5);
    float edgeFactor = 1.0 - max(centerDist.x, centerDist.y) * 2.0;
    thickness *= saturate(edgeFactor);
    
    return thickness;
}

float3 CalculateBloodColor(float2 uv, float thickness, float time)
{
    // Sample blood texture
    float4 bloodTex = BloodTexture.Sample(TextureSampler, uv);
    
    // Generate dynamic color variation
    float2 colorNoiseUV = uv * NoiseParams.x * 3.0 + UVOffset * FlowSpeed * time * 0.3;
    float colorNoise = NoiseTexture.Sample(TextureSampler, colorNoiseUV).r;
    
    // Blend between blood colors based on thickness and noise
    float3 baseColor = lerp(BloodColor.rgb, BloodColorDark.rgb, thickness * 0.5);
    baseColor = lerp(baseColor, BloodColorLight.rgb, colorNoise * 0.3);
    
    // Apply texture influence
    baseColor *= bloodTex.rgb;
    
    // Add coagulation effect (darkening over time)
    float coagulationFactor = saturate(time * CoagulationParams.z * 0.1);
    baseColor *= (1.0 - coagulationFactor * CoagulationParams.x);
    
    return baseColor * Intensity;
}

float CalculateSpecular(float3 normal, float3 viewDir, float3 lightDir, float roughness)
{
    // Calculate Blinn-Phong specular
    float3 halfwayDir = normalize(lightDir + viewDir);
    float NdotH = saturate(dot(normal, halfwayDir));
    
    float specular = pow(NdotH, Shininess);
    specular *= SpecularIntensity;
    specular *= (1.0 - roughness); // Rougher surfaces have less specular
    
    return specular;
}

float CalculateRimLighting(float3 normal, float3 viewDir, float thickness)
{
    // Calculate rim lighting for blood edges
    float rimFactor = 1.0 - saturate(dot(normal, viewDir));
    rimFactor = pow(rimFactor, 2.0);
    rimFactor *= RimLightIntensity;
    rimFactor *= (1.0 - thickness * 0.5); // Thinner areas have more rim lighting
    
    return rimFactor;
}

float GenerateSplatterPattern(float2 uv, float time)
{
    // Generate blood splatter pattern
    float splatter = 0.0;
    
    for (int i = 0; i < (int)SplatterParams.x; i++)
    {
        // Random splatter position
        float2 splatterUV = uv + float2(sin(time + i * 123.456), cos(time + i * 789.012)) * SplatterParams.z;
        
        // Random splatter size
        float splatterSize = SplatterParams.y * (0.5 + 0.5 * sin(time * 2.0 + i * 345.678));
        
        // Calculate splatter shape
        float2 splatterDist = abs(splatterUV - 0.5);
        float splatterShape = max(splatterDist.x, splatterDist.y);
        splatterShape = 1.0 - saturate(splatterShape / splatterSize);
        
        // Add randomness
        float splatterRandom = sin(time * 3.0 + i * 234.567) * 0.5 + 0.5;
        splatterShape *= splatterRandom;
        
        splatter += splatterShape;
    }
    
    return saturate(splatter);
}

float GenerateDrippingEffect(float2 uv, float time)
{
    // Generate blood dripping effect
    float drip = 0.0;
    
    for (int i = 0; i < 3; i++)
    {
        // Drip position and movement
        float dripY = uv.y + time * DripParams.x * (1.0 + i * 0.3);
        dripY = frac(dripY);
        
        // Drip shape
        float dripShape = 1.0 - abs(dripY - 0.5) * 2.0;
        dripShape = saturate(dripShape);
        
        // Drip width variation
        float dripWidth = 0.1 + 0.05 * sin(time * 2.0 + i * 123.456);
        float dripX = abs(uv.x - 0.5);
        dripX = 1.0 - saturate(dripX / dripWidth);
        
        drip += dripShape * dripX * DripParams.y * (1.0 - i * 0.3);
    }
    
    return saturate(drip);
}

// Main pixel shader
float4 main(PS_INPUT input) : SV_TARGET
{
    // Generate dynamic normal
    float3 bloodNormal = GenerateBloodNormal(input.TexCoord, Time);
    
    // Calculate thickness
    float thickness = CalculateBloodThickness(input.TexCoord, Time);
    
    // Calculate blood color
    float3 bloodColor = CalculateBloodColor(input.TexCoord, thickness, Time);
    
    // Generate splatter pattern
    float splatter = GenerateSplatterPattern(input.TexCoord * UVScale, Time);
    
    // Generate dripping effect
    float drip = GenerateDrippingEffect(input.TexCoord * UVScale * 2.0, Time);
    
    // Combine effects
    float finalThickness = thickness + splatter * 0.3 + drip * 0.2;
    bloodColor = lerp(bloodColor, bloodColor * 0.7, drip * 0.5); // Darken drips
    
    // Calculate lighting
    float3 normal = normalize(input.Normal + bloodNormal * 0.5);
    float3 viewDir = normalize(input.ViewDirection);
    float3 lightDir = normalize(-LightDirection);
    
    // Diffuse lighting
    float NdotL = saturate(dot(normal, lightDir));
    float3 diffuse = bloodColor * NdotL;
    
    // Ambient lighting
    float3 ambient = bloodColor * AmbientColor * AmbientIntensity;
    
    // Specular lighting
    float specular = CalculateSpecular(normal, viewDir, lightDir, Roughness);
    
    // Rim lighting
    float rim = CalculateRimLighting(normal, viewDir, finalThickness);
    
    // Combine lighting
    float3 finalColor = ambient + diffuse + specular + rim;
    
    // Add edge effects for coagulation
    float2 edgeDist = abs(input.TexCoord - 0.5);
    float edgeFactor = max(edgeDist.x, edgeDist.y);
    edgeFactor = saturate(edgeFactor * 2.0);
    
    float coagulationEdge = pow(edgeFactor, CoagulationParams.y) * CoagulationParams.x;
    finalColor *= (1.0 - coagulationEdge * 0.3);
    
    // Apply alpha based on thickness and effects
    float alpha = finalThickness + splatter * 0.5 + drip * 0.3;
    alpha = saturate(alpha);
    
    // Add some transparency for thin areas
    alpha = lerp(alpha * 0.3, alpha, finalThickness);
    
    return float4(finalColor, alpha);
}