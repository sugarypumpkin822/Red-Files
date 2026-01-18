#version 330 core

// Common OpenGL Lighting Functions
// Shared lighting calculations for all OpenGL shaders

// Light Types
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define LIGHT_TYPE_AREA 3

// Material Properties
struct Material {
    vec3 albedo;              // Base color
    float metallic;           // Metallic factor (0.0 = dielectric, 1.0 = metallic)
    float roughness;          // Roughness factor (0.0 = smooth, 1.0 = rough)
    float ao;                 // Ambient occlusion
    vec3 emission;            // Emission color
    float alpha;              // Alpha/opacity
    vec3 normal;              // Normal vector
    vec3 f0;                 // Fresnel reflectance at 0 degrees
};

// Light Properties
struct Light {
    int type;                 // Light type (directional, point, spot, area)
    vec3 position;            // Light position (for point/spot lights)
    vec3 direction;           // Light direction (for directional/spot lights)
    vec3 color;               // Light color/intensity
    float intensity;          // Light intensity multiplier
    float range;              // Light range (for point/spot lights)
    float innerCone;           // Inner cone angle (for spot lights)
    float outerCone;           // Outer cone angle (for spot lights)
    bool castShadow;           // Whether this light casts shadows
    int shadowMapIndex;       // Shadow map index
    mat4 shadowMatrix;         // Shadow projection matrix
};

// Lighting Calculation Functions
vec3 calculateF0(vec3 albedo, float metallic)
{
    // Calculate Fresnel reflectance at 0 degrees
    vec3 f0 = vec3(0.04); // Default for dielectrics
    f0 = mix(f0, albedo, metallic);
    return f0;
}

float calculateDistributionGGX(float NdotH, float roughness)
{
    // GGX/Trowbridge-Reitz distribution
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH2 = NdotH * NdotH;
    
    float nom = a2;
    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    return nom / (PI * denom * denom);
}

float calculateGeometrySchlickGGX(float NdotV, float roughness)
{
    // Schlick-GGX geometry function
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}

float calculateGeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    // Smith geometry function
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = calculateGeometrySchlickGGX(NdotV, roughness);
    float ggx1 = calculateGeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 calculateFresnelSchlick(float cosTheta, vec3 F0)
{
    // Schlick approximation to Fresnel reflectance
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 calculateFresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    // Schlick approximation with roughness
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Cook-Torrance BRDF
vec3 calculateCookTorranceBRDF(vec3 N, vec3 V, vec3 L, vec3 H, Material material)
{
    // Calculate radiance
    vec3 F0 = calculateF0(material.albedo, material.metallic);
    
    // Normal distribution function
    float NDF = calculateDistributionGGX(max(dot(N, H), 0.0), material.roughness);
    
    // Geometry function
    float G = calculateGeometrySmith(N, V, L, material.roughness);
    
    // Fresnel equation
    vec3 F = calculateFresnelSchlick(max(dot(H, V), 0.0), F0);
    
    // Cook-Torrance BRDF
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // Add epsilon to prevent division by zero
    vec3 specular = numerator / denominator;
    
    // Energy conservation
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - material.metallic;
    
    // Diffuse term (Lambertian)
    vec3 diffuse = kD * material.albedo / PI;
    
    return (diffuse + specular) * material.albedo;
}

// Lambertian Diffuse Lighting
vec3 calculateLambertianDiffuse(vec3 albedo, vec3 lightColor, vec3 N, vec3 L)
{
    float NdotL = max(dot(N, L), 0.0);
    return albedo * lightColor * NdotL / PI;
}

// Phong Specular Lighting
vec3 calculatePhongSpecular(vec3 lightColor, vec3 N, vec3 L, vec3 V, float shininess)
{
    vec3 R = reflect(-L, N);
    float RdotV = max(dot(R, V), 0.0);
    return lightColor * pow(RdotV, shininess);
}

// Blinn-Phong Specular Lighting
vec3 calculateBlinnPhongSpecular(vec3 lightColor, vec3 N, vec3 L, vec3 V, float shininess)
{
    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);
    return lightColor * pow(NdotH, shininess);
}

// Directional Light
vec3 calculateDirectionalLight(Light light, Material material, vec3 N, vec3 V, vec3 worldPos)
{
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(V + L);
    
    // Calculate attenuation (directional lights have no attenuation)
    float attenuation = 1.0;
    
    // Calculate radiance
    vec3 radiance = light.color * light.intensity * attenuation;
    
    // Cook-Torrance BRDF
    vec3 brdf = calculateCookTorranceBRDF(N, V, L, H, material);
    
    // Calculate outgoing radiance
    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = brdf * radiance * NdotL;
    
    return Lo;
}

// Point Light
vec3 calculatePointLight(Light light, Material material, vec3 N, vec3 V, vec3 worldPos)
{
    vec3 L = normalize(light.position - worldPos);
    vec3 H = normalize(V + L);
    
    // Calculate distance attenuation
    float distance = length(light.position - worldPos);
    float attenuation = 1.0 / (distance * distance);
    attenuation = pow(clamp(1.0 - pow(distance / light.range, 4.0), 0.0, 1.0), 2.0);
    
    // Calculate radiance
    vec3 radiance = light.color * light.intensity * attenuation;
    
    // Cook-Torrance BRDF
    vec3 brdf = calculateCookTorranceBRDF(N, V, L, H, material);
    
    // Calculate outgoing radiance
    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = brdf * radiance * NdotL;
    
    return Lo;
}

// Spot Light
vec3 calculateSpotLight(Light light, Material material, vec3 N, vec3 V, vec3 worldPos)
{
    vec3 L = normalize(light.position - worldPos);
    vec3 H = normalize(V + L);
    
    // Calculate distance attenuation
    float distance = length(light.position - worldPos);
    float distanceAttenuation = 1.0 / (distance * distance);
    distanceAttenuation = pow(clamp(1.0 - pow(distance / light.range, 4.0), 0.0, 1.0), 2.0);
    
    // Calculate spot attenuation
    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.innerCone - light.outerCone;
    float spotAttenuation = clamp((theta - light.outerCone) / epsilon, 0.0, 1.0);
    
    // Calculate total attenuation
    float attenuation = distanceAttenuation * spotAttenuation;
    
    // Calculate radiance
    vec3 radiance = light.color * light.intensity * attenuation;
    
    // Cook-Torrance BRDF
    vec3 brdf = calculateCookTorranceBRDF(N, V, L, H, material);
    
    // Calculate outgoing radiance
    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = brdf * radiance * NdotL;
    
    return Lo;
}

// Area Light (simplified)
vec3 calculateAreaLight(Light light, Material material, vec3 N, vec3 V, vec3 worldPos, vec3 lightRight, vec3 lightUp)
{
    // Simplified area light calculation using multiple point samples
    vec3 lightColor = vec3(0.0);
    int samples = 4; // 2x2 grid
    
    for (int x = 0; x < 2; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            vec2 offset = vec2((float(x) - 0.5) * 0.5, (float(y) - 0.5) * 0.5);
            vec3 samplePos = light.position + lightRight * offset.x + lightUp * offset.y;
            
            Light sampleLight = light;
            sampleLight.position = samplePos;
            sampleLight.intensity = light.intensity / float(samples);
            
            lightColor += calculatePointLight(sampleLight, material, N, V, worldPos);
        }
    }
    
    return lightColor;
}

// Multiple Lights
vec3 calculateLighting(Light lights[8], int lightCount, Material material, vec3 N, vec3 V, vec3 worldPos)
{
    vec3 totalColor = vec3(0.0);
    
    for (int i = 0; i < lightCount; i++)
    {
        Light light = lights[i];
        
        switch (light.type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalColor += calculateDirectionalLight(light, material, N, V, worldPos);
                break;
            case LIGHT_TYPE_POINT:
                totalColor += calculatePointLight(light, material, N, V, worldPos);
                break;
            case LIGHT_TYPE_SPOT:
                totalColor += calculateSpotLight(light, material, N, V, worldPos);
                break;
            case LIGHT_TYPE_AREA:
                // Area lights need additional parameters (lightRight, lightUp)
                // This is a simplified version
                totalColor += calculatePointLight(light, material, N, V, worldPos);
                break;
        }
    }
    
    return totalColor;
}

// Image-Based Lighting (IBL)
vec3 calculateIBL(vec3 N, vec3 V, Material material, samplerCube environmentMap, samplerCube prefilterMap, sampler2D brdfLUT)
{
    // Sample environment map
    vec3 R = reflect(-V, N);
    vec3 environmentColor = texture(environmentMap, R).rgb;
    
    // Sample prefiltered environment map
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, material.roughness * MAX_REFLECTION_LOD).rgb;
    
    // Sample BRDF lookup texture
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), material.roughness)).rg;
    
    // Calculate Fresnel
    vec3 F0 = calculateF0(material.albedo, material.metallic);
    vec3 F = calculateFresnelSchlickRoughness(max(dot(N, V), 0.0), F0, material.roughness);
    
    // Calculate specular IBL
    vec3 specularIBL = prefilteredColor * (F * brdf.x + brdf.y);
    
    // Calculate diffuse IBL
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - material.metallic;
    vec3 diffuseIBL = environmentColor * kD;
    
    return diffuseIBL + specularIBL;
}

// Ambient Lighting
vec3 calculateAmbientLighting(Material material, vec3 ambientColor, vec3 N, vec3 V)
{
    // Simple ambient lighting
    vec3 ambient = material.albedo * ambientColor * material.ao;
    
    // Add ambient specular (environment reflection)
    vec3 F0 = calculateF0(material.albedo, material.metallic);
    vec3 F = calculateFresnelSchlick(max(dot(N, V), 0.0), F0);
    ambient += F * material.roughness * 0.1; // Small ambient specular contribution
    
    return ambient;
}

// Rim Lighting
vec3 calculateRimLighting(Material material, vec3 N, vec3 V, vec3 lightColor, float rimPower)
{
    float rim = 1.0 - max(dot(N, V), 0.0);
    rim = pow(rim, rimPower);
    rim *= (1.0 - material.roughness); // Rougher surfaces have less rim lighting
    
    return lightColor * rim * material.albedo;
}

// Subsurface Scattering (simplified)
vec3 calculateSubsurfaceScattering(Material material, vec3 N, vec3 L, vec3 lightColor, float thickness)
{
    // Simplified subsurface scattering
    float wrap = 0.5; // Wrap lighting term
    float NdotL = dot(N, L) * (1.0 - wrap) + wrap;
    NdotL = max(NdotL, 0.0);
    
    vec3 sssColor = material.albedo * lightColor * NdotL;
    sssColor *= thickness; // Thickness affects scattering
    
    return sssColor;
}

// Shadow Calculations
float calculateShadowFactor(sampler2D shadowMap, vec4 shadowCoord, float bias)
{
    // Perform perspective divide
    vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Check if in shadow map bounds
    if (projCoords.x < 0.0 || projCoords.x > 1.0 || 
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0)
    {
        return 1.0; // Outside shadow map
    }
    
    // Sample shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    // Check if in shadow
    float shadow = currentDepth - bias > closestDepth ? 0.0 : 1.0;
    
    return shadow;
}

// PCF (Percentage Closer Filtering) Shadow
float calculatePCFShadow(sampler2D shadowMap, vec4 shadowCoord, float bias, vec2 texelSize)
{
    // Perform perspective divide
    vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Check if in shadow map bounds
    if (projCoords.x < 0.0 || projCoords.x > 1.0 || 
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0)
    {
        return 1.0; // Outside shadow map
    }
    
    // PCF filtering
    float shadow = 0.0;
    int samples = 4;
    vec2 offsets[4] = vec2[](
        vec2(-0.5, -0.5), vec2(0.5, -0.5),
        vec2(-0.5, 0.5), vec2(0.5, 0.5)
    );
    
    for (int i = 0; i < samples; i++)
    {
        vec2 offset = offsets[i] * texelSize;
        float closestDepth = texture(shadowMap, projCoords.xy + offset).r;
        float currentDepth = projCoords.z;
        shadow += currentDepth - bias > closestDepth ? 0.0 : 1.0;
    }
    
    shadow /= float(samples);
    return shadow;
}

// Cascaded Shadow Maps
float calculateCascadedShadow(sampler2D shadowMaps[4], vec4 shadowCoords[4], float bias, float cascadeSplits[4])
{
    float shadow = 1.0;
    
    for (int i = 0; i < 4; i++)
    {
        if (shadowCoords[i].w <= cascadeSplits[i])
        {
            shadow = calculateShadowFactor(shadowMaps[i], shadowCoords[i], bias);
            break;
        }
    }
    
    return shadow;
}

// Volumetric Lighting (simplified)
vec3 calculateVolumetricLighting(Light light, vec3 rayStart, vec3 rayEnd, float density)
{
    // Simplified volumetric lighting calculation
    vec3 lightDir = normalize(light.direction);
    float distance = length(rayEnd - rayStart);
    
    // Calculate light contribution along the ray
    float stepSize = distance / 10.0; // 10 samples
    vec3 volumetricColor = vec3(0.0);
    
    for (int i = 0; i < 10; i++)
    {
        float t = float(i) * stepSize;
        vec3 samplePos = rayStart + lightDir * t;
        
        // Simple light attenuation
        float attenuation = exp(-density * t);
        volumetricColor += light.color * light.intensity * attenuation * stepSize;
    }
    
    return volumetricColor * density;
}

// Tone Mapping
vec3 toneMap(vec3 color, float exposure)
{
    // Simple exposure tone mapping
    return vec3(1.0) - exp(-color * exposure);
}

vec3 toneMapACES(vec3 color)
{
    // ACES tone mapping
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

vec3 toneMapReinhard(vec3 color)
{
    // Reinhard tone mapping
    return color / (1.0 + color);
}

// Gamma Correction
vec3 gammaCorrect(vec3 color, float gamma)
{
    return pow(color, vec3(1.0 / gamma));
}

// Color Grading
vec3 colorGrade(vec3 color, float contrast, float brightness, float saturation)
{
    // Apply contrast
    color = (color - 0.5) * contrast + 0.5;
    
    // Apply brightness
    color += brightness;
    
    // Apply saturation
    float luminance = dot(color, vec3(0.299, 0.587, 0.114));
    vec3 grayscale = vec3(luminance);
    color = mix(grayscale, color, saturation);
    
    return clamp(color, 0.0, 1.0);
}

// Film Grain
vec3 addFilmGrain(vec3 color, vec2 uv, float time, float strength)
{
    float noise = fract(sin(dot(uv.xy + time, vec2(12.9898, 78.233))) * 43758.5453);
    return color + (noise - 0.5) * strength;
}

// Vignette
vec3 applyVignette(vec3 color, vec2 uv, float strength, float radius)
{
    vec2 center = vec2(0.5, 0.5);
    float dist = length(uv - center);
    float vignette = 1.0 - smoothstep(radius, radius + strength, dist);
    return color * vignette;
}

// Utility Functions for Material Creation
Material createMaterial(vec3 albedo, float metallic, float roughness, float ao)
{
    Material mat;
    mat.albedo = albedo;
    mat.metallic = metallic;
    mat.roughness = roughness;
    mat.ao = ao;
    mat.emission = vec3(0.0);
    mat.alpha = 1.0;
    mat.normal = vec3(0.0, 0.0, 1.0);
    mat.f0 = calculateF0(albedo, metallic);
    return mat;
}

Material createMaterialWithEmission(vec3 albedo, float metallic, float roughness, float ao, vec3 emission)
{
    Material mat = createMaterial(albedo, metallic, roughness, ao);
    mat.emission = emission;
    return mat;
}

Material createMaterialWithAlpha(vec3 albedo, float metallic, float roughness, float ao, float alpha)
{
    Material mat = createMaterial(albedo, metallic, roughness, ao);
    mat.alpha = alpha;
    return mat;
}

// Utility Functions for Light Creation
Light createDirectionalLight(vec3 direction, vec3 color, float intensity)
{
    Light light;
    light.type = LIGHT_TYPE_DIRECTIONAL;
    light.position = vec3(0.0);
    light.direction = normalize(direction);
    light.color = color;
    light.intensity = intensity;
    light.range = 0.0;
    light.innerCone = 0.0;
    light.outerCone = 0.0;
    light.castShadow = false;
    light.shadowMapIndex = -1;
    light.shadowMatrix = mat4(1.0);
    return light;
}

Light createPointLight(vec3 position, vec3 color, float intensity, float range)
{
    Light light;
    light.type = LIGHT_TYPE_POINT;
    light.position = position;
    light.direction = vec3(0.0);
    light.color = color;
    light.intensity = intensity;
    light.range = range;
    light.innerCone = 0.0;
    light.outerCone = 0.0;
    light.castShadow = false;
    light.shadowMapIndex = -1;
    light.shadowMatrix = mat4(1.0);
    return light;
}

Light createSpotLight(vec3 position, vec3 direction, vec3 color, float intensity, float range, float innerCone, float outerCone)
{
    Light light;
    light.type = LIGHT_TYPE_SPOT;
    light.position = position;
    light.direction = normalize(direction);
    light.color = color;
    light.intensity = intensity;
    light.range = range;
    light.innerCone = cos(innerCone);
    light.outerCone = cos(outerCone);
    light.castShadow = false;
    light.shadowMapIndex = -1;
    light.shadowMatrix = mat4(1.0);
    return light;
}

Light createAreaLight(vec3 position, vec3 direction, vec3 color, float intensity, float range)
{
    Light light;
    light.type = LIGHT_TYPE_AREA;
    light.position = position;
    light.direction = normalize(direction);
    light.color = color;
    light.intensity = intensity;
    light.range = range;
    light.innerCone = 0.0;
    light.outerCone = 0.0;
    light.castShadow = false;
    light.shadowMapIndex = -1;
    light.shadowMatrix = mat4(1.0);
    return light;
}