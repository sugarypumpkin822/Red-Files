#include "rf_blood_opacity.h"
#include "../core/rf_logger.h"

namespace RedFiles {
    namespace Blood {

        BloodOpacity::BloodOpacity()
            : m_baseOpacity(0.8f) // Base opacity for normal blood
            , m_hematocrit(0.45f) // 45% red blood cells
            , m_hemoglobinConcentration(150.0f) // g/L
            , m_plasmaProteinConcentration(70.0f) // g/L
            , m_temperature(37.0f) // Celsius
            , m_pH(7.4) // blood pH
            , m_oxygenSaturation(0.98f) // 98% oxygenated
            , m_coagulationLevel(0.0f) // 0-1 scale
            , m_dehydrationLevel(0.0f) // 0-1 scale
            , m_anemiaLevel(0.0f) // 0-1 scale
            , m_lipidLevel(0.0f) // 0-1 scale (hyperlipidemia)
            , m_hemolysisLevel(0.0f) // 0-1 scale
            , m_jaundiceLevel(0.0f) // 0-1 scale
            , m_sepsisLevel(0.0f) // 0-1 scale
            , m_methemoglobinLevel(0.0f) // % of total hemoglobin
            , m_carboxyhemoglobinLevel(0.0f) // % of total hemoglobin
            , m_bilirubinLevel(0.5f) // mg/dL
            , m_glucoseLevel(5.0f) // mmol/L
            , m_ureaLevel(5.0f) // mmol/L
            , m_electrolyteLevel(1.0f) // Normalized
            , m_thickness(1.0f) // mm (sample thickness)
            , m_wavelength(550.0f) // nm (green light, peak hemoglobin absorption)
            , m_lightIntensity(1.0f) // Normalized
            , m_scatteringCoefficient(10.0f) // cm^-1
            , m_absorptionCoefficient(1.0f) // cm^-1
            , m_reducedScatteringCoefficient(9.0f) // cm^-1
            , m_anisotropyFactor(0.9f) // g factor for scattering
            , m_refractiveIndex(1.4f) // Blood refractive index
            , m_pathLength(1.0f) // Optical path length
            , m_turbidity(0.1f) // Turbidity level
            , m_age(30.0f) // years
            , m_sex(Sex::Male)
            , m_isPregnant(false)
            , m_medicationEffect(0.0f) // 0-1 scale
            , m_diseaseEffect(0.0f) // 0-1 scale
            , m_exerciseLevel(0.0f) // 0-1 scale
            , m_stressLevel(0.0f) // 0-1 scale
            , m_altitude(0.0f) // meters
            , m_pressure(101.325f) // kPa
            , m_humidity(0.5f) // 0-1 scale
            , m_ambientTemperature(20.0f) // Celsius
            , m_exposureTime(0.0f) // seconds
            , m_processingTime(0.0f) // seconds
            , m_anticoagulantLevel(0.0f) // 0-1 scale
            , m_preservativeLevel(0.0f) // 0-1 scale
            , m_contaminationLevel(0.0f) // 0-1 scale
            , m_sampleAge(0.0f) // hours since collection
        {
            InitializeOpticalProperties();
            RF_LOG_INFO("BloodOpacity: Created blood opacity system");
        }

        BloodOpacity::~BloodOpacity()
        {
            RF_LOG_INFO("BloodOpacity: Destroyed blood opacity system");
        }

        void BloodOpacity::Initialize()
        {
            m_coagulationLevel = 0.0f;
            m_dehydrationLevel = 0.0f;
            m_anemiaLevel = 0.0f;
            m_hemolysisLevel = 0.0f;
            m_jaundiceLevel = 0.0f;
            m_sepsisLevel = 0.0f;
            m_exposureTime = 0.0f;
            m_processingTime = 0.0f;
            m_sampleAge = 0.0f;
            
            InitializeOpticalProperties();
            
            RF_LOG_INFO("BloodOpacity: Initialized blood opacity system");
        }

        void BloodOpacity::InitializeOpticalProperties()
        {
            // Initialize optical coefficients for different wavelengths
            m_absorptionSpectrum[400.0f] = 2.0f; // Violet
            m_absorptionSpectrum[450.0f] = 1.8f; // Blue
            m_absorptionSpectrum[500.0f] = 1.2f; // Cyan
            m_absorptionSpectrum[550.0f] = 0.8f; // Green
            m_absorptionSpectrum[600.0f] = 1.0f; // Orange
            m_absorptionSpectrum[650.0f] = 1.5f; // Red
            m_absorptionSpectrum[700.0f] = 1.8f; // Deep red
            
            // Initialize scattering coefficients
            m_scatteringSpectrum[400.0f] = 15.0f; // Higher scattering for shorter wavelengths
            m_scatteringSpectrum[450.0f] = 13.0f;
            m_scatteringSpectrum[500.0f] = 11.0f;
            m_scatteringSpectrum[550.0f] = 10.0f;
            m_scatteringSpectrum[600.0f] = 9.0f;
            m_scatteringSpectrum[650.0f] = 8.0f;
            m_scatteringSpectrum[700.0f] = 7.0f; // Lower scattering for longer wavelengths
            
            // Calculate initial optical properties
            CalculateOpticalProperties();
            
            RF_LOG_INFO("BloodOpacity: Initialized optical properties");
        }

        void BloodOpacity::Update(float deltaTime)
        {
            // Update time-based effects
            m_exposureTime += deltaTime;
            m_processingTime += deltaTime;
            m_sampleAge += deltaTime / 3600.0f; // Convert to hours
            
            // Update physiological effects
            UpdatePhysiologicalEffects(deltaTime);
            
            // Update environmental effects
            UpdateEnvironmentalEffects(deltaTime);
            
            // Update pathological effects
            UpdatePathologicalEffects(deltaTime);
            
            // Update sample aging effects
            UpdateSampleAgingEffects(deltaTime);
            
            // Recalculate optical properties
            CalculateOpticalProperties();
        }

        void BloodOpacity::UpdatePhysiologicalEffects(float deltaTime)
        {
            // Update oxygenation effects
            UpdateOxygenationEffects(deltaTime);
            
            // Update coagulation effects
            UpdateCoagulationEffects(deltaTime);
            
            // Update hydration effects
            UpdateHydrationEffects(deltaTime);
            
            // Update exercise effects
            UpdateExerciseEffects(deltaTime);
        }

        void BloodOpacity::UpdateOxygenationEffects(float deltaTime)
        {
            // Oxygen saturation affects absorption
            if (m_oxygenSaturation < 0.95f)
            {
                // Deoxygenation increases absorption in red spectrum
                float deoxygenationRate = 0.01f;
                float deoxygenationAmount = deoxygenationRate * (1.0f - m_oxygenSaturation) * deltaTime;
                
                // Update absorption spectrum for deoxygenated blood
                for (auto& absorption : m_absorptionSpectrum)
                {
                    if (absorption.first >= 600.0f) // Red spectrum
                    {
                        absorption.second *= (1.0f + deoxygenationAmount);
                    }
                }
            }
        }

        void BloodOpacity::UpdateCoagulationEffects(float deltaTime)
        {
            // Coagulation affects opacity through fibrin formation
            if (m_coagulationLevel > 0.0f)
            {
                float coagulationProgress = Math::Min(m_coagulationLevel * deltaTime * 0.1f, 1.0f);
                
                // Coagulation increases scattering
                m_scatteringCoefficient *= (1.0f + coagulationProgress * 0.5f);
                
                // Coagulation increases absorption due to fibrin
                m_absorptionCoefficient *= (1.0f + coagulationProgress * 0.2f);
                
                // Coagulation reduces anisotropy (more random scattering)
                m_anisotropyFactor *= (1.0f - coagulationProgress * 0.1f);
            }
        }

        void BloodOpacity::UpdateHydrationEffects(float deltaTime)
        {
            // Hydration affects opacity through concentration changes
            if (m_dehydrationLevel > 0.0f)
            {
                // Dehydration concentrates blood, increasing absorption
                float concentrationFactor = 1.0f + m_dehydrationLevel * 0.3f;
                m_absorptionCoefficient *= concentrationFactor;
                
                // Dehydration increases scattering due to higher cell density
                m_scatteringCoefficient *= (1.0f + m_dehydrationLevel * 0.2f);
            }
        }

        void BloodOpacity::UpdateExerciseEffects(float deltaTime)
        {
            // Exercise affects opacity through physiological changes
            if (m_exerciseLevel > 0.0f)
            {
                // Exercise increases plasma lactate, affecting absorption
                float lactateIncrease = m_exerciseLevel * 0.1f;
                m_absorptionCoefficient *= (1.0f + lactateIncrease);
                
                // Exercise increases heart rate, affecting blood flow dynamics
                float flowEffect = m_exerciseLevel * 0.05f;
                m_scatteringCoefficient *= (1.0f + flowEffect);
            }
        }

        void BloodOpacity::UpdateEnvironmentalEffects(float deltaTime)
        {
            // Update temperature effects
            UpdateTemperatureEffects(deltaTime);
            
            // Update pressure effects
            UpdatePressureEffects(deltaTime);
            
            // Update humidity effects
            UpdateHumidityEffects(deltaTime);
            
            // Update light exposure effects
            UpdateLightExposureEffects(deltaTime);
        }

        void BloodOpacity::UpdateTemperatureEffects(float deltaTime)
        {
            // Temperature affects optical properties through molecular motion
            float tempDeviation = m_temperature - 37.0f;
            
            if (Math::Abs(tempDeviation) > 1.0f)
            {
                // Higher temperature increases scattering (more molecular motion)
                float tempEffect = tempDeviation * 0.01f;
                m_scatteringCoefficient *= (1.0f + tempEffect);
                
                // Temperature affects absorption through hemoglobin conformation
                float absorptionEffect = tempDeviation * 0.005f;
                m_absorptionCoefficient *= (1.0f + absorptionEffect);
            }
        }

        void BloodOpacity::UpdatePressureEffects(float deltaTime)
        {
            // Pressure affects opacity through compression
            float pressureDeviation = m_pressure - 101.325f; // kPa
            
            if (Math::Abs(pressureDeviation) > 10.0f)
            {
                // Higher pressure compresses blood, affecting optical path
                float pressureEffect = pressureDeviation * 0.0001f;
                m_pathLength *= (1.0f + pressureEffect);
                
                // Pressure affects cell shape, affecting scattering
                m_scatteringCoefficient *= (1.0f + pressureEffect * 0.5f);
            }
        }

        void BloodOpacity::UpdateHumidityEffects(float deltaTime)
        {
            // Humidity affects evaporation and concentration
            if (m_humidity < 0.3f) // Low humidity
            {
                // Evaporation concentrates blood
                float evaporationRate = (0.3f - m_humidity) * 0.001f;
                float concentrationIncrease = evaporationRate * deltaTime;
                
                m_absorptionCoefficient *= (1.0f + concentrationIncrease);
                m_scatteringCoefficient *= (1.0f + concentrationIncrease * 0.5f);
            }
        }

        void BloodOpacity::UpdateLightExposureEffects(float deltaTime)
        {
            // Light exposure can cause photochemical changes
            if (m_lightIntensity > 0.8f)
            {
                // High light intensity can cause hemoglobin breakdown
                float photochemicalRate = (m_lightIntensity - 0.8f) * 0.0001f;
                float photochemicalEffect = photochemicalRate * m_exposureTime;
                
                // Photodegradation increases scattering
                m_scatteringCoefficient *= (1.0f + photochemicalEffect);
                
                // Photodegradation changes absorption spectrum
                for (auto& absorption : m_absorptionSpectrum)
                {
                    if (absorption.first < 500.0f) // Blue/violet spectrum most affected
                    {
                        absorption.second *= (1.0f + photochemicalEffect * 2.0f);
                    }
                }
            }
        }

        void BloodOpacity::UpdatePathologicalEffects(float deltaTime)
        {
            // Update anemia effects
            UpdateAnemiaEffects(deltaTime);
            
            // Update hemolysis effects
            UpdateHemolysisEffects(deltaTime);
            
            // Update jaundice effects
            UpdateJaundiceEffects(deltaTime);
            
            // Update sepsis effects
            UpdateSepsisEffects(deltaTime);
            
            // Update disease effects
            UpdateDiseaseEffects(deltaTime);
        }

        void BloodOpacity::UpdateAnemiaEffects(float deltaTime)
        {
            // Anemia reduces red blood cell count, affecting optical properties
            if (m_anemiaLevel > 0.0f)
            {
                float anemiaEffect = m_anemiaLevel * 0.5f;
                
                // Reduced hemoglobin decreases absorption
                m_absorptionCoefficient *= (1.0f - anemiaEffect);
                
                // Lower cell density decreases scattering
                m_scatteringCoefficient *= (1.0f - anemiaEffect * 0.3f);
                
                // Anemia increases transparency
                m_baseOpacity *= (1.0f - anemiaEffect * 0.2f);
            }
        }

        void BloodOpacity::UpdateHemolysisEffects(float deltaTime)
        {
            // Hemolysis releases hemoglobin into plasma
            if (m_hemolysisLevel > 0.0f)
            {
                float hemolysisEffect = m_hemolysisLevel * 0.3f;
                
                // Free hemoglobin increases plasma absorption
                m_absorptionCoefficient *= (1.0f + hemolysisEffect);
                
                // Hemolysis reduces cell scattering but increases molecular scattering
                m_scatteringCoefficient = m_scatteringCoefficient * (1.0f - hemolysisEffect * 0.5f) + 
                                       hemolysisEffect * 5.0f; // Molecular scattering
                
                // Hemolysis increases turbidity
                m_turbidity = Math::Min(m_turbidity + hemolysisEffect * deltaTime, 1.0f);
            }
        }

        void BloodOpacity::UpdateJaundiceEffects(float deltaTime)
        {
            // Jaundice increases bilirubin, affecting color and opacity
            if (m_jaundiceLevel > 0.0f)
            {
                float jaundiceEffect = m_jaundiceLevel * 0.4f;
                
                // Bilirubin increases absorption in blue spectrum
                for (auto& absorption : m_absorptionSpectrum)
                {
                    if (absorption.first < 500.0f) // Blue spectrum
                    {
                        absorption.second *= (1.0f + jaundiceEffect * 2.0f);
                    }
                }
                
                // Jaundice increases overall opacity
                m_baseOpacity *= (1.0f + jaundiceEffect * 0.3f);
                
                // Jaundice increases turbidity
                m_turbidity = Math::Min(m_turbidity + jaundiceEffect * 0.5f * deltaTime, 1.0f);
            }
        }

        void BloodOpacity::UpdateSepsisEffects(float deltaTime)
        {
            // Sepsis causes various changes in blood optical properties
            if (m_sepsisLevel > 0.0f)
            {
                float sepsisEffect = m_sepsisLevel * 0.3f;
                
                // Sepsis increases inflammatory cells, affecting scattering
                m_scatteringCoefficient *= (1.0f + sepsisEffect);
                
                // Sepsis can cause hemolysis
                m_hemolysisLevel = Math::Min(m_hemolysisLevel + sepsisEffect * 0.1f * deltaTime, 1.0f);
                
                // Sepsis increases turbidity due to inflammatory proteins
                m_turbidity = Math::Min(m_turbidity + sepsisEffect * 0.3f * deltaTime, 1.0f);
            }
        }

        void BloodOpacity::UpdateDiseaseEffects(float deltaTime)
        {
            // Various diseases affect optical properties
            if (m_diseaseEffect > 0.0f)
            {
                // Diabetes affects glucose levels
                if (m_diseaseEffect > 0.5f)
                {
                    m_glucoseLevel = Math::Min(m_glucoseLevel + m_diseaseEffect * 0.01f * deltaTime, 30.0f);
                    // High glucose increases refractive index
                    m_refractiveIndex += m_diseaseEffect * 0.001f * deltaTime;
                }
                
                // Kidney disease affects urea levels
                if (m_diseaseEffect > 0.3f)
                {
                    m_ureaLevel = Math::Min(m_ureaLevel + m_diseaseEffect * 0.02f * deltaTime, 50.0f);
                    // High urea increases absorption
                    m_absorptionCoefficient *= (1.0f + m_diseaseEffect * 0.001f);
                }
                
                // Liver disease affects protein levels
                if (m_diseaseEffect > 0.4f)
                {
                    m_plasmaProteinConcentration = Math::Max(m_plasmaProteinConcentration - m_diseaseEffect * 0.5f * deltaTime, 40.0f);
                    // Low protein decreases scattering
                    m_scatteringCoefficient *= (1.0f - m_diseaseEffect * 0.0005f);
                }
            }
        }

        void BloodOpacity::UpdateSampleAgingEffects(float deltaTime)
        {
            // Sample aging affects optical properties
            if (m_sampleAge > 0.1f) // After 6 minutes
            {
                float agingRate = 0.001f;
                float agingEffect = agingRate * m_sampleAge;
                
                // Aging causes hemolysis
                m_hemolysisLevel = Math::Min(m_hemolysisLevel + agingEffect * deltaTime, 1.0f);
                
                // Aging causes coagulation (without anticoagulants)
                if (m_anticoagulantLevel < 0.5f)
                {
                    m_coagulationLevel = Math::Min(m_coagulationLevel + agingEffect * 0.5f * deltaTime, 1.0f);
                }
                
                // Aging causes bacterial growth (affects turbidity)
                if (m_sampleAge > 2.0f) // After 2 hours
                {
                    m_turbidity = Math::Min(m_turbidity + agingEffect * 0.1f * deltaTime, 1.0f);
                }
            }
        }

        void BloodOpacity::CalculateOpticalProperties()
        {
            // Calculate absorption coefficient based on hemoglobin and other components
            float hemoglobinAbsorption = CalculateHemoglobinAbsorption();
            float plasmaAbsorption = CalculatePlasmaAbsorption();
            float bilirubinAbsorption = CalculateBilirubinAbsorption();
            float methemoglobinAbsorption = CalculateMethemoglobinAbsorption();
            float carboxyhemoglobinAbsorption = CalculateCarboxyhemoglobinAbsorption();
            
            m_absorptionCoefficient = hemoglobinAbsorption + plasmaAbsorption + bilirubinAbsorption + 
                                    methemoglobinAbsorption + carboxyhemoglobinAbsorption;
            
            // Calculate scattering coefficient
            float rayleighScattering = CalculateRayleighScattering();
            float mieScattering = CalculateMieScattering();
            
            m_scatteringCoefficient = rayleighScattering + mieScattering;
            m_reducedScatteringCoefficient = m_scatteringCoefficient * (1.0f - m_anisotropyFactor);
            
            // Calculate extinction coefficient
            m_extinctionCoefficient = m_absorptionCoefficient + m_scatteringCoefficient;
            
            // Calculate optical depth
            m_opticalDepth = m_extinctionCoefficient * m_pathLength;
            
            // Calculate transmittance
            m_transmittance = Math::Exp(-m_opticalDepth);
            
            // Calculate reflectance
            m_reflectance = CalculateReflectance();
            
            // Calculate final opacity
            m_calculatedOpacity = 1.0f - m_transmittance;
            
            // Apply base opacity modifications
            m_calculatedOpacity *= m_baseOpacity;
            
            // Apply thickness effect
            float thicknessEffect = Math::Min(m_thickness / 1.0f, 2.0f); // Normalize to 1mm
            m_calculatedOpacity *= thicknessEffect;
            
            // Apply turbidity effect
            m_calculatedOpacity = Math::Min(m_calculatedOpacity + m_turbidity * 0.3f, 1.0f);
            
            // Ensure opacity stays within bounds
            m_calculatedOpacity = Math::Clamp(m_calculatedOpacity, 0.0f, 1.0f);
            
            RF_LOG_INFO("BloodOpacity: Calculated opacity: ", m_calculatedOpacity, ", Transmittance: ", m_transmittance);
        }

        float BloodOpacity::CalculateHemoglobinAbsorption() const
        {
            // Hemoglobin absorption depends on concentration and wavelength
            float absorption = 0.0f;
            
            // Get absorption coefficient for current wavelength
            float wavelengthAbsorption = GetAbsorptionAtWavelength(m_wavelength);
            
            // Apply hemoglobin concentration
            float hemoglobinFactor = m_hemoglobinConcentration / 150.0f; // Normalize to normal
            
            // Apply oxygenation effect
            float oxygenationFactor = 1.0f;
            if (m_oxygenSaturation < 0.95f)
            {
                // Deoxygenated hemoglobin has different absorption
                oxygenationFactor = 0.8f + (m_oxygenSaturation * 0.4f);
            }
            
            // Apply hematocrit effect
            float hematocritFactor = m_hematocrit / 0.45f; // Normalize to normal
            
            absorption = wavelengthAbsorption * hemoglobinFactor * oxygenationFactor * hematocritFactor;
            
            return absorption;
        }

        float BloodOpacity::CalculatePlasmaAbsorption() const
        {
            // Plasma absorption from proteins and other components
            float proteinAbsorption = (m_plasmaProteinConcentration / 70.0f) * 0.1f; // Normalized
            float glucoseAbsorption = (m_glucoseLevel / 5.0f) * 0.05f; // Normalized
            float ureaAbsorption = (m_ureaLevel / 5.0f) * 0.02f; // Normalized
            float electrolyteAbsorption = m_electrolyteLevel * 0.01f; // Normalized
            
            return proteinAbsorption + glucoseAbsorption + ureaAbsorption + electrolyteAbsorption;
        }

        float BloodOpacity::CalculateBilirubinAbsorption() const
        {
            // Bilirubin absorption, especially in blue spectrum
            float bilirubinFactor = m_bilirubinLevel / 1.0f; // Normalize to 1 mg/dL
            
            float absorption = 0.0f;
            if (m_wavelength < 500.0f) // Blue spectrum
            {
                absorption = bilirubinFactor * 0.5f;
            }
            else if (m_wavelength < 600.0f) // Green spectrum
            {
                absorption = bilirubinFactor * 0.2f;
            }
            else // Red spectrum
            {
                absorption = bilirubinFactor * 0.05f;
            }
            
            return absorption;
        }

        float BloodOpacity::CalculateMethemoglobinAbsorption() const
        {
            // Methemoglobin has characteristic absorption
            float metHbFactor = m_methemoglobinLevel / 100.0f; // Convert percentage to fraction
            
            float absorption = 0.0f;
            if (m_wavelength > 600.0f) // Red spectrum
            {
                absorption = metHbFactor * 0.3f;
            }
            else
            {
                absorption = metHbFactor * 0.1f;
            }
            
            return absorption;
        }

        float BloodOpacity::CalculateCarboxyhemoglobinAbsorption() const
        {
            // Carboxyhemoglobin absorption
            float carboxyHbFactor = m_carboxyhemoglobinLevel / 100.0f; // Convert percentage to fraction
            
            float absorption = 0.0f;
            if (m_wavelength > 550.0f) // Orange-red spectrum
            {
                absorption = carboxyHbFactor * 0.2f;
            }
            else
            {
                absorption = carboxyHbFactor * 0.05f;
            }
            
            return absorption;
        }

        float BloodOpacity::CalculateRayleighScattering() const
        {
            // Rayleigh scattering from small particles
            float wavelengthFactor = Math::Pow(550.0f / m_wavelength, 4); // Inverse fourth power
            
            float particleDensity = m_hematocrit * 5.0f; // Approximate particle density
            
            return wavelengthFactor * particleDensity * 0.1f;
        }

        float BloodOpacity::CalculateMieScattering() const
        {
            // Mie scattering from larger particles (red blood cells)
            float cellSize = 7.0f; // micrometers (average RBC size)
            float sizeParameter = (2.0f * Math::PI * cellSize) / (m_wavelength / 1000.0f); // Convert nm to micrometers
            
            float scatteringEfficiency = 2.0f; // Approximate for blood cells
            float cellDensity = m_hematocrit * 5.0f; // Cells per mm³
            
            return scatteringEfficiency * cellDensity * 0.5f;
        }

        float BloodOpacity::CalculateReflectance() const
        {
            // Fresnel reflection at interface
            float n1 = 1.0f; // Air refractive index
            float n2 = m_refractiveIndex; // Blood refractive index
            
            float reflectance = Math::Pow((n1 - n2) / (n1 + n2), 2);
            
            // Add diffuse reflection from scattering
            float diffuseReflectance = m_scatteringCoefficient / (m_scatteringCoefficient + m_absorptionCoefficient) * 0.1f;
            
            return reflectance + diffuseReflectance;
        }

        float BloodOpacity::GetAbsorptionAtWavelength(float wavelength) const
        {
            // Interpolate absorption spectrum
            auto it = m_absorptionSpectrum.find(wavelength);
            if (it != m_absorptionSpectrum.end())
            {
                return it->second;
            }
            
            // Linear interpolation between nearby wavelengths
            float lowerWavelength = 0.0f, lowerAbsorption = 0.0f;
            float upperWavelength = 0.0f, upperAbsorption = 0.0f;
            
            for (const auto& pair : m_absorptionSpectrum)
            {
                if (pair.first < wavelength && pair.first > lowerWavelength)
                {
                    lowerWavelength = pair.first;
                    lowerAbsorption = pair.second;
                }
                if (pair.first > wavelength && (upperWavelength == 0.0f || pair.first < upperWavelength))
                {
                    upperWavelength = pair.first;
                    upperAbsorption = pair.second;
                }
            }
            
            if (lowerWavelength > 0.0f && upperWavelength > 0.0f)
            {
                float t = (wavelength - lowerWavelength) / (upperWavelength - lowerWavelength);
                return lowerAbsorption + t * (upperAbsorption - lowerAbsorption);
            }
            
            return 1.0f; // Default absorption
        }

        void BloodOpacity::SetBaseOpacity(float opacity)
        {
            m_baseOpacity = Math::Clamp(opacity, 0.0f, 1.0f);
        }

        void BloodOpacity::SetHematocrit(float hematocrit)
        {
            m_hematocrit = Math::Clamp(hematocrit, 0.2f, 0.65f);
        }

        void BloodOpacity::SetHemoglobinConcentration(float concentration)
        {
            m_hemoglobinConcentration = Math::Clamp(concentration, 80.0f, 200.0f);
        }

        void BloodOpacity::SetOxygenSaturation(float saturation)
        {
            m_oxygenSaturation = Math::Clamp(saturation, 0.0f, 1.0f);
        }

        void BloodOpacity::SetWavelength(float wavelength)
        {
            m_wavelength = Math::Clamp(wavelength, 380.0f, 780.0f);
        }

        void BloodOpacity::SetThickness(float thickness)
        {
            m_thickness = Math::Max(0.1f, thickness);
        }

        void BloodOpacity::SetTemperature(float temperature)
        {
            m_temperature = Math::Clamp(temperature, 35.0f, 42.0f);
        }

        void BloodOpacity::SetCoagulationLevel(float level)
        {
            m_coagulationLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodOpacity::SetDehydrationLevel(float level)
        {
            m_dehydrationLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodOpacity::SetAnemiaLevel(float level)
        {
            m_anemiaLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodOpacity::SetHemolysisLevel(float level)
        {
            m_hemolysisLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodOpacity::SetJaundiceLevel(float level)
        {
            m_jaundiceLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodOpacity::SetSepsisLevel(float level)
        {
            m_sepsisLevel = Math::Clamp(level, 0.0f, 1.0f);
        }

        void BloodOpacity::SetMedicationEffect(float effect)
        {
            m_medicationEffect = Math::Clamp(effect, 0.0f, 1.0f);
        }

        void BloodOpacity::SetDiseaseEffect(float effect)
        {
            m_diseaseEffect = Math::Clamp(effect, 0.0f, 1.0f);
        }

        float BloodOpacity::GetOpacity() const
        {
            return m_calculatedOpacity;
        }

        float BloodOpacity::GetTransmittance() const
        {
            return m_transmittance;
        }

        float BloodOpacity::GetAbsorptionCoefficient() const
        {
            return m_absorptionCoefficient;
        }

        float BloodOpacity::GetScatteringCoefficient() const
        {
            return m_scatteringCoefficient;
        }

        float BloodOpacity::GetExtinctionCoefficient() const
        {
            return m_extinctionCoefficient;
        }

        float BloodOpacity::GetOpticalDepth() const
        {
            return m_opticalDepth;
        }

        float BloodOpacity::GetReflectance() const
        {
            return m_reflectance;
        }

        bool BloodOpacity::IsOpaque() const
        {
            return m_calculatedOpacity > 0.8f;
        }

        bool BloodOpacity::IsTransparent() const
        {
            return m_calculatedOpacity < 0.2f;
        }

        bool BloodOpacity::IsTurbid() const
        {
            return m_turbidity > 0.5f;
        }

        void BloodOpacity::Reset()
        {
            m_baseOpacity = 0.8f;
            m_hematocrit = 0.45f;
            m_hemoglobinConcentration = 150.0f;
            m_oxygenSaturation = 0.98f;
            m_coagulationLevel = 0.0f;
            m_dehydrationLevel = 0.0f;
            m_anemiaLevel = 0.0f;
            m_hemolysisLevel = 0.0f;
            m_jaundiceLevel = 0.0f;
            m_sepsisLevel = 0.0f;
            m_turbidity = 0.1f;
            m_exposureTime = 0.0f;
            m_sampleAge = 0.0f;
            
            InitializeOpticalProperties();
            
            RF_LOG_INFO("BloodOpacity: Reset blood opacity system");
        }

    } // namespace Blood
} // namespace RedFiles