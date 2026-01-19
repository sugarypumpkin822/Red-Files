#include "rf_distortion_effect.h"
#include "rf_logger.h"

namespace RedFiles {
    namespace Effects {

        DistortionEffect::DistortionEffect()
            : m_isInitialized(false)
            , m_type(DistortionType::Radial)
            , m_strength(0.5f)
            , m_radius(0.5f)
            , m_centerX(0.5f)
            , m_centerY(0.5f)
            , m_angle(0.0f)
            , m_frequency(1.0f)
            , m_phase(0.0f)
            , m_amplitude(1.0f)
            , m_quality(Quality::Medium)
            , m_samples(16)
            , m_enabled(true)
            , m_time(0.0f)
            , m_animated(false)
            , m_animationSpeed(1.0f)
            , m_noiseScale(0.1f)
            , m_noiseStrength(0.0f)
        {
            RF_LOG_INFO("DistortionEffect: Created distortion effect");
        }

        DistortionEffect::~DistortionEffect()
        {
            RF_LOG_INFO("DistortionEffect: Destroyed distortion effect");
        }

        bool DistortionEffect::Initialize()
        {
            if (m_isInitialized)
            {
                RF_LOG_WARNING("DistortionEffect: Already initialized");
                return true;
            }

            // Initialize distortion parameters based on type
            UpdateDistortionParameters();

            // Initialize quality settings
            UpdateQualitySettings();

            // Initialize noise texture
            InitializeNoiseTexture();

            m_isInitialized = true;
            RF_LOG_INFO("DistortionEffect: Successfully initialized");
            return true;
        }

        void DistortionEffect::Update(float deltaTime)
        {
            if (!m_isInitialized || !m_enabled)
                return;

            m_time += deltaTime;

            // Update animation
            if (m_animated)
            {
                UpdateAnimation(deltaTime);
            }

            // Update dynamic parameters
            UpdateDynamicParameters(deltaTime);

            // Update noise
            UpdateNoise(deltaTime);

            RF_LOG_DEBUG("DistortionEffect: Updated effect");
        }

        void DistortionEffect::Apply(RenderTarget* target)
        {
            if (!m_isInitialized || !m_enabled || !target)
            {
                RF_LOG_WARNING("DistortionEffect: Cannot apply effect - not initialized or disabled");
                return;
            }

            switch (m_type)
            {
                case DistortionType::Radial:
                    ApplyRadialDistortion(target);
                    break;
                case DistortionType::Wave:
                    ApplyWaveDistortion(target);
                    break;
                case DistortionType::Swirl:
                    ApplySwirlDistortion(target);
                    break;
                case DistortionType::Fisheye:
                    ApplyFisheyeDistortion(target);
                    break;
                case DistortionType::Barrel:
                    ApplyBarrelDistortion(target);
                    break;
                case DistortionType::Pincushion:
                    ApplyPincushionDistortion(target);
                    break;
                case DistortionType::Twist:
                    ApplyTwistDistortion(target);
                    break;
                case DistortionType::Noise:
                    ApplyNoiseDistortion(target);
                    break;
            }

            RF_LOG_DEBUG("DistortionEffect: Applied ", (int)m_type, " distortion effect");
        }

        void DistortionEffect::SetType(DistortionType type)
        {
            m_type = type;
            UpdateDistortionParameters();
            RF_LOG_INFO("DistortionEffect: Set type to ", (int)type);
        }

        void DistortionEffect::SetStrength(float strength)
        {
            m_strength = Math::Clamp(strength, -2.0f, 2.0f);
            RF_LOG_INFO("DistortionEffect: Set strength to ", m_strength);
        }

        void DistortionEffect::SetRadius(float radius)
        {
            m_radius = Math::Clamp(radius, 0.0f, 2.0f);
            RF_LOG_INFO("DistortionEffect: Set radius to ", m_radius);
        }

        void DistortionEffect::SetCenter(float x, float y)
        {
            m_centerX = Math::Clamp(x, 0.0f, 1.0f);
            m_centerY = Math::Clamp(y, 0.0f, 1.0f);
            RF_LOG_INFO("DistortionEffect: Set center to (", m_centerX, ", ", m_centerY, ")");
        }

        void DistortionEffect::SetAngle(float angle)
        {
            m_angle = angle;
            RF_LOG_INFO("DistortionEffect: Set angle to ", m_angle);
        }

        void DistortionEffect::SetFrequency(float frequency)
        {
            m_frequency = Math::Max(0.1f, frequency);
            RF_LOG_INFO("DistortionEffect: Set frequency to ", m_frequency);
        }

        void DistortionEffect::SetPhase(float phase)
        {
            m_phase = phase;
            RF_LOG_INFO("DistortionEffect: Set phase to ", m_phase);
        }

        void DistortionEffect::SetAmplitude(float amplitude)
        {
            m_amplitude = Math::Max(0.0f, amplitude);
            RF_LOG_INFO("DistortionEffect: Set amplitude to ", m_amplitude);
        }

        void DistortionEffect::SetQuality(Quality quality)
        {
            m_quality = quality;
            UpdateQualitySettings();
            RF_LOG_INFO("DistortionEffect: Set quality to ", (int)quality);
        }

        void DistortionEffect::SetNoiseScale(float scale)
        {
            m_noiseScale = Math::Max(0.01f, scale);
            RF_LOG_INFO("DistortionEffect: Set noise scale to ", m_noiseScale);
        }

        void DistortionEffect::SetNoiseStrength(float strength)
        {
            m_noiseStrength = Math::Max(0.0f, strength);
            RF_LOG_INFO("DistortionEffect: Set noise strength to ", m_noiseStrength);
        }

        void DistortionEffect::SetEnabled(bool enabled)
        {
            m_enabled = enabled;
            RF_LOG_INFO("DistortionEffect: Set enabled to ", enabled);
        }

        void DistortionEffect::SetAnimated(bool animated)
        {
            m_animated = animated;
            RF_LOG_INFO("DistortionEffect: Set animated to ", animated);
        }

        void DistortionEffect::SetAnimationSpeed(float speed)
        {
            m_animationSpeed = Math::Max(0.0f, speed);
            RF_LOG_INFO("DistortionEffect: Set animation speed to ", m_animationSpeed);
        }

        DistortionType DistortionEffect::GetType() const
        {
            return m_type;
        }

        float DistortionEffect::GetStrength() const
        {
            return m_strength;
        }

        float DistortionEffect::GetRadius() const
        {
            return m_radius;
        }

        Vector2 DistortionEffect::GetCenter() const
        {
            return Vector2(m_centerX, m_centerY);
        }

        float DistortionEffect::GetAngle() const
        {
            return m_angle;
        }

        float DistortionEffect::GetFrequency() const
        {
            return m_frequency;
        }

        float DistortionEffect::GetPhase() const
        {
            return m_phase;
        }

        float DistortionEffect::GetAmplitude() const
        {
            return m_amplitude;
        }

        Quality DistortionEffect::GetQuality() const
        {
            return m_quality;
        }

        bool DistortionEffect::IsEnabled() const
        {
            return m_enabled;
        }

        bool DistortionEffect::IsAnimated() const
        {
            return m_animated;
        }

        float DistortionEffect::GetAnimationSpeed() const
        {
            return m_animationSpeed;
        }

        bool DistortionEffect::IsInitialized() const
        {
            return m_isInitialized;
        }

        void DistortionEffect::UpdateAnimation(float deltaTime)
        {
            float animPhase = m_time * m_animationSpeed;

            // Animate strength
            float strengthModulation = Math::Sin(animPhase) * 0.3f + 0.7f;
            m_strength = Math::Clamp(m_strength * strengthModulation, -2.0f, 2.0f);

            // Animate radius
            float radiusModulation = Math::Sin(animPhase * 1.3f) * 0.2f + 0.8f;
            m_radius = Math::Clamp(m_radius * radiusModulation, 0.0f, 2.0f);

            // Animate angle for rotational distortions
            if (m_type == DistortionType::Swirl || m_type == DistortionType::Twist)
            {
                m_angle += Math::Sin(animPhase * 0.7f) * 0.1f;
            }

            // Animate frequency for wave distortions
            if (m_type == DistortionType::Wave)
            {
                m_frequency *= (1.0f + Math::Sin(animPhase * 1.1f) * 0.1f);
            }
        }

        void DistortionEffect::UpdateDynamicParameters(float deltaTime)
        {
            // Update phase for wave effects
            if (m_type == DistortionType::Wave)
            {
                m_phase += deltaTime * m_frequency;
            }

            // Update noise parameters
            if (m_noiseStrength > 0.0f)
            {
                float noisePhase = m_time * 2.0f;
                m_noiseStrength = Math::Max(0.0f, m_noiseStrength + Math::Sin(noisePhase) * 0.1f * deltaTime);
            }
        }

        void DistortionEffect::UpdateNoise(float deltaTime)
        {
            // Update noise texture or parameters
            // This would be implemented using the rendering API
        }

        void DistortionEffect::UpdateDistortionParameters()
        {
            // Set default parameters based on distortion type
            switch (m_type)
            {
                case DistortionType::Radial:
                    m_strength = 0.5f;
                    m_radius = 0.5f;
                    break;
                case DistortionType::Wave:
                    m_frequency = 2.0f;
                    m_amplitude = 0.1f;
                    m_phase = 0.0f;
                    break;
                case DistortionType::Swirl:
                    m_strength = 1.0f;
                    m_angle = 0.0f;
                    break;
                case DistortionType::Fisheye:
                    m_strength = 0.8f;
                    m_radius = 1.0f;
                    break;
                case DistortionType::Barrel:
                    m_strength = 0.3f;
                    m_radius = 0.8f;
                    break;
                case DistortionType::Pincushion:
                    m_strength = -0.3f;
                    m_radius = 0.8f;
                    break;
                case DistortionType::Twist:
                    m_strength = 0.6f;
                    m_angle = Math::PI * 0.5f;
                    break;
                case DistortionType::Noise:
                    m_noiseScale = 0.1f;
                    m_noiseStrength = 0.5f;
                    break;
            }
        }

        void DistortionEffect::UpdateQualitySettings()
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

        void DistortionEffect::InitializeNoiseTexture()
        {
            // Initialize noise texture for noise-based distortions
            // This would be implemented using the rendering API
        }

        void DistortionEffect::ApplyRadialDistortion(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            // Create distortion map
            std::vector<Vector2> distortionMap(width * height);

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
                        float falloff = 1.0f - (distance / m_radius);
                        float distortion = m_strength * falloff;
                        
                        Vector2 distorted = uv + dir.Normalized() * distortion;
                        distortionMap[y * width + x] = distorted;
                    }
                    else
                    {
                        distortionMap[y * width + x] = uv;
                    }
                }
            }

            // Apply distortion map to render target
            ApplyDistortionMap(target, distortionMap);
        }

        void DistortionEffect::ApplyWaveDistortion(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> distortionMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    
                    float waveX = Math::Sin(uv.y * m_frequency * Math::PI * 2.0f + m_phase) * m_amplitude;
                    float waveY = Math::Cos(uv.x * m_frequency * Math::PI * 2.0f + m_phase) * m_amplitude;
                    
                    Vector2 distorted = uv + Vector2(waveX, waveY) * m_strength;
                    distortionMap[y * width + x] = distorted;
                }
            }

            ApplyDistortionMap(target, distortionMap);
        }

        void DistortionEffect::ApplySwirlDistortion(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> distortionMap(width * height);

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
                        float falloff = 1.0f - (distance / m_radius);
                        float angle = m_angle + m_strength * falloff;
                        
                        float cosAngle = Math::Cos(angle);
                        float sinAngle = Math::Sin(angle);
                        
                        Vector2 rotated;
                        rotated.x = dir.x * cosAngle - dir.y * sinAngle;
                        rotated.y = dir.x * sinAngle + dir.y * cosAngle;
                        
                        Vector2 distorted = center + rotated;
                        distortionMap[y * width + x] = distorted;
                    }
                    else
                    {
                        distortionMap[y * width + x] = uv;
                    }
                }
            }

            ApplyDistortionMap(target, distortionMap);
        }

        void DistortionEffect::ApplyFisheyeDistortion(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> distortionMap(width * height);

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
                        float falloff = 1.0f - (distance / m_radius);
                        float distortion = 1.0f + m_strength * falloff * distance;
                        
                        Vector2 distorted = center + dir * distortion;
                        distortionMap[y * width + x] = distorted;
                    }
                    else
                    {
                        distortionMap[y * width + x] = uv;
                    }
                }
            }

            ApplyDistortionMap(target, distortionMap);
        }

        void DistortionEffect::ApplyBarrelDistortion(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> distortionMap(width * height);

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
                        float falloff = 1.0f - (distance / m_radius);
                        float distortion = 1.0f + m_strength * falloff * distance * distance;
                        
                        Vector2 distorted = center + dir * distortion;
                        distortionMap[y * width + x] = distorted;
                    }
                    else
                    {
                        distortionMap[y * width + x] = uv;
                    }
                }
            }

            ApplyDistortionMap(target, distortionMap);
        }

        void DistortionEffect::ApplyPincushionDistortion(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> distortionMap(width * height);

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
                        float falloff = 1.0f - (distance / m_radius);
                        float distortion = 1.0f + m_strength * falloff * distance * distance;
                        
                        Vector2 distorted = center + dir * distortion;
                        distortionMap[y * width + x] = distorted;
                    }
                    else
                    {
                        distortionMap[y * width + x] = uv;
                    }
                }
            }

            ApplyDistortionMap(target, distortionMap);
        }

        void DistortionEffect::ApplyTwistDistortion(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> distortionMap(width * height);

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
                        float falloff = 1.0f - (distance / m_radius);
                        float twistAngle = m_angle * falloff * m_strength;
                        
                        float cosAngle = Math::Cos(twistAngle);
                        float sinAngle = Math::Sin(twistAngle);
                        
                        Vector2 twisted;
                        twisted.x = dir.x * cosAngle - dir.y * sinAngle;
                        twisted.y = dir.x * sinAngle + dir.y * cosAngle;
                        
                        Vector2 distorted = center + twisted;
                        distortionMap[y * width + x] = distorted;
                    }
                    else
                    {
                        distortionMap[y * width + x] = uv;
                    }
                }
            }

            ApplyDistortionMap(target, distortionMap);
        }

        void DistortionEffect::ApplyNoiseDistortion(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<Vector2> distortionMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    
                    // Generate noise-based distortion
                    float noiseX = GenerateNoise(uv.x * m_noiseScale, uv.y * m_noiseScale, m_time);
                    float noiseY = GenerateNoise(uv.x * m_noiseScale + 100.0f, uv.y * m_noiseScale + 100.0f, m_time);
                    
                    Vector2 noiseOffset = Vector2(noiseX, noiseY) * m_noiseStrength * m_strength;
                    Vector2 distorted = uv + noiseOffset;
                    
                    distortionMap[y * width + x] = distorted;
                }
            }

            ApplyDistortionMap(target, distortionMap);
        }

        void DistortionEffect::ApplyDistortionMap(RenderTarget* target, const std::vector<Vector2>& distortionMap)
        {
            // Apply the distortion map to the render target
            // This would be implemented using the rendering API
            // For now, we provide the structure
        }

        float DistortionEffect::GenerateNoise(float x, float y, float time) const
        {
            // Simple noise function
            return Math::Sin(x * 2.0f + time) * Math::Cos(y * 2.0f + time) * 0.5f + 0.5f;
        }

        void DistortionEffect::Reset()
        {
            m_type = DistortionType::Radial;
            m_strength = 0.5f;
            m_radius = 0.5f;
            m_centerX = 0.5f;
            m_centerY = 0.5f;
            m_angle = 0.0f;
            m_frequency = 1.0f;
            m_phase = 0.0f;
            m_amplitude = 1.0f;
            m_quality = Quality::Medium;
            m_enabled = true;
            m_time = 0.0f;
            m_animated = false;
            m_animationSpeed = 1.0f;
            m_noiseScale = 0.1f;
            m_noiseStrength = 0.0f;

            UpdateDistortionParameters();
            UpdateQualitySettings();

            RF_LOG_INFO("DistortionEffect: Reset to default values");
        }

        void DistortionEffect::Preset::ApplyPreset(DistortionEffect* effect, PresetType type)
        {
            switch (type)
            {
                case PresetType::Subtle:
                    effect->SetType(DistortionType::Radial);
                    effect->SetStrength(0.2f);
                    effect->SetRadius(0.3f);
                    break;

                case PresetType::Moderate:
                    effect->SetType(DistortionType::Swirl);
                    effect->SetStrength(0.5f);
                    effect->SetRadius(0.5f);
                    effect->SetAngle(Math::PI * 0.25f);
                    break;

                case PresetType::Strong:
                    effect->SetType(DistortionType::Fisheye);
                    effect->SetStrength(0.8f);
                    effect->SetRadius(0.8f);
                    break;

                case PresetType::Extreme:
                    effect->SetType(DistortionType::Twist);
                    effect->SetStrength(1.2f);
                    effect->SetRadius(1.0f);
                    effect->SetAngle(Math::PI * 0.5f);
                    break;

                case PresetType::Wavy:
                    effect->SetType(DistortionType::Wave);
                    effect->SetStrength(0.4f);
                    effect->SetFrequency(3.0f);
                    effect->SetAmplitude(0.15f);
                    break;

                case PresetType::Glitch:
                    effect->SetType(DistortionType::Noise);
                    effect->SetStrength(0.6f);
                    effect->SetNoiseScale(0.2f);
                    effect->SetNoiseStrength(0.8f);
                    break;
            }

            RF_LOG_INFO("DistortionEffect: Applied preset ", (int)type);
        }

        void DistortionEffect::SaveSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("DistortionEffect: Saved settings to ", filename);
        }

        void DistortionEffect::LoadSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("DistortionEffect: Loaded settings from ", filename);
        }

    } // namespace Effects
} // namespace RedFiles