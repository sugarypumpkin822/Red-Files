#include "rf_shadow_effect.h"
#include "rf_logger.h"

namespace RedFiles {
    namespace Effects {

        ShadowEffect::ShadowEffect()
            : m_isInitialized(false)
            , m_type(ShadowType::Drop)
            , m_offsetX(2.0f)
            , m_offsetY(2.0f)
            , m_color(0.0f, 0.0f, 0.0f)
            , m_opacity(0.5f)
            , m_blurRadius(3.0f)
            , m_spread(1.0f)
            , m_distance(0.0f)
            , m_angle(0.0f)
            , m_quality(Quality::Medium)
            , m_samples(16)
            , m_enabled(true)
            , m_time(0.0f)
            , m_animated(false)
            , m_animationSpeed(1.0f)
            , m_pulseFrequency(1.0f)
            , m_pulseAmplitude(0.0f)
            , m_noiseScale(0.1f)
            , m_noiseStrength(0.0f)
            , m_blendMode(BlendMode::Multiply)
            , m_inset(false)
            , m_centerX(0.5f)
            , m_centerY(0.5f)
            , m_radius(0.0f)
            , m_aspectRatio(1.0f)
            , m_rotation(0.0f)
            m_target(nullptr)
        {
            RF_LOG_INFO("ShadowEffect: Created shadow effect");
        }

        ShadowEffect::~ShadowEffect()
        {
            RF_LOG_INFO("ShadowEffect: Destroyed shadow effect");
        }

        bool ShadowEffect::Initialize()
        {
            if (m_isInitialized)
            {
                RF_LOG_WARNING("ShadowEffect: Already initialized");
                return true;
            }

            // Initialize shadow parameters based on type
            UpdateShadowParameters();

            // Initialize quality settings
            UpdateQualitySettings();

            // Initialize noise texture for noise-based shadows
            InitializeNoiseTexture();

            m_isInitialized = true;
            RF_LOG_INFO("ShadowEffect: Successfully initialized");
            return true;
        }

        void ShadowEffect::Update(float deltaTime)
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

            RF_LOG_DEBUG("ShadowEffect: Updated effect");
        }

        void ShadowEffect::Apply(RenderTarget* target)
        {
            if (!m_isInitialized || !m_enabled || !target)
            {
                RF_LOG_WARNING("ShadowEffect: Cannot apply effect - not initialized or disabled");
                return;
            }

            switch (m_type)
            {
                case ShadowType::Drop:
                    ApplyDropShadow(target);
                    break;
                case ShadowType::Inner:
                    ApplyInnerShadow(target);
                    break;
                case ShadowType::Outline:
                    ApplyOutlineShadow(target);
                    break;
                case ShadowType::Glow:
                    ApplyGlowShadow(target);
                    break;
                case ShadowType::Long:
                    ApplyLongShadow(target);
                    break;
                case ShadowType::Short:
                    ApplyShortShadow(target);
                    break;
                case ShadowType::Hard:
                    ApplyHardShadow(target);
                    break;
                case ShadowType::Soft:
                    ApplySoftShadow(target);
                    break;
                case ShadowType::Noise:
                    ApplyNoiseShadow(target);
                    break;
                case ShadowType::Radial:
                    ApplyRadialShadow(target);
                    break;
            }

            RF_LOG_DEBUG("ShadowEffect: Applied ", (int)m_type, " shadow effect");
        }

        void ShadowEffect::SetType(ShadowType type)
        {
            m_type = type;
            UpdateShadowParameters();
            RF_LOG_INFO("ShadowEffect: Set type to ", (int)type);
        }

        void ShadowEffect::SetOffset(float x, float y)
        {
            m_offsetX = x;
            m_offsetY = y;
            RF_LOG_INFO("ShadowEffect: Set offset to (", m_offsetX, ", ", m_offsetY, ")");
        }

        void ShadowEffect::SetColor(const Vector3& color)
        {
            m_color = color;
            RF_LOG_INFO("ShadowEffect: Set color to (", m_color.x, ", ", m_color.y, ", ", m_color.z, ")");
        }

        void ShadowEffect::SetColor(float r, float g, float b)
        {
            m_color = Vector3(r, g, b);
            RF_LOG_INFO("ShadowEffect: Set color to (", r, ", ", g, ", b, ")");
        }

        void ShadowEffect::SetOpacity(float opacity)
        {
            m_opacity = Math::Clamp(opacity, 0.0f, 1.0f);
            RF_LOG_INFO("ShadowEffect: Set opacity to ", m_opacity);
        }

        void ShadowEffect::SetBlurRadius(float radius)
        {
            m_blurRadius = Math::Max(0.0f, radius);
            RF_LOG_INFO("ShadowEffect: Set blur radius to ", m_blurRadius);
        }

        void ShadowEffect::SetSpread(float spread)
        {
            m_spread = Math::Max(0.0f, spread);
            RF_LOG_INFO("ShadowEffect: Set spread to ", m_spread);
        }

        void ShadowEffect::SetDistance(float distance)
        {
            m_distance = Math::Max(0.0f, distance);
            RF_LOG_INFO("ShadowEffect: Set distance to ", m_distance);
        }

        void ShadowEffect::SetAngle(float angle)
        {
            m_angle = angle;
            RF_LOG_INFO("ShadowEffect: Set angle to ", m_angle);
        }

        void ShadowEffect::SetQuality(Quality quality)
        {
            m_quality = quality;
            UpdateQualitySettings();
            RF_LOG_INFO("ShadowEffect: Set quality to ", (int)quality);
        }

        void ShadowEffect::SetInset(bool inset)
        {
            m_inset = inset;
            RF_LOG_INFO("ShadowEffect: Set inset to ", inset);
        }

        void ShadowEffect::SetCenter(float x, float y)
        {
            m_centerX = Math::Clamp(x, 0.0f, 1.0f);
            m_centerY = Math::Clamp(y, 0.0f, 1.0f);
            RF_LOG_INFO("ShadowEffect: Set center to (", m_centerX, ", ", m_centerY, ")");
        }

        void ShadowEffect::SetRadius(float radius)
        {
            m_radius = Math::Max(0.0f, radius);
            RF_LOG_INFO("ShadowEffect: Set radius to ", m_radius);
        }

        void ShadowEffect::SetAspectRatio(float aspectRatio)
        {
            m_aspectRatio = Math::Max(0.1f, aspectRatio);
            RF_LOG_INFO("ShadowEffect: Set aspect ratio to ", m_aspectRatio);
        }

        void ShadowEffect::SetRotation(float rotation)
        {
            m_rotation = rotation;
            RF_LOG_INFO("ShadowEffect: Set rotation to ", m_rotation);
        }

        void ShadowEffect::SetNoiseScale(float scale)
        {
            m_noiseScale = Math::Max(0.01f, scale);
            RF_LOG_INFO("ShadowEffect: Set noise scale to ", m_noiseScale);
        }

        void ShadowEffect::SetNoiseStrength(float strength)
        {
            m_noiseStrength = Math::Max(0.0f, strength);
            RF_LOG_INFO("ShadowEffect: Set noise strength to ", m_noiseStrength);
        }

        void ShadowEffect::SetBlendMode(BlendMode blendMode)
        {
            m_blendMode = blendMode;
            RF_LOG_INFO("ShadowEffect: Set blend mode to ", (int)blendMode);
        }

        void ShadowEffect::SetAnimated(bool animated)
        {
            m_animated = animated;
            RF_LOG_INFO("ShadowEffect: Set animated to ", animated);
        }

        void ShadowEffect::SetAnimationSpeed(float speed)
        {
            m_animationSpeed = Math::Max(0.0f, speed);
            RF_LOG_INFO("ShadowEffect: Set animation speed to ", m_animationSpeed);
        }

        void ShadowEffect::SetPulseFrequency(float frequency)
        {
            m_pulseFrequency = Math::Max(0.1f, frequency);
            RF_LOG_INFO("ShadowEffect: Set pulse frequency to ", m_pulseFrequency);
        }

        void ShadowEffect::SetPulseAmplitude(float amplitude)
        {
            m_pulseAmplitude = Math::Max(0.0f, amplitude);
            RF_LOG_INFO("ShadowEffect: Set pulse amplitude to ", m_pulseAmplitude);
        }

        void ShadowEffect::SetEnabled(bool enabled)
        {
            m_enabled = enabled;
            RF_LOG_INFO("ShadowEffect: Set enabled to ", enabled);
        }

        ShadowType ShadowEffect::GetType() const
        {
            return m_type;
        }

        Vector2 ShadowEffect::GetOffset() const
        {
            return Vector2(m_offsetX, m_offsetY);
        }

        Vector3 ShadowEffect::GetColor() const
        {
            return m_color;
        }

        float ShadowEffect::GetOpacity() const
        {
            return m_opacity;
        }

        float ShadowEffect::GetBlurRadius() const
        {
            return m_blurRadius;
        }

        float ShadowEffect::GetSpread() const
        {
            return m_spread;
        }

        float ShadowEffect::GetDistance() const
        {
            return m_distance;
        }

        float ShadowEffect::GetAngle() const
        {
            return m_angle;
        }

        Quality ShadowEffect::GetQuality() const
        {
            return m_quality;
        }

        bool ShadowEffect::IsInset() const
        {
            return m_inset;
        }

        Vector2 ShadowEffect::GetCenter() const
        {
            return Vector2(m_centerX, m_centerY);
        }

        float ShadowEffect::GetRadius() const
        {
            return m_radius;
        }

        float ShadowEffect::GetAspectRatio() const
        {
            return m_aspectRatio;
        }

        float ShadowEffect::GetRotation() const
        {
            return m_rotation;
        }

        BlendMode ShadowEffect::GetBlendMode() const
        {
            return m_blendMode;
        }

        bool ShadowEffect::IsAnimated() const
        {
            return m_animated;
        }

        float ShadowEffect::GetAnimationSpeed() const
        {
            return m_animationSpeed;
        }

        float ShadowEffect::GetPulseFrequency() const
        {
            return m_pulseFrequency;
        }

        float ShadowEffect::GetPulseAmplitude() const
        {
            return m_pulseAmplitude;
        }

        bool ShadowEffect::IsEnabled() const
        {
            return m_enabled;
        }

        bool ShadowEffect::IsInitialized() const
        {
            return m_isInitialized;
        }

        void ShadowEffect::UpdateAnimation(float deltaTime)
        {
            float animPhase = m_time * m_animationSpeed;

            // Pulse animation for offset
            float offsetModulation = Math::Sin(animPhase * m_pulseFrequency * Math::PI * 2.0f) * m_pulseAmplitude;
            m_offsetX += offsetModulation * Math::Cos(m_angle);
            m_offsetY += offsetModulation * Math::Sin(m_angle);

            // Pulse animation for blur radius
            float blurModulation = Math::Sin(animPhase * 0.7f) * 0.2f;
            m_blurRadius = Math::Max(0.0f, m_blurRadius * (1.0f + blurModulation));

            // Pulse animation for opacity
            float opacityModulation = Math::Sin(animPhase * 1.3f) * 0.1f;
            m_opacity = Math::Clamp(m_opacity * (1.0f + opacityModulation), 0.0f, 1.0f);

            // Pulse animation for spread
            float spreadModulation = Math::Sin(animPhase * 0.5f) * 0.1f;
            m_spread = Math::Max(0.0f, m_spread * (1.0f + spreadModulation));

            // Pulse animation for color
            float colorModulation = Math::Sin(animPhase * 0.3f) * 0.1f;
            m_color.x = Math::Clamp(m_color.x * (1.0f + colorModulation), 0.0f, 1.0f);
            m_color.y = Math::Clamp(m_color.y * (1.0f + colorModulation * 0.8f), 0.0f, 1.0f);
            m_color.z = Math::Clamp(m_color.z * (1.0f + colorModulation * 0.6f), 0.0f, 1.0f);
        }

        void ShadowEffect::UpdateDynamicParameters(float deltaTime)
        {
            // Update time-based effects
            float timeModulation = Math::Sin(m_time * 0.5f) * 0.05f;
            
            // Dynamic offset pulsing
            m_offsetX += timeModulation * deltaTime * Math::Cos(m_angle);
            m_offsetY += timeModulation * deltaTime * Math::Sin(m_angle);
            
            // Dynamic blur radius
            m_blurRadius = Math::Max(0.0f, m_blurRadius + Math::Cos(m_time * 0.3f) * 0.05f * deltaTime);
            
            // Dynamic noise
            if (m_noiseStrength > 0.0f)
            {
                float noisePhase = m_time * 2.0f;
                m_noiseStrength = Math::Max(0.0f, m_noiseStrength + Math::Sin(noisePhase) * 0.1f * deltaTime);
            }
        }

        void ShadowEffect::UpdateNoise(float deltaTime)
        {
            // Update noise texture or parameters
            // This would be implemented using the rendering API
        }

        void ShadowEffect::UpdateShadowParameters()
        {
            // Set default parameters based on shadow type
            switch (m_type)
            {
                case ShadowType::Drop:
                    m_offsetX = 2.0f;
                    m_offsetY = 2.0f;
                    m_blurRadius = 3.0f;
                    m_spread = 1.0f;
                    m_inset = false;
                    break;
                case ShadowType::Inner:
                    m_offsetX = 0.0f;
                    m_offsetY = 0.0f;
                    m_blurRadius = 2.0f;
                    m_spread = 1.0f;
                    m_inset = true;
                    break;
                case ShadowType::Outline:
                    m_offsetX = 0.0f;
                    m_offsetY = 0.0f;
                    m_blurRadius = 1.0f;
                    m_spread = 1.0f;
                    m_inset = false;
                    break;
                case ShadowType::Glow:
                    m_offsetX = 0.0f;
                    m_offsetY = 0.0f;
                    m_blurRadius = 5.0f;
                    m_spread = 2.0f;
                    m_inset = false;
                    m_blendMode = BlendMode::Screen;
                    break;
                case ShadowType::Long:
                    m_offsetX = 4.0f;
                    m_offsetY = 4.0f;
                    m_blurRadius = 4.0f;
                    m_spread = 1.0f;
                    m_inset = false;
                    break;
                case ShadowType::Short:
                    m_offsetX = 1.0f;
                    m_offsetY = 1.0f;
                    m_blurRadius = 2.0f;
                    m_spread = 0.5f;
                    m_inset = false;
                    break;
                case ShadowType::Hard:
                    m_offsetX = 2.0f;
                    m_offsetY = 2.0f;
                    m_blurRadius = 0.0f;
                    m_spread = 1.0f;
                    m_inset = false;
                    break;
                case ShadowType::Soft:
                    m_offsetX = 2.0f;
                    m_offsetY = 2.0f;
                    m_blurRadius = 6.0f;
                    m_spread = 1.5f;
                    m_inset = false;
                    break;
                case ShadowType::Noise:
                    m_noiseScale = 0.1f;
                    m_noiseStrength = 0.3f;
                    m_offsetX = 2.0f;
                    m_offsetY = 2.0f;
                    m_blurRadius = 3.0f;
                    m_spread = 1.0f;
                    m_inset = false;
                    break;
                case ShadowType::Radial:
                    m_centerX = 0.5f;
                    m_centerY = 0.5f;
                    m_radius = 0.3f;
                    m_blurRadius = 3.0f;
                    m_inset = false;
                    break;
            }
        }

        void ShadowEffect::UpdateQualitySettings()
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

        void ShadowEffect::InitializeNoiseTexture()
        {
            // Initialize noise texture for noise-based shadows
            // This would be implemented using the rendering API
        }

        void ShadowEffect::ApplyDropShadow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            // Create shadow map
            std::vector<float> shadowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    
                    // Calculate shadow intensity based on distance from shadow origin
                    Vector2 shadowOrigin = Vector2(0.5f, 0.5f) + Vector2(m_offsetX, m_offsetY) / Vector2((float)width, (float)height);
                    float distance = (uv - shadowOrigin).Length();
                    
                    // Apply spread and blur
                    float shadowIntensity = CalculateShadowIntensity(distance);
                    shadowMap[y * width + x] = shadowIntensity;
                }
            }

            ApplyShadowMap(target, shadowMap);
        }

        void ShadowEffect::ApplyInnerShadow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> shadowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    
                    // Inner shadow - calculate distance from edges
                    float edgeDistance = CalculateEdgeDistance(uv, Vector2((float)width, (float)height));
                    float shadowIntensity = CalculateShadowIntensity(edgeDistance);
                    
                    shadowMap[y * width + x] = shadowIntensity;
                }
            }

            ApplyShadowMap(target, shadowMap);
        }

        void ShadowEffect::ApplyOutlineShadow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> shadowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    
                    // Outline shadow - calculate distance from edges
                    float edgeDistance = CalculateEdgeDistance(uv, Vector2((float)width, (float)height));
                    float shadowIntensity = CalculateShadowIntensity(edgeDistance);
                    
                    // Apply only to edges
                    if (edgeDistance < m_blurRadius)
                    {
                        shadowMap[y * width + x] = shadowIntensity;
                    }
                    else
                    {
                        shadowMap[y * width + x] = 0.0f;
                    }
                }
            }

            ApplyShadowMap(target, shadowMap);
        }

        void ShadowEffect::ApplyGlowShadow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> shadowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    float distance = (uv - center).Length();
                    
                    // Glow shadow - radial falloff
                    float shadowIntensity = CalculateRadialShadowIntensity(distance);
                    shadowMap[y * width + x] = shadowIntensity;
                }
            }

            ApplyShadowMap(target, shadowMap);
        }

        void ShadowEffect::ApplyLongShadow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> shadowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    
                    // Long shadow - increased offset and blur
                    Vector2 shadowOrigin = Vector2(0.5f, 0.5f) + Vector2(m_offsetX * 2.0f, m_offsetY * 2.0f) / Vector2((float)width, (float)height);
                    float distance = (uv - shadowOrigin).Length();
                    
                    float shadowIntensity = CalculateShadowIntensity(distance);
                    shadowMap[y * width + x] = shadowIntensity;
                }
            }

            ApplyShadowMap(target, shadowMap);
        }

        void ShadowEffect::ApplyShortShadow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> shadowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    
                    // Short shadow - reduced offset and blur
                    Vector2 shadowOrigin = Vector2(0.5f, 0.5f) + Vector2(m_offsetX * 0.5f, m_offsetY * 0.5f) / Vector2((float)width, (float)height);
                    float distance = (uv - shadowOrigin).Length();
                    
                    float shadowIntensity = CalculateShadowIntensity(distance);
                    shadowMap[y * width + x] = shadowIntensity;
                }
            }

            ApplyShadowMap(target, shadowMap);
        }

        void ShadowEffect::ApplyHardShadow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> shadowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    
                    // Hard shadow - no blur
                    Vector2 shadowOrigin = Vector2(0.5f, 0.5f) + Vector2(m_offsetX, m_offsetY) / Vector2((float)width, (float)height);
                    float distance = (uv - shadowOrigin).Length();
                    
                    // Hard edge falloff
                    float shadowIntensity = distance < m_spread ? m_opacity : 0.0f;
                    shadowMap[y * width + x] = shadowIntensity;
                }
            }

            ApplyShadowMap(target, shadowMap);
        }

        void ShadowEffect::ApplySoftShadow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> shadowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    
                    // Soft shadow - large blur radius
                    Vector2 shadowOrigin = Vector2(0.5f, 0.5f) + Vector2(m_offsetX, m_offsetY) / Vector2((float)width, (float)height);
                    float distance = (uv - shadowOrigin).Length();
                    
                    float shadowIntensity = CalculateShadowIntensity(distance);
                    shadowMap[y * width + x] = shadowIntensity;
                }
            }

            ApplyShadowMap(target, shadowMap);
        }

        void ShadowEffect::ApplyNoiseShadow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> shadowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    
                    Vector2 shadowOrigin = Vector2(0.5f, 0.5f) + Vector2(m_offsetX, m_offsetY) / Vector2((float)width, (float)height);
                    float distance = (uv - shadowOrigin).Length();
                    
                    // Base shadow intensity
                    float baseIntensity = CalculateShadowIntensity(distance);
                    
                    // Add noise variation
                    float noise = GenerateNoise(uv.x * m_noiseScale, uv.y * m_noiseScale, m_time);
                    float noiseFactor = 1.0f + noise * m_noiseStrength;
                    
                    float shadowIntensity = baseIntensity * noiseFactor;
                    shadowMap[y * width + x] = shadowIntensity;
                }
            }

            ApplyShadowMap(target, shadowMap);
        }

        void ShadowEffect::ApplyRadialShadow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> shadowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    // Apply aspect ratio and rotation
                    Vector2 transformedDir = ApplyRotationAndAspectRatio(uv - center);
                    float distance = transformedDir.Length();
                    
                    // Radial shadow - based on distance from center
                    float shadowIntensity = CalculateRadialShadowIntensity(distance);
                    shadowMap[y * width + x] = shadowIntensity;
                }
            }

            ApplyShadowMap(target, shadowMap);
        }

        float ShadowEffect::CalculateShadowIntensity(float distance) const
        {
            // Gaussian falloff for soft shadows
            float sigma = m_blurRadius * 0.3f;
            float gaussian = Math::Exp(-(distance * distance) / (2.0f * sigma * sigma));
            
            // Apply spread
            float spreadFactor = 1.0f - (distance / (m_blurRadius * m_spread));
            spreadFactor = Math::Max(0.0f, spreadFactor);
            
            return m_opacity * gaussian * spreadFactor;
        }

        float ShadowEffect::CalculateRadialShadowIntensity(float distance) const
        {
            if (distance <= m_radius)
            {
                return m_opacity;
            }
            else
            {
                // Radial falloff
                float normalizedDistance = (distance - m_radius) / (m_blurRadius);
                float falloff = Math::Pow(1.0f - normalizedDistance, 2.0f);
                return m_opacity * falloff;
            }
        }

        float ShadowEffect::CalculateEdgeDistance(const Vector2& uv, const Vector2& size) const
        {
            // Calculate minimum distance to any edge
            float dx = Math::Min(uv.x, 1.0f - uv.x);
            float dy = Math::Min(uv.y, 1.0f - uv.y);
            return Math::Min(dx, dy);
        }

        Vector2 ShadowEffect::ApplyRotationAndAspectRatio(const Vector2& dir) const
        {
            // Apply rotation
            float cosRot = Math::Cos(m_rotation);
            float sinRot = Math::Sin(m_rotation);
            Vector2 rotated;
            rotated.x = dir.x * cosRot - dir.y * sinRot;
            rotated.y = dir.x * sinRot + dir.y * cosRot;

            // Apply aspect ratio
            rotated.x *= m_aspectRatio;

            return rotated;
        }

        void ShadowEffect::ApplyShadowMap(RenderTarget* target, const std::vector<float>& shadowMap)
        {
            // Apply the shadow map to the render target
            // This would be implemented using the rendering API
            // For now, we provide the structure

            // Create shadow overlay
            RenderTarget* shadowOverlay = CreateShadowOverlay(target);
            if (shadowOverlay)
            {
                // Apply color and opacity
                ApplyColorAndOpacity(shadowOverlay, shadowMap);

                // Blend with original target
                BlendWithTarget(target, shadowOverlay);

                // Cleanup
                DestroyShadowOverlay(shadowOverlay);
            }
        }

        RenderTarget* ShadowEffect::CreateShadowOverlay(RenderTarget* source)
        {
            // Create a render target for the shadow overlay
            // Platform-specific implementation
            return nullptr; // Placeholder
        }

        void ShadowEffect::ApplyColorAndOpacity(RenderTarget* overlay, const std::vector<float>& shadowMap)
        {
            // Apply the shadow color and opacity to the overlay
            // Platform-specific implementation
        }

        void ShadowEffect::BlendWithTarget(RenderTarget* target, RenderTarget* overlay)
        {
            // Blend the overlay with the target using the specified blend mode
            // Platform-specific implementation
        }

        void ShadowEffect::DestroyShadowOverlay(RenderTarget* overlay)
        {
            // Clean up the overlay render target
            // Platform-specific implementation
        }

        float ShadowEffect::GenerateNoise(float x, float y, float time) const
        {
            // Simple noise function
            return Math::Sin(x * 2.0f + time) * Math::Cos(y * 2.0f + time) * 0.5f + 0.5f;
        }

        void ShadowEffect::Reset()
        {
            m_type = ShadowType::Drop;
            m_offsetX = 2.0f;
            m_offsetY = 2.0f;
            m_color = Vector3(0.0f, 0.0f, 0.0f);
            m_opacity = 0.5f;
            m_blurRadius = 3.0f;
            m_spread = 1.0f;
            m_distance = 0.0f;
            m_angle = 0.0f;
            m_quality = Quality::Medium;
            m_enabled = true;
            m_time = 0.0f;
            m_animated = false;
            m_animationSpeed = 1.0f;
            m_pulseFrequency = 1.0f;
            m_pulseAmplitude = 0.0f;
            m_noiseScale = 0.1f;
            m_noiseStrength = 0.0f;
            m_blendMode = BlendMode::Multiply;
            m_inset = false;
            m_centerX = 0.5f;
            m_centerY = 0.5f;
            m_radius = 0.0f;
            m_aspectRatio = 1.0f;
            m_rotation = 0.0f;
            m_target = nullptr;

            UpdateShadowParameters();
            UpdateQualitySettings();

            RF_LOG_INFO("ShadowEffect: Reset to default values");
        }

        void ShadowEffect::Preset::ApplyPreset(ShadowEffect* effect, PresetType type)
        {
            switch (type)
            {
                case PresetType::Subtle:
                    effect->SetType(ShadowType::Soft);
                    effect->SetOffset(1.0f, 1.0f);
                    effect->SetOpacity(0.3f);
                    effect->SetBlurRadius(2.0f);
                    effect->SetColor(0.0f, 0.0f, 0.0f);
                    break;

                case PresetType::Medium:
                    effect->SetType(ShadowType::Drop);
                    effect->SetOffset(2.0f, 2.0f);
                    effect->SetOpacity(0.5f);
                    effect->SetBlurRadius(3.0f);
                    effect->SetColor(0.0f, 0.0f, 0.0f);
                    break;

                case PresetType::Strong:
                    effect->SetType(ShadowType::Drop);
                    effect->SetOffset(3.0f, 3.0f);
                    effect->SetOpacity(0.7f);
                    effect->SetBlurRadius(4.0f);
                    effect->SetColor(0.0f, 0.0f, 0.0f);
                    break;

                case PresetType::Dramatic:
                    effect->SetType(ShadowType::Long);
                    effect->SetOffset(5.0f, 5.0f);
                    effect->SetOpacity(0.8f);
                    effect->SetBlurRadius(6.0f);
                    effect->SetColor(0.0f, 0.0f, 0.0f);
                    break;

                case PresetType::Soft:
                    effect->SetType(ShadowType::Soft);
                    effect->SetOffset(2.0f, 2.0f);
                    effect->SetOpacity(0.4f);
                    effect->SetBlurRadius(6.0f);
                    effect->SetColor(0.1f, 0.1f, 0.1f);
                    break;

                case PresetType::Hard:
                    effect->SetType(ShadowType::Hard);
                    effect->SetOffset(2.0f, 2.0f);
                    effect->SetOpacity(0.6f);
                    effect->SetBlurRadius(0.0f);
                    effect->SetColor(0.0f, 0.0f, 0.0f);
                    break;

                case PresetType::Glow:
                    effect->SetType(ShadowType::Glow);
                    effect->SetOffset(0.0f, 0.0f);
                    effect->SetOpacity(0.6f);
                    effect->SetBlurRadius(5.0f);
                    effect->SetColor(0.8f, 0.8f, 0.8f);
                    effect->SetBlendMode(BlendMode::Screen);
                    break;

                case PresetType::Neon:
                    effect->SetType(ShadowType::Glow);
                    effect->SetOffset(0.0f, 0.0f);
                    effect->SetOpacity(0.8f);
                    effect->SetBlurRadius(4.0f);
                    effect->SetColor(0.0f, 1.0f, 1.0f);
                    effect->SetBlendMode(BlendMode::Screen);
                    break;

                case PresetType::Magical:
                    effect->SetType(ShadowType::Noise);
                    effect->SetOffset(2.0f, 2.0f);
                    effect->SetOpacity(0.5f);
                    effect->SetBlurRadius(3.0f);
                    effect->SetColor(0.8f, 0.4f, 1.0f);
                    effect->SetNoiseScale(0.2f);
                    effect->SetNoiseStrength(0.3f);
                    effect->SetAnimated(true);
                    effect->SetAnimationSpeed(2.0f);
                    effect->SetPulseFrequency(1.5f);
                    effect->SetPulseAmplitude(0.2f);
                    break;
            }

            RF_LOG_INFO("ShadowEffect: Applied preset ", (int)type);
        }

        void ShadowEffect::SaveSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("ShadowEffect: Saved settings to ", filename);
        }

        void ShadowEffect::LoadSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("ShadowEffect: Loaded settings from ", filename);
        }

    } // namespace Effects
} // namespace RedFiles