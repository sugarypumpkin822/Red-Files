#include "rf_fade_effect.h"
#include "rf_logger.h"

namespace RedFiles {
    namespace Effects {

        FadeEffect::FadeEffect()
            : m_isInitialized(false)
            , m_type(FadeType::Linear)
            , m_direction(FadeDirection::In)
            , m_duration(1.0f)
            , m_currentTime(0.0f)
            , m_startOpacity(0.0f)
            , m_endOpacity(1.0f)
            , m_currentOpacity(0.0f)
            , m_color(1.0f, 1.0f, 1.0f)
            , m_enabled(true)
            , m_isPlaying(false)
            , m_isPaused(false)
            , m_isComplete(false)
            , m_loop(false)
            , m_pingPong(false)
            , m_autoReverse(false)
            , m_easing(EasingType::Linear)
            , m_quality(Quality::Medium)
            , m_blendMode(BlendMode::Normal)
            , m_target(nullptr)
            , m_onCompleteCallback(nullptr)
            , m_onUpdateCallback(nullptr)
        {
            RF_LOG_INFO("FadeEffect: Created fade effect");
        }

        FadeEffect::~FadeEffect()
        {
            RF_LOG_INFO("FadeEffect: Destroyed fade effect");
        }

        bool FadeEffect::Initialize()
        {
            if (m_isInitialized)
            {
                RF_LOG_WARNING("FadeEffect: Already initialized");
                return true;
            }

            // Initialize fade parameters
            UpdateFadeParameters();

            // Initialize quality settings
            UpdateQualitySettings();

            m_isInitialized = true;
            RF_LOG_INFO("FadeEffect: Successfully initialized");
            return true;
        }

        void FadeEffect::Update(float deltaTime)
        {
            if (!m_isInitialized || !m_enabled || !m_isPlaying || m_isPaused)
                return;

            m_currentTime += deltaTime;

            // Handle loop and ping-pong
            if (m_currentTime >= m_duration)
            {
                if (m_loop)
                {
                    if (m_pingPong)
                    {
                        // Reverse direction
                        m_direction = (m_direction == FadeDirection::In) ? FadeDirection::Out : FadeDirection::In;
                        m_currentTime = m_currentTime - m_duration;
                        SwapStartEndOpacity();
                    }
                    else
                    {
                        // Reset to beginning
                        m_currentTime = m_currentTime - m_duration;
                    }
                }
                else if (m_autoReverse)
                {
                    // Reverse and continue
                    m_direction = (m_direction == FadeDirection::In) ? FadeDirection::Out : FadeDirection::In;
                    m_currentTime = m_currentTime - m_duration;
                    SwapStartEndOpacity();
                }
                else
                {
                    // Complete the fade
                    m_currentTime = m_duration;
                    m_isComplete = true;
                    m_isPlaying = false;

                    // Trigger completion callback
                    if (m_onCompleteCallback)
                    {
                        m_onCompleteCallback(this);
                    }

                    RF_LOG_INFO("FadeEffect: Fade completed");
                    return;
                }
            }

            // Calculate current opacity
            float normalizedTime = m_currentTime / m_duration;
            float easedTime = ApplyEasing(normalizedTime);
            m_currentOpacity = Math::Lerp(m_startOpacity, m_endOpacity, easedTime);

            // Apply fade to target
            if (m_target)
            {
                ApplyFadeToTarget();
            }

            // Trigger update callback
            if (m_onUpdateCallback)
            {
                m_onUpdateCallback(this, m_currentOpacity);
            }

            RF_LOG_DEBUG("FadeEffect: Updated effect - opacity: ", m_currentOpacity);
        }

        void FadeEffect::Apply(RenderTarget* target)
        {
            if (!m_isInitialized || !m_enabled || !target)
            {
                RF_LOG_WARNING("FadeEffect: Cannot apply effect - not initialized or disabled");
                return;
            }

            m_target = target;
            ApplyFadeToTarget();

            RF_LOG_DEBUG("FadeEffect: Applied effect to render target");
        }

        void FadeEffect::Play()
        {
            if (!m_isInitialized)
            {
                RF_LOG_ERROR("FadeEffect: Cannot play - not initialized");
                return;
            }

            m_isPlaying = true;
            m_isPaused = false;
            m_isComplete = false;
            
            if (m_currentTime >= m_duration)
            {
                Reset();
            }

            RF_LOG_INFO("FadeEffect: Started playing fade");
        }

        void FadeEffect::Pause()
        {
            m_isPaused = true;
            RF_LOG_INFO("FadeEffect: Paused fade");
        }

        void FadeEffect::Resume()
        {
            if (!m_isPaused)
            {
                RF_LOG_WARNING("FadeEffect: Cannot resume - not paused");
                return;
            }

            m_isPaused = false;
            RF_LOG_INFO("FadeEffect: Resumed fade");
        }

        void FadeEffect::Stop()
        {
            m_isPlaying = false;
            m_isPaused = false;
            m_isComplete = false;
            m_currentTime = 0.0f;
            m_currentOpacity = m_startOpacity;

            RF_LOG_INFO("FadeEffect: Stopped fade");
        }

        void FadeEffect::Reset()
        {
            m_currentTime = 0.0f;
            m_currentOpacity = m_startOpacity;
            m_isComplete = false;
            m_isPaused = false;

            // Reset direction if ping-pong
            if (m_pingPong)
            {
                m_direction = FadeDirection::In;
                SwapStartEndOpacity();
            }

            RF_LOG_INFO("FadeEffect: Reset fade");
        }

        void FadeEffect::SetType(FadeType type)
        {
            m_type = type;
            UpdateFadeParameters();
            RF_LOG_INFO("FadeEffect: Set type to ", (int)type);
        }

        void FadeEffect::SetDirection(FadeDirection direction)
        {
            m_direction = direction;
            UpdateFadeParameters();
            RF_LOG_INFO("FadeEffect: Set direction to ", (int)direction);
        }

        void FadeEffect::SetDuration(float duration)
        {
            m_duration = Math::Max(0.01f, duration);
            RF_LOG_INFO("FadeEffect: Set duration to ", m_duration);
        }

        void FadeEffect::SetStartOpacity(float opacity)
        {
            m_startOpacity = Math::Clamp(opacity, 0.0f, 1.0f);
            RF_LOG_INFO("FadeEffect: Set start opacity to ", m_startOpacity);
        }

        void FadeEffect::SetEndOpacity(float opacity)
        {
            m_endOpacity = Math::Clamp(opacity, 0.0f, 1.0f);
            RF_LOG_INFO("FadeEffect: Set end opacity to ", m_endOpacity);
        }

        void FadeEffect::SetColor(const Vector3& color)
        {
            m_color = color;
            RF_LOG_INFO("FadeEffect: Set color to (", m_color.x, ", ", m_color.y, ", ", m_color.z, ")");
        }

        void FadeEffect::SetColor(float r, float g, float b)
        {
            m_color = Vector3(r, g, b);
            RF_LOG_INFO("FadeEffect: Set color to (", r, ", ", g, ", ", b, ")");
        }

        void FadeEffect::SetEasing(EasingType easing)
        {
            m_easing = easing;
            RF_LOG_INFO("FadeEffect: Set easing to ", (int)easing);
        }

        void FadeEffect::SetQuality(Quality quality)
        {
            m_quality = quality;
            UpdateQualitySettings();
            RF_LOG_INFO("FadeEffect: Set quality to ", (int)quality);
        }

        void FadeEffect::SetBlendMode(BlendMode blendMode)
        {
            m_blendMode = blendMode;
            RF_LOG_INFO("FadeEffect: Set blend mode to ", (int)blendMode);
        }

        void FadeEffect::SetLoop(bool loop)
        {
            m_loop = loop;
            RF_LOG_INFO("FadeEffect: Set loop to ", loop);
        }

        void FadeEffect::SetPingPong(bool pingPong)
        {
            m_pingPong = pingPong;
            RF_LOG_INFO("FadeEffect: Set ping-pong to ", pingPong);
        }

        void FadeEffect::SetAutoReverse(bool autoReverse)
        {
            m_autoReverse = autoReverse;
            RF_LOG_INFO("FadeEffect: Set auto-reverse to ", autoReverse);
        }

        void FadeEffect::SetEnabled(bool enabled)
        {
            m_enabled = enabled;
            RF_LOG_INFO("FadeEffect: Set enabled to ", enabled);
        }

        void FadeEffect::SetOnCompleteCallback(FadeCallback callback)
        {
            m_onCompleteCallback = callback;
            RF_LOG_INFO("FadeEffect: Set on-complete callback");
        }

        void FadeEffect::SetOnUpdateCallback(FadeUpdateCallback callback)
        {
            m_onUpdateCallback = callback;
            RF_LOG_INFO("FadeEffect: Set on-update callback");
        }

        FadeType FadeEffect::GetType() const
        {
            return m_type;
        }

        FadeDirection FadeEffect::GetDirection() const
        {
            return m_direction;
        }

        float FadeEffect::GetDuration() const
        {
            return m_duration;
        }

        float FadeEffect::GetCurrentTime() const
        {
            return m_currentTime;
        }

        float FadeEffect::GetStartOpacity() const
        {
            return m_startOpacity;
        }

        float FadeEffect::GetEndOpacity() const
        {
            return m_endOpacity;
        }

        float FadeEffect::GetCurrentOpacity() const
        {
            return m_currentOpacity;
        }

        Vector3 FadeEffect::GetColor() const
        {
            return m_color;
        }

        EasingType FadeEffect::GetEasing() const
        {
            return m_easing;
        }

        Quality FadeEffect::GetQuality() const
        {
            return m_quality;
        }

        BlendMode FadeEffect::GetBlendMode() const
        {
            return m_blendMode;
        }

        bool FadeEffect::IsPlaying() const
        {
            return m_isPlaying;
        }

        bool FadeEffect::IsPaused() const
        {
            return m_isPaused;
        }

        bool FadeEffect::IsComplete() const
        {
            return m_isComplete;
        }

        bool FadeEffect::IsLooping() const
        {
            return m_loop;
        }

        bool FadeEffect::IsPingPong() const
        {
            return m_pingPong;
        }

        bool FadeEffect::IsAutoReverse() const
        {
            return m_autoReverse;
        }

        bool FadeEffect::IsEnabled() const
        {
            return m_enabled;
        }

        bool FadeEffect::IsInitialized() const
        {
            return m_isInitialized;
        }

        void FadeEffect::UpdateFadeParameters()
        {
            // Set default opacity values based on direction
            switch (m_direction)
            {
                case FadeDirection::In:
                    m_startOpacity = 0.0f;
                    m_endOpacity = 1.0f;
                    break;
                case FadeDirection::Out:
                    m_startOpacity = 1.0f;
                    m_endOpacity = 0.0f;
                    break;
                case FadeDirection::CrossFade:
                    m_startOpacity = 0.0f;
                    m_endOpacity = 1.0f;
                    break;
                case FadeDirection::Custom:
                    // Keep current values
                    break;
            }

            // Adjust for fade type
            switch (m_type)
            {
                case FadeType::Linear:
                    // No adjustment needed
                    break;
                case FadeType::Exponential:
                    // Adjust for exponential fade
                    if (m_direction == FadeDirection::In)
                    {
                        m_startOpacity = 0.0f;
                        m_endOpacity = 1.0f;
                    }
                    else
                    {
                        m_startOpacity = 1.0f;
                        m_endOpacity = 0.0f;
                    }
                    break;
                case FadeType::Logarithmic:
                    // Adjust for logarithmic fade
                    if (m_direction == FadeDirection::In)
                    {
                        m_startOpacity = 0.001f; // Near zero for log
                        m_endOpacity = 1.0f;
                    }
                    else
                    {
                        m_startOpacity = 1.0f;
                        m_endOpacity = 0.001f; // Near zero for log
                    }
                    break;
            }
        }

        void FadeEffect::UpdateQualitySettings()
        {
            // Quality settings for fade effects
            switch (m_quality)
            {
                case Quality::Low:
                    // Basic fade with minimal processing
                    break;
                case Quality::Medium:
                    // Standard fade with dithering
                    break;
                case Quality::High:
                    // High-quality fade with gamma correction
                    break;
                case Quality::Ultra:
                    // Ultra-high quality with advanced dithering and color correction
                    break;
            }
        }

        float FadeEffect::ApplyEasing(float t) const
        {
            switch (m_easing)
            {
                case EasingType::Linear:
                    return t;
                case EasingType::EaseIn:
                    return t * t;
                case EasingType::EaseOut:
                    return 1.0f - (1.0f - t) * (1.0f - t);
                case EasingType::EaseInOut:
                    return t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
                case EasingType::EaseInCubic:
                    return t * t * t;
                case EasingType::EaseOutCubic:
                    return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
                case EasingType::EaseInOutCubic:
                    return t < 0.5f ? 4.0f * t * t * t : 1.0f - 4.0f * (1.0f - t) * (1.0f - t) * (1.0f - t);
                case EasingType::EaseInQuart:
                    return t * t * t * t;
                case EasingType::EaseOutQuart:
                    return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
                case EasingType::EaseInOutQuart:
                    return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - 8.0f * (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
                case EasingType::EaseInBack:
                    return t * t * (2.7f * t - 1.7f);
                case EasingType::EaseOutBack:
                    return 1.0f + (t - 1.0f) * (t - 1.0f) * (2.7f * (t - 1.0f) + 1.7f);
                case EasingType::EaseInOutBack:
                    return t < 0.5f ? 2.0f * t * t * (2.7f * 2.0f * t - 1.7f) : 1.0f + 2.0f * (t - 1.0f) * (t - 1.0f) * (2.7f * 2.0f * (t - 1.0f) + 1.7f);
                case EasingType::EaseInElastic:
                    return -Math::Cos(Math::PI * 1.5f * t) * Math::Pow(2.0f, 10.0f * (t - 1.0f)) + 1.0f;
                case EasingType::EaseOutElastic:
                    return Math::Sin(-13.0f * Math::PI * 0.5f * (t + 1.0f)) * Math::Pow(2.0f, -10.0f * t) + 1.0f;
                case EasingType::EaseInOutElastic:
                    return t < 0.5f ? 0.5f * (-Math::Cos(Math::PI * 1.5f * 2.0f * t) * Math::Pow(2.0f, 10.0f * (2.0f * t - 1.0f)) + 1.0f) : 0.5f * (Math::Sin(-13.0f * Math::PI * 0.5f * (2.0f * t - 2.0f + 1.0f)) * Math::Pow(2.0f, -10.0f * (2.0f * t - 2.0f)) + 2.0f);
                case EasingType::EaseInBounce:
                    return 1.0f - EaseOutBounce(1.0f - t);
                case EasingType::EaseOutBounce:
                    return EaseOutBounce(t);
                case EasingType::EaseInOutBounce:
                    return t < 0.5f ? 0.5f * (1.0f - EaseOutBounce(1.0f - 2.0f * t)) : 0.5f * EaseOutBounce(2.0f * t - 1.0f) + 0.5f;
                default:
                    return t;
            }
        }

        float FadeEffect::EaseOutBounce(float t) const
        {
            if (t < 1.0f / 2.75f)
            {
                return 7.5625f * t * t;
            }
            else if (t < 2.0f / 2.75f)
            {
                t -= 1.5f / 2.75f;
                return 7.5625f * t * t + 0.75f;
            }
            else if (t < 2.5f / 2.75f)
            {
                t -= 2.25f / 2.75f;
                return 7.5625f * t * t + 0.9375f;
            }
            else
            {
                t -= 2.625f / 2.75f;
                return 7.5625f * t * t + 0.984375f;
            }
        }

        void FadeEffect::ApplyFadeToTarget()
        {
            if (!m_target)
                return;

            // Apply fade effect to render target
            // This would be implemented using the rendering API
            // For now, we provide the structure

            // Create fade overlay
            RenderTarget* fadeOverlay = CreateFadeOverlay(m_target);
            if (fadeOverlay)
            {
                // Apply color and opacity
                ApplyColorAndOpacity(fadeOverlay);

                // Blend with original target
                BlendWithTarget(m_target, fadeOverlay);

                // Cleanup
                DestroyFadeOverlay(fadeOverlay);
            }
        }

        RenderTarget* FadeEffect::CreateFadeOverlay(RenderTarget* source)
        {
            // Create a render target for the fade overlay
            // Platform-specific implementation
            return nullptr; // Placeholder
        }

        void FadeEffect::ApplyColorAndOpacity(RenderTarget* overlay)
        {
            // Apply the fade color and opacity to the overlay
            // Platform-specific implementation
        }

        void FadeEffect::BlendWithTarget(RenderTarget* target, RenderTarget* overlay)
        {
            // Blend the overlay with the target using the specified blend mode
            // Platform-specific implementation
        }

        void FadeEffect::DestroyFadeOverlay(RenderTarget* overlay)
        {
            // Clean up the overlay render target
            // Platform-specific implementation
        }

        void FadeEffect::SwapStartEndOpacity()
        {
            float temp = m_startOpacity;
            m_startOpacity = m_endOpacity;
            m_endOpacity = temp;
        }

        void FadeEffect::FadeTo(float opacity, float duration)
        {
            SetStartOpacity(m_currentOpacity);
            SetEndOpacity(opacity);
            SetDuration(duration);
            Play();
        }

        void FadeEffect::FadeIn(float duration)
        {
            SetDirection(FadeDirection::In);
            SetDuration(duration);
            Play();
        }

        void FadeEffect::FadeOut(float duration)
        {
            SetDirection(FadeDirection::Out);
            SetDuration(duration);
            Play();
        }

        void FadeEffect::CrossFade(float duration)
        {
            SetDirection(FadeDirection::CrossFade);
            SetDuration(duration);
            Play();
        }

        void FadeEffect::Preset::ApplyPreset(FadeEffect* effect, PresetType type)
        {
            switch (type)
            {
                case PresetType::FastFadeIn:
                    effect->SetDirection(FadeDirection::In);
                    effect->SetDuration(0.25f);
                    effect->SetEasing(EasingType::EaseOut);
                    break;

                case PresetType::SlowFadeIn:
                    effect->SetDirection(FadeDirection::In);
                    effect->SetDuration(2.0f);
                    effect->SetEasing(EasingType::EaseInOut);
                    break;

                case PresetType::FastFadeOut:
                    effect->SetDirection(FadeDirection::Out);
                    effect->SetDuration(0.25f);
                    effect->SetEasing(EasingType::EaseIn);
                    break;

                case PresetType::SlowFadeOut:
                    effect->SetDirection(FadeDirection::Out);
                    effect->SetDuration(2.0f);
                    effect->SetEasing(EasingType::EaseInOut);
                    break;

                case PresetType::SmoothFade:
                    effect->SetEasing(EasingType::EaseInOut);
                    effect->SetDuration(1.0f);
                    break;

                case PresetType::BouncyFade:
                    effect->SetEasing(EasingType::EaseOutBounce);
                    effect->SetDuration(1.5f);
                    break;

                case PresetType::ElasticFade:
                    effect->SetEasing(EasingType::EaseOutElastic);
                    effect->SetDuration(2.0f);
                    break;

                case PresetType::DramaticFade:
                    effect->SetEasing(EasingType::EaseInOutCubic);
                    effect->SetDuration(3.0f);
                    break;
            }

            RF_LOG_INFO("FadeEffect: Applied preset ", (int)type);
        }

        void FadeEffect::SaveSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("FadeEffect: Saved settings to ", filename);
        }

        void FadeEffect::LoadSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("FadeEffect: Loaded settings from ", filename);
        }

    } // namespace Effects
} // namespace RedFiles