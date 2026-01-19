#include "rf_chromatic_aberration.h"
#include "rf_logger.h"

namespace RedFiles {
    namespace Effects {

        ChromaticAberration::ChromaticAberration()
            : m_isInitialized(false)
            , m_intensity(0.5f)
            , m_offset(1.0f)
            , m_chromaticShift(0.0f)
            , m_angle(0.0f)
            , m_centerX(0.5f)
            , m_centerY(0.5f)
            , m_radius(1.0f)
            , m_falloff(1.0f)
            , m_quality(Quality::Medium)
            , m_channels(3)
            , m_redOffset(1.0f, 0.0f)
            , m_greenOffset(0.0f, 0.0f)
            , m_blueOffset(-1.0f, 0.0f)
            , m_enabled(true)
            , m_time(0.0f)
            , m_animated(false)
            , m_animationSpeed(1.0f)
            , m_animationPhase(0.0f)
        {
            RF_LOG_INFO("ChromaticAberration: Created chromatic aberration effect");
        }

        ChromaticAberration::~ChromaticAberration()
        {
            RF_LOG_INFO("ChromaticAberration: Destroyed chromatic aberration effect");
        }

        bool ChromaticAberration::Initialize()
        {
            if (m_isInitialized)
            {
                RF_LOG_WARNING("ChromaticAberration: Already initialized");
                return true;
            }

            // Initialize default channel offsets
            m_redOffset = Vector2(1.0f, 0.0f);
            m_greenOffset = Vector2(0.0f, 0.0f);
            m_blueOffset = Vector2(-1.0f, 0.0f);

            // Initialize quality settings
            UpdateQualitySettings();

            m_isInitialized = true;
            RF_LOG_INFO("ChromaticAberration: Successfully initialized");
            return true;
        }

        void ChromaticAberration::Update(float deltaTime)
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

            RF_LOG_DEBUG("ChromaticAberration: Updated effect");
        }

        void ChromaticAberration::Apply(RenderTarget* target)
        {
            if (!m_isInitialized || !m_enabled || !target)
            {
                RF_LOG_WARNING("ChromaticAberration: Cannot apply effect - not initialized or disabled");
                return;
            }

            // Get render target dimensions
            uint32_t width = target->GetWidth();
            uint32_t height = target->GetHeight();

            // Calculate aberration offsets based on current settings
            Vector2 redOffset = CalculateChannelOffset(m_redOffset, width, height);
            Vector2 greenOffset = CalculateChannelOffset(m_greenOffset, width, height);
            Vector2 blueOffset = CalculateChannelOffset(m_blueOffset, width, height);

            // Apply chromatic aberration to each channel
            ApplyChannelAberration(target, redOffset, greenOffset, blueOffset);

            RF_LOG_DEBUG("ChromaticAberration: Applied effect to render target");
        }

        void ChromaticAberration::SetIntensity(float intensity)
        {
            m_intensity = Math::Clamp(intensity, 0.0f, 2.0f);
            RF_LOG_INFO("ChromaticAberration: Set intensity to ", m_intensity);
        }

        void ChromaticAberration::SetOffset(float offset)
        {
            m_offset = Math::Max(0.0f, offset);
            RF_LOG_INFO("ChromaticAberration: Set offset to ", m_offset);
        }

        void ChromaticAberration::SetChromaticShift(float shift)
        {
            m_chromaticShift = Math::Clamp(shift, -1.0f, 1.0f);
            RF_LOG_INFO("ChromaticAberration: Set chromatic shift to ", m_chromaticShift);
        }

        void ChromaticAberration::SetAngle(float angle)
        {
            m_angle = angle;
            UpdateChannelOffsets();
            RF_LOG_INFO("ChromaticAberration: Set angle to ", m_angle);
        }

        void ChromaticAberration::SetCenter(float x, float y)
        {
            m_centerX = Math::Clamp(x, 0.0f, 1.0f);
            m_centerY = Math::Clamp(y, 0.0f, 1.0f);
            RF_LOG_INFO("ChromaticAberration: Set center to (", m_centerX, ", ", m_centerY, ")");
        }

        void ChromaticAberration::SetRadius(float radius)
        {
            m_radius = Math::Max(0.0f, radius);
            RF_LOG_INFO("ChromaticAberration: Set radius to ", m_radius);
        }

        void ChromaticAberration::SetFalloff(float falloff)
        {
            m_falloff = Math::Max(0.0f, falloff);
            RF_LOG_INFO("ChromaticAberration: Set falloff to ", m_falloff);
        }

        void ChromaticAberration::SetQuality(Quality quality)
        {
            m_quality = quality;
            UpdateQualitySettings();
            RF_LOG_INFO("ChromaticAberration: Set quality to ", (int)quality);
        }

        void ChromaticAberration::SetChannelOffsets(const Vector2& red, const Vector2& green, const Vector2& blue)
        {
            m_redOffset = red;
            m_greenOffset = green;
            m_blueOffset = blue;
            RF_LOG_INFO("ChromaticAberration: Set channel offsets");
        }

        void ChromaticAberration::SetEnabled(bool enabled)
        {
            m_enabled = enabled;
            RF_LOG_INFO("ChromaticAberration: Set enabled to ", enabled);
        }

        void ChromaticAberration::SetAnimated(bool animated)
        {
            m_animated = animated;
            RF_LOG_INFO("ChromaticAberration: Set animated to ", animated);
        }

        void ChromaticAberration::SetAnimationSpeed(float speed)
        {
            m_animationSpeed = Math::Max(0.0f, speed);
            RF_LOG_INFO("ChromaticAberration: Set animation speed to ", m_animationSpeed);
        }

        float ChromaticAberration::GetIntensity() const
        {
            return m_intensity;
        }

        float ChromaticAberration::GetOffset() const
        {
            return m_offset;
        }

        float ChromaticAberration::GetChromaticShift() const
        {
            return m_chromaticShift;
        }

        float ChromaticAberration::GetAngle() const
        {
            return m_angle;
        }

        Vector2 ChromaticAberration::GetCenter() const
        {
            return Vector2(m_centerX, m_centerY);
        }

        float ChromaticAberration::GetRadius() const
        {
            return m_radius;
        }

        float ChromaticAberration::GetFalloff() const
        {
            return m_falloff;
        }

        Quality ChromaticAberration::GetQuality() const
        {
            return m_quality;
        }

        bool ChromaticAberration::IsEnabled() const
        {
            return m_enabled;
        }

        bool ChromaticAberration::IsAnimated() const
        {
            return m_animated;
        }

        float ChromaticAberration::GetAnimationSpeed() const
        {
            return m_animationSpeed;
        }

        bool ChromaticAberration::IsInitialized() const
        {
            return m_isInitialized;
        }

        void ChromaticAberration::UpdateAnimation(float deltaTime)
        {
            m_animationPhase += m_animationSpeed * deltaTime;

            // Animate intensity
            float intensityModulation = Math::Sin(m_animationPhase) * 0.5f + 0.5f;
            m_intensity = Math::Clamp(m_intensity * (0.5f + intensityModulation), 0.0f, 2.0f);

            // Animate angle
            float angleModulation = Math::Sin(m_animationPhase * 0.7f) * 0.3f;
            SetAngle(m_angle + angleModulation);

            // Animate chromatic shift
            float shiftModulation = Math::Sin(m_animationPhase * 1.3f) * 0.2f;
            m_chromaticShift = Math::Clamp(m_chromaticShift + shiftModulation, -1.0f, 1.0f);
        }

        void ChromaticAberration::UpdateDynamicParameters(float deltaTime)
        {
            // Update time-based effects
            float timeModulation = Math::Sin(m_time * 0.5f) * 0.1f;
            
            // Dynamic radius pulsing
            m_radius = Math::Max(0.0f, m_radius + timeModulation * deltaTime);
            
            // Dynamic falloff
            m_falloff = Math::Max(0.0f, m_falloff + Math::Cos(m_time * 0.3f) * 0.05f * deltaTime);
        }

        void ChromaticAberration::UpdateQualitySettings()
        {
            switch (m_quality)
            {
                case Quality::Low:
                    m_channels = 3;
                    break;
                case Quality::Medium:
                    m_channels = 3;
                    break;
                case Quality::High:
                    m_channels = 3;
                    break;
                case Quality::Ultra:
                    m_channels = 4; // Include alpha channel
                    break;
            }
        }

        void ChromaticAberration::UpdateChannelOffsets()
        {
            // Calculate offsets based on angle
            float cosAngle = Math::Cos(m_angle);
            float sinAngle = Math::Sin(m_angle);

            // Red channel offset
            m_redOffset.x = cosAngle * m_offset;
            m_redOffset.y = sinAngle * m_offset;

            // Green channel offset (minimal)
            m_greenOffset.x = cosAngle * m_offset * 0.1f;
            m_greenOffset.y = sinAngle * m_offset * 0.1f;

            // Blue channel offset (opposite direction)
            m_blueOffset.x = -cosAngle * m_offset;
            m_blueOffset.y = -sinAngle * m_offset;

            // Apply chromatic shift
            m_redOffset.x += m_chromaticShift;
            m_blueOffset.x -= m_chromaticShift;
        }

        Vector2 ChromaticAberration::CalculateChannelOffset(const Vector2& baseOffset, uint32_t width, uint32_t height) const
        {
            Vector2 offset = baseOffset;

            // Apply intensity
            offset.x *= m_intensity;
            offset.y *= m_intensity;

            // Apply falloff based on distance from center
            float maxDistance = Math::Sqrt(m_centerX * m_centerX + m_centerY * m_centerY);
            float distance = Math::Sqrt(offset.x * offset.x + offset.y * offset.y);
            float falloffFactor = 1.0f - Math::Pow(distance / maxDistance, m_falloff);
            
            offset.x *= falloffFactor;
            offset.y *= falloffFactor;

            // Convert to pixel coordinates
            offset.x *= (float)width * 0.5f;
            offset.y *= (float)height * 0.5f;

            return offset;
        }

        void ChromaticAberration::ApplyChannelAberration(RenderTarget* target, 
                                                       const Vector2& redOffset,
                                                       const Vector2& greenOffset,
                                                       const Vector2& blueOffset)
        {
            // This would be implemented using the rendering API
            // For now, we'll provide the structure
            
            // Create temporary render targets for each channel
            RenderTarget* redTarget = CreateChannelTarget(target, Channel::Red);
            RenderTarget* greenTarget = CreateChannelTarget(target, Channel::Green);
            RenderTarget* blueTarget = CreateChannelTarget(target, Channel::Blue);

            if (redTarget && greenTarget && blueTarget)
            {
                // Apply offsets to each channel
                OffsetChannel(redTarget, redOffset);
                OffsetChannel(greenTarget, greenOffset);
                OffsetChannel(blueTarget, blueOffset);

                // Combine channels back to main target
                CombineChannels(target, redTarget, greenTarget, blueTarget);

                // Cleanup temporary targets
                DestroyChannelTarget(redTarget);
                DestroyChannelTarget(greenTarget);
                DestroyChannelTarget(blueTarget);
            }
        }

        RenderTarget* ChromaticAberration::CreateChannelTarget(RenderTarget* source, Channel channel)
        {
            // Platform-specific implementation
            // Create a render target for the specified channel
            return nullptr; // Placeholder
        }

        void ChromaticAberration::OffsetChannel(RenderTarget* target, const Vector2& offset)
        {
            // Platform-specific implementation
            // Apply the offset to the channel
        }

        void ChromaticAberration::CombineChannels(RenderTarget* dest, 
                                                  RenderTarget* red,
                                                  RenderTarget* green,
                                                  RenderTarget* blue)
        {
            // Platform-specific implementation
            // Combine the three channels into the destination
        }

        void ChromaticAberration::DestroyChannelTarget(RenderTarget* target)
        {
            // Platform-specific implementation
            // Clean up the temporary target
        }

        void ChromaticAberration::Reset()
        {
            m_intensity = 0.5f;
            m_offset = 1.0f;
            m_chromaticShift = 0.0f;
            m_angle = 0.0f;
            m_centerX = 0.5f;
            m_centerY = 0.5f;
            m_radius = 1.0f;
            m_falloff = 1.0f;
            m_quality = Quality::Medium;
            m_enabled = true;
            m_time = 0.0f;
            m_animated = false;
            m_animationSpeed = 1.0f;
            m_animationPhase = 0.0f;

            UpdateChannelOffsets();
            UpdateQualitySettings();

            RF_LOG_INFO("ChromaticAberration: Reset to default values");
        }

        void ChromaticAberration::Preset::ApplyPreset(ChromaticAberration* effect, PresetType type)
        {
            switch (type)
            {
                case PresetType::Subtle:
                    effect->SetIntensity(0.2f);
                    effect->SetOffset(0.5f);
                    effect->SetChromaticShift(0.1f);
                    effect->SetAngle(0.0f);
                    effect->SetRadius(0.8f);
                    effect->SetFalloff(2.0f);
                    break;

                case PresetType::Moderate:
                    effect->SetIntensity(0.5f);
                    effect->SetOffset(1.0f);
                    effect->SetChromaticShift(0.3f);
                    effect->SetAngle(Math::PI * 0.25f);
                    effect->SetRadius(1.0f);
                    effect->SetFalloff(1.5f);
                    break;

                case PresetType::Strong:
                    effect->SetIntensity(0.8f);
                    effect->SetOffset(2.0f);
                    effect->SetChromaticShift(0.5f);
                    effect->SetAngle(Math::PI * 0.5f);
                    effect->SetRadius(1.2f);
                    effect->SetFalloff(1.0f);
                    break;

                case PresetType::Extreme:
                    effect->SetIntensity(1.2f);
                    effect->SetOffset(3.0f);
                    effect->SetChromaticShift(0.8f);
                    effect->SetAngle(Math::PI * 0.75f);
                    effect->SetRadius(1.5f);
                    effect->SetFalloff(0.8f);
                    break;

                case PresetType::Vintage:
                    effect->SetIntensity(0.4f);
                    effect->SetOffset(0.8f);
                    effect->SetChromaticShift(-0.2f);
                    effect->SetAngle(Math::PI * 0.1f);
                    effect->SetRadius(0.6f);
                    effect->SetFalloff(1.8f);
                    break;

                case PresetType::SciFi:
                    effect->SetIntensity(0.6f);
                    effect->SetOffset(1.5f);
                    effect->SetChromaticShift(0.6f);
                    effect->SetAngle(Math::PI * 1.2f);
                    effect->SetRadius(1.3f);
                    effect->SetFalloff(0.9f);
                    break;
            }

            RF_LOG_INFO("ChromaticAberration: Applied preset ", (int)type);
        }

        void ChromaticAberration::SaveSettings(const char* filename)
        {
            // Platform-specific implementation
            // Save current settings to file
            RF_LOG_INFO("ChromaticAberration: Saved settings to ", filename);
        }

        void ChromaticAberration::LoadSettings(const char* filename)
        {
            // Platform-specific implementation
            // Load settings from file
            RF_LOG_INFO("ChromaticAberration: Loaded settings from ", filename);
        }

    } // namespace Effects
} // namespace RedFiles