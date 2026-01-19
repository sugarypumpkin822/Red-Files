#include "rf_pulse_effect.h"
#include "rf_logger.h"

namespace RedFiles {
    namespace Effects {

        PulseEffect::PulseEffect()
            : m_isInitialized(false)
            , m_type(PulseType::Linear)
            , m_amplitude(1.0f)
            , m_frequency(1.0f)
            , m_phase(0.0f)
            , m_duration(1.0f)
            , m_currentTime(0.0f)
            , m_currentValue(0.0f)
            , m_minValue(0.0f)
            , m_maxValue(1.0f)
            , m_centerValue(0.5f)
            , m_enabled(true)
            , m_isPlaying(false)
            , m_isPaused(false)
            , m_isComplete(false)
            , m_loop(false)
            , m_pingPong(false)
            , m_autoReverse(false)
            , m_easing(EasingType::Linear)
            , m_quality(Quality::Medium)
            , m_samples(16)
            , m_target(nullptr)
            , m_onCompleteCallback(nullptr)
            , m_onUpdateCallback(nullptr)
            , m_time(0.0f)
            , m_animationSpeed(1.0f)
            , m_damping(0.0f)
            , m_decay(0.0f)
            , m_noiseScale(0.1f)
            , m_noiseStrength(0.0f)
        {
            RF_LOG_INFO("PulseEffect: Created pulse effect");
        }

        PulseEffect::~PulseEffect()
        {
            RF_LOG_INFO("PulseEffect: Destroyed pulse effect");
        }

        bool PulseEffect::Initialize()
        {
            if (m_isInitialized)
            {
                RF_LOG_WARNING("PulseEffect: Already initialized");
                return true;
            }

            // Initialize pulse parameters based on type
            UpdatePulseParameters();

            // Initialize quality settings
            UpdateQualitySettings();

            // Initialize noise texture for noise-based pulses
            InitializeNoiseTexture();

            m_isInitialized = true;
            RF_LOG_INFO("PulseEffect: Successfully initialized");
            return true;
        }

        void PulseEffect::Update(float deltaTime)
        {
            if (!m_isInitialized || !m_enabled || !m_isPlaying || m_isPaused)
                return;

            m_time += deltaTime;
            m_currentTime += deltaTime;

            // Handle loop and ping-pong
            if (m_currentTime >= m_duration)
            {
                if (m_loop)
                {
                    if (m_pingPong)
                    {
                        // Reverse direction
                        m_phase = Math::PI - m_phase;
                        m_currentTime = m_currentTime - m_duration;
                    }
                    else
                    {
                        // Reset to beginning
                        m_currentTime = m_currentTime - m_duration;
                        m_phase = 0.0f;
                    }
                }
                else if (m_autoReverse)
                {
                    // Reverse and continue
                    m_phase = Math::PI - m_phase;
                    m_currentTime = m_currentTime - m_duration;
                }
                else
                {
                    // Complete the pulse
                    m_currentTime = m_duration;
                    m_isComplete = true;
                    m_isPlaying = false;

                    // Trigger completion callback
                    if (m_onCompleteCallback)
                    {
                        m_onCompleteCallback(this);
                    }

                    RF_LOG_INFO("PulseEffect: Pulse completed");
                    return;
                }
            }

            // Update pulse phase
            UpdatePulsePhase(deltaTime);

            // Calculate current value
            CalculateCurrentValue();

            // Apply pulse to target
            if (m_target)
            {
                ApplyPulseToTarget();
            }

            // Trigger update callback
            if (m_onUpdateCallback)
            {
                m_onUpdateCallback(this, m_currentValue);
            }

            RF_LOG_DEBUG("PulseEffect: Updated effect - value: ", m_currentValue);
        }

        void PulseEffect::Apply(RenderTarget* target)
        {
            if (!m_isInitialized || !m_enabled || !target)
            {
                RF_LOG_WARNING("PulseEffect: Cannot apply effect - not initialized or disabled");
                return;
            }

            m_target = target;
            ApplyPulseToTarget();

            RF_LOG_DEBUG("PulseEffect: Applied effect to render target");
        }

        void PulseEffect::Play()
        {
            if (!m_isInitialized)
            {
                RF_LOG_ERROR("PulseEffect: Cannot play - not initialized");
                return;
            }

            m_isPlaying = true;
            m_isPaused = false;
            m_isComplete = false;
            
            if (m_currentTime >= m_duration)
            {
                Reset();
            }

            RF_LOG_INFO("PulseEffect: Started playing pulse");
        }

        void PulseEffect::Pause()
        {
            m_isPaused = true;
            RF_LOG_INFO("PulseEffect: Paused pulse");
        }

        void PulseEffect::Resume()
        {
            if (!m_isPaused)
            {
                RF_LOG_WARNING("PulseEffect: Cannot resume - not paused");
                return;
            }

            m_isPaused = false;
            RF_LOG_INFO("PulseEffect: Resumed pulse");
        }

        void PulseEffect::Stop()
        {
            m_isPlaying = false;
            m_isPaused = false;
            m_isComplete = false;
            m_currentTime = 0.0f;
            m_phase = 0.0f;
            m_currentValue = m_minValue;

            RF_LOG_INFO("PulseEffect: Stopped pulse");
        }

        void PulseEffect::Reset()
        {
            m_currentTime = 0.0f;
            m_phase = 0.0f;
            m_currentValue = m_minValue;
            m_isComplete = false;
            m_isPaused = false;

            // Reset phase if ping-pong
            if (m_pingPong)
            {
                m_phase = 0.0f;
            }

            RF_LOG_INFO("PulseEffect: Reset pulse");
        }

        void PulseEffect::SetType(PulseType type)
        {
            m_type = type;
            UpdatePulseParameters();
            RF_LOG_INFO("PulseEffect: Set type to ", (int)type);
        }

        void PulseEffect::SetAmplitude(float amplitude)
        {
            m_amplitude = Math::Max(0.0f, amplitude);
            RF_LOG_INFO("PulseEffect: Set amplitude to ", m_amplitude);
        }

        void PulseEffect::SetFrequency(float frequency)
        {
            m_frequency = Math::Max(0.1f, frequency);
            RF_LOG_INFO("PulseEffect: Set frequency to ", m_frequency);
        }

        void PulseEffect::SetPhase(float phase)
        {
            m_phase = phase;
            RF_LOG_INFO("PulseEffect: Set phase to ", m_phase);
        }

        void PulseEffect::SetDuration(float duration)
        {
            m_duration = Math::Max(0.01f, duration);
            RF_LOG_INFO("PulseEffect: Set duration to ", m_duration);
        }

        void PulseEffect::SetMinValue(float minValue)
        {
            m_minValue = minValue;
            m_maxValue = Math::Max(m_maxValue, minValue);
            m_centerValue = (m_minValue + m_maxValue) * 0.5f;
            RF_LOG_INFO("PulseEffect: Set min value to ", m_minValue);
        }

        void PulseEffect::SetMaxValue(float maxValue)
        {
            m_maxValue = maxValue;
            m_minValue = Math::Min(m_minValue, maxValue);
            m_centerValue = (m_minValue + m_maxValue) * 0.5f;
            RF_LOG_INFO("PulseEffect: Set max value to ", m_maxValue);
        }

        void PulseEffect::SetCenterValue(float centerValue)
        {
            m_centerValue = centerValue;
            RF_LOG_INFO("PulseEffect: Set center value to ", m_centerValue);
        }

        void PulseEffect::SetEasing(EasingType easing)
        {
            m_easing = easing;
            RF_LOG_INFO("PulseEffect: Set easing to ", (int)easing);
        }

        void PulseEffect::SetQuality(Quality quality)
        {
            m_quality = quality;
            UpdateQualitySettings();
            RF_LOG_INFO("PulseEffect: Set quality to ", (int)quality);
        }

        void PulseEffect::SetDamping(float damping)
        {
            m_damping = Math::Clamp(damping, 0.0f, 1.0f);
            RF_LOG_INFO("PulseEffect: Set damping to ", m_damping);
        }

        void PulseEffect::SetDecay(float decay)
        {
            m_decay = Math::Max(0.0f, decay);
            RF_LOG_INFO("PulseEffect: Set decay to ", m_decay);
        }

        void PulseEffect::SetNoiseScale(float scale)
        {
            m_noiseScale = Math::Max(0.01f, scale);
            RF_LOG_INFO("PulseEffect: Set noise scale to ", m_noiseScale);
        }

        void PulseEffect::SetNoiseStrength(float strength)
        {
            m_noiseStrength = Math::Max(0.0f, strength);
            RF_LOG_INFO("PulseEffect: Set noise strength to ", m_noiseStrength);
        }

        void PulseEffect::SetLoop(bool loop)
        {
            m_loop = loop;
            RF_LOG_INFO("PulseEffect: Set loop to ", loop);
        }

        void PulseEffect::SetPingPong(bool pingPong)
        {
            m_pingPong = pingPong;
            RF_LOG_INFO("PulseEffect: Set ping-pong to ", pingPong);
        }

        void PulseEffect::SetAutoReverse(bool autoReverse)
        {
            m_autoReverse = autoReverse;
            RF_LOG_INFO("PulseEffect: Set auto-reverse to ", autoReverse);
        }

        void PulseEffect::SetEnabled(bool enabled)
        {
            m_enabled = enabled;
            RF_LOG_INFO("PulseEffect: Set enabled to ", enabled);
        }

        void PulseEffect::SetOnCompleteCallback(PulseCallback callback)
        {
            m_onCompleteCallback = callback;
            RF_LOG_INFO("PulseEffect: Set on-complete callback");
        }

        void PulseEffect::SetOnUpdateCallback(PulseUpdateCallback callback)
        {
            m_onUpdateCallback = callback;
            RF_LOG_INFO("PulseEffect: Set on-update callback");
        }

        PulseType PulseEffect::GetType() const
        {
            return m_type;
        }

        float PulseEffect::GetAmplitude() const
        {
            return m_amplitude;
        }

        float PulseEffect::GetFrequency() const
        {
            return m_frequency;
        }

        float PulseEffect::GetPhase() const
        {
            return m_phase;
        }

        float PulseEffect::GetDuration() const
        {
            return m_duration;
        }

        float PulseEffect::GetCurrentTime() const
        {
            return m_currentTime;
        }

        float PulseEffect::GetMinValue() const
        {
            return m_minValue;
        }

        float PulseEffect::GetMaxValue() const
        {
            return m_maxValue;
        }

        float PulseEffect::GetCenterValue() const
        {
            return m_centerValue;
        }

        float PulseEffect::GetCurrentValue() const
        {
            return m_currentValue;
        }

        EasingType PulseEffect::GetEasing() const
        {
            return m_easing;
        }

        Quality PulseEffect::GetQuality() const
        {
            return m_quality;
        }

        float PulseEffect::GetDamping() const
        {
            return m_damping;
        }

        float PulseEffect::GetDecay() const
        {
            return m_decay;
        }

        bool PulseEffect::IsPlaying() const
        {
            return m_isPlaying;
        }

        bool PulseEffect::IsPaused() const
        {
            return m_isPaused;
        }

        bool PulseEffect::IsComplete() const
        {
            return m_isComplete;
        }

        bool PulseEffect::IsLooping() const
        {
            return m_loop;
        }

        bool PulseEffect::IsPingPong() const
        {
            return m_pingPong;
        }

        bool PulseEffect::IsAutoReverse() const
        {
            return m_autoReverse;
        }

        bool PulseEffect::IsEnabled() const
        {
            return m_enabled;
        }

        bool PulseEffect::IsInitialized() const
        {
            return m_isInitialized;
        }

        void PulseEffect::UpdatePulsePhase(float deltaTime)
        {
            // Update phase based on frequency
            m_phase += m_frequency * Math::PI * 2.0f * deltaTime;

            // Apply damping
            if (m_damping > 0.0f)
            {
                float dampingFactor = Math::Exp(-m_damping * m_time);
                m_amplitude *= dampingFactor;
            }

            // Apply decay
            if (m_decay > 0.0f)
            {
                float decayFactor = Math::Exp(-m_decay * m_time);
                m_amplitude *= decayFactor;
            }

            // Apply noise
            if (m_noiseStrength > 0.0f)
            {
                float noise = GenerateNoise(m_time * 0.5f, m_time * 0.3f, m_time);
                float noiseModulation = noise * m_noiseStrength;
                m_amplitude *= (1.0f + noiseModulation);
            }
        }

        void PulseEffect::CalculateCurrentValue()
        {
            float baseValue = 0.0f;

            switch (m_type)
            {
                case PulseType::Linear:
                    baseValue = CalculateLinearPulse();
                    break;
                case PulseType::Sine:
                    baseValue = CalculateSinePulse();
                    break;
                case PulseType::Square:
                    baseValue = CalculateSquarePulse();
                    break;
                case PulseType::Triangle:
                    baseValue = CalculateTrianglePulse();
                    break;
                case PulseType::Sawtooth:
                    baseValue = CalculateSawtoothPulse();
                    break;
                case PulseType::Exponential:
                    baseValue = CalculateExponentialPulse();
                    break;
                case PulseType::Logarithmic:
                    baseValue = CalculateLogarithmicPulse();
                    break;
                case PulseType::Noise:
                    baseValue = CalculateNoisePulse();
                    break;
                case PulseType::Custom:
                    baseValue = CalculateCustomPulse();
                    break;
            }

            // Apply easing
            float easedValue = ApplyEasing(baseValue);

            // Map to value range
            m_currentValue = Math::Lerp(m_minValue, m_maxValue, easedValue);
        }

        float PulseEffect::CalculateLinearPulse() const
        {
            float normalizedTime = m_currentTime / m_duration;
            return normalizedTime;
        }

        float PulseEffect::CalculateSinePulse() const
        {
            return Math::Sin(m_phase) * 0.5f + 0.5f;
        }

        float PulseEffect::CalculateSquarePulse() const
        {
            return Math::Sin(m_phase) > 0.0f ? 1.0f : 0.0f;
        }

        float PulseEffect::CalculateTrianglePulse() const
        {
            float normalizedPhase = m_phase / (Math::PI * 2.0f);
            if (normalizedPhase < 0.5f)
            {
                return normalizedPhase * 2.0f;
            }
            else
            {
                return 2.0f - normalizedPhase * 2.0f;
            }
        }

        float PulseEffect::CalculateSawtoothPulse() const
        {
            float normalizedPhase = m_phase / (Math::PI * 2.0f);
            return normalizedPhase - Math::Floor(normalizedPhase);
        }

        float PulseEffect::CalculateExponentialPulse() const
        {
            float normalizedTime = m_currentTime / m_duration;
            return Math::Pow(normalizedTime, 2.0f);
        }

        float PulseEffect::CalculateLogarithmicPulse() const
        {
            float normalizedTime = m_currentTime / m_duration;
            return Math::Log(1.0f + normalizedTime * 9.0f) / Math::Log(10.0f);
        }

        float PulseEffect::CalculateNoisePulse() const
        {
            return GenerateNoise(m_phase, m_phase * 0.7f, m_time);
        }

        float PulseEffect::CalculateCustomPulse() const
        {
            // Custom pulse calculation - can be overridden by user
            float normalizedTime = m_currentTime / m_duration;
            return Math::Sin(normalizedTime * Math::PI * 2.0f * m_frequency) * 0.5f + 0.5f;
        }

        float PulseEffect::ApplyEasing(float t) const
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

        float PulseEffect::EaseOutBounce(float t) const
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

        void PulseEffect::UpdatePulseParameters()
        {
            // Set default parameters based on pulse type
            switch (m_type)
            {
                case PulseType::Linear:
                    m_amplitude = 1.0f;
                    m_frequency = 1.0f;
                    break;
                case PulseType::Sine:
                    m_amplitude = 1.0f;
                    m_frequency = 2.0f;
                    break;
                case PulseType::Square:
                    m_amplitude = 1.0f;
                    m_frequency = 1.0f;
                    break;
                case PulseType::Triangle:
                    m_amplitude = 1.0f;
                    m_frequency = 1.0f;
                    break;
                case PulseType::Sawtooth:
                    m_amplitude = 1.0f;
                    m_frequency = 1.0f;
                    break;
                case PulseType::Exponential:
                    m_amplitude = 1.0f;
                    m_frequency = 1.0f;
                    break;
                case PulseType::Logarithmic:
                    m_amplitude = 1.0f;
                    m_frequency = 1.0f;
                    break;
                case PulseType::Noise:
                    m_noiseScale = 0.1f;
                    m_noiseStrength = 0.5f;
                    break;
                case PulseType::Custom:
                    m_amplitude = 1.0f;
                    m_frequency = 1.0f;
                    break;
            }
        }

        void PulseEffect::UpdateQualitySettings()
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

        void PulseEffect::InitializeNoiseTexture()
        {
            // Initialize noise texture for noise-based pulses
            // This would be implemented using the rendering API
        }

        void PulseEffect::ApplyPulseToTarget()
        {
            if (!m_target)
                return;

            // Apply pulse effect to render target
            // This would be implemented using the rendering API
            // For now, we provide the structure

            // Create pulse overlay
            RenderTarget* pulseOverlay = CreatePulseOverlay(m_target);
            if (pulseOverlay)
            {
                // Apply pulse value
                ApplyPulseValue(pulseOverlay);

                // Blend with original target
                BlendWithTarget(m_target, pulseOverlay);

                // Cleanup
                DestroyPulseOverlay(pulseOverlay);
            }
        }

        RenderTarget* PulseEffect::CreatePulseOverlay(RenderTarget* source)
        {
            // Create a render target for the pulse overlay
            // Platform-specific implementation
            return nullptr; // Placeholder
        }

        void PulseEffect::ApplyPulseValue(RenderTarget* overlay)
        {
            // Apply the pulse value to the overlay
            // Platform-specific implementation
        }

        void PulseEffect::BlendWithTarget(RenderTarget* target, RenderTarget* overlay)
        {
            // Blend the overlay with the target
            // Platform-specific implementation
        }

        void PulseEffect::DestroyPulseOverlay(RenderTarget* overlay)
        {
            // Clean up the overlay render target
            // Platform-specific implementation
        }

        float PulseEffect::GenerateNoise(float x, float y, float time) const
        {
            // Simple noise function
            return Math::Sin(x * 2.0f + time) * Math::Cos(y * 2.0f + time) * 0.5f + 0.5f;
        }

        void PulseEffect::PulseTo(float value, float duration)
        {
            SetMinValue(m_currentValue);
            SetMaxValue(value);
            SetDuration(duration);
            Play();
        }

        void PulseEffect::PulseFromTo(float fromValue, float toValue, float duration)
        {
            SetMinValue(fromValue);
            SetMaxValue(toValue);
            SetDuration(duration);
            Play();
        }

        void PulseEffect::Reset()
        {
            m_type = PulseType::Linear;
            m_amplitude = 1.0f;
            m_frequency = 1.0f;
            m_phase = 0.0f;
            m_duration = 1.0f;
            m_currentTime = 0.0f;
            m_currentValue = 0.0f;
            m_minValue = 0.0f;
            m_maxValue = 1.0f;
            m_centerValue = 0.5f;
            m_enabled = true;
            m_isPlaying = false;
            m_isPaused = false;
            m_isComplete = false;
            m_loop = false;
            m_pingPong = false;
            m_autoReverse = false;
            m_easing = EasingType::Linear;
            m_quality = Quality::Medium;
            m_time = 0.0f;
            m_damping = 0.0f;
            m_decay = 0.0f;
            m_noiseScale = 0.1f;
            m_noiseStrength = 0.0f;
            m_target = nullptr;

            UpdatePulseParameters();
            UpdateQualitySettings();

            RF_LOG_INFO("PulseEffect: Reset to default values");
        }

        void PulseEffect::Preset::ApplyPreset(PulseEffect* effect, PresetType type)
        {
            switch (type)
            {
                case PresetType::Heartbeat:
                    effect->SetType(PulseType::Sine);
                    effect->SetFrequency(1.2f);
                    effect->SetAmplitude(0.8f);
                    effect->SetMinValue(0.2f);
                    effect->SetMaxValue(1.0f);
                    effect->SetEasing(EasingType::EaseOut);
                    break;

                case PresetType::Breathing:
                    effect->SetType(PulseType::Sine);
                    effect->SetFrequency(0.3f);
                    effect->SetAmplitude(0.3f);
                    effect->SetMinValue(0.7f);
                    effect->SetMaxValue(1.0f);
                    effect->SetEasing(EasingType::EaseInOut);
                    break;

                case PresetType::Flash:
                    effect->SetType(PulseType::Square);
                    effect->SetFrequency(2.0f);
                    effect->SetAmplitude(1.0f);
                    effect->SetMinValue(0.0f);
                    effect->SetMaxValue(1.0f);
                    break;

                case PresetType::Flicker:
                    effect->SetType(PulseType::Noise);
                    effect->SetFrequency(10.0f);
                    effect->SetAmplitude(0.5f);
                    effect->SetMinValue(0.5f);
                    effect->SetMaxValue(1.0f);
                    effect->SetNoiseScale(0.5f);
                    effect->SetNoiseStrength(0.8f);
                    break;

                case PresetType::Wave:
                    effect->SetType(PulseType::Sine);
                    effect->SetFrequency(0.5f);
                    effect->SetAmplitude(0.6f);
                    effect->SetMinValue(0.4f);
                    effect->SetMaxValue(1.0f);
                    effect->SetEasing(EasingType::EaseInOut);
                    break;

                case PresetType::Panic:
                    effect->SetType(PulseType::Triangle);
                    effect->SetFrequency(3.0f);
                    effect->SetAmplitude(1.0f);
                    effect->SetMinValue(0.0f);
                    effect->SetMaxValue(1.0f);
                    effect->SetDamping(0.1f);
                    break;

                case PresetType::Gentle:
                    effect->SetType(PulseType::Sine);
                    effect->SetFrequency(0.2f);
                    effect->SetAmplitude(0.2f);
                    effect->SetMinValue(0.8f);
                    effect->SetMaxValue(1.0f);
                    effect->SetEasing(EasingType::EaseInOut);
                    break;

                case PresetType::Energetic:
                    effect->SetType(PulseType::Sawtooth);
                    effect->SetFrequency(2.5f);
                    effect->SetAmplitude(0.9f);
                    effect->SetMinValue(0.1f);
                    effect->SetMaxValue(1.0f);
                    break;
            }

            RF_LOG_INFO("PulseEffect: Applied preset ", (int)type);
        }

        void PulseEffect::SaveSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("PulseEffect: Saved settings to ", filename);
        }

        void PulseEffect::LoadSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("PulseEffect: Loaded settings from ", filename);
        }

    } // namespace Effects
} // namespace RedFiles