#include "rf_shake_effect.h"
#include "rf_logger.h"

namespace RedFiles {
    namespace Effects {

        ShakeEffect::ShakeEffect()
            : m_isInitialized(false)
            , m_type(ShakeType::Positional)
            , m_intensity(1.0f)
            , m_frequency(10.0f)
            , m_duration(1.0f)
            , m_currentTime(0.0f)
            , m_positionX(0.0f)
            , m_positionY(0.0f)
            , m_rotation(0.0f)
            , m_scaleX(1.0f)
            , m_scaleY(1.0f)
            , m_falloff(1.0f)
            , m_quality(Quality::Medium)
            , m_samples(16)
            , m_enabled(true)
            , m_time(0.0f)
            , m_animated(false)
            , m_animationSpeed(1.0f)
            , m_decay(0.0f)
            , m_damping(0.0f)
            , m_noiseScale(0.1f)
            , m_noiseStrength(0.0f)
            , m_octaves(1)
            , m_persistence(0.5f)
            , m_lacunarity(2.0f)
            , m_target(nullptr)
            , m_onCompleteCallback(nullptr)
            , m_onUpdateCallback(nullptr)
        {
            RF_LOG_INFO("ShakeEffect: Created shake effect");
        }

        ShakeEffect::~ShakeEffect()
        {
            RF_LOG_INFO("ShakeEffect: Destroyed shake effect");
        }

        bool ShakeEffect::Initialize()
        {
            if (m_isInitialized)
            {
                RF_LOG_WARNING("ShakeEffect: Already initialized");
                return true;
            }

            // Initialize shake parameters based on type
            UpdateShakeParameters();

            // Initialize quality settings
            UpdateQualitySettings();

            // Initialize noise texture for noise-based shakes
            InitializeNoiseTexture();

            m_isInitialized = true;
            RF_LOG_INFO("ShakeEffect: Successfully initialized");
            return true;
        }

        void ShakeEffect::Update(float deltaTime)
        {
            if (!m_isInitialized || !m_enabled)
                return;

            m_time += deltaTime;
            m_currentTime += deltaTime;

            // Check if shake should stop
            if (m_currentTime >= m_duration)
            {
                Stop();
                return;
            }

            // Update animation
            if (m_animated)
            {
                UpdateAnimation(deltaTime);
            }

            // Update dynamic parameters
            UpdateDynamicParameters(deltaTime);

            // Update noise
            UpdateNoise(deltaTime);

            // Calculate shake values
            CalculateShakeValues();

            // Apply shake to target
            if (m_target)
            {
                ApplyShakeToTarget();
            }

            // Trigger update callback
            if (m_onUpdateCallback)
            {
                m_onUpdateCallback(this, m_positionX, m_positionY, m_rotation);
            }

            RF_LOG_DEBUG("ShakeEffect: Updated effect");
        }

        void ShakeEffect::Apply(RenderTarget* target)
        {
            if (!m_isInitialized || !m_enabled || !target)
            {
                RF_LOG_WARNING("ShakeEffect: Cannot apply effect - not initialized or disabled");
                return;
            }

            m_target = target;
            ApplyShakeToTarget();

            RF_LOG_DEBUG("ShakeEffect: Applied effect to render target");
        }

        void ShakeEffect::Play()
        {
            if (!m_isInitialized)
            {
                RF_LOG_ERROR("ShakeEffect: Cannot play - not initialized");
                return;
            }

            m_currentTime = 0.0f;
            m_time = 0.0f;
            m_enabled = true;

            RF_LOG_INFO("ShakeEffect: Started playing shake");
        }

        void ShakeEffect::Stop()
        {
            m_enabled = false;
            m_positionX = 0.0f;
            m_positionY = 0.0f;
            m_rotation = 0.0f;
            m_scaleX = 1.0f;
            m_scaleY = 1.0f;

            // Trigger completion callback
            if (m_onCompleteCallback)
            {
                m_onCompleteCallback(this);
            }

            RF_LOG_INFO("ShakeEffect: Stopped shake");
        }

        void ShakeEffect::SetType(ShakeType type)
        {
            m_type = type;
            UpdateShakeParameters();
            RF_LOG_INFO("ShakeEffect: Set type to ", (int)type);
        }

        void ShakeEffect::SetIntensity(float intensity)
        {
            m_intensity = Math::Max(0.0f, intensity);
            RF_LOG_INFO("ShakeEffect: Set intensity to ", m_intensity);
        }

        void ShakeEffect::SetFrequency(float frequency)
        {
            m_frequency = Math::Max(0.1f, frequency);
            RF_LOG_INFO("ShakeEffect: Set frequency to ", m_frequency);
        }

        void ShakeEffect::SetDuration(float duration)
        {
            m_duration = Math::Max(0.01f, duration);
            RF_LOG_INFO("ShakeEffect: Set duration to ", m_duration);
        }

        void ShakeEffect::SetFalloff(float falloff)
        {
            m_falloff = Math::Max(0.1f, falloff);
            RF_LOG_INFO("ShakeEffect: Set falloff to ", m_falloff);
        }

        void ShakeEffect::SetQuality(Quality quality)
        {
            m_quality = quality;
            UpdateQualitySettings();
            RF_LOG_INFO("ShakeEffect: Set quality to ", (int)quality);
        }

        void ShakeEffect::SetDecay(float decay)
        {
            m_decay = Math::Max(0.0f, decay);
            RF_LOG_INFO("ShakeEffect: Set decay to ", m_decay);
        }

        void ShakeEffect::SetDamping(float damping)
        {
            m_damping = Math::Clamp(damping, 0.0f, 1.0f);
            RF_LOG_INFO("ShakeEffect: Set damping to ", m_damping);
        }

        void ShakeEffect::SetNoiseScale(float scale)
        {
            m_noiseScale = Math::Max(0.01f, scale);
            RF_LOG_INFO("ShakeEffect: Set noise scale to ", m_noiseScale);
        }

        void ShakeEffect::SetNoiseStrength(float strength)
        {
            m_noiseStrength = Math::Max(0.0f, strength);
            RF_LOG_INFO("ShakeEffect: Set noise strength to ", m_noiseStrength);
        }

        void ShakeEffect::SetOctaves(uint32_t octaves)
        {
            m_octaves = Math::Clamp(octaves, 1u, 8u);
            RF_LOG_INFO("ShakeEffect: Set octaves to ", m_octaves);
        }

        void ShakeEffect::SetPersistence(float persistence)
        {
            m_persistence = Math::Clamp(persistence, 0.0f, 1.0f);
            RF_LOG_INFO("ShakeEffect: Set persistence to ", m_persistence);
        }

        void ShakeEffect::SetLacunarity(float lacunarity)
        {
            m_lacunarity = Math::Max(1.0f, lacunarity);
            RF_LOG_INFO("ShakeEffect: Set lacunarity to ", m_lacunarity);
        }

        void ShakeEffect::SetAnimated(bool animated)
        {
            m_animated = animated;
            RF_LOG_INFO("ShakeEffect: Set animated to ", animated);
        }

        void ShakeEffect::SetAnimationSpeed(float speed)
        {
            m_animationSpeed = Math::Max(0.0f, speed);
            RF_LOG_INFO("ShakeEffect: Set animation speed to ", m_animationSpeed);
        }

        void ShakeEffect::SetEnabled(bool enabled)
        {
            m_enabled = enabled;
            RF_LOG_INFO("ShakeEffect: Set enabled to ", enabled);
        }

        void ShakeEffect::SetOnCompleteCallback(ShakeCallback callback)
        {
            m_onCompleteCallback = callback;
            RF_LOG_INFO("ShakeEffect: Set on-complete callback");
        }

        void ShakeEffect::SetOnUpdateCallback(ShakeUpdateCallback callback)
        {
            m_onUpdateCallback = callback;
            RF_LOG_INFO("ShakeEffect: Set on-update callback");
        }

        ShakeType ShakeEffect::GetType() const
        {
            return m_type;
        }

        float ShakeEffect::GetIntensity() const
        {
            return m_intensity;
        }

        float ShakeEffect::GetFrequency() const
        {
            return m_frequency;
        }

        float ShakeEffect::GetDuration() const
        {
            return m_duration;
        }

        float ShakeEffect::GetCurrentTime() const
        {
            return m_currentTime;
        }

        float ShakeEffect::GetPositionX() const
        {
            return m_positionX;
        }

        float ShakeEffect::GetPositionY() const
        {
            return m_positionY;
        }

        float ShakeEffect::GetRotation() const
        {
            return m_rotation;
        }

        float ShakeEffect::GetScaleX() const
        {
            return m_scaleX;
        }

        float ShakeEffect::GetScaleY() const
        {
            return m_scaleY;
        }

        float ShakeEffect::GetFalloff() const
        {
            return m_falloff;
        }

        Quality ShakeEffect::GetQuality() const
        {
            return m_quality;
        }

        float ShakeEffect::GetDecay() const
        {
            return m_decay;
        }

        float ShakeEffect::GetDamping() const
        {
            return m_damping;
        }

        bool ShakeEffect::IsAnimated() const
        {
            return m_animated;
        }

        float ShakeEffect::GetAnimationSpeed() const
        {
            return m_animationSpeed;
        }

        bool ShakeEffect::IsEnabled() const
        {
            return m_enabled;
        }

        bool ShakeEffect::IsInitialized() const
        {
            return m_isInitialized;
        }

        void ShakeEffect::UpdateAnimation(float deltaTime)
        {
            float animPhase = m_time * m_animationSpeed;

            // Animation for intensity
            float intensityModulation = Math::Sin(animPhase * 0.5f) * 0.2f;
            m_intensity = Math::Max(0.0f, m_intensity * (1.0f + intensityModulation));

            // Animation for frequency
            float frequencyModulation = Math::Sin(animPhase * 0.7f) * 0.3f;
            m_frequency = Math::Max(0.1f, m_frequency * (1.0f + frequencyModulation));

            // Animation for falloff
            float falloffModulation = Math::Sin(animPhase * 0.3f) * 0.1f;
            m_falloff = Math::Max(0.1f, m_falloff * (1.0f + falloffModulation));
        }

        void ShakeEffect::UpdateDynamicParameters(float deltaTime)
        {
            // Update time-based effects
            float timeModulation = Math::Sin(m_time * 0.5f) * 0.05f;
            
            // Dynamic intensity pulsing
            m_intensity = Math::Max(0.0f, m_intensity + timeModulation * deltaTime);
            
            // Dynamic frequency
            m_frequency = Math::Max(0.1f, m_frequency + Math::Cos(m_time * 0.3f) * 0.05f * deltaTime);
            
            // Dynamic noise
            if (m_noiseStrength > 0.0f)
            {
                float noisePhase = m_time * 2.0f;
                m_noiseStrength = Math::Max(0.0f, m_noiseStrength + Math::Sin(noisePhase) * 0.1f * deltaTime);
            }
        }

        void ShakeEffect::UpdateNoise(float deltaTime)
        {
            // Update noise texture or parameters
            // This would be implemented using the rendering API
        }

        void ShakeEffect::UpdateShakeParameters()
        {
            // Set default parameters based on shake type
            switch (m_type)
            {
                case ShakeType::Positional:
                    m_intensity = 1.0f;
                    m_frequency = 10.0f;
                    m_falloff = 1.0f;
                    break;
                case ShakeType::Rotational:
                    m_intensity = 0.5f;
                    m_frequency = 8.0f;
                    m_falloff = 1.0f;
                    break;
                case ShakeType::Scale:
                    m_intensity = 0.2f;
                    m_frequency = 6.0f;
                    m_falloff = 1.0f;
                    break;
                case ShakeType::Combined:
                    m_intensity = 0.8f;
                    m_frequency = 10.0f;
                    m_falloff = 1.0f;
                    break;
                case ShakeType::Perlin:
                    m_noiseScale = 0.1f;
                    m_noiseStrength = 0.5f;
                    m_octaves = 3;
                    m_persistence = 0.5f;
                    m_lacunarity = 2.0f;
                    break;
                case ShakeType::Random:
                    m_intensity = 1.0f;
                    m_frequency = 15.0f;
                    m_falloff = 1.5f;
                    break;
                case ShakeType::Earthquake:
                    m_intensity = 2.0f;
                    m_frequency = 5.0f;
                    m_falloff = 0.5f;
                    m_octaves = 4;
                    m_persistence = 0.7f;
                    m_lacunarity = 2.5f;
                    break;
                case ShakeType::Explosion:
                    m_intensity = 3.0f;
                    m_frequency = 20.0f;
                    m_falloff = 2.0f;
                    m_decay = 2.0f;
                    break;
                case ShakeType::Vibration:
                    m_intensity = 0.3f;
                    m_frequency = 30.0f;
                    m_falloff = 1.0f;
                    break;
            }
        }

        void ShakeEffect::UpdateQualitySettings()
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

        void ShakeEffect::InitializeNoiseTexture()
        {
            // Initialize noise texture for noise-based shakes
            // This would be implemented using the rendering API
        }

        void ShakeEffect::CalculateShakeValues()
        {
            float timeFactor = m_currentTime / m_duration;
            float decayFactor = 1.0f;

            // Apply decay
            if (m_decay > 0.0f)
            {
                decayFactor = Math::Exp(-m_decay * timeFactor);
            }

            // Apply damping
            if (m_damping > 0.0f)
            {
                decayFactor *= Math::Exp(-m_damping * m_time);
            }

            float currentIntensity = m_intensity * decayFactor;

            switch (m_type)
            {
                case ShakeType::Positional:
                    CalculatePositionalShake(currentIntensity);
                    break;
                case ShakeType::Rotational:
                    CalculateRotationalShake(currentIntensity);
                    break;
                case ShakeType::Scale:
                    CalculateScaleShake(currentIntensity);
                    break;
                case ShakeType::Combined:
                    CalculateCombinedShake(currentIntensity);
                    break;
                case ShakeType::Perlin:
                    CalculatePerlinShake(currentIntensity);
                    break;
                case ShakeType::Random:
                    CalculateRandomShake(currentIntensity);
                    break;
                case ShakeType::Earthquake:
                    CalculateEarthquakeShake(currentIntensity);
                    break;
                case ShakeType::Explosion:
                    CalculateExplosionShake(currentIntensity);
                    break;
                case ShakeType::Vibration:
                    CalculateVibrationShake(currentIntensity);
                    break;
            }
        }

        void ShakeEffect::CalculatePositionalShake(float intensity)
        {
            float phase = m_time * m_frequency * Math::PI * 2.0f;
            
            m_positionX = Math::Sin(phase) * intensity;
            m_positionY = Math::Cos(phase * 1.3f) * intensity;
            m_rotation = 0.0f;
            m_scaleX = 1.0f;
            m_scaleY = 1.0f;
        }

        void ShakeEffect::CalculateRotationalShake(float intensity)
        {
            float phase = m_time * m_frequency * Math::PI * 2.0f;
            
            m_positionX = 0.0f;
            m_positionY = 0.0f;
            m_rotation = Math::Sin(phase) * intensity * 0.1f;
            m_scaleX = 1.0f;
            m_scaleY = 1.0f;
        }

        void ShakeEffect::CalculateScaleShake(float intensity)
        {
            float phase = m_time * m_frequency * Math::PI * 2.0f;
            
            m_positionX = 0.0f;
            m_positionY = 0.0f;
            m_rotation = 0.0f;
            m_scaleX = 1.0f + Math::Sin(phase) * intensity * 0.1f;
            m_scaleY = 1.0f + Math::Cos(phase * 1.2f) * intensity * 0.1f;
        }

        void ShakeEffect::CalculateCombinedShake(float intensity)
        {
            float phase = m_time * m_frequency * Math::PI * 2.0f;
            
            m_positionX = Math::Sin(phase) * intensity * 0.7f;
            m_positionY = Math::Cos(phase * 1.3f) * intensity * 0.7f;
            m_rotation = Math::Sin(phase * 0.8f) * intensity * 0.05f;
            m_scaleX = 1.0f + Math::Sin(phase * 1.5f) * intensity * 0.05f;
            m_scaleY = 1.0f + Math::Cos(phase * 1.7f) * intensity * 0.05f;
        }

        void ShakeEffect::CalculatePerlinShake(float intensity)
        {
            float noiseX = GeneratePerlinNoise(m_time * m_frequency, 0.0f, m_octaves, m_persistence, m_lacunarity);
            float noiseY = GeneratePerlinNoise(0.0f, m_time * m_frequency, m_octaves, m_persistence, m_lacunarity);
            float noiseRotation = GeneratePerlinNoise(m_time * m_frequency * 0.5f, m_time * m_frequency * 0.5f, m_octaves, m_persistence, m_lacunarity);
            
            m_positionX = noiseX * intensity;
            m_positionY = noiseY * intensity;
            m_rotation = noiseRotation * intensity * 0.05f;
            m_scaleX = 1.0f;
            m_scaleY = 1.0f;
        }

        void ShakeEffect::CalculateRandomShake(float intensity)
        {
            // Generate random values based on time
            uint32_t seed = (uint32_t)(m_time * 1000.0f);
            float randomX = GenerateRandom(seed) * 2.0f - 1.0f;
            float randomY = GenerateRandom(seed + 1) * 2.0f - 1.0f;
            float randomRotation = GenerateRandom(seed + 2) * 2.0f - 1.0f;
            
            m_positionX = randomX * intensity;
            m_positionY = randomY * intensity;
            m_rotation = randomRotation * intensity * 0.05f;
            m_scaleX = 1.0f;
            m_scaleY = 1.0f;
        }

        void ShakeEffect::CalculateEarthquakeShake(float intensity)
        {
            float phase1 = m_time * m_frequency * Math::PI * 2.0f;
            float phase2 = m_time * m_frequency * 2.3f * Math::PI * 2.0f;
            float phase3 = m_time * m_frequency * 3.7f * Math::PI * 2.0f;
            
            // Multiple frequency components for realistic earthquake
            m_positionX = (Math::Sin(phase1) * 0.6f + Math::Sin(phase2) * 0.3f + Math::Sin(phase3) * 0.1f) * intensity;
            m_positionY = (Math::Cos(phase1 * 1.1f) * 0.6f + Math::Cos(phase2 * 1.2f) * 0.3f + Math::Cos(phase3 * 1.3f) * 0.1f) * intensity;
            m_rotation = Math::Sin(phase1 * 0.5f) * intensity * 0.02f;
            m_scaleX = 1.0f;
            m_scaleY = 1.0f;
        }

        void ShakeEffect::CalculateExplosionShake(float intensity)
        {
            float timeFactor = 1.0f - (m_currentTime / m_duration);
            float explosionIntensity = intensity * timeFactor * timeFactor; // Quadratic falloff
            
            float phase = m_time * m_frequency * Math::PI * 2.0f;
            
            m_positionX = Math::Sin(phase) * explosionIntensity;
            m_positionY = Math::Cos(phase * 1.3f) * explosionIntensity;
            m_rotation = Math::Sin(phase * 0.8f) * explosionIntensity * 0.1f;
            m_scaleX = 1.0f + Math::Sin(phase * 1.5f) * explosionIntensity * 0.05f;
            m_scaleY = 1.0f + Math::Cos(phase * 1.7f) * explosionIntensity * 0.05f;
        }

        void ShakeEffect::CalculateVibrationShake(float intensity)
        {
            float phase = m_time * m_frequency * Math::PI * 2.0f;
            
            // High frequency, low amplitude vibration
            m_positionX = Math::Sin(phase) * intensity * 0.1f;
            m_positionY = Math::Sin(phase * 1.1f) * intensity * 0.1f;
            m_rotation = 0.0f;
            m_scaleX = 1.0f;
            m_scaleY = 1.0f;
        }

        float ShakeEffect::GeneratePerlinNoise(float x, float y, uint32_t octaves, float persistence, float lacunarity) const
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

        float ShakeEffect::GenerateSimpleNoise(float x, float y) const
        {
            // Simple noise function using sine waves
            return Math::Sin(x * 2.0f) * Math::Cos(y * 2.0f) * 0.5f + 0.5f;
        }

        float ShakeEffect::GenerateRandom(uint32_t seed) const
        {
            // Simple pseudo-random number generator
            seed = (seed * 1103515245 + 12345) & 0x7fffffff;
            return (float)seed / 2147483648.0f;
        }

        void ShakeEffect::ApplyShakeToTarget()
        {
            if (!m_target)
                return;

            // Apply shake effect to render target
            // This would be implemented using the rendering API
            // For now, we provide the structure

            // Create shake transformation
            RenderTarget* shakeTarget = CreateShakeTarget(m_target);
            if (shakeTarget)
            {
                // Apply transformation
                ApplyShakeTransformation(shakeTarget);

                // Blend with original target
                BlendWithTarget(m_target, shakeTarget);

                // Cleanup
                DestroyShakeTarget(shakeTarget);
            }
        }

        RenderTarget* ShakeEffect::CreateShakeTarget(RenderTarget* source)
        {
            // Create a render target for the shake effect
            // Platform-specific implementation
            return nullptr; // Placeholder
        }

        void ShakeEffect::ApplyShakeTransformation(RenderTarget* target)
        {
            // Apply the shake transformation to the target
            // Platform-specific implementation
        }

        void ShakeEffect::BlendWithTarget(RenderTarget* target, RenderTarget* shakeTarget)
        {
            // Blend the shake target with the original target
            // Platform-specific implementation
        }

        void ShakeEffect::DestroyShakeTarget(RenderTarget* shakeTarget)
        {
            // Clean up the shake target
            // Platform-specific implementation
        }

        void ShakeEffect::Reset()
        {
            m_type = ShakeType::Positional;
            m_intensity = 1.0f;
            m_frequency = 10.0f;
            m_duration = 1.0f;
            m_currentTime = 0.0f;
            m_positionX = 0.0f;
            m_positionY = 0.0f;
            m_rotation = 0.0f;
            m_scaleX = 1.0f;
            m_scaleY = 1.0f;
            m_falloff = 1.0f;
            m_quality = Quality::Medium;
            m_enabled = true;
            m_time = 0.0f;
            m_animated = false;
            m_animationSpeed = 1.0f;
            m_decay = 0.0f;
            m_damping = 0.0f;
            m_noiseScale = 0.1f;
            m_noiseStrength = 0.0f;
            m_octaves = 1;
            m_persistence = 0.5f;
            m_lacunarity = 2.0f;
            m_target = nullptr;

            UpdateShakeParameters();
            UpdateQualitySettings();

            RF_LOG_INFO("ShakeEffect: Reset to default values");
        }

        void ShakeEffect::Preset::ApplyPreset(ShakeEffect* effect, PresetType type)
        {
            switch (type)
            {
                case PresetType::Light:
                    effect->SetType(ShakeType::Positional);
                    effect->SetIntensity(0.3f);
                    effect->SetFrequency(8.0f);
                    effect->SetDuration(0.5f);
                    break;

                case PresetType::Medium:
                    effect->SetType(ShakeType::Combined);
                    effect->SetIntensity(0.8f);
                    effect->SetFrequency(10.0f);
                    effect->SetDuration(1.0f);
                    break;

                case PresetType::Heavy:
                    effect->SetType(ShakeType::Combined);
                    effect->SetIntensity(1.5f);
                    effect->SetFrequency(12.0f);
                    effect->SetDuration(1.5f);
                    break;

                case PresetType::Earthquake:
                    effect->SetType(ShakeType::Earthquake);
                    effect->SetIntensity(2.0f);
                    effect->SetFrequency(5.0f);
                    effect->SetDuration(3.0f);
                    break;

                case PresetType::Explosion:
                    effect->SetType(ShakeType::Explosion);
                    effect->SetIntensity(3.0f);
                    effect->SetFrequency(20.0f);
                    effect->SetDuration(0.5f);
                    break;

                case PresetType::Vibration:
                    effect->SetType(ShakeType::Vibration);
                    effect->SetIntensity(0.2f);
                    effect->SetFrequency(30.0f);
                    effect->SetDuration(2.0f);
                    break;

                case PresetType::Camera:
                    effect->SetType(ShakeType::Positional);
                    effect->SetIntensity(0.5f);
                    effect->SetFrequency(15.0f);
                    effect->SetDuration(0.3f);
                    effect->SetFalloff(2.0f);
                    break;

                case PresetType::Screen:
                    effect->SetType(ShakeType::Combined);
                    effect->SetIntensity(1.0f);
                    effect->SetFrequency(10.0f);
                    effect->SetDuration(1.0f);
                    effect->SetFalloff(1.5f);
                    break;
            }

            RF_LOG_INFO("ShakeEffect: Applied preset ", (int)type);
        }

        void ShakeEffect::SaveSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("ShakeEffect: Saved settings to ", filename);
        }

        void ShakeEffect::LoadSettings(const char* filename)
        {
            // Platform-specific implementation
            RF_LOG_INFO("ShakeEffect: Loaded settings from ", filename);
        }

    } // namespace Effects
} // namespace RedFiles