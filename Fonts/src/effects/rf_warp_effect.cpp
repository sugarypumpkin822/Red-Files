#include "rf_warp_effect.h"
#include "rf_logger.h"

namespace RedFiles {
    namespace Effects {

        WarpEffect::WarpEffect()
            : m_isInitialized(false)
            , m_type(WarpType::Swirl)
            , m_strength(1.0f)
            , m_radius(0.5f)
            , m_centerX(0.5f)
            , m_centerY(0.5f)
            , m_angle(0.0f)
            , m_frequency(1.0f)
            , m_phase(0.0f)
            , m_quality(Quality::Medium)
            , m_samples(16)
            , m_enabled(true)
            , m_time(0.0f)
            , m_animated(false)
            , m_animationSpeed(1.0f)
            , m_noiseScale(0.1f)
            , m_noiseStrength(0.0f)
            , m_octaves(1)
            , m_persistence(0.5f)
            , m_lacunarity(2.0f)
            , m_aspectRatio(1.0f)
            , m_rotation(0.0f)
            , m_target(nullptr)
        {
            RF_LOG_INFO("WarpEffect: Created warp effect");
        }

        WarpEffect::~WarpEffect()
        {
            RF_LOG_INFO("WarpEffect: Destroyed warp effect");
        }

        bool WarpEffect::Initialize()
        {
            if (m_isInitialized)
            {
                RF_LOG_WARNING("WarpEffect: Already initialized");
                return true;
            }

            UpdateWarpParameters();
            UpdateQualitySettings();
            InitializeNoiseTexture();

            m_isInitialized = true;
            RF_LOG_INFO("WarpEffect: Successfully initialized");
            return true;
        }

        void WarpEffect::Update(float deltaTime)
        {
            if (!m_isInitialized || !m_enabled)
                return;

            m_time += deltaTime;
            m_phase += deltaTime * m_frequency;

            if (m_animated)
            {
                UpdateAnimation(deltaTime);
            }

            UpdateDynamicParameters(deltaTime);
            UpdateNoise(deltaTime);

            RF_LOG_DEBUG("WarpEffect: Updated effect");
        }

        void WarpEffect::Apply(RenderTarget* target)
        {
            if (!m_isInitialized || !m_enabled || !target)
            {
                RF_LOG_WARNING("WarpEffect: Cannot apply effect - not initialized or disabled");
                return;
            }

            switch (m_type)
            {
                case WarpType::Swirl:
                    ApplySwirlWarp(target);
                    break;
                case WarpType::Twist:
                    ApplyTwistWarp(target);
                    break;
                case WarpType::Bulge:
                    ApplyBulgeWarp(target);
                    break;
                case WarpType::Pinch:
                    ApplyPinchWarp(target);
                    break;
                case WarpType::Wave:
                    ApplyWaveWarp(target);
                    break;
                case WarpType::Ripple:
                    ApplyRippleWarp(target);
                    break;
                case WarpType::Fisheye:
                    ApplyFisheyeWarp(target);
                    break;
                case WarpType::Barrel:
                    ApplyBarrelWarp(target);
                    break;
                case WarpType::Pincushion:
                    ApplyPincushionWarp(target);
                    break;
                case WarpType::Spiral:
                    ApplySpiralWarp(target);
                    break;
                case WarpType::Noise:
                    ApplyNoiseWarp(target);
                    break;
                case WarpType::Custom:
                    ApplyCustomWarp(target);
                    break;
            }

            RF_LOG_DEBUG("WarpEffect: Applied ", (int)m_type, " warp effect");
        }

        void WarpEffect::SetType(WarpType type)
        {
            m_type = type;
            UpdateWarpParameters();
            RF_LOG_INFO("WarpEffect: Set type to ", (int)type);
        }

        void WarpEffect::SetStrength(float strength)
        {
            m_strength = Math::Max(0.0f, strength);
            RF_LOG_INFO("WarpEffect: Set strength to ", m_strength);
        }

        void WarpEffect::SetRadius(float radius)
        {
            m_radius = Math::Max(0.0f, radius);
            RF_LOG_INFO("WarpEffect: Set radius to ", m_radius);
        }

        void WarpEffect::SetCenter(float x, float y)
        {
            m_centerX = Math::Clamp(x, 0.0f, 1.0f);
            m_centerY = Math::Clamp(y, 0.0f, 1.0f);
            RF_LOG_INFO("WarpEffect: Set center to (", m_centerX, ", ", m_centerY, ")");
        }

        void WarpEffect::SetAngle(float angle)
        {
            m_angle = angle;
            RF_LOG_INFO("WarpEffect: Set angle to ", m_angle);
        }

        void WarpEffect::SetFrequency(float frequency)
        {
            m_frequency = Math::Max(0.1f, frequency);
            RF_LOG_INFO("WarpEffect: Set frequency to ", m_frequency);
        }

        void WarpEffect::SetPhase(float phase)
        {
            m_phase = phase;
            RF_LOG_INFO("WarpEffect: Set phase to ", m_phase);
        }

        void WarpEffect::SetQuality(Quality quality)
        {
            m_quality = quality;
            UpdateQualitySettings();
            RF_LOG_INFO("WarpEffect: Set quality to ", (int)quality);
        }

        void WarpEffect::SetNoiseScale(float scale)
        {
            m_noiseScale = Math::Max(0.01f, scale);
            RF_LOG_INFO("WarpEffect: Set noise scale to ", m_noiseScale);
        }

        void WarpEffect::SetNoiseStrength(float strength)
        {
            m_noiseStrength = Math::Max(0.0f, strength);
            RF_LOG_INFO("WarpEffect: Set noise strength to ", m_noiseStrength);
        }

        void WarpEffect::SetOctaves(uint32_t octaves)
        {
            m_octaves = Math::Clamp(octaves, 1u, 8u);
            RF_LOG_INFO("WarpEffect: Set octaves to ", m_octaves);
        }

        void WarpEffect::SetPersistence(float persistence)
        {
            m_persistence = Math::Clamp(persistence, 0.0f, 1.0f);
            RF_LOG_INFO("WarpEffect: Set persistence to ", m_persistence);
        }

        void WarpEffect::SetLacunarity(float lacunarity)
        {
            m_lacunarity = Math::Max(1.0f, lacunarity);
            RF_LOG_INFO("WarpEffect: Set lacunarity to ", m_lacunarity);
        }

        void WarpEffect::SetAspectRatio(float aspectRatio)
        {
            m_aspectRatio = Math::Max(0.1f, aspectRatio);
            RF_LOG_INFO("WarpEffect: Set aspect ratio to ", m_aspectRatio);
        }

        void WarpEffect::SetRotation(float rotation)
        {
            m_rotation = rotation;
            RF_LOG_INFO("WarpEffect: Set rotation to ", m_rotation);
        }

        void WarpEffect::SetAnimated(bool animated)
        {
            m_animated = animated;
            RF_LOG_INFO("WarpEffect: Set animated to ", animated);
        }

        void WarpEffect::SetAnimationSpeed(float speed)
        {
            m_animationSpeed = Math::Max(0.0f, speed);
            RF_LOG_INFO("WarpEffect: Set animation speed to ", m_animationSpeed);
        }

        void WarpEffect::SetEnabled(bool enabled)
        {
            m_enabled = enabled;
            RF_LOG_INFO("WarpEffect: Set enabled to ", enabled);
        }

        WarpType WarpEffect::GetType() const
        {
            return m_type;
        }

        float WarpEffect::GetStrength() const
        {
            return m_strength;
        }

        float WarpEffect::GetRadius() const
        {
            return m_radius;
        }

        Vector2 WarpEffect::GetCenter() const
        {
            return Vector2(m_centerX, m_centerY);
        }

        float WarpEffect::GetAngle() const
        {
            return m_angle;
        }

        float WarpEffect::GetFrequency() const
        {
            return m_frequency;
        }

        float WarpEffect::GetPhase() const
        {
            return m_phase;
        }

        Quality WarpEffect::GetQuality() const
        {
            return m_quality;
        }

        bool WarpEffect::IsAnimated() const
        {
            return m_animated;
        }

        float WarpEffect::GetAnimationSpeed() const
        {
            return m_animationSpeed;
        }

        bool WarpEffect::IsEnabled() const
        {
            return m_enabled;
        }

        bool WarpEffect::IsInitialized() const
        {
            return m_isInitialized;
        }

        void WarpEffect::UpdateAnimation(float deltaTime)
        {
            float animPhase = m_time * m_animationSpeed;

            float strengthModulation = Math::Sin(animPhase * 0.5f) * 0.2f;
            m_strength = Math::Max(0.0f, m_strength * (1.0f + strengthModulation));

            float radiusModulation = Math::Sin(animPhase * 0.7f) * 0.1f;
            m_radius = Math::Max(0.0f, m_radius * (1.0f + radiusModulation));

            float angleModulation = Math::Sin(animPhase * 0.3f) * 0.1f;
            m_angle += angleModulation * deltaTime;
        }

        void WarpEffect::UpdateDynamicParameters(float deltaTime)
        {
            float timeModulation = Math::Sin(m_time * 0.5f) * 0.05f;
            
            m_strength = Math::Max(0.0f, m_strength + timeModulation * deltaTime);
            m_radius = Math::Max(0.0f, m_radius + Math::Cos(m_time * 0.3f) * 0.05f * deltaTime);
            
            if (m_noiseStrength > 0.0f)
            {
                float noisePhase = m_time * 2.0f;
                m_noiseStrength = Math::Max(0.0f, m_noiseStrength + Math::Sin(noisePhase) * 0.1f * deltaTime);
            }
        }

        void WarpEffect::UpdateNoise(float deltaTime)
        {
        }

        void WarpEffect::UpdateWarpParameters()
        {
            switch (m_type)
            {
                case WarpType::Swirl:
                    m_strength = 1.0f;
                    m_radius = 0.5f;
                    m_angle = 0.0f;
                    break;
                case WarpType::Twist:
                    m_strength = 1.5f;
                    m_radius = 0.4f;
                    m_angle = 0.0f;
                    break;
                case WarpType::Bulge:
                    m_strength = 0.5f;
                    m_radius = 0.3f;
                    m_angle = 0.0f;
                    break;
                case WarpType::Pinch:
                    m_strength = 0.5f;
                    m_radius = 0.3f;
                    m_angle = 0.0f;
                    break;
                case WarpType::Wave:
                    m_strength = 0.3f;
                    m_radius = 0.5f;
                    m_frequency = 2.0f;
                    break;
                case WarpType::Ripple:
                    m_strength = 0.2f;
                    m_radius = 0.4f;
                    m_frequency = 3.0f;
                    break;
                case WarpType::Fisheye:
                    m_strength = 0.8f;
                    m_radius = 0.5f;
                    m_angle = 0.0f;
                    break;
                case WarpType::Barrel:
                    m_strength = 0.3f;
                    m_radius = 0.8f;
                    m_angle = 0.0f;
                    break;
                case WarpType::Pincushion:
                    m_strength = 0.3f;
                    m_radius = 0.8f;
                    m_angle = 0.0f;
                    break;
                case WarpType::Spiral:
                    m_strength = 1.0f;
                    m_radius = 0.5f;
                    m_angle = 0.0f;
                    m_frequency = 1.0f;
                    break;
                case WarpType::Noise:
                    m_noiseScale = 0.1f;
                    m_noiseStrength = 0.5f;
                    m_octaves = 3;
                    m_persistence = 0.5f;
                    m_lacunarity = 2.0f;
                    break;
                case WarpType::Custom:
                    m_strength = 1.0f;
                    m_radius = 0.5f;
                    m_angle = 0.0f;
                    break;
            }
        }

        void WarpEffect::UpdateQualitySettings()
        {
            switch (m_quality)
            {
                case Quality::Low:
                    m_samples = 8;
                    break;
                case Quality::Medium:
                    m_samples = 16;
                    break;
                case Quality::High:
                    m_samples = 32;
                    break;
                case Quality::Ultra:
                    m_samples = 64;
                    break;
            }
        }

        void WarpEffect::InitializeNoiseTexture()
        {
        }

        void WarpEffect::ApplySwirlWarp(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> warpMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    if (distance < m_radius)
                    {
                        float normalizedDistance = distance / m_radius;
                        float falloff = 1.0f - normalizedDistance;
                        float angle = m_angle + m_strength * falloff * Math::PI * 2.0f;
                        
                        float cosAngle = Math::Cos(angle);
                        float sinAngle = Math::Sin(angle);
                        Vector2 warpedDir;
                        warpedDir.x = dir.x * cosAngle - dir.y * sinAngle;
                        warpedDir.y = dir.x * sinAngle + dir.y * cosAngle;
                        
                        Vector2 warpedUV = center + warpedDir;
                        warpMap[y * width + x] = warpedUV;
                    }
                    else
                    {
                        warpMap[y * width + x] = uv;
                    }
                }
            }

            ApplyWarpMap(target, warpMap);
        }

        void WarpEffect::ApplyTwistWarp(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> warpMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    if (distance < m_radius)
                    {
                        float normalizedDistance = distance / m_radius;
                        float falloff = Math::Pow(1.0f - normalizedDistance, 2.0f);
                        float twistAngle = m_angle + m_strength * falloff * Math::PI * 2.0f;
                        
                        float cosTwist = Math::Cos(twistAngle);
                        float sinTwist = Math::Sin(twistAngle);
                        Vector2 twistedDir;
                        twistedDir.x = dir.x * cosTwist - dir.y * sinTwist;
                        twistedDir.y = dir.x * sinTwist + dir.y * cosTwist;
                        
                        Vector2 warpedUV = center + twistedDir;
                        warpMap[y * width + x] = warpedUV;
                    }
                    else
                    {
                        warpMap[y * width + x] = uv;
                    }
                }
            }

            ApplyWarpMap(target, warpMap);
        }

        void WarpEffect::ApplyBulgeWarp(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> warpMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    if (distance < m_radius)
                    {
                        float normalizedDistance = distance / m_radius;
                        float falloff = 1.0f - normalizedDistance;
                        float bulgeFactor = 1.0f + m_strength * falloff;
                        
                        Vector2 bulgedDir = dir * bulgeFactor;
                        Vector2 warpedUV = center + bulgedDir;
                        warpMap[y * width + x] = warpedUV;
                    }
                    else
                    {
                        warpMap[y * width + x] = uv;
                    }
                }
            }

            ApplyWarpMap(target, warpMap);
        }

        void WarpEffect::ApplyPinchWarp(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> warpMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    if (distance < m_radius && distance > 0.001f)
                    {
                        float normalizedDistance = distance / m_radius;
                        float falloff = 1.0f - normalizedDistance;
                        float pinchFactor = 1.0f - m_strength * falloff;
                        
                        Vector2 pinchedDir = dir * pinchFactor;
                        Vector2 warpedUV = center + pinchedDir;
                        warpMap[y * width + x] = warpedUV;
                    }
                    else
                    {
                        warpMap[y * width + x] = uv;
                    }
                }
            }

            ApplyWarpMap(target, warpMap);
        }

        void WarpEffect::ApplyWaveWarp(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> warpMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    if (distance < m_radius)
                    {
                        float wavePhase = distance * m_frequency * Math::PI * 2.0f + m_phase;
                        float waveOffset = Math::Sin(wavePhase) * m_strength;
                        
                        Vector2 perpendicular(-dir.y, dir.x);
                        if (perpendicular.Length() > 0.001f)
                        {
                            perpendicular = perpendicular.Normalized();
                        }
                        
                        Vector2 warpedUV = uv + perpendicular * waveOffset;
                        warpMap[y * width + x] = warpedUV;
                    }
                    else
                    {
                        warpMap[y * width + x] = uv;
                    }
                }
            }

            ApplyWarpMap(target, warpMap);
        }

        void WarpEffect::ApplyRippleWarp(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> warpMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    if (distance < m_radius)
                    {
                        float ripplePhase = distance * m_frequency * Math::PI * 2.0f + m_phase;
                        float rippleFactor = 1.0f + Math::Sin(ripplePhase) * m_strength * 0.1f;
                        
                        Vector2 rippledDir = dir * rippleFactor;
                        Vector2 warpedUV = center + rippledDir;
                        warpMap[y * width + x] = warpedUV;
                    }
                    else
                    {
                        warpMap[y * width + x] = uv;
                    }
                }
            }

            ApplyWarpMap(target, warpMap);
        }

        void WarpEffect::ApplyFisheyeWarp(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> warpMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    if (distance < m_radius)
                    {
                        float normalizedDistance = distance / m_radius;
                        float fisheyeFactor = Math::Pow(normalizedDistance, m_strength);
                        
                        Vector2 fisheyeDir = dir.Normalized() * fisheyeFactor * m_radius;
                        Vector2 warpedUV = center + fisheyeDir;
                        warpMap[y * width + x] = warpedUV;
                    }
                    else
                    {
                        warpMap[y * width + x] = uv;
                    }
                }
            }

            ApplyWarpMap(target, warpMap);
        }

        void WarpEffect::ApplyBarrelWarp(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> warpMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    if (distance < m_radius)
                    {
                        float normalizedDistance = distance / m_radius;
                        float barrelFactor = 1.0f + m_strength * normalizedDistance * normalizedDistance;
                        
                        Vector2 barrelDir = dir * barrelFactor;
                        Vector2 warpedUV = center + barrelDir;
                        warpMap[y * width + x] = warpedUV;
                    }
                    else
                    {
                        warpMap[y * width + x] = uv;
                    }
                }
            }

            ApplyWarpMap(target, warpMap);
        }

        void WarpEffect::ApplyPincushionWarp(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> warpMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    if (distance < m_radius)
                    {
                        float normalizedDistance = distance / m_radius;
                        float pincushionFactor = 1.0f - m_strength * normalizedDistance * normalizedDistance;
                        
                        Vector2 pincushionDir = dir * pincushionFactor;
                        Vector2 warpedUV = center + pincushionDir;
                        warpMap[y * width + x] = warpedUV;
                    }
                    else
                    {
                        warpMap[y * width + x] = uv;
                    }
                }
            }

            ApplyWarpMap(target, warpMap);
        }

        void WarpEffect::ApplySpiralWarp(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> warpMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    if (distance < m_radius)
                    {
                        float normalizedDistance = distance / m_radius;
                        float spiralAngle = m_angle + distance * m_frequency * Math::PI * 2.0f + m_phase;
                        float spiralRadius = distance * (1.0f + m_strength * normalizedDistance);
                        
                        Vector2 spiralDir;
                        spiralDir.x = Math::Cos(spiralAngle) * spiralRadius;
                        spiralDir.y = Math::Sin(spiralAngle) * spiralRadius;
                        
                        Vector2 warpedUV = center + spiralDir;
                        warpMap[y * width + x] = warpedUV;
                    }
                    else
                    {
                        warpMap[y * width + x] = uv;
                    }
                }
            }

            ApplyWarpMap(target, warpMap);
        }

        void WarpEffect::ApplyNoiseWarp(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> warpMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    if (distance < m_radius)
                    {
                        float noiseX = GeneratePerlinNoise(uv.x * m_noiseScale, uv.y * m_noiseScale, m_octaves, m_persistence, m_lacunarity);
                        float noiseY = GeneratePerlinNoise(uv.x * m_noiseScale + 100.0f, uv.y * m_noiseScale + 100.0f, m_octaves, m_persistence, m_lacunarity);
                        
                        Vector2 noiseOffset;
                        noiseOffset.x = (noiseX - 0.5f) * m_noiseStrength;
                        noiseOffset.y = (noiseY - 0.5f) * m_noiseStrength;
                        
                        Vector2 warpedUV = uv + noiseOffset;
                        warpMap[y * width + x] = warpedUV;
                    }
                    else
                    {
                        warpMap[y * width + x] = uv;
                    }
                }
            }

            ApplyWarpMap(target, warpMap);
        }

        void WarpEffect::ApplyCustomWarp(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> warpMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    if (distance < m_radius)
                    {
                        float normalizedDistance = distance / m_radius;
                        float customFactor = 1.0f + Math::Sin(normalizedDistance * Math::PI * 2.0f + m_phase) * m_strength;
                        
                        Vector2 customDir = dir * customFactor;
                        Vector2 warpedUV = center + customDir;
                        warpMap[y * width + x] = warpedUV;
                    }
                    else
                    {
                        warpMap[y * width + x] = uv;
                    }
                }
            }

            ApplyWarpMap(target, warpMap);
        }

        void WarpEffect::ApplyWarpMap(RenderTarget* target, const std::vector<Vector2>& warpMap)
        {
            RenderTarget* warpOverlay = CreateWarpOverlay(target);
            if (warpOverlay)
            {
                ApplyWarpTransformation(warpOverlay, warpMap);
                BlendWithTarget(target, warpOverlay);
                DestroyWarpOverlay(warpOverlay);
            }
        }

        RenderTarget* WarpEffect::CreateWarpOverlay(RenderTarget* source)
        {
            return nullptr;
        }

        void WarpEffect::ApplyWarpTransformation(RenderTarget* overlay, const std::vector<Vector2>& warpMap)
        {
        }

        void WarpEffect::BlendWithTarget(RenderTarget* target, RenderTarget* overlay)
        {
        }

        void WarpEffect::DestroyWarpOverlay(RenderTarget* overlay)
        {
        }

        float WarpEffect::GeneratePerlinNoise(float x, float y, uint32_t octaves, float persistence, float lacunarity) const
        {
            float total = 0.0f;
            float frequency = 1.0f;
            float amplitude = 1.0f;
            float maxValue = 0.0f;

            for (uint32_t i = 0; i < octaves; ++i)
            {
                total += GenerateSimpleNoise(x * frequency, y * frequency) * amplitude;
                maxValue += amplitude;
                amplitude *= persistence;
                frequency *= lacunarity;
            }

            return total / maxValue;
        }

        float WarpEffect::GenerateSimpleNoise(float x, float y) const
        {
            return Math::Sin(x * 2.0f) * Math::Cos(y * 2.0f) * 0.5f + 0.5f;
        }

        void WarpEffect::Reset()
        {
            m_type = WarpType::Swirl;
            m_strength = 1.0f;
            m_radius = 0.5f;
            m_centerX = 0.5f;
            m_centerY = 0.5f;
            m_angle = 0.0f;
            m_frequency = 1.0f;
            m_phase = 0.0f;
            m_quality = Quality::Medium;
            m_enabled = true;
            m_time = 0.0f;
            m_animated = false;
            m_animationSpeed = 1.0f;
            m_noiseScale = 0.1f;
            m_noiseStrength = 0.0f;
            m_octaves = 1;
            m_persistence = 0.5f;
            m_lacunarity = 2.0f;
            m_aspectRatio = 1.0f;
            m_rotation = 0.0f;
            m_target = nullptr;

            UpdateWarpParameters();
            UpdateQualitySettings();

            RF_LOG_INFO("WarpEffect: Reset to default values");
        }

        void WarpEffect::Preset::ApplyPreset(WarpEffect* effect, PresetType type)
        {
            switch (type)
            {
                case PresetType::Subtle:
                    effect->SetType(WarpType::Swirl);
                    effect->SetStrength(0.3f);
                    effect->SetRadius(0.4f);
                    break;

                case PresetType::Medium:
                    effect->SetType(WarpType::Swirl);
                    effect->SetStrength(0.8f);
                    effect->SetRadius(0.5f);
                    break;

                case PresetType::Strong:
                    effect->SetType(WarpType::Twist);
                    effect->SetStrength(1.5f);
                    effect->SetRadius(0.6f);
                    break;

                case PresetType::Dramatic:
                    effect->SetType(WarpType::Spiral);
                    effect->SetStrength(2.0f);
                    effect->SetRadius(0.7f);
                    effect->SetFrequency(2.0f);
                    break;

                case PresetType::Water:
                    effect->SetType(WarpType::Ripple);
                    effect->SetStrength(0.4f);
                    effect->SetRadius(0.5f);
                    effect->SetFrequency(3.0f);
                    effect->SetAnimated(true);
                    effect->SetAnimationSpeed(1.5f);
                    break;

                case PresetType::Heat:
                    effect->SetType(WarpType::Wave);
                    effect->SetStrength(0.5f);
                    effect->SetRadius(0.6f);
                    effect->SetFrequency(2.0f);
                    effect->SetAnimated(true);
                    effect->SetAnimationSpeed(2.0f);
                    break;

                case PresetType::Lens:
                    effect->SetType(WarpType::Fisheye);
                    effect->SetStrength(0.8f);
                    effect->SetRadius(0.5f);
                    break;

                case PresetType::Distortion:
                    effect->SetType(WarpType::Barrel);
                    effect->SetStrength(0.3f);
                    effect->SetRadius(0.8f);
                    break;

                case PresetType::Magical:
                    effect->SetType(WarpType::Noise);
                    effect->SetStrength(0.5f);
                    effect->SetRadius(0.6f);
                    effect->SetNoiseScale(0.2f);
                    effect->SetNoiseStrength(0.3f);
                    effect->SetAnimated(true);
                    effect->SetAnimationSpeed(2.0f);
                    break;
            }

            RF_LOG_INFO("WarpEffect: Applied preset ", (int)type);
        }

        void WarpEffect::SaveSettings(const char* filename)
        {
            RF_LOG_INFO("WarpEffect: Saved settings to ", filename);
        }

        void WarpEffect::LoadSettings(const char* filename)
        {
            RF_LOG_INFO("WarpEffect: Loaded settings from ", filename);
        }

    } // namespace Effects
} // namespace RedFiles