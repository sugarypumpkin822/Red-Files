#version 330 core

// Distortion Fragment Shader for OpenGL
// Applies various screen distortion and warping effects

// Input textures
uniform sampler2D uTexture;           // Source texture
uniform sampler2D uDistortionMap;      // Distortion map texture
uniform sampler2D uNoiseTexture;       // Noise texture for procedural distortion
uniform sampler2D uMaskTexture;        // Mask texture for selective distortion
uniform vec2 uResolution;             // Screen resolution
uniform float uTime;                  // Animation time
uniform float uStrength;              // Distortion strength
uniform int uDistortionType;          // Type of distortion (0=radial, 1=wave, 2=swirl, 3=bubble, 4=heat, 5=procedural)
uniform vec2 uCenter;                 // Distortion center
uniform float uRadius;                // Distortion radius
uniform vec2 uFrequency;              // Wave frequency
uniform vec2 uAmplitude;              // Wave amplitude
uniform float uPhase;                 // Wave phase
uniform bool uAnimate;                // Animate the distortion
uniform float uAnimationSpeed;        // Animation speed
uniform bool uUseDistortionMap;       // Use distortion map
uniform float uDistortionScale;       // Distortion map scale
uniform bool uRadialDistortion;       // Apply radial distortion
uniform float uBarrelDistortion;      // Barrel distortion amount
uniform float uPincushionDistortion;  // Pincushion distortion amount
uniform bool uChromaticDistortion;    // Apply chromatic distortion
uniform vec3 uChromaticShift;         // RGB shift amounts
uniform bool uLensDistortion;         // Apply lens distortion
uniform float uFocalLength;           // Lens focal length
uniform float uAperture;              // Lens aperture
uniform bool uFishEye;                // Fish-eye effect
uniform float uFishEyeStrength;      // Fish-eye strength
uniform bool uRipple;                 // Ripple effect
uniform vec2 uRippleCenter;           // Ripple center
uniform float uRippleRadius;          // Ripple radius
uniform float uRippleFrequency;       // Ripple frequency
uniform float uRippleAmplitude;       // Ripple amplitude
uniform bool uTwirl;                  // Twirl effect
uniform vec2 uTwirlCenter;            // Twirl center
uniform float uTwirlRadius;           // Twirl radius
uniform float uTwirlAngle;            // Twirl angle
uniform bool uSpherize;               // Spherize effect
uniform vec2 uSpherizeCenter;         // Spherize center
uniform float uSpherizeRadius;        // Spherize radius
uniform float uSpherizeStrength;      // Spherize strength
uniform bool uPolar;                  // Polar coordinates effect
uniform bool uCylindrical;            // Cylindrical projection
uniform bool uTunnel;                 // Tunnel effect
uniform float uTunnelDepth;           // Tunnel depth
uniform bool uKaleidoscope;           // Kaleidoscope effect
uniform int uKaleidoscopeSegments;     // Number of segments
uniform bool uPixelate;                // Pixelate effect
uniform float uPixelSize;             // Pixel size
uniform bool uGlitch;                 // Glitch effect
uniform float uGlitchIntensity;       // Glitch intensity
uniform bool uShockwave;              // Shockwave effect
uniform vec2 uShockwaveCenter;        // Shockwave center
uniform float uShockwaveRadius;       // Shockwave radius
uniform float uShockwaveStrength;     // Shockwave strength
uniform bool uHeatHaze;               // Heat haze effect
uniform float uHeatIntensity;         // Heat intensity
uniform vec2 uHeatDirection;          // Heat direction

// Input from vertex shader
in vec2 vTexCoord;
in vec2 vScreenPos;

// Output color
out vec4 fragColor;

// Include common functions
#include "common/common.glsl"
#include "common/noise.glsl"

// Radial distortion
vec2 radialDistortion(vec2 uv, vec2 center, float strength, float radius)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return uv;
    
    direction = normalize(direction);
    
    // Apply distortion based on distance from center
    float distortion = 1.0;
    if (distance < radius)
    {
        float normalizedDist = distance / radius;
        distortion = 1.0 + strength * (1.0 - normalizedDist) * (1.0 - normalizedDist);
    }
    
    return center + direction * distance * distortion;
}

// Barrel/Pincushion distortion
vec2 lensDistortion(vec2 uv, vec2 center, float barrel, float pincushion)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return uv;
    
    direction = normalize(direction);
    
    // Lens distortion formula
    float r2 = distance * distance;
    float r4 = r2 * r2;
    float distortion = 1.0 + barrel * r2 + pincushion * r4;
    
    return center + direction * distance * distortion;
}

// Wave distortion
vec2 waveDistortion(vec2 uv, vec2 frequency, vec2 amplitude, float phase, float time)
{
    vec2 distorted = uv;
    
    // Horizontal wave
    distorted.x += sin(uv.y * frequency.x + phase + time) * amplitude.x;
    
    // Vertical wave
    distorted.y += sin(uv.x * frequency.y + phase + time) * amplitude.y;
    
    return distorted;
}

// Swirl distortion
vec2 swirlDistortion(vec2 uv, vec2 center, float radius, float angle, float strength)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return uv;
    
    // Calculate swirl amount based on distance
    float swirlAmount = 0.0;
    if (distance < radius)
    {
        float normalizedDist = distance / radius;
        swirlAmount = angle * strength * (1.0 - normalizedDist);
    }
    
    // Apply rotation
    float cosAngle = cos(swirlAmount);
    float sinAngle = sin(swirlAmount);
    
    vec2 rotated;
    rotated.x = direction.x * cosAngle - direction.y * sinAngle;
    rotated.y = direction.x * sinAngle + direction.y * cosAngle;
    
    return center + rotated;
}

// Bubble distortion
vec2 bubbleDistortion(vec2 uv, vec2 center, float radius, float strength)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return uv;
    
    direction = normalize(direction);
    
    // Bubble distortion formula
    float distortion = 1.0;
    if (distance < radius)
    {
        float normalizedDist = distance / radius;
        distortion = 1.0 + strength * sin(normalizedDist * PI) * (1.0 - normalizedDist);
    }
    
    return center + direction * distance * distortion;
}

// Heat haze distortion
vec2 heatHazeDistortion(vec2 uv, sampler2D noiseTex, float time, float intensity, vec2 direction)
{
    vec2 noiseUV = uv * 2.0 + time * 0.1;
    float noise = texture(noiseTex, noiseUV).r;
    
    // Apply heat distortion
    vec2 distorted = uv;
    distorted += direction * noise * intensity * 0.01;
    
    // Add temporal variation
    distorted += vec2(sin(time * 2.0), cos(time * 3.0)) * noise * intensity * 0.005;
    
    return distorted;
}

// Procedural distortion using noise
vec2 proceduralDistortion(vec2 uv, sampler2D noiseTex, float time, float strength)
{
    vec2 noiseUV = uv * 4.0 + time * 0.05;
    vec2 noise = texture(noiseTex, noiseUV).rg;
    
    // Convert noise to [-1, 1] range
    noise = noise * 2.0 - 1.0;
    
    // Apply distortion
    return uv + noise * strength * 0.02;
}

// Fish-eye distortion
vec2 fishEyeDistortion(vec2 uv, vec2 center, float strength)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return uv;
    
    direction = normalize(direction);
    
    // Fish-eye formula
    float distortion = 1.0 + strength * distance * distance;
    
    return center + direction * distance * distortion;
}

// Ripple distortion
vec2 rippleDistortion(vec2 uv, vec2 center, float radius, float frequency, float amplitude, float time)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return uv;
    
    direction = normalize(direction);
    
    // Calculate ripple
    float ripple = 0.0;
    if (distance < radius)
    {
        ripple = sin(distance * frequency - time * 2.0) * amplitude;
        ripple *= (1.0 - distance / radius); // Fade out at edges
    }
    
    return center + direction * (distance + ripple);
}

// Twirl distortion
vec2 twirlDistortion(vec2 uv, vec2 center, float radius, float angle, float time)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return uv;
    
    // Calculate twirl amount
    float twirlAmount = 0.0;
    if (distance < radius)
    {
        float normalizedDist = distance / radius;
        twirlAmount = angle * (1.0 - normalizedDist) + time * 0.5;
    }
    
    // Apply rotation
    float cosAngle = cos(twirlAmount);
    float sinAngle = sin(twirlAmount);
    
    vec2 rotated;
    rotated.x = direction.x * cosAngle - direction.y * sinAngle;
    rotated.y = direction.x * sinAngle + direction.y * cosAngle;
    
    return center + rotated;
}

// Spherize distortion
vec2 spherizeDistortion(vec2 uv, vec2 center, float radius, float strength)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return uv;
    
    direction = normalize(direction);
    
    // Spherize formula
    float distortion = 1.0;
    if (distance < radius)
    {
        float normalizedDist = distance / radius;
        float sphereFactor = sqrt(1.0 - normalizedDist * normalizedDist);
        distortion = 1.0 + strength * (1.0 - sphereFactor);
    }
    
    return center + direction * distance * distortion;
}

// Polar coordinates distortion
vec2 polarDistortion(vec2 uv, vec2 center)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    float angle = atan(direction.y, direction.x);
    
    // Map to polar coordinates
    vec2 polar;
    polar.x = distance / (center.x * 2.0); // Normalize radius
    polar.y = angle / TWO_PI + 0.5;        // Normalize angle
    
    return polar;
}

// Cylindrical projection
vec2 cylindricalDistortion(vec2 uv, float strength)
{
    // Convert to cylindrical coordinates
    float theta = uv.x * TWO_PI;
    float y = uv.y;
    
    // Apply cylindrical distortion
    float x = sin(theta) * strength;
    float z = cos(theta) * strength;
    
    // Project back to 2D
    vec2 projected;
    projected.x = (x / (z + 1.0)) * 0.5 + 0.5;
    projected.y = y;
    
    return projected;
}

// Tunnel effect
vec2 tunnelDistortion(vec2 uv, float depth, float time)
{
    vec2 center = vec2(0.5, 0.5);
    vec2 direction = uv - center;
    float distance = length(direction);
    float angle = atan(direction.y, direction.x);
    
    // Tunnel coordinates
    vec2 tunnel;
    tunnel.x = 1.0 / (distance + 0.1) * depth; // Depth based on distance
    tunnel.y = angle / TWO_PI + 0.5;            // Angle
    
    // Add rotation
    tunnel.x += time * 0.1;
    
    return tunnel;
}

// Kaleidoscope effect
vec2 kaleidoscopeDistortion(vec2 uv, vec2 center, int segments, float time)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    float angle = atan(direction.y, direction.x);
    
    // Calculate segment angle
    float segmentAngle = TWO_PI / float(segments);
    
    // Mirror within segment
    float segmentIndex = floor(angle / segmentAngle);
    float segmentAngleOffset = segmentIndex * segmentAngle;
    float localAngle = angle - segmentAngleOffset;
    
    // Mirror every other segment
    if (int(segmentIndex) % 2 == 1)
    {
        localAngle = segmentAngle - localAngle;
    }
    
    // Reconstruct position
    vec2 kaleidoscope;
    kaleidoscope.x = cos(localAngle + segmentAngleOffset + time * 0.1) * distance;
    kaleidoscope.y = sin(localAngle + segmentAngleOffset + time * 0.1) * distance;
    
    return center + kaleidoscope;
}

// Pixelate effect
vec2 pixelateDistortion(vec2 uv, float pixelSize)
{
    vec2 pixelated = floor(uv / pixelSize) * pixelSize;
    return pixelated;
}

// Glitch effect
vec2 glitchDistortion(vec2 uv, float intensity, float time)
{
    vec2 distorted = uv;
    
    // Random glitch blocks
    float glitch = random(vec3(uv * 10.0, time));
    if (glitch < intensity)
    {
        // Random offset
        distorted.x += random(vec3(uv * 20.0, time + 1.0)) * 0.1;
        distorted.y += random(vec3(uv * 30.0, time + 2.0)) * 0.1;
    }
    
    // Color channel separation
    if (random(vec3(uv * 5.0, time + 3.0)) < intensity * 0.5)
    {
        distorted.x += sin(time * 10.0) * 0.02;
    }
    
    return distorted;
}

// Shockwave effect
vec2 shockwaveDistortion(vec2 uv, vec2 center, float radius, float strength, float time)
{
    vec2 direction = uv - center;
    float distance = length(direction);
    
    if (distance < 0.001) return uv;
    
    direction = normalize(direction);
    
    // Calculate shockwave
    float shockwave = 0.0;
    float waveRadius = mod(time * 2.0, radius * 2.0) - radius;
    
    float waveDistance = abs(distance - waveRadius);
    if (waveDistance < 0.1)
    {
        shockwave = strength * (1.0 - waveDistance / 0.1);
    }
    
    return uv + direction * shockwave;
}

// Chromatic distortion
vec3 chromaticDistortion(sampler2D tex, vec2 uv, vec3 shift)
{
    vec3 color;
    
    // Sample RGB channels at different positions
    color.r = texture(tex, uv + vec2(shift.r, 0.0)).r;
    color.g = texture(tex, uv + vec2(0.0, shift.g)).g;
    color.b = texture(tex, uv + vec2(shift.b, 0.0)).b;
    
    return color;
}

// Main distortion function
vec2 applyDistortion(vec2 uv)
{
    vec2 distorted = uv;
    
    switch (uDistortionType)
    {
        case 0: // Radial
            distorted = radialDistortion(uv, uCenter, uStrength, uRadius);
            break;
        case 1: // Wave
            float phase = uPhase;
            if (uAnimate) phase += uTime * uAnimationSpeed;
            distorted = waveDistortion(uv, uFrequency, uAmplitude, phase, uTime);
            break;
        case 2: // Swirl
            distorted = swirlDistortion(uv, uCenter, uRadius, uTime, uStrength);
            break;
        case 3: // Bubble
            distorted = bubbleDistortion(uv, uCenter, uRadius, uStrength);
            break;
        case 4: // Heat haze
            distorted = heatHazeDistortion(uv, uNoiseTexture, uTime, uHeatIntensity, uHeatDirection);
            break;
        case 5: // Procedural
            distorted = proceduralDistortion(uv, uNoiseTexture, uTime, uStrength);
            break;
    }
    
    // Apply additional distortion types
    if (uRadialDistortion)
    {
        distorted = radialDistortion(distorted, uCenter, uStrength, uRadius);
    }
    
    if (uBarrelDistortion != 0.0 || uPincushionDistortion != 0.0)
    {
        distorted = lensDistortion(distorted, uCenter, uBarrelDistortion, uPincushionDistortion);
    }
    
    if (uFishEye)
    {
        distorted = fishEyeDistortion(distorted, uCenter, uFishEyeStrength);
    }
    
    if (uRipple)
    {
        distorted = rippleDistortion(distorted, uRippleCenter, uRippleRadius, uRippleFrequency, uRippleAmplitude, uTime);
    }
    
    if (uTwirl)
    {
        distorted = twirlDistortion(distorted, uTwirlCenter, uTwirlRadius, uTwirlAngle, uTime);
    }
    
    if (uSpherize)
    {
        distorted = spherizeDistortion(distorted, uSpherizeCenter, uSpherizeRadius, uSpherizeStrength);
    }
    
    if (uPolar)
    {
        distorted = polarDistortion(distorted, uCenter);
    }
    
    if (uCylindrical)
    {
        distorted = cylindricalDistortion(distorted, uStrength);
    }
    
    if (uTunnel)
    {
        distorted = tunnelDistortion(distorted, uTunnelDepth, uTime);
    }
    
    if (uKaleidoscope)
    {
        distorted = kaleidoscopeDistortion(distorted, uCenter, uKaleidoscopeSegments, uTime);
    }
    
    if (uPixelate)
    {
        distorted = pixelateDistortion(distorted, uPixelSize);
    }
    
    if (uGlitch)
    {
        distorted = glitchDistortion(distorted, uGlitchIntensity, uTime);
    }
    
    if (uShockwave)
    {
        distorted = shockwaveDistortion(distorted, uShockwaveCenter, uShockwaveRadius, uShockwaveStrength, uTime);
    }
    
    // Apply distortion map if available
    if (uUseDistortionMap && uDistortionMap != sampler2D(0))
    {
        vec2 mapUV = uv * uDistortionScale;
        vec4 distortionData = texture(uDistortionMap, mapUV);
        distorted += (distortionData.rg - 0.5) * uStrength * 0.1;
    }
    
    return distorted;
}

// Main function
void main()
{
    vec2 distortedUV = applyDistortion(vTexCoord);
    
    // Clamp UV coordinates
    distortedUV = clamp(distortedUV, 0.0, 1.0);
    
    vec3 color;
    
    // Apply chromatic distortion if enabled
    if (uChromaticDistortion)
    {
        color = chromaticDistortion(uTexture, distortedUV, uChromaticShift * uStrength * 0.01);
    }
    else
    {
        color = texture(uTexture, distortedUV).rgb;
    }
    
    // Apply mask if available
    if (uMaskTexture != sampler2D(0))
    {
        float mask = texture(uMaskTexture, vTexCoord).r;
        vec3 originalColor = texture(uTexture, vTexCoord).rgb;
        color = mix(originalColor, color, mask);
    }
    
    // Ensure color is in valid range
    color = saturate(color);
    
    fragColor = vec4(color, 1.0);
}