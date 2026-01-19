// Font Rendering Pixel Shader for DirectX
// High-quality text rendering with support for various font effects

cbuffer FontConstants : register(b0)
{
    float4 FontColor;              // Base font color (RGBA)
    float4 OutlineColor;           // Outline color for text borders
    float4 ShadowColor;            // Shadow color for text shadows
    float4 GlowColor;              // Glow color for text glow effect
    float4 BackgroundColor;        // Background color for text background
    float Time;                    // Animation time for dynamic effects
    float Alpha;                   // Overall alpha multiplier
    float OutlineWidth;            // Width of text outline
    float ShadowOffsetX;           // Shadow X offset
    float ShadowOffsetY;           // Shadow Y offset
    float ShadowBlur;              // Shadow blur amount
    float GlowIntensity;           // Glow effect intensity
    float GlowRadius;              // Glow effect radius
    float2 UVScale;                // UV scaling for font texture
    float2 UVOffset;               // UV offset for animation
    float4 EffectParams;           // General effect parameters
    float4 AnimationParams;        // Animation parameters (speed, type, intensity, phase)
    float4 QualityParams;          // Quality parameters (antialiasing, smoothing, contrast, gamma)
    float4 DistortionParams;       // Distortion parameters (amount, frequency, direction, randomness)
};

cbuffer LightingConstants : register(b1)
{
    float3 LightDirection;         // Light direction for 3D text effects
    float3 LightColor;             // Light color
    float3 AmbientColor;           // Ambient light color
    float AmbientIntensity;        // Ambient light intensity
    float SpecularIntensity;       // Specular highlight intensity
    float Shininess;               // Specular shininess factor
    float3 ViewDirection;          // Camera/view direction
    float4 LightingParams;          // Additional lighting parameters
};

// Textures
Texture2D FontTexture : register(t0);           // Font atlas texture
Texture2D FontSDF : register(t1);                // Signed distance field font texture
Texture2D NoiseTexture : register(t2);            // Noise texture for effects
Texture2D GradientTexture : register(t3);         // Gradient texture for backgrounds
Texture2D PatternTexture : register(t4);          // Pattern texture for decorative effects

// Samplers
SamplerState FontSampler : register(s0);          // Point sampler for pixel-perfect text
SamplerState LinearSampler : register(s1);        // Linear sampler for effects
SamplerState AnisotropicSampler : register(s2);    // Anisotropic sampler for high-quality sampling

// Input structure from vertex shader
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float2 SDFCoord : TEXCOORD1;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
    float3 WorldPosition : WORLD_POSITION;
    float4 Color : COLOR;
    float3 ViewDirection : TEXCOORD2;
    float4 ScreenPosition : TEXCOORD3;
    float2 UVAnimation : TEXCOORD4;
    float3 WorldNormal : WORLD_NORMAL;
    float4 VertexData : TEXCOORD5;                // Custom vertex data
    float FogFactor : TEXCOORD6;
};

// Utility functions
float SampleFontSDF(float2 uv, float2 offset)
{
    // Sample signed distance field with sub-pixel precision
    float2 sampleUV = uv + offset;
    return FontSDF.Sample(LinearSampler, sampleUV).r;
}

float CalculateSDFAlpha(float sdf, float width)
{
    // Calculate alpha from signed distance field
    return saturate(0.5 - sdf / width);
}

float CalculateSmoothAlpha(float sdf, float width, float smoothing)
{
    // Calculate smooth alpha with anti-aliasing
    float alpha = CalculateSDFAlpha(sdf, width);
    float smoothWidth = smoothing * width;
    float smoothAlpha = smoothstep(0.5 - smoothWidth, 0.5 + smoothWidth, 0.5 - sdf / width);
    return smoothAlpha;
}

float3 CalculateOutline(float sdf, float width, float outlineWidth, float3 outlineColor)
{
    // Calculate text outline
    float outlineAlpha = CalculateSmoothAlpha(sdf, width + outlineWidth, QualityParams.x);
    float textAlpha = CalculateSmoothAlpha(sdf, width, QualityParams.x);
    float outlineFactor = saturate(outlineAlpha - textAlpha);
    return outlineColor * outlineFactor;
}

float3 CalculateShadow(float2 uv, float alpha, float2 shadowOffset, float shadowBlur)
{
    // Calculate text shadow
    float2 shadowUV = uv + shadowOffset;
    float shadowSDF = SampleFontSDF(shadowUV, 0.0);
    float shadowAlpha = CalculateSmoothAlpha(shadowSDF, 1.0, QualityParams.x);
    
    // Apply blur to shadow
    if (shadowBlur > 0.0)
    {
        float blurSamples[9];
        float2 blurOffsets[9] = {
            float2(-shadowBlur, -shadowBlur), float2(0.0, -shadowBlur), float2(shadowBlur, -shadowBlur),
            float2(-shadowBlur, 0.0), float2(0.0, 0.0), float2(shadowBlur, 0.0),
            float2(-shadowBlur, shadowBlur), float2(0.0, shadowBlur), float2(shadowBlur, shadowBlur)
        };
        
        for (int i = 0; i < 9; i++)
        {
            blurSamples[i] = SampleFontSDF(shadowUV + blurOffsets[i], 0.0);
        }
        
        float blurredSDF = 0.0;
        for (int i = 0; i < 9; i++)
        {
            blurredSDF += blurSamples[i];
        }
        blurredSDF /= 9.0;
        
        shadowAlpha = CalculateSmoothAlpha(blurredSDF, 1.0, QualityParams.x);
    }
    
    return ShadowColor.rgb * shadowAlpha * ShadowColor.a;
}

float3 CalculateGlow(float sdf, float width, float glowIntensity, float glowRadius, float3 glowColor)
{
    // Calculate text glow effect
    float glowAlpha = CalculateSmoothAlpha(sdf, width + glowRadius, QualityParams.x);
    float glowFactor = pow(glowAlpha, 2.0) * glowIntensity;
    return glowColor * glowFactor;
}

float3 Calculate3DLighting(float3 normal, float3 worldPos, float3 viewDir, float3 baseColor)
{
    // Calculate 3D lighting for embossed text
    float3 lightDir = normalize(LightDirection);
    float3 halfDir = normalize(lightDir + viewDir);
    
    // Diffuse lighting
    float NdotL = saturate(dot(normal, lightDir));
    float3 diffuse = baseColor * NdotL * LightColor;
    
    // Ambient lighting
    float3 ambient = baseColor * AmbientColor * AmbientIntensity;
    
    // Specular lighting
    float NdotH = saturate(dot(normal, halfDir));
    float specular = pow(NdotH, Shininess) * SpecularIntensity;
    
    return ambient + diffuse + specular;
}

float3 CalculateGradient(float2 uv, float time)
{
    // Calculate animated gradient background
    float2 gradientUV = uv * 2.0 + float2(time * 0.1, time * 0.05);
    float3 gradient = GradientTexture.Sample(LinearSampler, gradientUV).rgb;
    
    // Add animation
    float animation = sin(time * AnimationParams.x + uv.x * 3.0) * 0.5 + 0.5;
    gradient = lerp(gradient, gradient * animation, AnimationParams.z);
    
    return gradient;
}

float3 CalculatePattern(float2 uv, float time)
{
    // Calculate decorative pattern
    float2 patternUV = uv * 5.0 + float2(time * 0.2, time * 0.15);
    float3 pattern = PatternTexture.Sample(LinearSampler, patternUV).rgb;
    
    // Apply pattern parameters
    pattern *= EffectParams.x;
    pattern = saturate(pattern);
    
    return pattern;
}

float3 CalculateDistortion(float2 uv, float time)
{
    // Calculate UV distortion for wavy text effects
    float2 distortionUV = uv;
    
    float distortionAmount = DistortionParams.x;
    float distortionFreq = DistortionParams.y;
    float distortionDir = DistortionParams.z;
    
    // Apply wave distortion
    float wave = sin(uv.x * distortionFreq + time * 2.0) * cos(uv.y * distortionFreq + time * 1.5);
    distortionUV.x += wave * distortionAmount * cos(distortionDir);
    distortionUV.y += wave * distortionAmount * sin(distortionDir);
    
    // Add random distortion
    float random = NoiseTexture.Sample(LinearSampler, uv * 10.0 + time).r;
    distortionUV += (random - 0.5) * DistortionParams.w * 0.1;
    
    return FontTexture.Sample(FontSampler, distortionUV).rgb;
}

float3 CalculateFresnel(float3 normal, float3 viewDir, float3 baseColor, float fresnelPower)
{
    // Calculate Fresnel effect for shiny text
    float fresnel = 1.0 - saturate(dot(normal, viewDir));
    fresnel = pow(fresnel, fresnelPower);
    return baseColor * fresnel;
}

float3 CalculateSubsurfaceScattering(float3 baseColor, float sdf, float thickness)
{
    // Calculate subsurface scattering for realistic text
    float scatter = saturate(1.0 - abs(sdf) / thickness);
    float3 scatteredColor = baseColor * scatter * 0.5;
    return scatteredColor;
}

float3 CalculateColorCorrection(float3 color)
{
    // Apply color correction
    // Contrast
    color = (color - 0.5) * QualityParams.z + 0.5;
    
    // Gamma correction
    color = pow(color, 1.0 / QualityParams.w);
    
    // Clamp to valid range
    color = saturate(color);
    
    return color;
}

float3 CalculateAnimatedColor(float3 baseColor, float time, float2 uv)
{
    // Calculate animated color effects
    float3 animatedColor = baseColor;
    
    // Pulsing effect
    float pulse = sin(time * AnimationParams.x + AnimationParams.w) * 0.5 + 0.5;
    animatedColor *= (1.0 + pulse * AnimationParams.y * 0.3);
    
    // Rainbow effect
    if (AnimationParams.z > 0.5)
    {
        float3 rainbow = float3(
            sin(time * 2.0 + uv.x * 3.0) * 0.5 + 0.5,
            sin(time * 2.0 + uv.x * 3.0 + 2.094) * 0.5 + 0.5,
            sin(time * 2.0 + uv.x * 3.0 + 4.189) * 0.5 + 0.5
        );
        animatedColor = lerp(animatedColor, rainbow, AnimationParams.z);
    }
    
    return animatedColor;
}

// Main pixel shader
float4 main(PS_INPUT input) : SV_TARGET
{
    // Sample font SDF
    float sdf = SampleFontSDF(input.SDFCoord, 0.0);
    
    // Calculate base alpha with smoothing
    float textAlpha = CalculateSmoothAlpha(sdf, 1.0, QualityParams.x);
    
    // Early exit if no text
    if (textAlpha <= 0.0)
    {
        return float4(0, 0, 0, 0);
    }
    
    // Calculate base color
    float3 baseColor = FontColor.rgb;
    
    // Apply animated color
    baseColor = CalculateAnimatedColor(baseColor, Time, input.TexCoord);
    
    // Apply color correction
    baseColor = CalculateColorCorrection(baseColor);
    
    // Calculate outline
    float3 outlineColor = CalculateOutline(sdf, 1.0, OutlineWidth, OutlineColor.rgb);
    
    // Calculate shadow
    float3 shadowColor = CalculateShadow(input.TexCoord, textAlpha, 
                                           float2(ShadowOffsetX, ShadowOffsetY), ShadowBlur);
    
    // Calculate glow
    float3 glowColor = CalculateGlow(sdf, 1.0, GlowIntensity, GlowRadius, GlowColor.rgb);
    
    // Calculate 3D lighting
    float3 litColor = Calculate3DLighting(input.WorldNormal, input.WorldPosition, 
                                         input.ViewDirection, baseColor);
    
    // Calculate Fresnel effect
    float3 fresnelColor = CalculateFresnel(input.WorldNormal, input.ViewDirection, 
                                           baseColor, 5.0);
    
    // Calculate subsurface scattering
    float3 scatteredColor = CalculateSubsurfaceScattering(baseColor, sdf, 0.5);
    
    // Calculate gradient background
    float3 gradientColor = CalculateGradient(input.TexCoord, Time);
    
    // Calculate pattern
    float3 patternColor = CalculatePattern(input.TexCoord, Time);
    
    // Calculate distortion effect
    float3 distortedColor = CalculateDistortion(input.TexCoord, Time);
    
    // Combine all color effects
    float3 finalColor = baseColor;
    finalColor = lerp(finalColor, litColor, 0.5);          // Mix with 3D lighting
    finalColor = lerp(finalColor, outlineColor, 0.3);       // Add outline
    finalColor += shadowColor * 0.5;                      // Add shadow
    finalColor += glowColor;                              // Add glow
    finalColor += fresnelColor * 0.2;                     // Add Fresnel
    finalColor = lerp(finalColor, scatteredColor, 0.3);    // Add subsurface scattering
    finalColor = lerp(finalColor, gradientColor, 0.1);     // Add gradient
    finalColor = lerp(finalColor, patternColor, 0.05);     // Add pattern
    finalColor = lerp(finalColor, distortedColor, 0.1);   // Add distortion
    
    // Apply vertex color
    finalColor *= input.Color.rgb;
    
    // Apply alpha
    float finalAlpha = textAlpha * Alpha;
    
    // Apply fog
    finalColor = lerp(finalColor, float3(0, 0, 0), input.FogFactor);
    finalAlpha *= (1.0 - input.FogFactor);
    
    // Apply background color for text with transparency
    if (finalAlpha < 1.0)
    {
        finalColor = lerp(BackgroundColor.rgb, finalColor, finalAlpha);
        finalAlpha = max(finalAlpha, BackgroundColor.a);
    }
    
    return float4(finalColor, finalAlpha);
}