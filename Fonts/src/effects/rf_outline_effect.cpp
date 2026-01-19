#include "rf_outline_effect.h"
#include "rf_logger.h"

namespace RedFiles {
    namespace Effects {

        OutlineEffect::OutlineEffect()
            : m_isInitialized(false)
            , m_width(1.0f)
            , m_color(0.0f, 0.0f, 0.0f)
            , m_intensity(1.0f)
            , m_quality(Quality::Medium)
            , m_samples(16)
            , m_enabled(true)
            , m_time(0.0f)
            , m_animated(false)
            , m_animationSpeed(1.0f)
            m_pulseFrequency(1.0f)
            , m_pulseAmplitude(0.0f)
            , m_noiseScale(0.1f)
            , m_noiseStrength(0.0f)
            m_blendMode(BlendMode::Normal)
            , m_outlineType(OutlineType::Solid)
            m_innerRadius(0.0f)
            m_outerRadius(1.0f)
            m_centerX(0.5f)
            m_centerY(0.5f)
            m_angle(0.0f)
            m_aspectRatio(1.0f)
            m_rotation(0.0f)
            m_smoothness(1.0f)
            m_antialiasing(true)
            m_dithering(true)
            m_gammaCorrection(true)
            m_target(nullptr)
        {
            RF_LOG_INFO("OutlineEffect: Created outline effect");
        }

        OutlineEffect::~OutlineEffect()
        {
            RF_LOG_INFO("OutlineEffect: Destroyed outline effect");
        }

        bool OutlineEffect::Initialize()
        {
            if (m_isInitialized)
            {
                RF_LOG_WARNING("OutlineEffect: Already initialized");
                return true;
            }

            // Initialize outline parameters based on type
            UpdateOutlineParameters();

            // Initialize quality settings
            UpdateQualitySettings();

            // Initialize noise texture for noise-based outlines
            InitializeNoiseTexture();

            m_isInitialized = true;
            RF_LOG_INFO("OutlineEffect: Successfully initialized");
            return true;
        }

        void OutlineEffect::Update(float deltaTime)
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

            RF_LOG_DEBUG("OutlineEffect: Updated effect");
        }

        void OutlineEffect::Apply(RenderTarget* target)
        {
            if (!m_isInitialized || !m_enabled || !target)
            {
                RF_LOG_WARNING("OutlineEffect: Cannot apply effect - not initialized or disabled");
                return;
            }

            switch (m_outlineType)
            {
                case OutlineType::Solid:
                    ApplySolidOutline(target);
                    break;
                case OutlineType::Gradient:
                    ApplyGradientOutline(target);
                    break;
                case OutlineType::Glow:
                    ApplyLowOutline(target);
                    break;
                case OutlineType::High:
                    ApplyHighOutline(target);
                    break;
                case OutlineType::Noise:
                    ApplyNoiseOutline(target);
                    break;
                case OutlineType::Dashed:
                    ApplyDashedOutline(target);
                    break;
                case OutlineType::Dotted:
                    ApplyDottedOutline(target);
                    break;
                case OutlineType::Double:
                    ApplyDoubleOutline(target);
                    break;
                case OutlineType::GlowQuality:
                    ApplyLowQualityOutline(target);
                    break;
                case OutlineType::HighQuality:
                    ApplyHighQualityOutline(target);
                    break;
            }

            RF_LOG_DEBUG("OutlineEffect: Applied ", (int)m_outlineType, " outline effect");
        }

        void OutlineEffect::SetWidth(float width)
        {
            m_width = Math::Max(0.0f, width);
            RF_LOG_INFO("OutlineEffect: Set width to ", m_width);
        }

        void OutlineEffect::SetColor(const Vector3& color)
        {
            m_color = color;
            RF_LOG_INFO("OutlineEffect: Set color to (", m_color.x, ", ", m_color.y, ", m_color.z, ")");
        }

        void OutlineColor(float r, float g, float b)
        {
            m_color = Vector3(r, g, b);
            RF_LOG_INFO("OutlineEffect: Set color to (", r, ", ", g, ", b, ")");
        }

        void OutlineEffect::SetIntensity(float intensity)
        {
            m_intensity = Math::Max(0.0f, intensity);
            RF_LOG_INFO("OutlineEffect: Set intensity to ", m_intensity);
        }

        void OutlineEffect::SetQuality(Quality quality)
        {
            m_quality = quality;
            UpdateQualitySettings();
            RF_LOG_INFO("OutlineEffect: Set quality to ", (int)quality);
        }

        void OutlineEffect::SetOutlineType(OutlineType type)
        {
            m_outlineType = type;
            UpdateOutlineParameters();
            RF_LOG_INFO("OutlineEffect: Set outline type to ", (int)type);
        }

        void OutlineEffect::SetInnerRadius(float radius)
        {
            m_innerRadius = Math::Max(0.0f, radius);
            m_outerRadius = Math::Max(m_innerRadius, m_outerRadius);
            RF_LOG_INFO("OutlineEffect: Set inner radius to ", m_innerRadius);
        }

        void OutlineEffect::SetOuterRadius(float radius)
        {
            m_outerRadius = Math::Max(m_innerRadius, radius);
            RF_LOG_INFO("OutlineEffect: Set outer radius to ", m_outerRadius);
        }

        void OutlineEffect::SetCenter(float x, float y)
        {
            m_centerX = Math::Clamp(x, 0.0f, 1.0f);
            m_centerY = Math::Clamp(y, 0.0f, 1.0f);
            RF_LOG_INFO("OutlineEffect: Set center to (", m_centerX, ", ", m_centerY, ")");
        }

        void OutlineEffect::SetAngle(float angle)
        {
            m_angle = angle;
            RF_LOG_INFO("OutlineEffect: Set angle to ", m_angle);
        }

        void OutlineEffect::SetAspectRatio(float aspectRatio)
        {
            m_aspectRatio = Math::Max(0.1f, aspectRatio);
            RF_LOG_INFO("OutlineEffect: Set aspect ratio to ", m_aspectRatio);
        }

        void OutlineEffect::SetRotation(float rotation)
        {
            m_rotation = rotation;
            RF_LOG_INFO("OutlineEffect: Set rotation to ", m_rotation);
        }

        void OutlineEffect::SetSmoothness(float smoothness)
        {
            m_smoothness = Math::Max(0.0f, smoothness);
            RF_LOG_INFO("OutlineEffect: Set smoothness to ", m_smoothness);
        }

        void OutlineEffect::SetAntialiasing(bool enabled)
        {
            m_antialiasing = enabled;
            RF_LOG_INFO("OutlineEffect: Set antialiasing to ", enabled);
        }

        void OutlineEffect::SetDithering(bool enabled)
        {
            m_dithering = enabled;
            RF_LOG_INFO("OutlineEffect: Set dithering to ", enabled);
        }

        void OutlineEffect::SetGammaCorrection(bool enabled)
        {
            m_gammaCorrection = enabled;
            RF_LOG_INFO("OutlineEffect: Set gamma correction to ", enabled);
        }

        void OutlineEffect::SetAnimated(bool animated)
        {
            m_animated = animated;
            RF_LOG_INFO("OutlineEffect: Set animated to ", animated);
        }

        void OutlineEffect::SetAnimationSpeed(float speed)
        {
            m_animationSpeed = Math::Max(0.0f, speed);
            RF_LOG_INFO("OutlineEffect: Set animation speed to ", m_animationSpeed);
        }

        void OutlineEffect::SetPulseFrequency(float frequency)
        {
            m_pulseFrequency = Math::Max(0.1f, frequency);
            RF_LOG_INFO("OutlineEffect: Set pulse frequency to ", m_pulseFrequency);
        }

        void OutlineEffect::SetPulseAmplitude(float amplitude)
        {
            m_pulseAmplitude = Math::Max(0.0f, amplitude);
            RF_LOG_INFO("OutlineEffect: Set pulse amplitude to ", m_pulseAmplitude);
        }

        void OutlineEffect::SetNoiseScale(float scale)
        {
            m_noiseScale = Math::Max(0.01f, scale);
            RF_LOG_INFO("OutlineEffect: Set noise scale to ", m_noiseScale);
        }

        void OutlineEffect::SetNoiseStrength(float strength)
        {
            m_noiseStrength = Math::Max(0.0f, strength);
            RF_LOG_INFO("OutlineEffect: Set noise strength to ", m_noiseStrength);
        }

        void OutlineEffect::SetBlendMode(BlendMode blendMode)
        {
            m_blendMode = blendMode;
            RF_LOG_INFO("OutlineEffect: Set blend mode to ", (int)blendMode);
        }

        float OutlineEffect::GetWidth() const
        {
            return m_width;
        }

        Vector3 OutlineEffect::GetColor() const
        {
            return m_color;
        }

        float OutlineEffect::GetIntensity() const
        {
            return m_intensity;
        }

        Quality OutlineEffect::GetQuality() const
        {
            return m_quality;
        }

        OutlineType OutlineEffect::GetOutlineType() const
        {
            return m_outlineType;
        }

        float OutlineEffect::GetInnerRadius() const
        {
            return m_innerRadius;
        }

        float OutlineEffect::GetOuterRadius() const
        {
            return m_outerRadius;
        }

        Vector2 OutlineEffect::GetCenter() const
        {
            return Vector2(m_centerX, m_centerY);
        }

        float OutlineEffect::GetAngle() const
        {
            return m_angle;
        }

        float OutlineEffect::GetAspectRatio() const
        {
            return m_aspectRatio;
        }

        float OutlineEffect::GetRotation() const
        {
            return m_rotation;
        }

        float OutlineEffect::GetSmoothness() const
        {
            return m_smoothness;
        }

        bool OutlineEffect::IsAntialiased() const
        {
            return m_antialiasing;
        }

        bool OutlineEffect::IsDithered() const
        {
            return m_dithering;
        }

        bool OutlineEffect::IsGammaCorrected() const
        {
            return m_gammaCorrection;
        }

        bool OutlineEffect::IsAnimated() const
        {
            return m_animated;
        }

        bool OutlineEffect::IsEnabled() const
        {
            return m_enabled;
        }

        bool OutlineEffect::IsInitialized() const
        {
            return m_isInitialized;
        }

        void OutlineEffect::UpdateAnimation(float deltaTime)
        {
            float animPhase = m_time * m_animationSpeed;

            // Pulse animation for outline width
            float widthModulation = Math::Sin(animPhase * m_pulseFrequency * Math::PI * 2.0f) * m_pulseAmplitude;
            m_width = Math::Max(0.1f, m_width * (1.0f + widthModulation));

            // Pulse animation for intensity
            float intensityModulation = Math::Sin(animPhase * 0.7f) * 0.2f;
            m_intensity = Math::Max(0.0f, m_intensity * (1.0f + intensityModulation));

            // Pulse animation for color
            float colorModulation = Math::Sin(animPhase * 1.3f) * 0.1f;
            m_color.x = Math::Clamp(m_color.x * (1.0f + colorModulation), 0.0f, 1.0f);
            m_color.y = Math::Clamp(m_color.y * (1.0f + colorModulation * 0.8f), 0.0f, 1.0f);
            m_color.z = Math::Clamp(m_color.z * (1.0f + colorModulation * 0.6f), 0.0f, 1.0f);

            // Pulse animation for inner/outer radius
            float radiusModulation = Math::Sin(animPhase * 0.5f) * 0.1f;
            float innerRadiusModulation = Math::Sin(animPhase * 0.3f) * 0.05f;
            float outerRadiusModulation = Math::Sin(animPhase * 0.7f) * 0.05f;
            
            m_innerRadius = Math::Max(0.0f, m_innerRadius * (1.0f + innerRadiusModulation));
            m_outerRadius = Math::Max(m_innerRadius, m_outerRadius * (1.0f + outerRadiusModulation));

            RF_LOG_DEBUG("OutlineEffect: Updated animation");
        }

        void OutlineEffect::UpdateDynamicParameters(float deltaTime)
        {
            // Update time-based effects
            float timeModulation = Math::Sin(m_time * 0.5f) * 0.05f;
            
            // Dynamic width pulsing
            m_width = Math::Max(0.1f, m_width + timeModulation * deltaTime);
            
            // Dynamic smoothness
            m_smoothness = Math::Max(0.1f, m_smoothness + Math::Cos(m_time * 0.3f) * 0.02f * deltaTime);
            
            // Dynamic noise
            if (m_noiseStrength > 0.0f)
            {
                float noisePhase = m_time * 2.0f;
                m_noiseStrength = Math::Max(0.0f, m_noiseStrength + Math::Sin(noisePhase) * 0.1f * deltaTime);
            }
        }

        void OutlineEffect::UpdateNoise(float deltaTime)
        {
            // Update noise texture or parameters
            // This would be implemented using the rendering API
        }

        void OutlineEffect::UpdateOutlineParameters()
        {
            // Set default parameters based on outline type
            switch (m_outlineType)
            {
                case OutlineType::Solid:
                    m_innerRadius = 0.0f;
                    m_outerRadius = 1.0f;
                    m_smoothness = 1.0f;
                    break;
                case OutlineType::Gradient:
                    m_innerRadius = 0.0f;
                    m_outerRadius = 1.0f;
                    m_smoothness = 0.5f;
                    break;
                case OutlineType::Low:
                    m_innerRadius = 0.0f;
                    m_outerRadius = 0.5f;
                    m_smoothness = 0.3f;
                    break;
                case OutlineType::High:
                    m_innerRadius = 0.0f;
                    m_outerRadius = 1.5f;
                    m_smoothness = 2.0f;
                    break;
                case OutlineType::Noise:
                    m_noiseScale = 0.1f;
                    m_noiseStrength = 0.5f;
                    break;
                case OutlineType::Dashed:
                    m_dashPattern = {0.2f, 0.2f};
                    m_dashCount = 5;
                    break;
                case OutlineType::Dotted:
                    m_dotSpacing = 0.3f;
                    m_dotCount = 10;
                    break;
                case OutlineType::Double:
                    m_innerRadius = 0.0f;
                    m_outerRadius = 1.2f;
                    m_smoothness = 1.5f;
                    break;
                case OutlineType::LowQuality:
                    m_samples = 8;
                    m_antialiasing = false;
                    m_dithering = false;
                    m_gammaCorrection = false;
                    break;
                case OutlineType::HighQuality:
                    m_samples = 32;
                    m_antialiasing = true;
                    m_dithering = true;
                    m_gammaCorrection = true;
                    break;
            }
        }

        void OutlineEffect::UpdateQualitySettings()
        {
            switch (m_quality)
            {
                case Quality::Low:
                    m_samples = 8;
                    m_antialiasing = false;
                    m_dithering = false;
                    m_gammaCorrection = false;
                    break;
                case Quality::Medium:
                    m_samples = 16;
                    m_antialiasing = true;
                    m_dithering = true;
                    m_gammaCorrection = false;
                    break;
                case Quality::High:
                    m_samples = 32;
                    m_antialiasing = true;
                    m_dithering = true;
                    m_gammaCorrection = true;
                    break;
                case Quality::Ultra:
                    m_samples = 64;
                    m_antialiasing = true;
                    m_dithering = true;
                    m_gammaCorrection = true;
                    break;
            }
        }

        void OutlineEffect::InitializeNoiseTexture()
        {
            // Initialize noise texture for noise-based outlines
            // This would be implemented using the rendering API
        }

        void OutlineEffect::ApplySolidOutline(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            // Create outline map
            std::vector<float> outlineMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 transformedDir = ApplyRotationAndAspectRatio(dir);
                    
                    float outline = CalculateSolidOutline(transformedDir.Length());
                    outlineMap[y * width + x] = outline;
                }
            }

            ApplyOutlineMap(target, outlineMap);
        }

        void OutlineEffect::ApplyGradientOutline(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> outlineMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 transformedDir = ApplyRotationAndAspectRatio(dir);
                    
                    // Gradient outline - softer edges
                    float gradientFactor = 1.0f - (distance / m_outerRadius);
                    float outline = m_intensity * gradientFactor * m_smoothness;
                    outlineMap[y * width + x] = outline;
                }
            }

            ApplyOutlineMap(target, outlineMap);
        }

        void OutlineEffect::ApplyLowOutline(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> outlineMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 transformedDir = ApplyRotationAndAspectRatio(dir);
                    
                    // Low quality - simple falloff
                    float falloff = 1.0f - (distance / (m_outerRadius * 2.0f));
                    float outline = m_intensity * falloff * m_smoothness;
                    outlineMap[y * width + x] = outline;
                }
            }

            ApplyOutlineMap(target, outlineMap);
        }

        void OutlineEffect::ApplyHighOutline(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> outlineMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 transformedDir = ApplyRotationAndAspectRatio(dir);
                    
                    // High quality - enhanced falloff and smoothness
                    float falloff = Math::Pow(1.0f - (distance / (m_outerRadius * 2.0f), 2.0f);
                    float smoothnessFactor = 1.0f + (m_smoothness - 1.0f) * 0.5f;
                    float outline = m_intensity * falloff * smoothnessFactor;
                    outlineMap[y * width + x] = outline;
                }
            }

            ApplyOutlineMap(target, outlineMap);
        }

        void OutlineEffect::ApplyNoiseOutline(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> outlineMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 transformedDir = ApplyRotationAndAspectRatio(dir);
                    
                    // Base outline
                    float baseOutline = CalculateSolidOutline(transformedDir.Length());
                    
                    // Add noise variation
                    float noise = GenerateNoise(uv.x * m_noiseScale, uv.y * m_noiseScale, m_time);
                    float noiseFactor = 1.0f + noise * m_noiseStrength;
                    
                    float outline = baseOutline * noiseFactor;
                    outlineMap[y * width + x] = outline;
                }
            }

            ApplyOutlineMap(target, outlineMap);
        }

        void OutlineEffect::ApplyDashedOutline(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> outlineMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 transformedDir = ApplyRotationAndAspectRatio(dir);
                    
                    float outline = CalculateSolidOutline(transformedDir.Length());
                    
                    // Apply dashed pattern
                    bool isDash = ((int)(x + y) % (int)(m_dashCount * m_dashPattern[0]) < (int)(m_dashPattern[0] + m_dashPattern[1]));
                    float dashFactor = isDash ? 0.0f : 1.0f;
                    
                    outline = outline * dashFactor;
                    outlineMap[y * width + x] = outline;
                }
            }

            ApplyOutlineMap(target, outlineMap);
        }

        void OutlineEffect::ApplyDottedOutline(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> outlineMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 transformedDir = ApplyRotationAndAspectRatio(dir);
                    
                    float outline = CalculateSolidOutline(transformedDir.Length());
                    outlineMap[y * width + x] = outline;
                }
            }

            ApplyOutlineMap(target, outlineMap);
        }

        void OutlineEffect::ApplyDoubleOutline(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> outlineMap(width * height);
            std::vector<float> innerOutlineMap(width * height);

            // First pass - inner outline
            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 transformedDir = ApplyRotationAndAspectRatio(dir);
                    
                    float innerOutline = CalculateSolidOutline(transformedDir.Length());
                    innerOutlineMap[y * width + x] = innerOutline;
                }
            }

            // Second pass - outer outline
            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 transformedDir = ApplyRotationAndAspectRatio(dir);
                    
                    float outerOutline = CalculateSolidOutline(transformedDir.Length());
                    float innerOutline = innerOutlineMap[y * width + x];
                    float combinedOutline = outerOutline + innerOutline;
                    outlineMap[y * width + x] = combinedOutline;
                }
            }

            ApplyOutlineMap(target, outlineMap);
        }

        void OutlineEffect::ApplyLowQualityOutline(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> outlineMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 transformedDir = ApplyRotationAndAspectRatio(dir);
                    
                    // Low quality - simple falloff
                    float falloff = 1.0f - (distance / (m_outerRadius * 2.0f));
                    float outline = m_intensity * falloff * 0.3f; // Lower quality
                    outlineMap[y * width + x] = outline;
                }
            }

            ApplyOutlineMap(target, outlineMap);
        }

        void OutlineEffect::ApplyHighQualityOutline(RenderTarget* target)
        {
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            std::vector<float> outlineMap(width * height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    Vector2 uv((float)x / (float)width, (float)y / (float)height);
                    Vector2 center(m_centerX, m_centerY);
                    
                    Vector2 dir = uv - center;
                    float distance = dir.Length();
                    
                    // Apply aspect ratio and rotation
                    Vector2 transformedDir = ApplyRotationAndAspectRatio(dir);
                    
                    // High quality - enhanced falloff and smoothness
                    float falloff = Math::Pow(1.0f - (distance / (m_outerRadius * 2.0f), 2.0f);
                    float smoothnessFactor = 1.0f + (m_smoothness - 1.0f) * 0.5f;
                    float outline = m_intensity * falloff * smoothnessFactor;
                    outlineMap[y * width + x] = outline;
                }
            }

            ApplyOutlineMap(target, outlineMap);
        }

        void OutlineEffect::ApplyOutlineMap(RenderTarget* target, const std::vector<float>& outlineMap)
        {
            // Apply the outline map to the render target
            // This would be implemented using the rendering API
            // For now, we provide the structure

            // Create outline overlay
            RenderTarget* outlineOverlay = CreateOutlineOverlay(target);
            if (outlineOverlay)
            {
                // Apply color and intensity
                ApplyColorAndIntensity(outlineOverlay, outlineMap);

                // Blend with original target
                BlendWithTarget(target, outlineOverlay);

                // Cleanup
                DestroyOutlineOverlay(outlineOverlay);
            }
        }

        RenderTarget* OutlineEffect::CreateOutlineOverlay(RenderTarget* source)
        {
            // Create a render target for the outline overlay
            // Platform-specific implementation
            return nullptr; // Placeholder
        }

        void OutlineEffect::ApplyColorAndIntensity(RenderTarget* overlay, const std::vector<float>& outlineMap)
        {
            // Apply the outline color and intensity to the overlay
            // Platform-specific implementation
        }

        void OutlineEffect::BlendWithTarget(RenderTarget* target, RenderTarget* overlay)
        {
            // Blend the overlay with the target using the specified blend mode
            // Platform-specific implementation
        }

        void OutlineEffect::DestroyOutlineOverlay(RenderTarget* overlay)
        {
            // Clean up the overlay render target
            // Platform-specific implementation
        }

        Vector2 OutlineEffect::ApplyRotationAndAspectRatio(const Vector2& dir) const
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

        float OutlineEffect::CalculateSolidOutline(float distance) const
        {
            if (distance <= m_innerRadius)
            {
                return m_intensity;
            }
            else if (distance >= m_outerRadius)
            {
                return 0.0f;
            }
            else
            {
                // Smooth falloff
                float normalizedDistance = (distance - m_innerRadius) / (m_outerRadius - m_innerRadius);
                float falloff = Math::Pow(1.0f - normalizedDistance, m_falloff);
                return m_intensity * falloff;
            }
        }

        void OutlineEffect::SetDashPattern(const std::vector<float>& pattern, uint32_t dashCount)
        {
            m_dashPattern = pattern;
            m_dashCount = dashCount;
            RF_LOG_INFO("OutlineEffect: Set dash pattern with ", dashCount, " dashes");
        }

        void OutlineEffect::SetDotSpacing(float spacing)
        {
            m_dotSpacing = spacing;
            RF_LOG_INFO("OutlineEffect: Set dot spacing to ", spacing);
        }

        void OutlineEffect::Reset()
        {
            m_width = 1.0f;
            m_color = Vector3(0.0f, 0.0f, 0.0f);
            m_intensity = 1.0f;
            m_quality = Quality::Medium;
            m_samples = 16;
            m_enabled = true;
            m_time = 0.0f;
            m_animated = false;
            m_animationSpeed = 1.0f;
            m_pulseFrequency = 1.0f;
            m_pulseAmplitude = 0.0f;
            m_noiseScale = 0.1f;
            m_noiseStrength = 0.0f;
            m_blendMode = BlendMode::Normal;
            m_outlineType = OutlineType::Solid;
            m_innerRadius = 0.0f;
            m_outerRadius = 1.0f;
            m_centerX = 0.5f;
            m_centerY = 0.5f;
            m_angle = 0.0f;
            m_aspectRatio = 1.0f;
            m_rotation = 0.0f;
            m_smoothness = 1.0f;
            m_antialiasing = true;
            m_dithering = true;
            m_gammaCorrection = false;
            m_target = nullptr;

            UpdateOutlineParameters();
            UpdateQualitySettings();

            RF_LOG_INFO("OutlineEffect: Reset to default values");
        }

        void OutlineEffect::Preset::ApplyPreset(OutlineEffect* effect, PresetType type)
        {
            switch (type)
            {
                case PresetType::ThinOutline:
                    effect->SetOutlineType(OutlineType::Low);
                    effect->SetWidth(0.5f);
                    effect->SetColor(1.0f, 1.0f, 1.0f);
                    effect->SetIntensity(0.8f);
                    break;

                case PresetType::NormalOutline:
                    effect->SetOutlineType(OutlineType::Solid);
                    effect->SetWidth(1.0f);
                    effect->SetColor(1.0f, 1.0f, 1.0f);
                    effect->SetIntensity(1.0f);
                    break;

                case PresetType::ThickOutline:
                    effect->SetOutlineType(OutlineType::Solid);
                    effect->SetWidth(2.0f);
                    effect->SetColor(1.0f, 1.0f, 1.0f);
                    effect->SetIntensity(1.2f);
                    break;

                case PresetType::DoubleOutline:
                    effect->SetOutlineType(OutlineType::Double);
                    effect->SetWidth(1.5f);
                    effect->SetColor(1.0f, 1.0f, 1.0f);
                    effect->SetIntensity(1.1f);
                    break;

                case Preset::SoftOutline:
                    effect->SetOutlineType(OutlineType::Low);
                    effect->SetWidth(0.5f);
                    effect->SetColor(0.8f, 0.8f, 0.8f);
                    effect->SetIntensity(0.6f);
                    break;

                case Preset::HardOutline:
                    effect->SetOutlineType(OutlineType::High);
                    effect->SetWidth(1.5f);
                    effect->SetColor(0.2f, 0.2f, 0.2f);
                    effect->SetIntensity(1.5f);
                    break;

                case Preset::NeonOutline:
                    effect->SetOutlineType(OutlineType::Noise);
                    effect->SetColor(0.0f, 1.0f, 1.0f);
                    effect->SetIntensity(0.7f);
                    effect->SetNoiseScale(0.2f);
                    effect->SetNoiseStrength(0.3f);
                    break;

                case Preset::GlowQualityOutline:
                    effect->SetQuality(Quality::Low);
                    effect->SetAntialiasing(false);
                    effect->SetDithering(false);
                    effect->SetGammaCorrection(false);
                    break;

                case Preset::HighQualityOutline:
                    effect->SetQuality(Quality::High);
                    effect->SetAntialiasing(true);
                    effect->SetDithering(true);
                    effect->SetGammaCorrection(true);
                    break;

                case Preset::MagicalOutline:
                    effect->SetOutlineType(OutlineType::Noise);
                    effect->SetColor(0.8f, 0.4f, 1.0f);
                    effect->SetIntensity(0.9f);
                    effect->SetNoiseScale(0.3f);
                    effect->SetNoiseStrength(0.4f);
                    effect->SetAnimated(true);
                    effect->SetAnimationSpeed(2.0f);
                    effect->SetPulseFrequency(2.0f);
                    effect->SetPulseAmplitude(0.3f);
                    break;
            }

            RF_LOG_INFO("OutlineEffect: Applied preset ", (int)type);
        }

        void OutlineEffect::SaveSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("OutlineEffect: Saved settings to ", filename);
        }

        void OutlineEffect::LoadSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("OutlineEffect: Loaded settings from ", filename);
        }

    } // namespace Effects
} // namespace RedFiles