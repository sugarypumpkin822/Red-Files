#include "rf_blood_density.h"
#include "../core/rf_logger.h"

namespace RedFiles {
    namespace Blood {

        BloodDensity::BloodDensity()
            : m_baseDensity(1060.0f) // kg/m³ (normal blood density)
            , m_hematocrit(0.45f) // 45% red blood cells (normal range: 40-54% for males, 36-48% for females)
            , m_hemoglobinConcentration(150.0f) // g/L (normal range: 120-160 g/L)
            , m_plasmaProteinConcentration(70.0f) // g/L (normal range: 60-80 g/L)
            , m_temperature(37.0f) // Celsius
            , m_pH(7.4) // blood pH
            , m_osmolarity(290.0f) // mOsm/kg (normal range: 275-295 mOsm/kg)
            , m_dehydrationLevel(0.0f) // 0-1 scale
            , m_anemiaLevel(0.0f) // 0-1 scale
            , m_polycythemiaLevel(0.0f) // 0-1 scale
            , m_hemolysisLevel(0.0f) // 0-1 scale
            , m_lipidLevel(0.0f) // 0-1 scale (hyperlipidemia)
            , m_proteinLevel(1.0f) // Normalized protein level
            , m_glucoseLevel(5.0f) // mmol/L (normal range: 3.9-6.1 mmol/L)
            , m_ureaLevel(5.0f) // mmol/L (normal range: 2.5-7.1 mmol/L)
            , m_electrolyteLevel(1.0f) // Normalized electrolyte level
            , m_altitude(0.0f) // meters above sea level
            , m_pressure(101.325f) // kPa (atmospheric pressure)
            , m_age(30.0f) // years
            , m_sex(Sex::Male)
            , m_isPregnant(false)
            , m_exerciseLevel(0.0f) // 0-1 scale
            , m_stressLevel(0.0f) // 0-1 scale
            , m_medicationEffect(0.0f) // 0-1 scale
            , m_diseaseEffect(0.0f) // 0-1 scale
            , m_timeSinceLastMeal(0.0f) // hours
            , m_hydrationStatus(1.0f) // 0-1 scale
            , m_bodyMassIndex(22.0f) // kg/m²
            , m_bloodVolume(5.0f) // liters (normal: ~5L for adults)
            , m_plasmaVolume(3.0f) // liters (normal: ~3L for adults)
            , m_cellVolume(2.0f) // liters (normal: ~2L for adults)
        {
            InitializeDensityComponents();
            RF_LOG_INFO("BloodDensity: Created blood density system");
        }

        BloodDensity::~BloodDensity()
        {
            RF_LOG_INFO("BloodDensity: Destroyed blood density system");
        }

        void BloodDensity::Initialize()
        {
            m_dehydrationLevel = 0.0f;
            m_anemiaLevel = 0.0f;
            m_polycythemiaLevel = 0.0f;
            m_hemolysisLevel = 0.0f;
            
            InitializeDensityComponents();
            
            RF_LOG_INFO("BloodDensity: Initialized blood density system");
        }

        void BloodDensity::InitializeDensityComponents()
        {
            // Initialize component densities
            m_redBloodCellDensity = 1095.0f; // kg/m³
            m_whiteBloodCellDensity = 1080.0f; // kg/m³
            m_plateletDensity = 1040.0f; // kg/m³
            m_plasmaDensity = 1025.0f; // kg/m³
            m_proteinDensity = 1340.0f; // kg/m³
            m_lipidDensity = 920.0f; // kg/m³
            m_glucoseDensity = 1540.0f; // kg/m³
            m_waterDensity = 1000.0f; // kg/m³
            m_electrolyteDensity = 2000.0f; // kg/m³ (average)
            
            // Calculate initial density
            CalculateDensity();
            
            RF_LOG_INFO("BloodDensity: Initialized density components");
        }

        void BloodDensity::Update(float deltaTime)
        {
            // Update time-based effects
            m_timeSinceLastMeal += deltaTime / 3600.0f; // Convert to hours
            
            // Update physiological effects
            UpdatePhysiologicalEffects(deltaTime);
            
            // Update environmental effects
            UpdateEnvironmentalEffects(deltaTime);
            
            // Update pathological effects
            UpdatePathologicalEffects(deltaTime);
            
            // Recalculate density
            CalculateDensity();
        }

        void BloodDensity::UpdatePhysiologicalEffects(float deltaTime)
        {
            // Update hydration status
            UpdateHydrationStatus(deltaTime);
            
            // Update exercise effects
            UpdateExerciseEffects(deltaTime);
            
            // Update stress effects
            UpdateStressEffects(deltaTime);
            
            // Update circadian effects
            UpdateCircadianEffects(deltaTime);
        }

        void BloodDensity::UpdateHydrationStatus(float deltaTime)
        {
            // Hydration affects plasma volume and thus density
            float hydrationChange = 0.0f;
            
            // Dehydration increases density
            if (m_dehydrationLevel > 0.0f)
            {
                hydrationChange = -m_dehydrationLevel * 0.001f * deltaTime;
            }
            
            // Overhydration decreases density
            if (m_hydrationStatus > 1.2f)
            {
                hydrationChange = (m_hydrationStatus - 1.2f) * 0.0005f * deltaTime;
            }
            
            m_hydrationStatus = Math::Clamp(m_hydrationStatus + hydrationChange, 0.5f, 1.5f);
        }

        void BloodDensity::UpdateExerciseEffects(float deltaTime)
        {
            // Exercise affects blood density through fluid loss and plasma volume changes
            if (m_exerciseLevel > 0.0f)
            {
                // Exercise causes temporary increase in density due to sweating
                float fluidLossRate = m_exerciseLevel * 0.0002f; // % per second
                float plasmaVolumeChange = -fluidLossRate * deltaTime;
                
                m_plasmaVolume = Math::Max(m_plasmaVolume * (1.0f + plasmaVolumeChange), 2.0f);
                
                // Hematocrit increases with plasma volume loss
                float hematocritChange = -plasmaVolumeChange * 0.1f;
                m_hematocrit = Math::Min(m_hematocrit + hematocritChange, 0.65f);
            }
        }

        void BloodDensity::UpdateStressEffects(float deltaTime)
        {
            // Stress affects blood density through hormonal changes
            if (m_stressLevel > 0.0f)
            {
                // Stress causes plasma volume reduction through capillary leakage
                float plasmaReduction = m_stressLevel * 0.0001f * deltaTime;
                m_plasmaVolume = Math::Max(m_plasmaVolume * (1.0f - plasmaReduction), 2.5f);
                
                // Stress increases hematocrit
                m_hematocrit = Math::Min(m_hematocrit + plasmaReduction * 0.05f, 0.65f);
            }
        }

        void BloodDensity::UpdateCircadianEffects(float deltaTime)
        {
            // Circadian rhythm affects blood density
            // Blood is typically denser in the morning due to overnight dehydration
            float circadianFactor = 1.0f;
            
            // Simplified circadian model (higher density in morning)
            float timeOfDay = fmod(GetCurrentTime() / 3600.0f, 24.0f); // Hours in 0-24 range
            
            if (timeOfDay >= 6.0f && timeOfDay <= 10.0f)
            {
                circadianFactor = 1.002f; // Morning peak
            }
            else if (timeOfDay >= 14.0f && timeOfDay <= 18.0f)
            {
                circadianFactor = 0.998f; // Afternoon dip
            }
            
            m_circadianEffect = circadianFactor;
        }

        void BloodDensity::UpdateEnvironmentalEffects(float deltaTime)
        {
            // Update temperature effects
            UpdateTemperatureEffects(deltaTime);
            
            // Update altitude effects
            UpdateAltitudeEffects(deltaTime);
            
            // Update pressure effects
            UpdatePressureEffects(deltaTime);
        }

        void BloodDensity::UpdateTemperatureEffects(float deltaTime)
        {
            // Temperature affects blood density through thermal expansion
            float thermalExpansionCoefficient = 0.00021f; // per °C for blood
            float tempDeviation = m_temperature - 37.0f;
            
            m_temperatureEffect = 1.0f - (thermalExpansionCoefficient * tempDeviation);
        }

        void BloodDensity::UpdateAltitudeEffects(float deltaTime)
        {
            // Altitude affects blood density through increased hematocrit
            if (m_altitude > 1000.0f) // Above 1000m
            {
                // Acclimatization increases red blood cell production
                float altitudeEffect = Math::Min(m_altitude / 8000.0f, 0.3f); // Max 30% increase at 8000m
                m_altitudeEffect = 1.0f + altitudeEffect;
                
                // Increase hematocrit
                m_hematocrit = Math::Min(m_hematocrit + altitudeEffect * 0.1f, 0.65f);
            }
            else
            {
                m_altitudeEffect = 1.0f;
            }
        }

        void BloodDensity::UpdatePressureEffects(float deltaTime)
        {
            // Pressure affects blood density through compression
            float pressureDeviation = m_pressure - 101.325f; // kPa deviation from standard
            float compressibility = 4.5e-10f; // Pa^-1 for blood
            
            m_pressureEffect = 1.0f + (compressibility * pressureDeviation * 1000.0f); // Convert kPa to Pa
        }

        void BloodDensity::UpdatePathologicalEffects(float deltaTime)
        {
            // Update anemia effects
            UpdateAnemiaEffects(deltaTime);
            
            // Update polycythemia effects
            UpdatePolycythemiaEffects(deltaTime);
            
            // Update hemolysis effects
            UpdateHemolysisEffects(deltaTime);
            
            // Update disease effects
            UpdateDiseaseEffects(deltaTime);
        }

        void BloodDensity::UpdateAnemiaEffects(float deltaTime)
        {
            // Anemia reduces red blood cell count, decreasing density
            if (m_anemiaLevel > 0.0f)
            {
                float hematocritReduction = m_anemiaLevel * 0.2f; // Max 20% reduction
                m_hematocrit = Math::Max(m_hematocrit - hematocritReduction * 0.01f * deltaTime, 0.25f);
                
                // Reduce hemoglobin concentration
                m_hemoglobinConcentration = Math::Max(m_hemoglobinConcentration - m_anemiaLevel * 0.5f * deltaTime, 80.0f);
            }
        }

        void BloodDensity::UpdatePolycythemiaEffects(float deltaTime)
        {
            // Polycythemia increases red blood cell count, increasing density
            if (m_polycythemiaLevel > 0.0f)
            {
                float hematocritIncrease = m_polycythemiaLevel * 0.2f; // Max 20% increase
                m_hematocrit = Math::Min(m_hematocrit + hematocritIncrease * 0.01f * deltaTime, 0.65f);
                
                // Increase hemoglobin concentration
                m_hemoglobinConcentration = Math::Min(m_hemoglobinConcentration + m_polycythemiaLevel * 0.5f * deltaTime, 200.0f);
            }
        }

        void BloodDensity::UpdateHemolysisEffects(float deltaTime)
        {
            // Hemolysis releases hemoglobin into plasma, affecting density
            if (m_hemolysisLevel > 0.0f)
            {
                // Free hemoglobin increases plasma density
                float plasmaDensityIncrease = m_hemolysisLevel * 5.0f; // kg/m³
                m_plasmaDensity = Math::Min(m_plasmaDensity + plasmaDensityIncrease * deltaTime, 1035.0f);
                
                // Red blood cell density decreases
                float cellDensityDecrease = m_hemolysisLevel * 10.0f; // kg/m³
                m_redBloodCellDensity = Math::Max(m_redBloodCellDensity - cellDensityDecrease * deltaTime, 1080.0f);
            }
        }

        void BloodDensity::UpdateDiseaseEffects(float deltaTime)
        {
            // Various diseases affect blood density
            if (m_diseaseEffect > 0.0f)
            {
                // Diabetes increases glucose, affecting density
                if (m_diseaseEffect > 0.5f) // High disease severity
                {
                    m_glucoseLevel = Math::Min(m_glucoseLevel + m_diseaseEffect * 0.01f * deltaTime, 15.0f);
                }
                
                // Kidney disease affects urea and protein levels
                if (m_diseaseEffect > 0.3f)
                {
                    m_ureaLevel = Math::Min(m_ureaLevel + m_diseaseEffect * 0.02f * deltaTime, 20.0f);
                    m_proteinLevel = Math::Max(m_proteinLevel - m_diseaseEffect * 0.01f * deltaTime, 0.5f);
                }
                
                // Liver disease affects protein production
                if (m_diseaseEffect > 0.4f)
                {
                    m_plasmaProteinConcentration = Math::Max(m_plasmaProteinConcentration - m_diseaseEffect * 0.5f * deltaTime, 40.0f);
                }
            }
        }

        void BloodDensity::CalculateDensity()
        {
            // Calculate blood density based on component volumes and densities
            
            // Calculate component volumes
            float rbcVolume = m_bloodVolume * m_hematocrit;
            float plasmaVolume = m_bloodVolume * (1.0f - m_hematocrit);
            float whiteBloodCellVolume = m_bloodVolume * 0.01f; // ~1% WBC
            float plateletVolume = m_bloodVolume * 0.003f; // ~0.3% platelets
            
            // Calculate component masses
            float rbcMass = rbcVolume * m_redBloodCellDensity;
            float plasmaMass = plasmaVolume * m_plasmaDensity;
            float wbcMass = whiteBloodCellVolume * m_whiteBloodCellDensity;
            float plateletMass = plateletVolume * m_plateletDensity;
            
            // Add dissolved components mass
            float proteinMass = plasmaVolume * (m_plasmaProteinConcentration / 1000.0f) * m_proteinDensity;
            float glucoseMass = plasmaVolume * (m_glucoseLevel / 180.0f) * m_glucoseDensity; // Glucose MW = 180 g/mol
            float electrolyteMass = plasmaVolume * 0.009f * m_electrolyteDensity; // ~0.9% electrolytes
            
            // Total mass and volume
            float totalMass = rbcMass + plasmaMass + wbcMass + plateletMass + proteinMass + glucoseMass + electrolyteMass;
            float totalVolume = m_bloodVolume;
            
            // Calculate base density
            m_calculatedDensity = totalMass / totalVolume;
            
            // Apply physiological effects
            m_calculatedDensity *= m_hydrationStatus;
            m_calculatedDensity *= m_temperatureEffect;
            m_calculatedDensity *= m_altitudeEffect;
            m_calculatedDensity *= m_pressureEffect;
            m_calculatedDensity *= m_circadianEffect;
            
            // Apply pathological effects
            if (m_anemiaLevel > 0.0f)
            {
                m_calculatedDensity *= (1.0f - m_anemiaLevel * 0.05f);
            }
            
            if (m_polycythemiaLevel > 0.0f)
            {
                m_calculatedDensity *= (1.0f + m_polycythemiaLevel * 0.08f);
            }
            
            if (m_lipidLevel > 0.0f)
            {
                // Lipids decrease overall density
                m_calculatedDensity *= (1.0f - m_lipidLevel * 0.03f);
            }
            
            // Apply sex-specific adjustments
            if (m_sex == Sex::Female)
            {
                m_calculatedDensity *= 0.998f; // Females typically have slightly lower density
            }
            
            // Apply pregnancy effect
            if (m_isPregnant)
            {
                m_calculatedDensity *= 0.995f; // Pregnancy increases plasma volume
            }
            
            // Apply medication effects
            if (m_medicationEffect > 0.0f)
            {
                m_calculatedDensity *= (1.0f - m_medicationEffect * 0.02f);
            }
            
            // Ensure density stays within reasonable bounds
            m_calculatedDensity = Math::Clamp(m_calculatedDensity, 1020.0f, 1100.0f);
            
            RF_LOG_INFO("BloodDensity: Calculated density: ", m_calculatedDensity, " kg/m³");
        }

        void BloodDensity::SetBaseDensity(float density)
        {
            m_baseDensity = Math::Clamp(density, 1020.0f, 1100.0f);
        }

        void BloodDensity::SetHematocrit(float hematocrit)
        {
            m_hematocrit = Math::Clamp(hematocrit, 0.2f, 0.65f);
        }

        void BloodDensity::SetHemoglobinConcentration(float concentration)
        {
            m_hemoglobinConcentration = Math::Clamp(concentration, 80.0f, 200.0f);
        }

        void BloodDensity::SetPlasmaProteinConcentration(float concentration)
        {
            m_plasmaProteinConcentration = Math::Clamp(concentration, 40.0f, 100.0f);
        }

        void BloodDensity::SetTemperature(float temperature)
        {
            m_temperature = Math::Clamp(temperature, 35.0f, 42.0f);
        }

        void BloodDensity::SetpH(float pH)
        {
            m_pH = Math::Clamp(pH, 6.8f, 8.0f);
        }

        void BloodDensity::SetOsmolarity(float osmolarity)
        {
            m_osmolarity = Math::Clamp(osmolarity, 250.0f, 320.0f);
        }

        void BloodDensity::SetDehydrationLevel(float level)
        {
            m_dehydrationLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodDensity::SetAnemiaLevel(float level)
        {
            m_anemiaLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodDensity::SetPolycythemiaLevel(float level)
        {
            m_polycythemiaLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodDensity::SetHemolysisLevel(float level)
        {
            m_hemolysisLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodDensity::SetLipidLevel(float level)
        {
            m_lipidLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodDensity::SetGlucoseLevel(float level)
        {
            m_glucoseLevel = Math::Clamp(level, 2.0f, 30.0f);
        }

        void BloodDensity::SetUreaLevel(float level)
        {
            m_ureaLevel = Math::Clamp(level, 1.0f, 50.0f);
        }

        void BloodDensity::SetAltitude(float altitude)
        {
            m_altitude = Math::Max(0.0f, altitude);
        }

        void BloodDensity::SetPressure(float pressure)
        {
            m_pressure = Math::Max(0.0f, pressure);
        }

        void BloodDensity::SetSex(Sex sex)
        {
            m_sex = sex;
        }

        void BloodDensity::SetPregnant(bool pregnant)
        {
            m_isPregnant = pregnant;
        }

        void BloodDensity::SetExerciseLevel(float level)
        {
            m_exerciseLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodDensity::SetStressLevel(float level)
        {
            m_stressLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodDensity::SetMedicationEffect(float effect)
        {
            m_medicationEffect = Math::Clamp(effect, 0.0f, 1.0f);
        }

        void BloodDensity::SetDiseaseEffect(float effect)
        {
            m_diseaseEffect = Math::Clamp(effect, 0.0f, 1.0f);
        }

        void BloodDensity::SetHydrationStatus(float status)
        {
            m_hydrationStatus = Math::Clamp(status, 0.5f, 1.5f);
        }

        void BloodDensity::SetBodyMassIndex(float bmi)
        {
            m_bodyMassIndex = Math::Clamp(bmi, 15.0f, 40.0f);
        }

        void BloodDensity::SetBloodVolume(float volume)
        {
            m_bloodVolume = Math::Clamp(volume, 3.0f, 8.0f);
        }

        float BloodDensity::GetDensity() const
        {
            return m_calculatedDensity;
        }

        float BloodDensity::GetBaseDensity() const
        {
            return m_baseDensity;
        }

        float BloodDensity::GetHematocrit() const
        {
            return m_hematocrit;
        }

        float BloodDensity::GetHemoglobinConcentration() const
        {
            return m_hemoglobinConcentration;
        }

        float BloodDensity::GetPlasmaDensity() const
        {
            return m_plasmaDensity;
        }

        float BloodDensity::GetRedBloodCellDensity() const
        {
            return m_redBloodCellDensity;
        }

        float BloodDensity::GetTemperatureEffect() const
        {
            return m_temperatureEffect;
        }

        float BloodDensity::GetAltitudeEffect() const
        {
            return m_altitudeEffect;
        }

        float BloodDensity::GetPressureEffect() const
        {
            return m_pressureEffect;
        }

        bool BloodDensity::IsDehydrated() const
        {
            return m_dehydrationLevel > 0.1f || m_hydrationStatus < 0.9f;
        }

        bool BloodDensity::IsAnemic() const
        {
            return m_anemiaLevel > 0.1f || m_hematocrit < 0.36f || m_hemoglobinConcentration < 120.0f;
        }

        bool BloodDensity::IsPolycythemic() const
        {
            return m_polycythemiaLevel > 0.1f || m_hematocrit > 0.54f || m_hemoglobinConcentration > 160.0f;
        }

        float BloodDensity::GetCurrentTime() const
        {
            // This should be implemented to return actual system time
            // For now, return a placeholder
            return 0.0f;
        }

        void BloodDensity::Reset()
        {
            m_baseDensity = 1060.0f;
            m_hematocrit = 0.45f;
            m_hemoglobinConcentration = 150.0f;
            m_plasmaProteinConcentration = 70.0f;
            m_temperature = 37.0f;
            m_pH = 7.4;
            m_osmolarity = 290.0f;
            m_dehydrationLevel = 0.0f;
            m_anemiaLevel = 0.0f;
            m_polycythemiaLevel = 0.0f;
            m_hemolysisLevel = 0.0f;
            m_lipidLevel = 0.0f;
            m_glucoseLevel = 5.0f;
            m_ureaLevel = 5.0f;
            m_hydrationStatus = 1.0f;
            m_exerciseLevel = 0.0f;
            m_stressLevel = 0.0f;
            m_medicationEffect = 0.0f;
            m_diseaseEffect = 0.0f;
            
            InitializeDensityComponents();
            
            RF_LOG_INFO("BloodDensity: Reset blood density system");
        }

    } // namespace Blood
} // namespace RedFiles