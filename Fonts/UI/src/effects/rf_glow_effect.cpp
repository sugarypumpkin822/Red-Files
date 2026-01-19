#include "rf_glow_effect.h"
#include "rf_logger.h"

namespace RedFiles {
    namespace Effects {

        GlowEffect::GlowEffect()
            : m_isInitialized(false)
            , m_intensity(1.0f)
            , m_radius(10.0f)
            , m_color(1.0f, 1.0f, 1.0f)
            , m_centerX(0.5f)
            , m_centerY(0.5f)
            , m_falloff(1.0f)
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
            , m_blendMode(BlendMode::Additive)
            , m_glowType(GlowType::Radial)
            , m_innerRadius(0.0f)
            , m_outerRadius(1.0f)
            , m_angle(0.0f)
            , m_aspectRatio(1.0f)
            , m_rotation(0.0f)
        {
            RF_LOG_INFO("GlowEffect: Created glow effect");
        }

        GlowEffect::~GlowEffect()
        {
            RF_LOG_INFO("GlowEffect: Destroyed glow effect");
        }

        bool GlowEffect::Initialize()
        {
            if (m_isInitialized)
            {
                RF_LOG_WARNING("GlowEffect: Already initialized");
                return true;
            }

            // Initialize glow parameters based on type
            UpdateGlowParameters();

            // Initialize quality settings
            UpdateQualitySettings();

            // Initialize noise texture
            InitializeNoiseTexture();

            m_isInitialized = true;
            RF_LOG_INFO("GlowEffect: Successfully initialized");
            return true;
        }

        void GlowEffect::Update(float deltaTime)
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

            RF_LOG_DEBUG("GlowEffect: Updated effect");
        }

        void GlowEffect::Apply(RenderTarget* target)
        {
            if (!m_isInitialized || !m_enabled || !target)
            {
                RF_LOG_WARNING("GlowEffect: Cannot apply effect - not initialized or disabled");
                return;
            }

            switch (m_glowType)
            {
                case GlowType::Radial:
                    ApplyRadialGlow(target);
                    break;
                case GlowType::Linear:
                    ApplyLinearGlow(target);
                    break;
                case GlowEffect::Gaussian:
                    ApplyGaussianGlow(target);
                    break;
                case GlowEffect::Box:
                    ApplyBoxGlow(target);
                    break;
                case GlowEffect::Directional:
                    ApplyDirectionalGlow(target);
                    break;
                case GlowEffect::Noise:
                    ApplyNoiseGlow(target);
                    break;
            }

            RF_LOG_DEBUG("GlowEffect: Applied ", (int)m_glowType, " glow effect");
        }

        void GlowEffect::SetIntensity(float intensity)
        {
            m_intensity = Math::Max(0.0f, intensity);
            RF_LOG_INFO("GlowEffect: Set intensity to ", m_intensity);
        }

        void GlowEffect::SetRadius(float radius)
        {
            m_radius = Math::Max(0.0f, radius);
            RF_LOG_INFO("GlowEffect: Set radius to ", m_radius);
        }

        void GlowEffect::SetColor(const Vector3& color)
        {
            m_color = color;
            RF_LOG_INFO("GlowEffect: Set color to (", m_color.x, ", ", m_color.y, ", ", m_color.z, ")");
        }

        void GlowEffect::SetColor(float r, float g, float b)
        {
            m_color = Vector3(r, g, b);
            RF_LOG_INFO("GlowEffect: Set color to (", r, ", ", g, ", ", b, ")");
        }

        void GlowEffect::SetCenter(float x, float y)
        {
            m_centerX = Math::Clamp(x, 0.0f, 1.0f);
            m_centerY = Math::Clamp(y, 0.0f, 1.0f);
            RF_LOG_INFO("GlowEffect: Set center to (", m_centerX, ", ", m_centerY, ")");
        }

        void GlowEffect::SetFalloff(float falloff)
        {
            m_falloff = Math::Max(0.1f, falloff);
            RF_LOG_INFO("GlowEffect: Set falloff to ", m_falloff);
        }

        void GlowEffect::SetQuality(Quality quality)
        {
            m_quality = quality;
            UpdateQualitySettings();
            RF_LOG_INFO("GlowEffect: Set quality to ", (int)quality);
        }

        void GlowEffect::SetGlowType(GlowType type)
        {
            m_glowType = type;
            UpdateGlowParameters();
            RF_LOG_INFO("GlowEffect: Set glow type to ", (int)type);
        }

        void GlowEffect::SetInnerRadius(float radius)
        {
            m_innerRadius = Math::Max(0.0f, radius);
            RF_LOG_INFO("GlowEffect: Set inner radius to ", m_innerRadius);
        }

        void GlowEffect::SetOuterRadius(float radius)
        {
            m_outerRadius = Math::Max(m_innerRadius, radius);
            RF_LOG_INFO("GlowEffect: Set outer radius to ", m_outerRadius);
        }

        void GlowEffect::SetAngle(float angle)
        {
            m_angle = angle;
            RF_LOG_INFO("GlowEffect: Set angle to ", m_angle);
        }

        void GlowEffect::SetAspectRatio(float aspectRatio)
        {
            m_aspectRatio = Math::Max(0.1f, aspectRatio);
            RF_LOG_INFO("GlowEffect: Set aspect ratio to ", m_aspectRatio);
        }

        void GlowEffect::SetRotation(float rotation)
        {
            m_rotation = rotation;
            RF_LOG_INFO("GlowEffect: Set rotation to ", m_rotation);
        }

        void GlowEffect::SetNoiseScale(float scale)
        {
            m_noiseScale = Math::Max(0.01f, scale);
            RF_LOG_INFO("GlowEffect: Set noise scale to ", m_noiseScale);
        }

        void GlowEffect::SetNoiseStrength(float strength)
        {
            m_noiseStrength = Math::Max(0.0f, strength);
            RF_LOG_INFO("GlowEffect: Set noise strength to ", m_noiseStrength);
        }

        void GlowEffect::SetBlendMode(BlendMode blendMode)
        {
            m_blendMode = blendMode;
            RF_LOG_INFO("GlowEffect: Set blend mode to ", (int)blendMode);
        }

        void GlowEffect::SetAnimated(bool animated)
        {
            m_animated = animated;
            RF_LOG_INFO("GlowEffect: Set animated to ", animated);
        }

        void GlowEffect::SetAnimationSpeed(float speed)
        {
            m_animationSpeed = Math::Max(0.0f, speed);
            RF_LOG_INFO("GlowEffect: Set animation speed to ", m_animationSpeed);
        }

        void GlowEffect::SetPulseFrequency(float frequency)
        {
            m_pulseFrequency = Math::Max(0.1f, frequency);
            RF_LOG_INFO("GlowEffect: Set pulse frequency to ", m_pulseFrequency);
        }

        void GlowEffect::SetPulseAmplitude(float amplitude)
        {
            m_pulseAmplitude = Math::Max(0.0f, amplitude);
            RF_LOG_INFO("GlowEffect: Set pulse amplitude to ", m_pulseAmplitude);
        }

        void GlowEffect::SetEnabled(bool enabled)
        {
            m_enabled = enabled;
            RF_LOG_INFO("GlowEffect: Set enabled to ", enabled);
        }

        float GlowEffect::GetIntensity() const
        {
            return m_intensity;
        }

        float GlowEffect::GetRadius() const
        {
            return m_radius;
        }

        Vector3 GlowEffect::GetColor() const
        {
            return m_color;
        }

        Vector2 GlowEffect::GetCenter() const
        {
            return Vector2(m_centerX, m_centerY);
        }

        float GlowEffect::GetFalloff() const
        {
            return m_falloff;
        }

        Quality GlowEffect::GetQuality() const
        {
            return m_quality;
        }

        GlowType GlowEffect::GetGlowType() const
        {
            return m_glowType;
        }

        float GlowEffect::GetInnerRadius() const
        {
            return m_innerRadius;
        }

        float GlowEffect::GetOuterRadius() const
        {
            return m_outerRadius;
        }

        float GlowEffect::GetAngle() const
        {
            return m_angle;
        }

        float GlowEffect::GetAspectRatio() const
        {
            return m_aspectRatio;
        }

        float GlowEffect::GetRotation() const
        {
            return m_rotation;
        }

        BlendMode GlowEffect::GetBlendMode() const
        {
            return m_blendMode;
        }

        bool GlowEffect::IsAnimated() const
        {
            return m_animated;
        }

        float GlowEffect::GetAnimationSpeed() const
        {
            return m_animationSpeed;
        }

        float GlowEffect::GetPulseFrequency() const
        {
            return m_pulseFrequency;
        }

        float GlowEffect::GetPulseAmplitude() const
        {
            return m_pulseAmplitude;
        }

        bool GlowEffect::IsEnabled() const
        {
            return m_enabled;
        }

        bool GlowEffect::IsInitialized() const
        {
            return m_isInitialized;
        }

        void GlowEffect::UpdateAnimation(float deltaTime)
        {
            float animPhase = m_time * m_animationSpeed;

            // Pulse animation
            float pulseModulation = Math::Sin(animPhase * m_pulseFrequency * Math::PI * 2.0f) * m_pulseAmplitude;
            m_intensity = Math::Max(0.0f, m_intensity * (1.0f + pulseModulation));

            // Rotation animation
            if (m_glowType == GlowType::Directional)
            {
                m_angle += Math::Sin(animPhase * 0.5f) * 0.1f;
            }

            // Color animation
            float colorModulation = Math::Sin(animPhase * 0.3f) * 0.1f;
            m_color.x = Math::Clamp(m_color.x * (1.0f + colorModulation), 0.0f, 1.0f);
            m_color.y = Math::Clamp(m_color.y * (1.0f + colorModulation * 0.8f), 0.0f, 1.0f);
            m_color.z = Math::Clamp(m_color.z * (1.0f + colorModulation * 0.6f), 0.0f, 1.0f);
        }

        void GlowEffect::UpdateDynamicParameters(float deltaTime)
        {
            // Update time-based effects
            float timeModulation = Math::Sin(m_time * 0.5f) * 0.05f;
            
            // Dynamic radius pulsing
            m_radius = Math::Max(0.0f, m_radius + timeModulation * deltaTime);
            
            // Dynamic falloff
            m_falloff = Math::Max(0.1f, m_falloff + Math::Cos(m_time * 0.3f) * 0.05f * deltaTime);
            
            // Dynamic noise
            if (m_noiseStrength > 0.0f)
            {
                float noisePhase = m_time * 2.0f;
                m_noiseStrength = Math::Max(0.0f, m_noiseStrength + Math::Sin(noisePhase) * 0.1f * deltaTime);
            }
        }

        void GlowEffect::UpdateNoise(float deltaTime)
        {
            // Update noise texture or parameters
            // This would be implemented using the rendering API
        }

        void GlowEffect::UpdateGlowParameters()
        {
            // Set default parameters based on glow type
            switch (m_glowType)
            {
                case GlowType::Radial:
                    m_innerRadius = 0.0f;
                    m_outerRadius = m_radius;
                    break;
                case GlowType::Linear:
                    m_innerRadius = 0.0f;
                    m_outerRadius = m_radius;
                    m_angle = 0.0f;
                    break;
                case GlowEffect::Gaussian:
                    m_innerRadius = 0.0f;
                    m_outerRadius = m_radius * 2.0f;
                    break;
                case GlowEffect::Box:
                    m_innerRadius = 0.0f;
                    m_outerRadius = m_radius;
                    m_aspectRatio = 1.0f;
                    break;
                case GlowEffect::Directional:
                    m_innerRadius = 0.0f;
                    m_outerRadius = m_radius * 1.5f;
                    m_angle = 0.0f;
                    break;
                case GlowEffect::Noise:
                    m_noiseScale = 0.1f;
                    m_noiseStrength = 0.5f;
                    break;
            }
        }

        void GlowEffect::UpdateQualitySettings()
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

        void GlowEffect::InitializeNoiseTexture()
        {
            // Initialize noise texture for noise-based glows
            // This would be implemented using the rendering API
        }

        void GlowEffect::ApplyRadialGlow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            // Create glow map
            std::vector<float> glowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 rotatedDir = ApplyRotationAndAspectRatio(dir);
                    
                    float glow = CalculateRadialGlow(rotatedDir.Length());
                    glowMap[y * width + x] = glow;
                }
            }

            ApplyGlowMap(target, glowMap);
        }

        void GlowEffect::ApplyLinearGlow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> glowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 rotatedDir = ApplyRotationAndAspectRatio(dir);
                    
                    float glow = CalculateLinearGlow(rotatedDir);
                    glowMap[y * width + x] = glow;
                }
            }

            ApplyGlowMap(target, glowMap);
        }

        void GlowEffect::ApplyGaussianGlow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> glowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 rotatedDir = ApplyRotationAndAspectRatio(dir);
                    
                    float glow = CalculateGaussianGlow(rotatedDir.Length());
                    glowMap[y * width + x] = glow;
                }
            }

            ApplyGlowMap(target, glowMap);
        }

        void GlowEffect::ApplyBoxGlow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> glowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    
                    // Apply aspect ratio and rotation
                    Vector2 rotatedDir = ApplyRotationAndAspectRatio(dir);
                    
                    float glow = CalculateBoxGlow(rotatedDir);
                    glowMap[y * width + x] = glow;
                }
            }

            ApplyGlowMap(target, glowMap);
        }

        void GlowEffect::ApplyDirectionalGlow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = targetGetHeight();

            std::vector<float> glowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    
                    // Apply aspect ratio and rotation
                    Vector2 rotatedDir = ApplyRotationAndAspectRatio(dir);
                    
                    float glow = CalculateDirectionalGlow(rotatedDir);
                    glowMap[y * width + x] = glow;
                }
            }

            ApplyGlowMap(target, glowMap);
        }

        void GlowEffect::ApplyNoiseGlow(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> glowMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    
                    // Generate noise-based glow
                    float noise = GenerateNoise(uv.x * m_noiseScale, uv.y * m_noiseScale, m_time);
                    float baseGlow = CalculateRadialGlow(dir.Length());
                    
                    float glow = baseGlow * (1.0f + noise * m_noiseStrength);
                    glowMap[y * width + x] = glow;
                }
            }

            ApplyGlowMap(target, glowMap);
        }

        Vector2 GlowEffect::ApplyRotationAndAspectRatio(const Vector2& dir) const
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

        float GlowEffect::CalculateRadialGlow(float distance) const
        {
            if (distance < m_innerRadius)
            {
                return m_intensity;
            }
            else if (distance > m_outerRadius)
            {
                return 0.0f;
            }
            else
            {
                float normalizedDistance = (distance - m_innerRadius) / (m_outerRadius - m_innerRadius);
                float falloff = Math::Pow(1.0f - normalizedDistance, m_falloff);
                return m_intensity * falloff;
            }
        }

        float GlowEffect::CalculateLinearGlow(const Vector2& dir) const
        {
            float distance = dir.Length();
            float directionalFactor = Math::Cos(Math::Atan2(dir.y, dir.x) - m_angle) * 0.5f + 0.5f;
            
            float glow = CalculateRadialGlow(distance);
            return glow * directionalFactor;
        }

        float GlowEffect::CalculateGaussianGlow(float distance) const
        {
            float sigma = m_radius * 0.3f;
            float gaussian = Math::Exp(-(distance * distance) / (2.0f * sigma * sigma));
            return m_intensity * gaussian;
        }

        float GlowEffect::CalculateBoxGlow(const Vector2& dir) const
        {
            float boxGlow = 1.0f;
            
            if (Math::Abs(dir.x) > m_outerRadius || Math::Abs(dir.y) > m_outerRadius)
            {
                boxGlow = 0.0f;
            }
            else if (Math::Abs(dir.x) < m_innerRadius && Math::Abs(dir.y) < m_innerRadius)
            {
                boxGlow = m_intensity;
            }
            else
            {
                float falloffX = 1.0f - Math::Pow(Math::Abs(dir.x) / m_outerRadius, m_falloff);
                float falloffY = 1.0f - Math::Pow(Math::Abs(dir.y) / m_outerRadius, m_falloff);
                boxGlow = m_intensity * falloffX * falloffY;
            }
            
            return boxGlow;
        }

        float GlowEffect::CalculateDirectionalGlow(const Vector2& dir) const
        {
            float distance = dir.Length();
            float angle = Math::Atan2(dir.y, dir.x);
            float angleDifference = Math::Abs(angle - m_angle);
            float directionalFactor = Math::Cos(angleDifference) * 0.5f + 0.5f;
            
            float glow = CalculateRadialGlow(distance);
            return glow * directionalFactor;
        }

        void GlowEffect::ApplyGlowMap(RenderTarget* target, const std::vector<float>& glowMap)
        {
            // Apply the glow map to the render target
            // This would be implemented using the rendering API
            // For now, we provide the structure

            // Create glow overlay
            RenderTarget* glowOverlay = CreateGlowOverlay(target);
            if (glowOverlay)
            {
                // Apply color and intensity
                ApplyColorAndIntensity(glowOverlay, glowMap);

                // Blend with original target
                BlendWithTarget(target, glowOverlay);

                // Cleanup
                DestroyGlowOverlay(glowOverlay);
            }
        }

        RenderTarget* GlowEffect::CreateGlowOverlay(RenderTarget* source)
        {
            // Create a render target for the glow overlay
            // Platform-specific implementation
            return nullptr; // Placeholder
        }

        void GlowEffect::ApplyColorAndIntensity(RenderTarget* overlay, const std::vector<float>& glowMap)
        {
            // Apply the glow color and intensity to the overlay
            // Platform-specific implementation
        }

        void GlowEffect::BlendWithTarget(RenderTarget* target, RenderTarget* overlay)
        {
            // Blend the overlay with the target using the specified blend mode
            // Platform-specific implementation
        }

        void GlowEffect::DestroyGlowOverlay(RenderTarget* overlay)
        {
            // Clean up the overlay render target
            // Platform-specific implementation
        }

        float GlowEffect::GenerateNoise(float x, float y, float time) const
        {
            // Simple noise function
            return Math::Sin(x * 2.0f + time) * Math::Cos(y * 2.0f + time) * 0.5f + 0.5f;
        }

        void GlowEffect::Reset()
        {
            m_intensity = 1.0f;
            m_radius = 10.0f;
            m_color = Vector3(1.0f, 1.0f, 1.0f);
            m_centerX = 0.5f;
            m_centerY = 0.5f;
            m_falloff = 1.0f;
            m_quality = Quality::Medium;
            m_enabled = true;
            m_time = 0.0f;
            m_animated = false;
            m_animationSpeed = 1.0f;
            m_pulseFrequency = 1.0f;
            m_pulseAmplitude = 0.0f;
            m_noiseScale = 0.1f;
            m_noiseStrength = 0.0f;
            m_blendMode = BlendMode::Additive;
            m_glowType = GlowType::Radial;
            m_innerRadius = 0.0f;
            m_outerRadius = 1.0f;
            m_angle = 0.0f;
            m_aspectRatio = 1.0f;
            m_rotation = 0.0f;

            UpdateGlowParameters();
            UpdateQualitySettings();

            RF_LOG_INFO("GlowEffect: Reset to default values");
        }

        void GlowEffect::Preset::ApplyPreset(GlowEffect* effect, PresetType type)
        {
            switch (type)
            {
                case PresetType::SoftGlow:
                    effect->SetGlowType(GlowType::Radial);
                    effect->SetIntensity(0.3f);
                    effect->SetRadius(5.0f);
                    effect->SetFalloff(2.0f);
                    break;

                case PresetType::MediumGlow:
                    effect->SetGlowType(GlowType::Radial);
                    effect->SetIntensity(0.6f);
                    effect->SetRadius(10.0f);
                    effect->SetFalloff(1.5f);
                    break;

                case PresetType::StrongGlow:
                    effect->SetGlowType(GlowType::Radial);
                    effect->SetIntensity(1.0f);
                    effect->SetRadius(15.0f);
                    effect->SetFalloff(1.0f);
                    break;

                case PresetType::IntenseGlow:
                    effect->SetGlowType(GlowType::Gaussian);
                    effect->SetIntensity(1.5f);
                    effect->SetRadius(20.0f);
                    effect->SetFalloff(0.8f);
                    break;

                case PresetType::NeonGlow:
                    effect->SetGlowType(GlowType::Radial);
                    effect->SetColor(0.0f, 1.0f, 1.0f);
                    effect->SetIntensity(1.2f);
                    effect->SetRadius(12.0f);
                    effect->SetFalloff(0.5f);
                    effect->SetBlendMode(BlendMode::Additive);
                    break;

                case PresetType::FireGlow:
                    effect->SetGlowType(GlowType::Radial);
                    effect->SetColor(1.0f, 0.5f, 0.0f);
                    effect->SetIntensity(0.8f);
                    effect->SetRadius(8.0f);
                    effect->SetFalloff(1.2f);
                    effect->SetBlendMode(BlendMode::Screen);
                    break;

                case PresetType::MagicalGlow:
                    effect->SetGlowType(GlowType::Noise);
                    effect->SetColor(0.8f, 0.4f, 1.0f);
                    effect->SetIntensity(0.7f);
                    effect->SetRadius(10.0f);
                    effect->SetNoiseScale(0.2f);
                    effect->SetNoiseStrength(0.3f);
                    break;
            }

            RF_LOG_INFO("GlowEffect: Applied preset ", (int)type);
        }

        void GlowEffect::SaveSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("GlowEffect: Saved settings to ", filename);
        }

        void GlowEffect::LoadSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("GlowEffect: Loaded settings from ", filename);
        }

    } // namespace Effects
} // namespace RedFiles