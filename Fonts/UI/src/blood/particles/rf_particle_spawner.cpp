#include "rf_particle_spawner.h"
#include "rf_blood_particle.h"
#include "../core/rf_logger.h"

namespace RedFiles {
    namespace Blood {

        ParticleSpawner::ParticleSpawner()
            : m_spawnRate(10.0f)
            , m_spawnTimer(0.0f)
            , m_maxParticles(1000)
            , m_particleCount(0)
            , m_isActive(true)
            , m_emissionShape(EmissionShape::Point)
            , m_emissionRadius(1.0f)
            , m_emissionWidth(1.0f)
            , m_emissionHeight(1.0f)
            , m_emissionDepth(1.0f)
            , m_burstMode(false)
            , m_burstCount(10)
            , m_burstTimer(0.0f)
            , m_burstInterval(1.0f)
            , m_initialVelocityMin(Vector3(0.0f, 0.0f, 0.0f))
            , m_initialVelocityMax(Vector3(0.0f, 0.0f, 0.0f))
            , m_initialSizeMin(0.1f)
            , m_initialSizeMax(0.3f)
            , m_initialLifetimeMin(3.0f)
            , m_initialLifetimeMax(5.0f)
            , m_initialVolumeMin(1.0f)
            , m_initialVolumeMax(2.0f)
            , m_initialPressureMin(100.0f)
            , m_initialPressureMax(120.0f)
            , m_initialTemperatureMin(36.5f)
            , m_initialTemperatureMax(37.5f)
            , m_initialViscosityMin(0.004f)
            , m_initialViscosityMax(0.006f)
            , m_initialCoagulationRateMin(0.1f)
            , m_initialCoagulationRateMax(0.2f)
            , m_initialEvaporationRateMin(0.05f)
            , m_initialEvaporationRateMax(0.1f)
            , m_colorVariation(0.1f)
            , m_sizeVariation(0.2f)
            , m_velocityVariation(0.3f)
            , m_lifetimeVariation(0.25f)
            , m_spawnDirection(Vector3(0.0f, 1.0f, 0.0f))
            , m_directionSpread(45.0f)
            , m_surfaceNormal(Vector3(0.0f, 1.0f, 0.0f))
            , m_spawnOnSurface(false)
            , m_inheritVelocity(false)
            , m_parentVelocity(Vector3(0.0f, 0.0f, 0.0f))
            , m_gravityInfluence(1.0f)
            , m_windInfluence(1.0f)
            , m_turbulenceInfluence(0.5f)
            , m_pressureInfluence(0.3f)
            , m_temperatureInfluence(0.2f)
            , m_humidityInfluence(0.1f)
            , m_spawnDelay(0.0f)
            , m_spawnDuration(-1.0f)
            , m_spawnTimer(0.0f)
            , m_loopInfinitely(true)
            , m_spawnCount(0)
            , m_maxSpawnCount(-1)
        {
            RF_LOG_INFO("ParticleSpawner: Created new spawner");
        }

        ParticleSpawner::~ParticleSpawner()
        {
            RF_LOG_INFO("ParticleSpawner: Destroyed spawner");
        }

        void ParticleSpawner::Initialize()
        {
            m_spawnTimer = 0.0f;
            m_particleCount = 0;
            m_spawnCount = 0;
            
            if (m_spawnDelay > 0.0f)
            {
                m_spawnTimer = -m_spawnDelay;
            }
            
            RF_LOG_INFO("ParticleSpawner: Initialized spawner with rate ", m_spawnRate, " particles/sec");
        }

        void ParticleSpawner::Update(float deltaTime, const Vector3& position)
        {
            if (!m_isActive)
            {
                return;
            }
            
            m_spawnTimer += deltaTime;
            
            // Check if we're still within spawn duration
            if (m_spawnDuration > 0.0f && m_spawnTimer > m_spawnDuration)
            {
                if (!m_loopInfinitely)
                {
                    m_isActive = false;
                    return;
                }
                else
                {
                    m_spawnTimer = 0.0f;
                }
            }
            
            // Check if we've reached max spawn count
            if (m_maxSpawnCount > 0 && m_spawnCount >= m_maxSpawnCount)
            {
                m_isActive = false;
                return;
            }
            
            if (m_burstMode)
            {
                UpdateBurstMode(deltaTime, position);
            }
            else
            {
                UpdateContinuousMode(deltaTime, position);
            }
        }

        void ParticleSpawner::UpdateContinuousMode(float deltaTime, const Vector3& position)
        {
            float spawnInterval = 1.0f / m_spawnRate;
            
            while (m_spawnTimer >= spawnInterval && m_particleCount < m_maxParticles)
            {
                SpawnParticle(position);
                m_spawnTimer -= spawnInterval;
            }
        }

        void ParticleSpawner::UpdateBurstMode(float deltaTime, const Vector3& position)
        {
            m_burstTimer += deltaTime;
            
            if (m_burstTimer >= m_burstInterval)
            {
                int particlesToSpawn = Math::Min(m_burstCount, m_maxParticles - m_particleCount);
                
                for (int i = 0; i < particlesToSpawn; ++i)
                {
                    SpawnParticle(position);
                }
                
                m_burstTimer = 0.0f;
            }
        }

        BloodParticle* ParticleSpawner::SpawnParticle(const Vector3& position)
        {
            if (m_particleCount >= m_maxParticles)
            {
                return nullptr;
            }
            
            BloodParticle* particle = RF_NEW BloodParticle();
            
            // Calculate spawn position based on emission shape
            Vector3 spawnPosition = CalculateSpawnPosition(position);
            
            // Calculate initial properties with variations
            Vector3 velocity = CalculateInitialVelocity();
            float size = CalculateInitialSize();
            float lifetime = CalculateInitialLifetime();
            float volume = CalculateInitialVolume();
            float pressure = CalculateInitialPressure();
            float temperature = CalculateInitialTemperature();
            float viscosity = CalculateInitialViscosity();
            float coagulationRate = CalculateInitialCoagulationRate();
            float evaporationRate = CalculateInitialEvaporationRate();
            Color color = CalculateInitialColor();
            
            // Initialize particle
            particle->Initialize(spawnPosition, velocity, size, lifetime);
            particle->SetBloodProperties(volume, pressure, temperature, viscosity, coagulationRate, evaporationRate);
            particle->SetColor(color);
            
            // Apply environmental influences
            particle->SetGravityInfluence(m_gravityInfluence);
            particle->SetWindInfluence(m_windInfluence);
            particle->SetTurbulenceInfluence(m_turbulenceInfluence);
            particle->SetPressureInfluence(m_pressureInfluence);
            particle->SetTemperatureInfluence(m_temperatureInfluence);
            particle->SetHumidityInfluence(m_humidityInfluence);
            
            m_particleCount++;
            m_spawnCount++;
            
            RF_LOG_INFO("ParticleSpawner: Spawned particle at (", spawnPosition.x, ", ", spawnPosition.y, ", ", spawnPosition.z, ")");
            
            return particle;
        }

        Vector3 ParticleSpawner::CalculateSpawnPosition(const Vector3& basePosition)
        {
            Vector3 position = basePosition;
            
            switch (m_emissionShape)
            {
                case EmissionShape::Point:
                    // No offset from base position
                    break;
                    
                case EmissionShape::Sphere:
                {
                    float theta = Math::Random() * Math::TWO_PI;
                    float phi = Math::Random() * Math::PI;
                    float r = Math::Random() * m_emissionRadius;
                    
                    position.x += r * Math::Sin(phi) * Math::Cos(theta);
                    position.y += r * Math::Sin(phi) * Math::Sin(theta);
                    position.z += r * Math::Cos(phi);
                    break;
                }
                    
                case EmissionShape::Circle:
                {
                    float angle = Math::Random() * Math::TWO_PI;
                    float radius = Math::Random() * m_emissionRadius;
                    
                    position.x += radius * Math::Cos(angle);
                    position.z += radius * Math::Sin(angle);
                    break;
                }
                    
                case EmissionShape::Box:
                {
                    position.x += (Math::Random() - 0.5f) * m_emissionWidth;
                    position.y += (Math::Random() - 0.5f) * m_emissionHeight;
                    position.z += (Math::Random() - 0.5f) * m_emissionDepth;
                    break;
                }
                    
                case EmissionShape::Cylinder:
                {
                    float angle = Math::Random() * Math::TWO_PI;
                    float radius = Math::Random() * m_emissionRadius;
                    float height = (Math::Random() - 0.5f) * m_emissionHeight;
                    
                    position.x += radius * Math::Cos(angle);
                    position.y += height;
                    position.z += radius * Math::Sin(angle);
                    break;
                }
                    
                case EmissionShape::Cone:
                {
                    float angle = Math::Random() * Math::TWO_PI;
                    float height = Math::Random() * m_emissionHeight;
                    float radiusAtHeight = (height / m_emissionHeight) * m_emissionRadius;
                    
                    position.x += radiusAtHeight * Math::Cos(angle);
                    position.y += height;
                    position.z += radiusAtHeight * Math::Sin(angle);
                    break;
                }
            }
            
            // If spawning on surface, project position onto surface
            if (m_spawnOnSurface)
            {
                // Simple projection onto surface defined by normal
                Vector3 toSurface = m_surfaceNormal * Vector3::Dot(position - basePosition, m_surfaceNormal);
                position = position - toSurface;
            }
            
            return position;
        }

        Vector3 ParticleSpawner::CalculateInitialVelocity()
        {
            Vector3 velocity = m_spawnDirection;
            
            // Apply direction spread
            if (m_directionSpread > 0.0f)
            {
                float yaw = (Math::Random() - 0.5f) * m_directionSpread * Math::DEG_TO_RAD;
                float pitch = (Math::Random() - 0.5f) * m_directionSpread * Math::DEG_TO_RAD;
                
                // Create rotation matrix for yaw and pitch
                float cosYaw = Math::Cos(yaw);
                float sinYaw = Math::Sin(yaw);
                float cosPitch = Math::Cos(pitch);
                float sinPitch = Math::Sin(pitch);
                
                Vector3 rotated;
                rotated.x = velocity.x * cosYaw - velocity.z * sinYaw;
                rotated.z = velocity.x * sinYaw + velocity.z * cosYaw;
                rotated.y = rotated.y * cosPitch - velocity.z * sinPitch;
                rotated.z = rotated.y * sinPitch + rotated.z * cosPitch;
                
                velocity = rotated;
            }
            
            // Apply velocity variation
            Vector3 minVel = m_initialVelocityMin;
            Vector3 maxVel = m_initialVelocityMax;
            
            float speedVariation = 1.0f + (Math::Random() - 0.5f) * m_velocityVariation;
            
            velocity.x = velocity.x * speedVariation + Math::RandomRange(minVel.x, maxVel.x);
            velocity.y = velocity.y * speedVariation + Math::RandomRange(minVel.y, maxVel.y);
            velocity.z = velocity.z * speedVariation + Math::RandomRange(minVel.z, maxVel.z);
            
            // Inherit parent velocity if enabled
            if (m_inheritVelocity)
            {
                velocity += m_parentVelocity;
            }
            
            return velocity;
        }

        float ParticleSpawner::CalculateInitialSize()
        {
            float baseSize = Math::RandomRange(m_initialSizeMin, m_initialSizeMax);
            float variation = 1.0f + (Math::Random() - 0.5f) * m_sizeVariation;
            return baseSize * variation;
        }

        float ParticleSpawner::CalculateInitialLifetime()
        {
            float baseLifetime = Math::RandomRange(m_initialLifetimeMin, m_initialLifetimeMax);
            float variation = 1.0f + (Math::Random() - 0.5f) * m_lifetimeVariation;
            return baseLifetime * variation;
        }

        float ParticleSpawner::CalculateInitialVolume()
        {
            return Math::RandomRange(m_initialVolumeMin, m_initialVolumeMax);
        }

        float ParticleSpawner::CalculateInitialPressure()
        {
            return Math::RandomRange(m_initialPressureMin, m_initialPressureMax);
        }

        float ParticleSpawner::CalculateInitialTemperature()
        {
            return Math::RandomRange(m_initialTemperatureMin, m_initialTemperatureMax);
        }

        float ParticleSpawner::CalculateInitialViscosity()
        {
            return Math::RandomRange(m_initialViscosityMin, m_initialViscosityMax);
        }

        float ParticleSpawner::CalculateInitialCoagulationRate()
        {
            return Math::RandomRange(m_initialCoagulationRateMin, m_initialCoagulationRateMax);
        }

        float ParticleSpawner::CalculateInitialEvaporationRate()
        {
            return Math::RandomRange(m_initialEvaporationRateMin, m_initialEvaporationRateMax);
        }

        Color ParticleSpawner::CalculateInitialColor()
        {
            Color baseColor = Color(0.8f, 0.1f, 0.1f, 1.0f); // Blood red
            
            float variation = (Math::Random() - 0.5f) * m_colorVariation;
            
            Color color;
            color.r = Math::Clamp(baseColor.r + variation, 0.0f, 1.0f);
            color.g = Math::Clamp(baseColor.g + variation * 0.5f, 0.0f, 1.0f);
            color.b = Math::Clamp(baseColor.b + variation * 0.5f, 0.0f, 1.0f);
            color.a = Math::Clamp(baseColor.a + variation * 0.2f, 0.0f, 1.0f);
            
            return color;
        }

        void ParticleSpawner::SetSpawnRate(float rate)
        {
            m_spawnRate = Math::Max(0.0f, rate);
        }

        void ParticleSpawner::SetMaxParticles(int maxParticles)
        {
            m_maxParticles = Math::Max(0, maxParticles);
        }

        void ParticleSpawner::SetEmissionShape(EmissionShape shape)
        {
            m_emissionShape = shape;
        }

        void ParticleSpawner::SetEmissionRadius(float radius)
        {
            m_emissionRadius = Math::Max(0.0f, radius);
        }

        void ParticleSpawner::SetEmissionBox(float width, float height, float depth)
        {
            m_emissionWidth = Math::Max(0.0f, width);
            m_emissionHeight = Math::Max(0.0f, height);
            m_emissionDepth = Math::Max(0.0f, depth);
        }

        void ParticleSpawner::SetBurstMode(bool burstMode, int burstCount, float burstInterval)
        {
            m_burstMode = burstMode;
            m_burstCount = Math::Max(1, burstCount);
            m_burstInterval = Math::Max(0.01f, burstInterval);
            m_burstTimer = 0.0f;
        }

        void ParticleSpawner::SetInitialVelocityRange(const Vector3& minVel, const Vector3& maxVel)
        {
            m_initialVelocityMin = minVel;
            m_initialVelocityMax = maxVel;
        }

        void ParticleSpawner::SetInitialSizeRange(float minSize, float maxSize)
        {
            m_initialSizeMin = Math::Max(0.001f, minSize);
            m_initialSizeMax = Math::Max(m_initialSizeMin, maxSize);
        }

        void ParticleSpawner::SetInitialLifetimeRange(float minLifetime, float maxLifetime)
        {
            m_initialLifetimeMin = Math::Max(0.1f, minLifetime);
            m_initialLifetimeMax = Math::Max(m_initialLifetimeMin, maxLifetime);
        }

        void ParticleSpawner::SetSpawnDirection(const Vector3& direction, float spread)
        {
            m_spawnDirection = direction.Normalized();
            m_directionSpread = Math::Max(0.0f, spread);
        }

        void ParticleSpawner::SetVariation(float color, float size, float velocity, float lifetime)
        {
            m_colorVariation = Math::Max(0.0f, color);
            m_sizeVariation = Math::Max(0.0f, size);
            m_velocityVariation = Math::Max(0.0f, velocity);
            m_lifetimeVariation = Math::Max(0.0f, lifetime);
        }

        void ParticleSpawner::SetActive(bool active)
        {
            m_isActive = active;
            if (active)
            {
                m_spawnTimer = 0.0f;
                m_burstTimer = 0.0f;
            }
        }

        void ParticleSpawner::Reset()
        {
            m_particleCount = 0;
            m_spawnCount = 0;
            m_spawnTimer = 0.0f;
            m_burstTimer = 0.0f;
            
            if (m_spawnDelay > 0.0f)
            {
                m_spawnTimer = -m_spawnDelay;
            }
            
            RF_LOG_INFO("ParticleSpawner: Reset spawner");
        }

        void ParticleSpawner::OnParticleDestroyed()
        {
            if (m_particleCount > 0)
            {
                m_particleCount--;
            }
        }

        bool ParticleSpawner::CanSpawn() const
        {
            return m_isActive && m_particleCount < m_maxParticles;
        }

        float ParticleSpawner::GetSpawnProgress() const
        {
            if (m_spawnDuration <= 0.0f)
            {
                return 1.0f;
            }
            
            return Math::Clamp(m_spawnTimer / m_spawnDuration, 0.0f, 1.0f);
        }

    } // namespace Blood
} // namespace RedFiles
