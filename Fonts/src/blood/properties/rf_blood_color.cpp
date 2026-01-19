#include "rf_blood_color.h"
#include "../core/rf_logger.h"

namespace RedFiles {
    namespace Blood {

        BloodColor::BloodColor()
            : m_oxygenSaturation(0.98f) // 98% oxygenated (arterial)
            , m_hemoglobinLevel(150.0f) // g/L (normal range: 120-160 g/L)
            , m_carboxyhemoglobinLevel(0.0f) // % (normal: <2%)
            , m_methemoglobinLevel(0.0f) // % (normal: <1%)
            , m_bilirubinLevel(0.5f) // mg/dL (normal: 0.3-1.0 mg/dL)
            , m_age(30.0f) // years
            , m_temperature(37.0f) // Celsius
            , m_pH(7.4) // blood pH
            , m_isArterial(true)
            , m_isVenous(false)
            , m_isCapillary(false)
            , m_timeSinceOxygenation(0.0f)
            , m_deoxygenationRate(0.01f)
            , m_oxidationRate(0.001f)
            , m_hemolysisLevel(0.0f) // 0-1 scale
            , m_jaundiceLevel(0.0f) // 0-1 scale
            , m_anemiaLevel(0.0f) // 0-1 scale
            , m_dehydrationLevel(0.0f) // 0-1 scale
            , m_lightingCondition(LightingCondition::Neutral)
            , m_ambientTemperature(20.0f) // Celsius
            , m_humidity(0.5f) // 0-1 scale
            , m_exposureTime(0.0f) // seconds
            , m_coagulationEffect(0.0f) // 0-1 scale
            , m_dryingEffect(0.0f) // 0-1 scale
            , m_contaminationLevel(0.0f) // 0-1 scale
            , m_medicationEffect(0.0f) // 0-1 scale
            , m_dietEffect(0.0f) // 0-1 scale
        {
            InitializeColorComponents();
            RF_LOG_INFO("BloodColor: Created blood color system");
        }

        BloodColor::~BloodColor()
        {
            RF_LOG_INFO("BloodColor: Destroyed blood color system");
        }

        void BloodColor::Initialize()
        {
            m_timeSinceOxygenation = 0.0f;
            m_exposureTime = 0.0f;
            m_coagulationEffect = 0.0f;
            m_dryingEffect = 0.0f;
            
            InitializeColorComponents();
            
            RF_LOG_INFO("BloodColor: Initialized blood color system");
        }

        void BloodColor::InitializeColorComponents()
        {
            // Initialize RGB components based on blood type and oxygenation
            UpdateBaseColor();
            
            // Initialize HSV components
            UpdateHSVComponents();
            
            // Initialize color variations
            m_colorVariation = Color(0.05f, 0.05f, 0.05f, 0.1f);
            
            RF_LOG_INFO("BloodColor: Initialized color components");
        }

        void BloodColor::Update(float deltaTime)
        {
            // Update time-based effects
            m_timeSinceOxygenation += deltaTime;
            m_exposureTime += deltaTime;
            
            // Update deoxygenation
            UpdateDeoxygenation(deltaTime);
            
            // Update oxidation
            UpdateOxidation(deltaTime);
            
            // Update coagulation effects
            UpdateCoagulationEffects(deltaTime);
            
            // Update drying effects
            UpdateDryingEffects(deltaTime);
            
            // Update environmental effects
            UpdateEnvironmentalEffects(deltaTime);
            
            // Update final color
            UpdateFinalColor();
        }

        void BloodColor::UpdateDeoxygenation(float deltaTime)
        {
            if (m_oxygenSaturation > 0.75f) // Venous blood saturation
            {
                // Deoxygenation rate depends on temperature and exposure
                float deoxygenationMultiplier = 1.0f;
                
                // Higher temperature increases deoxygenation
                float tempEffect = (m_temperature - 20.0f) / 20.0f;
                deoxygenationMultiplier *= (1.0f + tempEffect);
                
                // Exposure to air increases deoxygenation
                float exposureEffect = Math::Min(m_exposureTime / 60.0f, 1.0f); // Max effect after 1 minute
                deoxygenationMultiplier *= (1.0f + exposureEffect);
                
                // Apply deoxygenation
                float deoxygenationAmount = m_deoxygenationRate * deoxygenationMultiplier * deltaTime;
                m_oxygenSaturation = Math::Max(m_oxygenSaturation - deoxygenationAmount, 0.75f);
            }
        }

        void BloodColor::UpdateOxidation(float deltaTime)
        {
            // Oxidation causes blood to darken over time
            if (m_exposureTime > 10.0f) // Start oxidation after 10 seconds
            {
                float oxidationMultiplier = 1.0f;
                
                // Light exposure increases oxidation
                if (m_lightingCondition == LightingCondition::Bright)
                {
                    oxidationMultiplier *= 2.0f;
                }
                else if (m_lightingCondition == LightingCondition::Dim)
                {
                    oxidationMultiplier *= 0.5f;
                }
                
                // Temperature affects oxidation rate
                float tempEffect = (m_ambientTemperature - 20.0f) / 20.0f;
                oxidationMultiplier *= (1.0f + tempEffect);
                
                // Apply oxidation
                float oxidationAmount = m_oxidationRate * oxidationMultiplier * deltaTime;
                m_oxidationLevel = Math::Min(m_oxidationLevel + oxidationAmount, 1.0f);
            }
        }

        void BloodColor::UpdateCoagulationEffects(float deltaTime)
        {
            // Coagulation affects color by making it darker and more brown
            if (m_coagulationEffect > 0.0f)
            {
                // Coagulation progresses over time
                float coagulationProgress = Math::Min(m_coagulationEffect * deltaTime * 0.1f, 1.0f);
                
                // Update coagulation color shift
                m_coagulationColorShift = Color(
                    coagulationProgress * 0.2f,  // Red reduction
                    coagulationProgress * 0.3f,  // Green reduction  
                    coagulationProgress * 0.1f,  // Blue reduction
                    0.0f
                );
            }
        }

        void BloodColor::UpdateDryingEffects(float deltaTime)
        {
            // Drying causes blood to darken and become more opaque
            if (m_dryingEffect > 0.0f)
            {
                // Drying rate depends on humidity and temperature
                float dryingRate = 0.01f;
                
                // Lower humidity increases drying rate
                dryingRate *= (2.0f - m_humidity);
                
                // Higher temperature increases drying rate
                float tempEffect = (m_ambientTemperature - 20.0f) / 20.0f;
                dryingRate *= (1.0f + tempEffect);
                
                // Apply drying
                float dryingAmount = dryingRate * deltaTime;
                m_dryingLevel = Math::Min(m_dryingLevel + dryingAmount, 1.0f);
                
                // Update drying color shift
                m_dryingColorShift = Color(
                    -m_dryingLevel * 0.3f,  // Red reduction
                    -m_dryingLevel * 0.2f,  // Green reduction
                    -m_dryingLevel * 0.1f,  // Blue reduction
                    m_dryingLevel * 0.5f    // Alpha increase (more opaque)
                );
            }
        }

        void BloodColor::UpdateEnvironmentalEffects(float deltaTime)
        {
            // Update lighting effects
            UpdateLightingEffects(deltaTime);
            
            // Update temperature effects
            UpdateTemperatureEffects(deltaTime);
            
            // Update contamination effects
            UpdateContaminationEffects(deltaTime);
        }

        void BloodColor::UpdateLightingEffects(float deltaTime)
        {
            // Lighting affects perceived color
            switch (m_lightingCondition)
            {
                case LightingCondition::Bright:
                    m_lightingColorShift = Color(0.1f, 0.05f, 0.05f, 0.0f);
                    break;
                case LightingCondition::Dim:
                    m_lightingColorShift = Color(-0.1f, -0.05f, -0.05f, 0.0f);
                    break;
                case LightingCondition::Fluorescent:
                    m_lightingColorShift = Color(0.0f, 0.05f, 0.1f, 0.0f);
                    break;
                case LightingCondition::Incandescent:
                    m_lightingColorShift = Color(0.1f, 0.05f, -0.05f, 0.0f);
                    break;
                case LightingCondition::Neutral:
                default:
                    m_lightingColorShift = Color(0.0f, 0.0f, 0.0f, 0.0f);
                    break;
            }
        }

        void BloodColor::UpdateTemperatureEffects(float deltaTime)
        {
            // Temperature affects blood color through hemoglobin conformation
            float tempDeviation = m_temperature - 37.0f;
            
            if (Math::Abs(tempDeviation) > 2.0f)
            {
                float tempEffect = tempDeviation * 0.01f;
                m_temperatureColorShift = Color(
                    tempEffect,     // Red shift
                    tempEffect * 0.5f, // Green shift
                    tempEffect * 0.3f, // Blue shift
                    0.0f
                );
            }
            else
            {
                m_temperatureColorShift = Color(0.0f, 0.0f, 0.0f, 0.0f);
            }
        }

        void BloodColor::UpdateContaminationEffects(float deltaTime)
        {
            // Contamination affects color appearance
            if (m_contaminationLevel > 0.0f)
            {
                // Different types of contamination
                m_contaminationColorShift = Color(
                    -m_contaminationLevel * 0.1f,  // Red reduction
                    m_contaminationLevel * 0.05f,   // Green increase
                    m_contaminationLevel * 0.05f,   // Blue increase
                    -m_contaminationLevel * 0.2f    // Alpha reduction
                );
            }
        }

        void BloodColor::UpdateBaseColor()
        {
            // Calculate base color from hemoglobin and oxygenation
            float hemoglobinFactor = Math::Clamp(m_hemoglobinLevel / 150.0f, 0.5f, 2.0f);
            
            if (m_isArterial)
            {
                // Arterial blood: bright red
                m_baseColor = Color(
                    0.9f * hemoglobinFactor,  // High red
                    0.1f * hemoglobinFactor,  // Low green
                    0.1f * hemoglobinFactor,  // Low blue
                    1.0f
                );
            }
            else if (m_isVenous)
            {
                // Venous blood: dark red
                m_baseColor = Color(
                    0.6f * hemoglobinFactor,  // Medium red
                    0.1f * hemoglobinFactor,  // Very low green
                    0.1f * hemoglobinFactor,  // Very low blue
                    1.0f
                );
            }
            else // Capillary
            {
                // Capillary blood: between arterial and venous
                float capillaryRed = 0.75f * hemoglobinFactor;
                m_baseColor = Color(
                    capillaryRed,
                    0.1f * hemoglobinFactor,
                    0.1f * hemoglobinFactor,
                    1.0f
                );
            }
            
            // Apply oxygenation effect
            ApplyOxygenationEffect();
            
            // Apply pathological effects
            ApplyPathologicalEffects();
        }

        void BloodColor::ApplyOxygenationEffect()
        {
            // Oxygen saturation affects the red component
            float oxygenEffect = Math::Clamp(m_oxygenSaturation, 0.75f, 1.0f);
            float redBoost = (oxygenEffect - 0.75f) * 0.6f; // 0 to 0.15 range
            
            m_baseColor.r = Math::Min(m_baseColor.r + redBoost, 1.0f);
            
            // Apply carboxyhemoglobin effect (cherry red)
            if (m_carboxyhemoglobinLevel > 0.0f)
            {
                float coEffect = Math::Min(m_carboxyhemoglobinLevel / 10.0f, 0.3f);
                m_baseColor.r = Math::Min(m_baseColor.r + coEffect, 1.0f);
                m_baseColor.g = Math::Min(m_baseColor.g + coEffect * 0.5f, 1.0f);
            }
            
            // Apply methemoglobin effect (brown)
            if (m_methemoglobinLevel > 0.0f)
            {
                float metEffect = Math::Min(m_methemoglobinLevel / 20.0f, 0.5f);
                m_baseColor.r = Math::Max(m_baseColor.r - metEffect * 0.3f, 0.0f);
                m_baseColor.g = Math::Max(m_baseColor.g - metEffect * 0.2f, 0.0f);
                m_baseColor.b = Math::Max(m_baseColor.b - metEffect * 0.1f, 0.0f);
            }
        }

        void BloodColor::ApplyPathologicalEffects()
        {
            // Apply anemia effect (paler color)
            if (m_anemiaLevel > 0.0f)
            {
                float anemiaEffect = m_anemiaLevel * 0.5f;
                m_baseColor.r = Math::Max(m_baseColor.r - anemiaEffect, 0.3f);
                m_baseColor.g = Math::Max(m_baseColor.g - anemiaEffect * 0.5f, 0.05f);
                m_baseColor.b = Math::Max(m_baseColor.b - anemiaEffect * 0.5f, 0.05f);
            }
            
            // Apply jaundice effect (yellowish tint)
            if (m_jaundiceLevel > 0.0f)
            {
                float jaundiceEffect = m_jaundiceLevel * 0.3f;
                m_baseColor.r = Math::Min(m_baseColor.r + jaundiceEffect, 1.0f);
                m_baseColor.g = Math::Min(m_baseColor.g + jaundiceEffect * 1.5f, 1.0f);
            }
            
            // Apply dehydration effect (darker, more concentrated)
            if (m_dehydrationLevel > 0.0f)
            {
                float dehydrationEffect = m_dehydrationLevel * 0.4f;
                m_baseColor.r = Math::Min(m_baseColor.r + dehydrationEffect, 1.0f);
                m_baseColor.g = Math::Max(m_baseColor.g - dehydrationEffect * 0.5f, 0.0f);
                m_baseColor.b = Math::Max(m_baseColor.b - dehydrationEffect * 0.5f, 0.0f);
            }
            
            // Apply hemolysis effect (free hemoglobin in plasma)
            if (m_hemolysisLevel > 0.0f)
            {
                float hemolysisEffect = m_hemolysisLevel * 0.2f;
                m_baseColor.r = Math::Min(m_baseColor.r + hemolysisEffect, 1.0f);
                m_baseColor.g = Math::Min(m_baseColor.g + hemolysisEffect * 0.3f, 1.0f);
                m_baseColor.b = Math::Min(m_baseColor.b + hemolysisEffect * 0.3f, 1.0f);
            }
        }

        void BloodColor::UpdateHSVComponents()
        {
            // Convert RGB to HSV for easier manipulation
            float max = Math::Max(Math::Max(m_baseColor.r, m_baseColor.g), m_baseColor.b);
            float min = Math::Min(Math::Min(m_baseColor.r, m_baseColor.g), m_baseColor.b);
            float delta = max - min;
            
            // Hue
            if (delta == 0.0f)
            {
                m_hue = 0.0f;
            }
            else if (max == m_baseColor.r)
            {
                m_hue = 60.0f * ((m_baseColor.g - m_baseColor.b) / delta);
                if (m_hue < 0.0f) m_hue += 360.0f;
            }
            else if (max == m_baseColor.g)
            {
                m_hue = 60.0f * ((m_baseColor.b - m_baseColor.r) / delta + 2.0f);
            }
            else
            {
                m_hue = 60.0f * ((m_baseColor.r - m_baseColor.g) / delta + 4.0f);
            }
            
            // Saturation
            m_saturation = (max == 0.0f) ? 0.0f : (delta / max);
            
            // Value
            m_value = max;
        }

        void BloodColor::UpdateFinalColor()
        {
            // Start with base color
            m_finalColor = m_baseColor;
            
            // Apply oxidation
            m_finalColor.r = Math::Max(m_finalColor.r - m_oxidationLevel * 0.4f, 0.2f);
            m_finalColor.g = Math::Max(m_finalColor.g - m_oxidationLevel * 0.3f, 0.05f);
            m_finalColor.b = Math::Max(m_finalColor.b - m_oxidationLevel * 0.2f, 0.05f);
            
            // Apply coagulation effects
            m_finalColor += m_coagulationColorShift;
            
            // Apply drying effects
            m_finalColor += m_dryingColorShift;
            
            // Apply lighting effects
            m_finalColor += m_lightingColorShift;
            
            // Apply temperature effects
            m_finalColor += m_temperatureColorShift;
            
            // Apply contamination effects
            m_finalColor += m_contaminationColorShift;
            
            // Apply medication effects
            if (m_medicationEffect > 0.0f)
            {
                m_finalColor.r *= (1.0f - m_medicationEffect * 0.1f);
                m_finalColor.g *= (1.0f - m_medicationEffect * 0.05f);
            }
            
            // Apply diet effects
            if (m_dietEffect > 0.0f)
            {
                m_finalColor.g += m_dietEffect * 0.05f;
            }
            
            // Clamp final color values
            m_finalColor.r = Math::Clamp(m_finalColor.r, 0.0f, 1.0f);
            m_finalColor.g = Math::Clamp(m_finalColor.g, 0.0f, 1.0f);
            m_finalColor.b = Math::Clamp(m_finalColor.b, 0.0f, 1.0f);
            m_finalColor.a = Math::Clamp(m_finalColor.a, 0.0f, 1.0f);
        }

        void BloodColor::SetOxygenSaturation(float saturation)
        {
            m_oxygenSaturation = Math::Clamp(saturation, 0.0f, 1.0f);
            UpdateBaseColor();
        }

        void BloodColor::SetHemoglobinLevel(float level)
        {
            m_hemoglobinLevel = Math::Max(0.0f, level);
            UpdateBaseColor();
        }

        void BloodColor::SetBloodType(BloodType type)
        {
            m_isArterial = (type == BloodType::Arterial);
            m_isVenous = (type == BloodType::Venous);
            m_isCapillary = (type == BloodType::Capillary);
            UpdateBaseColor();
        }

        void BloodColor::SetCarboxyhemoglobinLevel(float level)
        {
            m_carboxyhemoglobinLevel = Math::Max(0.0f, level);
            UpdateBaseColor();
        }

        void BloodColor::SetMethemoglobinLevel(float level)
        {
            m_methemoglobinLevel = Math::Max(0.0f, level);
            UpdateBaseColor();
        }

        void BloodColor::SetBilirubinLevel(float level)
        {
            m_bilirubinLevel = Math::Max(0.0f, level);
            m_jaundiceLevel = Math::Min(level / 3.0f, 1.0f); // Jaundice at >3mg/dL
            UpdateBaseColor();
        }

        void BloodColor::SetAnemiaLevel(float level)
        {
            m_anemiaLevel = Math::Clamp(level, 0.0f, 1.0f);
            UpdateBaseColor();
        }

        void BloodColor::SetJaundiceLevel(float level)
        {
            m_jaundiceLevel = Math::Clamp(level, 0.0f, 1.0f);
            UpdateBaseColor();
        }

        void BloodColor::SetHemolysisLevel(float level)
        {
            m_hemolysisLevel = Math::Clamp(level, 0.0f, 1.0f);
            UpdateBaseColor();
        }

        void BloodColor::SetDehydrationLevel(float level)
        {
            m_dehydrationLevel = Math::Clamp(level, 0.0f, 1.0f);
            UpdateBaseColor();
        }

        void BloodColor::SetLightingCondition(LightingCondition condition)
        {
            m_lightingCondition = condition;
        }

        void BloodColor::SetAmbientTemperature(float temperature)
        {
            m_ambientTemperature = temperature;
        }

        void BloodColor::SetHumidity(float humidity)
        {
            m_humidity = Math::Clamp(humidity, 0.0f, 1.0f);
        }

        void BloodColor::SetCoagulationEffect(float effect)
        {
            m_coagulationEffect = Math::Clamp(effect, 0.0f, 1.0f);
        }

        void BloodColor::SetDryingEffect(float effect)
        {
            m_dryingEffect = Math::Clamp(effect, 0.0f, 1.0f);
        }

        void BloodColor::SetContaminationLevel(float level)
        {
            m_contaminationLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodColor::SetMedicationEffect(float effect)
        {
            m_medicationEffect = Math::Clamp(effect, 0.0f, 1.0f);
        }

        void BloodColor::SetDietEffect(float effect)
        {
            m_dietEffect = Math::Clamp(effect, 0.0f, 1.0f);
        }

        Color BloodColor::GetBaseColor() const
        {
            return m_baseColor;
        }

        Color BloodColor::GetFinalColor() const
        {
            return m_finalColor;
        }

        Color BloodColor::GetColorWithVariation() const
        {
            Color color = m_finalColor;
            color.r += (Math::Random() - 0.5f) * m_colorVariation.r;
            color.g += (Math::Random() - 0.5f) * m_colorVariation.g;
            color.b += (Math::Random() - 0.5f) * m_colorVariation.b;
            color.a += (Math::Random() - 0.5f) * m_colorVariation.a;
            
            color.r = Math::Clamp(color.r, 0.0f, 1.0f);
            color.g = Math::Clamp(color.g, 0.0f, 1.0f);
            color.b = Math::Clamp(color.b, 0.0f, 1.0f);
            color.a = Math::Clamp(color.a, 0.0f, 1.0f);
            
            return color;
        }

        float BloodColor::GetHue() const
        {
            return m_hue;
        }

        float BloodColor::GetSaturation() const
        {
            return m_saturation;
        }

        float BloodColor::GetValue() const
        {
            return m_value;
        }

        float BloodColor::GetOxygenSaturation() const
        {
            return m_oxygenSaturation;
        }

        float BloodColor::GetOxidationLevel() const
        {
            return m_oxidationLevel;
        }

        float BloodColor::GetDryingLevel() const
        {
            return m_dryingLevel;
        }

        bool BloodColor::IsOxygenated() const
        {
            return m_oxygenSaturation > 0.95f;
        }

        bool BloodColor::IsDeoxygenated() const
        {
            return m_oxygenSaturation < 0.80f;
        }

        bool BloodColor::IsOxidized() const
        {
            return m_oxidationLevel > 0.5f;
        }

        bool BloodColor::IsDried() const
        {
            return m_dryingLevel > 0.8f;
        }

        void BloodColor::Reset()
        {
            m_oxygenSaturation = 0.98f;
            m_oxidationLevel = 0.0f;
            m_dryingLevel = 0.0f;
            m_timeSinceOxygenation = 0.0f;
            m_exposureTime = 0.0f;
            m_coagulationEffect = 0.0f;
            
            InitializeColorComponents();
            
            RF_LOG_INFO("BloodColor: Reset blood color system");
        }

    } // namespace Blood
} // namespace RedFiles