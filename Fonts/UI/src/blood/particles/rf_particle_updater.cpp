#include "rf_particle_updater.h"
#include "rf_blood_particle.h"
#include "../core/rf_logger.h"

namespace RedFiles {
    namespace Blood {

        ParticleUpdater::ParticleUpdater()
            : m_particles(nullptr)
            , m_maxParticles(1000)
            , m_particleCount(0)
            , m_gravity(Vector3(0.0f, -9.81f, 0.0f))
            , m_wind(Vector3(0.0f, 0.0f, 0.0f))
            , m_temperature(20.0f)
            , m_humidity(0.5f)
            , m_pressure(101.325f)
            , m_turbulenceStrength(0.1f)
            , m_turbulenceFrequency(1.0f)
            , m_timeScale(1.0f)
            , m_physicsEnabled(true)
            , m_collisionEnabled(true)
            , m_bloodEffectsEnabled(true)
            , m_environmentalEffectsEnabled(true)
            , m_updateInterval(0.016f) // 60 FPS default
            , m_accumulatedTime(0.0f)
            , m_globalDamping(0.99f)
            , m_globalDrag(0.98f)
            , m_boundaryMin(Vector3(-100.0f, -100.0f, -100.0f))
            , m_boundaryMax(Vector3(100.0f, 100.0f, 100.0f))
            , m_boundaryMode(BoundaryMode::Clamp)
            , m_collisionNormal(Vector3(0.0f, 1.0f, 0.0f))
            , m_collisionDamping(0.8f)
            , m_collisionFriction(0.7f)
            , m_surfaceTension(0.0728f)
            , m_coagulationFactor(1.0f)
            , m_evaporationFactor(1.0f)
            , m_oxidationRate(0.01f)
            , m_bloodDensity(1060.0f) // kg/m³
            , m_airDensity(1.225f) // kg/m³
            , m_updateCount(0)
            , m_totalUpdateTime(0.0f)
        {
            m_particles = RF_NEW BloodParticle*[m_maxParticles];
            for (int i = 0; i < m_maxParticles; ++i)
            {
                m_particles[i] = nullptr;
            }
            
            RF_LOG_INFO("ParticleUpdater: Created updater with capacity for ", m_maxParticles, " particles");
        }

        ParticleUpdater::~ParticleUpdater()
        {
            ClearAllParticles();
            RF_DELETE[] m_particles;
            RF_LOG_INFO("ParticleUpdater: Destroyed updater");
        }

        void ParticleUpdater::Initialize()
        {
            m_accumulatedTime = 0.0f;
            m_updateCount = 0;
            m_totalUpdateTime = 0.0f;
            
            RF_LOG_INFO("ParticleUpdater: Initialized with ", m_particleCount, " particles");
        }

        void ParticleUpdater::Update(float deltaTime)
        {
            if (!m_physicsEnabled)
            {
                return;
            }
            
            m_accumulatedTime += deltaTime * m_timeScale;
            
            // Fixed timestep for consistent physics
            while (m_accumulatedTime >= m_updateInterval)
            {
                float fixedDelta = m_updateInterval;
                
                // Update all particles
                for (int i = 0; i < m_maxParticles; ++i)
                {
                    BloodParticle* particle = m_particles[i];
                    if (particle && particle->IsActive())
                    {
                        UpdateParticle(particle, fixedDelta);
                    }
                }
                
                // Remove dead particles
                RemoveDeadParticles();
                
                m_accumulatedTime -= fixedDelta;
                m_updateCount++;
                m_totalUpdateTime += fixedDelta;
            }
        }

        void ParticleUpdater::UpdateParticle(BloodParticle* particle, float deltaTime)
        {
            if (!particle || !particle->IsActive())
            {
                return;
            }
            
            // Update particle lifecycle
            particle->Update(deltaTime);
            
            if (!particle->IsActive())
            {
                return;
            }
            
            // Apply physics
            ApplyPhysics(particle, deltaTime);
            
            // Apply environmental effects
            if (m_environmentalEffectsEnabled)
            {
                ApplyEnvironmentalEffects(particle, deltaTime);
            }
            
            // Apply blood-specific effects
            if (m_bloodEffectsEnabled)
            {
                ApplyBloodEffects(particle, deltaTime);
            }
            
            // Handle collisions
            if (m_collisionEnabled)
            {
                HandleCollisions(particle, deltaTime);
            }
            
            // Apply boundaries
            ApplyBoundaries(particle, deltaTime);
            
            // Apply global damping and drag
            ApplyGlobalEffects(particle, deltaTime);
        }

        void ParticleUpdater::ApplyPhysics(BloodParticle* particle, float deltaTime)
        {
            Vector3 position = particle->GetPosition();
            Vector3 velocity = particle->GetVelocity();
            Vector3 acceleration = particle->GetAcceleration();
            
            // Apply gravity
            Vector3 gravityForce = m_gravity * particle->GetGravityInfluence();
            acceleration += gravityForce;
            
            // Apply air resistance (drag)
            float dragCoefficient = 0.47f; // Sphere drag coefficient
            float crossSection = Math::PI * particle->GetSize() * particle->GetSize();
            Vector3 dragForce = -0.5f * m_airDensity * dragCoefficient * crossSection * velocity.Length() * velocity.Normalized();
            acceleration += dragForce / m_bloodDensity;
            
            // Update velocity and position
            velocity += acceleration * deltaTime;
            position += velocity * deltaTime;
            
            particle->SetVelocity(velocity);
            particle->SetPosition(position);
            particle->SetAcceleration(Vector3(0.0f, 0.0f, 0.0f)); // Reset acceleration
        }

        void ParticleUpdater::ApplyEnvironmentalEffects(BloodParticle* particle, float deltaTime)
        {
            Vector3 velocity = particle->GetVelocity();
            
            // Apply wind force
            Vector3 windForce = m_wind * particle->GetWindInfluence();
            Vector3 acceleration = windForce * 0.1f; // Scale wind effect
            
            // Apply turbulence
            if (m_turbulenceStrength > 0.0f)
            {
                Vector3 turbulence = GenerateTurbulence(particle->GetPosition(), m_totalUpdateTime);
                acceleration += turbulence * m_turbulenceStrength * particle->GetTurbulenceInfluence();
            }
            
            // Apply pressure effects
            float pressureEffect = (m_pressure - 101.325f) * 0.001f; // Normalize around standard pressure
            acceleration += Vector3(pressureEffect, 0.0f, pressureEffect) * particle->GetPressureInfluence();
            
            // Apply temperature effects on viscosity
            float temperatureEffect = (m_temperature - 20.0f) * 0.01f;
            float viscosityModifier = 1.0f + temperatureEffect * particle->GetTemperatureInfluence();
            velocity *= viscosityModifier;
            
            // Apply humidity effects on evaporation
            float humidityEffect = m_humidity * particle->GetHumidityInfluence();
            
            particle->SetAcceleration(acceleration);
            particle->SetVelocity(velocity);
        }

        void ParticleUpdater::ApplyBloodEffects(BloodParticle* particle, float deltaTime)
        {
            float volume = particle->GetVolume();
            float coagulationRate = particle->GetCoagulationRate();
            float evaporationRate = particle->GetEvaporationRate();
            float temperature = particle->GetTemperature();
            
            // Apply coagulation
            float coagulationAmount = coagulationRate * m_coagulationFactor * deltaTime;
            float newCoagulation = particle->GetCoagulationLevel() + coagulationAmount;
            particle->SetCoagulationLevel(newCoagulation);
            
            // Coagulation affects viscosity and color
            if (newCoagulation > 0.5f)
            {
                float viscosityIncrease = newCoagulation * 0.002f;
                float newViscosity = particle->GetViscosity() + viscosityIncrease;
                particle->SetViscosity(newViscosity);
                
                // Darken color as blood coagulates
                Color color = particle->GetColor();
                color.r *= (1.0f - newCoagulation * 0.3f);
                color.g *= (1.0f - newCoagulation * 0.2f);
                particle->SetColor(color);
            }
            
            // Apply evaporation
            float evaporationAmount = evaporationRate * m_evaporationFactor * deltaTime;
            float newVolume = Math::Max(volume - evaporationAmount, 0.1f);
            particle->SetVolume(newVolume);
            
            // Volume affects size
            float sizeRatio = Math::Pow(newVolume / volume, 1.0f / 3.0f);
            float newSize = particle->GetSize() * sizeRatio;
            particle->SetSize(newSize);
            
            // Apply oxidation (blood darkens over time)
            float oxidationAmount = m_oxidationRate * deltaTime;
            Color color = particle->GetColor();
            color.r = Math::Max(color.r - oxidationAmount, 0.3f);
            color.g = Math::Max(color.g - oxidationAmount * 0.5f, 0.05f);
            particle->SetColor(color);
            
            // Apply surface tension effects for small droplets
            if (newSize < 0.5f)
            {
                Vector3 velocity = particle->GetVelocity();
                float surfaceTensionForce = m_surfaceTension / newSize;
                velocity *= (1.0f - surfaceTensionForce * deltaTime * 0.1f);
                particle->SetVelocity(velocity);
            }
        }

        void ParticleUpdater::HandleCollisions(BloodParticle* particle, float deltaTime)
        {
            Vector3 position = particle->GetPosition();
            Vector3 velocity = particle->GetVelocity();
            
            // Simple ground collision (y = 0 plane)
            if (position.y <= 0.0f)
            {
                position.y = 0.0f;
                
                // Apply collision response
                Vector3 normal = Vector3(0.0f, 1.0f, 0.0f);
                float dotProduct = Vector3::Dot(velocity, normal);
                
                if (dotProduct < 0.0f) // Moving into surface
                {
                    // Reflect velocity with damping
                    velocity = velocity - 2.0f * dotProduct * normal;
                    velocity *= m_collisionDamping;
                    
                    // Apply friction
                    Vector3 tangentVelocity = velocity - Vector3::Dot(velocity, normal) * normal;
                    tangentVelocity *= m_collisionFriction;
                    velocity = Vector3::Dot(velocity, normal) * normal + tangentVelocity;
                    
                    // Create splash effect for high-velocity collisions
                    if (Math::Abs(dotProduct) > 2.0f)
                    {
                        CreateSplashEffect(particle, position, normal);
                    }
                }
                
                particle->SetPosition(position);
                particle->SetVelocity(velocity);
            }
            
            // Particle-to-particle collisions (simplified)
            for (int i = 0; i < m_maxParticles; ++i)
            {
                BloodParticle* other = m_particles[i];
                if (other && other != particle && other->IsActive())
                {
                    Vector3 otherPosition = other->GetPosition();
                    float distance = Vector3::Distance(position, otherPosition);
                    float minDistance = particle->GetSize() + other->GetSize();
                    
                    if (distance < minDistance && distance > 0.0f)
                    {
                        // Simple separation
                        Vector3 separation = (position - otherPosition).Normalized();
                        float overlap = minDistance - distance;
                        position += separation * overlap * 0.5f;
                        
                        // Exchange some velocity (simplified elastic collision)
                        Vector3 relativeVelocity = velocity - other->GetVelocity();
                        float velocityAlongNormal = Vector3::Dot(relativeVelocity, separation);
                        
                        if (velocityAlongNormal > 0.0f)
                        {
                            float impulse = 2.0f * velocityAlongNormal / 2.0f; // Equal mass assumption
                            velocity -= impulse * separation * 0.8f; // Some energy loss
                        }
                    }
                }
            }
            
            particle->SetPosition(position);
            particle->SetVelocity(velocity);
        }

        void ParticleUpdater::ApplyBoundaries(BloodParticle* particle, float deltaTime)
        {
            Vector3 position = particle->GetPosition();
            Vector3 velocity = particle->GetVelocity();
            bool hitBoundary = false;
            
            switch (m_boundaryMode)
            {
                case BoundaryMode::Clamp:
                {
                    if (position.x < m_boundaryMin.x) { position.x = m_boundaryMin.x; velocity.x = Math::Abs(velocity.x) * m_collisionDamping; hitBoundary = true; }
                    if (position.x > m_boundaryMax.x) { position.x = m_boundaryMax.x; velocity.x = -Math::Abs(velocity.x) * m_collisionDamping; hitBoundary = true; }
                    if (position.y < m_boundaryMin.y) { position.y = m_boundaryMin.y; velocity.y = Math::Abs(velocity.y) * m_collisionDamping; hitBoundary = true; }
                    if (position.y > m_boundaryMax.y) { position.y = m_boundaryMax.y; velocity.y = -Math::Abs(velocity.y) * m_collisionDamping; hitBoundary = true; }
                    if (position.z < m_boundaryMin.z) { position.z = m_boundaryMin.z; velocity.z = Math::Abs(velocity.z) * m_collisionDamping; hitBoundary = true; }
                    if (position.z > m_boundaryMax.z) { position.z = m_boundaryMax.z; velocity.z = -Math::Abs(velocity.z) * m_collisionDamping; hitBoundary = true; }
                    break;
                }
                
                case BoundaryMode::Wrap:
                {
                    if (position.x < m_boundaryMin.x) position.x = m_boundaryMax.x;
                    if (position.x > m_boundaryMax.x) position.x = m_boundaryMin.x;
                    if (position.y < m_boundaryMin.y) position.y = m_boundaryMax.y;
                    if (position.y > m_boundaryMax.y) position.y = m_boundaryMin.y;
                    if (position.z < m_boundaryMin.z) position.z = m_boundaryMax.z;
                    if (position.z > m_boundaryMax.z) position.z = m_boundaryMin.z;
                    break;
                }
                
                case BoundaryMode::Destroy:
                {
                    if (position.x < m_boundaryMin.x || position.x > m_boundaryMax.x ||
                        position.y < m_boundaryMin.y || position.y > m_boundaryMax.y ||
                        position.z < m_boundaryMin.z || position.z > m_boundaryMax.z)
                    {
                        particle->SetLifetime(0.0f); // Mark for destruction
                        return;
                    }
                    break;
                }
            }
            
            particle->SetPosition(position);
            particle->SetVelocity(velocity);
        }

        void ParticleUpdater::ApplyGlobalEffects(BloodParticle* particle, float deltaTime)
        {
            Vector3 velocity = particle->GetVelocity();
            
            // Apply global damping
            velocity *= Math::Pow(m_globalDamping, deltaTime);
            
            // Apply global drag
            velocity *= Math::Pow(m_globalDrag, deltaTime);
            
            particle->SetVelocity(velocity);
        }

        Vector3 ParticleUpdater::GenerateTurbulence(const Vector3& position, float time)
        {
            float x = position.x * m_turbulenceFrequency + time;
            float y = position.y * m_turbulenceFrequency + time * 1.3f;
            float z = position.z * m_turbulenceFrequency + time * 0.7f;
            
            // Simple noise function (can be replaced with Perlin noise)
            float noiseX = Math::Sin(x * 1.1f) * Math::Cos(y * 0.9f) * Math::Sin(z * 1.2f);
            float noiseY = Math::Cos(x * 0.8f) * Math::Sin(y * 1.3f) * Math::Cos(z * 0.7f);
            float noiseZ = Math::Sin(x * 0.9f) * Math::Cos(y * 1.1f) * Math::Sin(z * 0.8f);
            
            return Vector3(noiseX, noiseY, noiseZ);
        }

        void ParticleUpdater::CreateSplashEffect(BloodParticle* particle, const Vector3& position, const Vector3& normal)
        {
            // This would create smaller droplets - simplified for now
            float impactForce = Math::Abs(Vector3::Dot(particle->GetVelocity(), normal));
            
            if (impactForce > 5.0f && m_particleCount < m_maxParticles - 5)
            {
                // Create 2-3 smaller droplets
                int dropletCount = Math::RandomRange(2, 4);
                
                for (int i = 0; i < dropletCount && m_particleCount < m_maxParticles; ++i)
                {
                    BloodParticle* droplet = RF_NEW BloodParticle();
                    
                    Vector3 dropletPosition = position + normal * 0.1f;
                    Vector3 dropletVelocity = Vector3(
                        Math::RandomRange(-2.0f, 2.0f),
                        Math::RandomRange(1.0f, 3.0f),
                        Math::RandomRange(-2.0f, 2.0f)
                    );
                    
                    float dropletSize = particle->GetSize() * 0.3f;
                    float dropletLifetime = Math::RandomRange(0.5f, 1.5f);
                    
                    droplet->Initialize(dropletPosition, dropletVelocity, dropletSize, dropletLifetime);
                    droplet->SetBloodProperties(
                        particle->GetVolume() * 0.1f,
                        particle->GetPressure(),
                        particle->GetTemperature(),
                        particle->GetViscosity(),
                        particle->GetCoagulationRate(),
                        particle->GetEvaporationRate() * 1.5f // Higher evaporation for droplets
                    );
                    droplet->SetColor(particle->GetColor());
                    
                    AddParticle(droplet);
                }
            }
        }

        void ParticleUpdater::AddParticle(BloodParticle* particle)
        {
            if (!particle || m_particleCount >= m_maxParticles)
            {
                return;
            }
            
            // Find first empty slot
            for (int i = 0; i < m_maxParticles; ++i)
            {
                if (m_particles[i] == nullptr)
                {
                    m_particles[i] = particle;
                    m_particleCount++;
                    return;
                }
            }
            
            // No empty slots found, delete particle
            RF_DELETE particle;
        }

        void ParticleUpdater::RemoveParticle(BloodParticle* particle)
        {
            if (!particle)
            {
                return;
            }
            
            for (int i = 0; i < m_maxParticles; ++i)
            {
                if (m_particles[i] == particle)
                {
                    m_particles[i] = nullptr;
                    m_particleCount--;
                    RF_DELETE particle;
                    return;
                }
            }
        }

        void ParticleUpdater::RemoveDeadParticles()
        {
            for (int i = 0; i < m_maxParticles; ++i)
            {
                BloodParticle* particle = m_particles[i];
                if (particle && !particle->IsActive())
                {
                    m_particles[i] = nullptr;
                    m_particleCount--;
                    RF_DELETE particle;
                }
            }
        }

        void ParticleUpdater::ClearAllParticles()
        {
            for (int i = 0; i < m_maxParticles; ++i)
            {
                if (m_particles[i])
                {
                    RF_DELETE m_particles[i];
                    m_particles[i] = nullptr;
                }
            }
            m_particleCount = 0;
        }

        void ParticleUpdater::SetEnvironment(const Vector3& gravity, const Vector3& wind, float temperature, float humidity, float pressure)
        {
            m_gravity = gravity;
            m_wind = wind;
            m_temperature = temperature;
            m_humidity = Math::Clamp(humidity, 0.0f, 1.0f);
            m_pressure = Math::Max(0.0f, pressure);
        }

        void ParticleUpdater::SetTurbulence(float strength, float frequency)
        {
            m_turbulenceStrength = Math::Max(0.0f, strength);
            m_turbulenceFrequency = Math::Max(0.01f, frequency);
        }

        void ParticleUpdater::SetBoundaries(const Vector3& min, const Vector3& max, BoundaryMode mode)
        {
            m_boundaryMin = min;
            m_boundaryMax = max;
            m_boundaryMode = mode;
        }

        void ParticleUpdater::SetCollisionProperties(float damping, float friction)
        {
            m_collisionDamping = Math::Clamp(damping, 0.0f, 1.0f);
            m_collisionFriction = Math::Clamp(friction, 0.0f, 1.0f);
        }

        void ParticleUpdater::SetBloodEffects(float surfaceTension, float coagulationFactor, float evaporationFactor, float oxidationRate)
        {
            m_surfaceTension = Math::Max(0.0f, surfaceTension);
            m_coagulationFactor = Math::Max(0.0f, coagulationFactor);
            m_evaporationFactor = Math::Max(0.0f, evaporationFactor);
            m_oxidationRate = Math::Max(0.0f, oxidationRate);
        }

        void ParticleUpdater::SetGlobalEffects(float damping, float drag)
        {
            m_globalDamping = Math::Clamp(damping, 0.0f, 1.0f);
            m_globalDrag = Math::Clamp(drag, 0.0f, 1.0f);
        }

        int ParticleUpdater::GetActiveParticleCount() const
        {
            int count = 0;
            for (int i = 0; i < m_maxParticles; ++i)
            {
                if (m_particles[i] && m_particles[i]->IsActive())
                {
                    count++;
                }
            }
            return count;
        }

        float ParticleUpdater::GetAverageUpdateTime() const
        {
            return m_updateCount > 0 ? m_totalUpdateTime / m_updateCount : 0.0f;
        }

    } // namespace Blood
} // namespace RedFiles