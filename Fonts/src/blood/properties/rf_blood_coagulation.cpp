#include "rf_blood_coagulation.h"
#include "../core/rf_logger.h"

namespace RedFiles {
    namespace Blood {

        BloodCoagulation::BloodCoagulation()
            : m_coagulationLevel(0.0f)
            , m_coagulationRate(0.1f)
            , m_coagulationFactor(1.0f)
            , m_fibrinogenLevel(2.0f) // g/L (normal range: 2-4 g/L)
            , m_plateletCount(250000) // per microliter (normal range: 150,000-450,000)
            , m_prothrombinTime(12.0f) // seconds (normal range: 11-13.5s)
            , m_activatedPartialThromboplastinTime(30.0f) // seconds (normal range: 25-35s)
            , m_internationalNormalizedRatio(1.0f) // normal range: 0.8-1.2
            , m_clottingFactorVIII(100.0f) // % of normal
            , m_clottingFactorIX(100.0f) // % of normal
            , m_clottingFactorXI(100.0f) // % of normal
            , m_anticoagulantLevel(0.0f)
            , m_temperature(37.0f) // Celsius
            , m_pH(7.4) // normal blood pH
            , m_calciumLevel(2.5f) // mmol/L (normal range: 2.1-2.6 mmol/L)
            , m_isCoagulating(false)
            , m_coagulationStartTime(0.0f)
            , m_coagulationDuration(0.0f)
            , m_clotFormationRate(0.05f)
            , m_fibrinolysisRate(0.01f)
            , m_heparinLevel(0.0f) // U/mL
            , m_warfarinLevel(0.0f) // mg/L
            , m_aspirinEffect(0.0f) // 0-1 scale
            , m_geneticFactor(1.0f) // Genetic predisposition factor
            , m_ageFactor(1.0f) // Age-related factor
            , m_medicationFactor(1.0f) // Medication influence factor
            , m_diseaseFactor(1.0f) // Disease state factor
        {
            InitializeCoagulationFactors();
            RF_LOG_INFO("BloodCoagulation: Created coagulation system");
        }

        BloodCoagulation::~BloodCoagulation()
        {
            RF_LOG_INFO("BloodCoagulation: Destroyed coagulation system");
        }

        void BloodCoagulation::Initialize()
        {
            m_coagulationLevel = 0.0f;
            m_isCoagulating = false;
            m_coagulationStartTime = 0.0f;
            m_coagulationDuration = 0.0f;
            
            InitializeCoagulationFactors();
            
            RF_LOG_INFO("BloodCoagulation: Initialized coagulation system");
        }

        void BloodCoagulation::InitializeCoagulationFactors()
        {
            // Initialize primary coagulation cascade factors
            m_coagulationFactors[CoagulationFactor::Fibrinogen] = 2.5f; // g/L
            m_coagulationFactors[CoagulationFactor::Prothrombin] = 100.0f; // % of normal
            m_coagulationFactors[CoagulationFactor::TissueFactor] = 1.0f; // Relative units
            m_coagulationFactors[CoagulationFactor::Calcium] = m_calciumLevel;
            m_coagulationFactors[CoagulationFactor::FactorV] = 100.0f; // % of normal
            m_coagulationFactors[CoagulationFactor::FactorVII] = 100.0f; // % of normal
            m_coagulationFactors[CoagulationFactor::FactorVIII] = m_clottingFactorVIII;
            m_coagulationFactors[CoagulationFactor::FactorIX] = m_clottingFactorIX;
            m_coagulationFactors[CoagulationFactor::FactorX] = 100.0f; // % of normal
            m_coagulationFactors[CoagulationFactor::FactorXI] = m_clottingFactorXI;
            m_coagulationFactors[CoagulationFactor::FactorXII] = 100.0f; // % of normal
            m_coagulationFactors[CoagulationFactor::FactorXIII] = 100.0f; // % of normal
            
            // Initialize inhibitors
            m_coagulationInhibitors[CoagulationInhibitor::Antithrombin] = 100.0f; // % of normal
            m_coagulationInhibitors[CoagulationInhibitor::ProteinC] = 100.0f; // % of normal
            m_coagulationInhibitors[CoagulationInhibitor::ProteinS] = 100.0f; // % of normal
            m_coagulationInhibitors[CoagulationInhibitor::TissueFactorPathwayInhibitor] = 100.0f; // % of normal
            
            RF_LOG_INFO("BloodCoagulation: Initialized coagulation factors");
        }

        void BloodCoagulation::Update(float deltaTime)
        {
            if (m_isCoagulating)
            {
                UpdateCoagulationProcess(deltaTime);
            }
            
            UpdateFibrinolysis(deltaTime);
            UpdateMedicationEffects(deltaTime);
            UpdateEnvironmentalEffects(deltaTime);
        }

        void BloodCoagulation::UpdateCoagulationProcess(float deltaTime)
        {
            m_coagulationDuration += deltaTime;
            
            // Calculate effective coagulation rate based on all factors
            float effectiveRate = CalculateEffectiveCoagulationRate();
            
            // Update coagulation level
            float coagulationIncrease = effectiveRate * deltaTime;
            m_coagulationLevel = Math::Min(m_coagulationLevel + coagulationIncrease, 1.0f);
            
            // Update clot formation
            UpdateClotFormation(deltaTime);
            
            // Update coagulation factors consumption
            UpdateFactorConsumption(deltaTime);
            
            RF_LOG_INFO("BloodCoagulation: Coagulation level: ", m_coagulationLevel, ", Duration: ", m_coagulationDuration);
        }

        void BloodCoagulation::UpdateClotFormation(float deltaTime)
        {
            // Clot formation follows sigmoid curve
            float clotStrength = CalculateClotStrength();
            float formationRate = m_clotFormationRate * clotStrength * (1.0f - clotStrength);
            
            // Update fibrin mesh formation
            float fibrinFormation = formationRate * deltaTime;
            UpdateFibrinMesh(fibrinFormation);
            
            // Update platelet aggregation
            UpdatePlateletAggregation(deltaTime);
        }

        void BloodCoagulation::UpdateFibrinMesh(float formationAmount)
        {
            // Fibrin mesh density affects clot stability
            float meshDensity = GetFibrinMeshDensity();
            meshDensity = Math::Min(meshDensity + formationAmount, 1.0f);
            
            // Mesh density affects overall coagulation
            m_coagulationLevel = Math::Max(m_coagulationLevel, meshDensity * 0.8f);
        }

        void BloodCoagulation::UpdatePlateletAggregation(float deltaTime)
        {
            // Platelet aggregation rate depends on various factors
            float aggregationRate = m_plateletCount / 250000.0f; // Normalized to 1.0
            aggregationRate *= m_plateletActivationFactor;
            
            // Temperature affects platelet function
            float temperatureFactor = CalculateTemperatureEffect();
            aggregationRate *= temperatureFactor;
            
            // Medication effects
            aggregationRate *= (1.0f - m_aspirinEffect * 0.5f);
            
            // Update platelet aggregation
            float aggregationIncrease = aggregationRate * deltaTime * 0.1f;
            m_plateletAggregationLevel = Math::Min(m_plateletAggregationLevel + aggregationIncrease, 1.0f);
        }

        void BloodCoagulation::UpdateFactorConsumption(float deltaTime)
        {
            // Coagulation factors are consumed during the process
            float consumptionRate = m_coagulationLevel * 0.1f; // Base consumption rate
            
            for (auto& factor : m_coagulationFactors)
            {
                float factorConsumption = consumptionRate * deltaTime * 0.01f;
                factor.second = Math::Max(factor.second - factorConsumption, 10.0f); // Minimum 10% of normal
            }
        }

        void BloodCoagulation::UpdateFibrinolysis(float deltaTime)
        {
            // Natural fibrinolysis breaks down clots
            if (m_coagulationLevel > 0.1f)
            {
                float fibrinolysisRate = m_fibrinolysisRate;
                
                // Plasmin activity increases with clot age
                float clotAgeFactor = Math::Min(m_coagulationDuration / 300.0f, 1.0f); // Max effect after 5 minutes
                fibrinolysisRate *= (1.0f + clotAgeFactor);
                
                // Temperature affects fibrinolysis
                float temperatureFactor = CalculateTemperatureEffect();
                fibrinolysisRate *= temperatureFactor;
                
                float fibrinolysisAmount = fibrinolysisRate * deltaTime;
                m_coagulationLevel = Math::Max(m_coagulationLevel - fibrinolysisAmount, 0.0f);
            }
        }

        void BloodCoagulation::UpdateMedicationEffects(float deltaTime)
        {
            // Update medication levels over time (metabolism)
            UpdateHeparinLevel(deltaTime);
            UpdateWarfarinLevel(deltaTime);
            UpdateAspirinEffect(deltaTime);
            
            // Apply medication effects to coagulation
            ApplyMedicationEffects();
        }

        void BloodCoagulation::UpdateHeparinLevel(float deltaTime)
        {
            // Heparin has a half-life of about 1-2 hours
            float halfLife = 5400.0f; // 90 minutes in seconds
            float decayRate = Math::Log(2.0f) / halfLife;
            
            m_heparinLevel *= Math::Exp(-decayRate * deltaTime);
        }

        void BloodCoagulation::UpdateWarfarinLevel(float deltaTime)
        {
            // Warfarin has a half-life of 20-60 hours
            float halfLife = 129600.0f; // 36 hours in seconds
            float decayRate = Math::Log(2.0f) / halfLife;
            
            m_warfarinLevel *= Math::Exp(-decayRate * deltaTime);
        }

        void BloodCoagulation::UpdateAspirinEffect(float deltaTime)
        {
            // Aspirin effect on platelets lasts for platelet lifetime (~7-10 days)
            float halfLife = 604800.0f; // 7 days in seconds
            float decayRate = Math::Log(2.0f) / halfLife;
            
            m_aspirinEffect *= Math::Exp(-decayRate * deltaTime);
        }

        void BloodCoagulation::UpdateEnvironmentalEffects(float deltaTime)
        {
            // Temperature effects on coagulation
            UpdateTemperatureEffects(deltaTime);
            
            // pH effects on coagulation
            UpdatepHEffects(deltaTime);
            
            // Calcium level effects
            UpdateCalciumEffects(deltaTime);
        }

        void BloodCoagulation::UpdateTemperatureEffects(float deltaTime)
        {
            // Optimal coagulation temperature is 37°C
            float optimalTemp = 37.0f;
            float tempDeviation = Math::Abs(m_temperature - optimalTemp);
            
            // Temperature affects enzyme activity in coagulation cascade
            float tempEffect = 1.0f;
            if (tempDeviation > 2.0f)
            {
                tempEffect = Math::Max(0.1f, 1.0f - (tempDeviation - 2.0f) * 0.1f);
            }
            
            m_temperatureEffect = tempEffect;
        }

        void BloodCoagulation::UpdatepHEffects(float deltaTime)
        {
            // Optimal pH for coagulation is 7.4
            float optimalpH = 7.4f;
            float pHDeviation = Math::Abs(m_pH - optimalpH);
            
            // pH affects coagulation factor activity
            float pHEffect = 1.0f;
            if (pHDeviation > 0.2f)
            {
                pHEffect = Math::Max(0.3f, 1.0f - (pHDeviation - 0.2f) * 2.0f);
            }
            
            m_pHEffect = pHEffect;
        }

        void BloodCoagulation::UpdateCalciumEffects(float deltaTime)
        {
            // Calcium is essential for coagulation cascade
            float normalCalcium = 2.5f; // mmol/L
            float calciumRatio = m_calciumLevel / normalCalcium;
            
            // Calcium effect follows a curve
            float calciumEffect = Math::Clamp(calciumRatio * calciumRatio, 0.0f, 1.0f);
            
            m_calciumEffect = calciumEffect;
        }

        float BloodCoagulation::CalculateEffectiveCoagulationRate() const
        {
            float baseRate = m_coagulationRate * m_coagulationFactor;
            
            // Factor in genetic predisposition
            baseRate *= m_geneticFactor;
            
            // Factor in age
            baseRate *= m_ageFactor;
            
            // Factor in medications
            baseRate *= m_medicationFactor;
            
            // Factor in disease state
            baseRate *= m_diseaseFactor;
            
            // Factor in environmental conditions
            baseRate *= m_temperatureEffect;
            baseRate *= m_pHEffect;
            baseRate *= m_calciumEffect;
            
            // Factor in platelet count
            float plateletFactor = Math::Clamp(m_plateletCount / 250000.0f, 0.3f, 1.5f);
            baseRate *= plateletFactor;
            
            // Factor in fibrinogen level
            float fibrinogenFactor = Math::Clamp(m_fibrinogenLevel / 3.0f, 0.5f, 1.2f);
            baseRate *= fibrinogenFactor;
            
            return baseRate;
        }

        float BloodCoagulation::CalculateClotStrength() const
        {
            // Clot strength depends on multiple factors
            float fibrinStrength = GetFibrinMeshDensity();
            float plateletStrength = m_plateletAggregationLevel;
            float factorStrength = CalculateOverallFactorActivity();
            
            // Weighted combination
            float clotStrength = (fibrinStrength * 0.5f) + (plateletStrength * 0.3f) + (factorStrength * 0.2f);
            
            return Math::Clamp(clotStrength, 0.0f, 1.0f);
        }

        float BloodCoagulation::CalculateOverallFactorActivity() const
        {
            float totalActivity = 0.0f;
            int factorCount = 0;
            
            for (const auto& factor : m_coagulationFactors)
            {
                totalActivity += factor.second;
                factorCount++;
            }
            
            return factorCount > 0 ? totalActivity / factorCount / 100.0f : 0.0f;
        }

        float BloodCoagulation::CalculateTemperatureEffect() const
        {
            // Q10 temperature coefficient for enzymatic reactions
            float q10 = 2.0f; // Reaction rate doubles for every 10°C increase
            float tempDifference = m_temperature - 37.0f;
            
            return Math::Pow(q10, tempDifference / 10.0f);
        }

        void BloodCoagulation::ApplyMedicationEffects()
        {
            // Heparin potentiates antithrombin
            if (m_heparinLevel > 0.0f)
            {
                float heparinEffect = Math::Min(m_heparinLevel * 0.1f, 0.9f);
                m_coagulationInhibitors[CoagulationInhibitor::Antithrombin] *= (1.0f + heparinEffect);
            }
            
            // Warfarin affects vitamin K-dependent factors
            if (m_warfarinLevel > 0.0f)
            {
                float warfarinEffect = Math::Min(m_warfarinLevel * 0.05f, 0.8f);
                m_coagulationFactors[CoagulationFactor::Prothrombin] *= (1.0f - warfarinEffect);
                m_coagulationFactors[CoagulationFactor::FactorVII] *= (1.0f - warfarinEffect);
                m_coagulationFactors[CoagulationFactor::FactorIX] *= (1.0f - warfarinEffect);
                m_coagulationFactors[CoagulationFactor::FactorX] *= (1.0f - warfarinEffect);
            }
            
            // Aspirin affects platelet function
            if (m_aspirinEffect > 0.0f)
            {
                m_plateletActivationFactor *= (1.0f - m_aspirinEffect * 0.8f);
            }
        }

        void BloodCoagulation::StartCoagulation()
        {
            if (!m_isCoagulating)
            {
                m_isCoagulating = true;
                m_coagulationStartTime = GetCurrentTime();
                m_coagulationDuration = 0.0f;
                
                // Activate platelets
                m_plateletActivationFactor = 1.0f;
                
                RF_LOG_INFO("BloodCoagulation: Started coagulation process");
            }
        }

        void BloodCoagulation::StopCoagulation()
        {
            if (m_isCoagulating)
            {
                m_isCoagulating = false;
                RF_LOG_INFO("BloodCoagulation: Stopped coagulation process");
            }
        }

        void BloodCoagulation::ResetCoagulation()
        {
            m_coagulationLevel = 0.0f;
            m_isCoagulating = false;
            m_coagulationStartTime = 0.0f;
            m_coagulationDuration = 0.0f;
            m_plateletAggregationLevel = 0.0f;
            m_plateletActivationFactor = 0.0f;
            
            InitializeCoagulationFactors();
            
            RF_LOG_INFO("BloodCoagulation: Reset coagulation system");
        }

        void BloodCoagulation::SetCoagulationRate(float rate)
        {
            m_coagulationRate = Math::Max(0.0f, rate);
        }

        void BloodCoagulation::SetCoagulationFactor(float factor)
        {
            m_coagulationFactor = Math::Max(0.0f, factor);
        }

        void BloodCoagulation::SetFibrinogenLevel(float level)
        {
            m_fibrinogenLevel = Math::Max(0.0f, level);
            m_coagulationFactors[CoagulationFactor::Fibrinogen] = level;
        }

        void BloodCoagulation::SetPlateletCount(int count)
        {
            m_plateletCount = Math::Max(0, count);
        }

        void BloodCoagulation::SetCalciumLevel(float level)
        {
            m_calciumLevel = Math::Max(0.0f, level);
            m_coagulationFactors[CoagulationFactor::Calcium] = level;
        }

        void BloodCoagulation::SetTemperature(float temperature)
        {
            m_temperature = temperature;
        }

        void BloodCoagulation::SetpH(float pH)
        {
            m_pH = Math::Clamp(pH, 6.8f, 8.0f);
        }

        void BloodCoagulation::AddHeparin(float amount)
        {
            m_heparinLevel = Math::Min(m_heparinLevel + amount, 10.0f);
        }

        void BloodCoagulation::AddWarfarin(float amount)
        {
            m_warfarinLevel = Math::Min(m_warfarinLevel + amount, 5.0f);
        }

        void BloodCoagulation::AddAspirin(float effect)
        {
            m_aspirinEffect = Math::Min(m_aspirinEffect + effect, 1.0f);
        }

        void BloodCoagulation::SetGeneticFactor(float factor)
        {
            m_geneticFactor = Math::Max(0.1f, factor);
        }

        void BloodCoagulation::SetAgeFactor(float factor)
        {
            m_ageFactor = Math::Max(0.1f, factor);
        }

        void BloodCoagulation::SetMedicationFactor(float factor)
        {
            m_medicationFactor = Math::Max(0.0f, factor);
        }

        void BloodCoagulation::SetDiseaseFactor(float factor)
        {
            m_diseaseFactor = Math::Max(0.0f, factor);
        }

        float BloodCoagulation::GetCoagulationLevel() const
        {
            return m_coagulationLevel;
        }

        float BloodCoagulation::GetCoagulationProgress() const
        {
            return m_isCoagulating ? m_coagulationLevel : 0.0f;
        }

        float BloodCoagulation::GetClotStrength() const
        {
            return CalculateClotStrength();
        }

        float BloodCoagulation::GetFibrinMeshDensity() const
        {
            return m_coagulationLevel * 0.8f; // Simplified relationship
        }

        float BloodCoagulation::GetPlateletAggregationLevel() const
        {
            return m_plateletAggregationLevel;
        }

        bool BloodCoagulation::IsCoagulating() const
        {
            return m_isCoagulating;
        }

        float BloodCoagulation::GetCoagulationDuration() const
        {
            return m_coagulationDuration;
        }

        float BloodCoagulation::GetProthrombinTime() const
        {
            // PT increases as coagulation factors decrease
            float factorActivity = CalculateOverallFactorActivity();
            float ptMultiplier = 1.0f / Math::Max(factorActivity, 0.1f);
            return m_prothrombinTime * ptMultiplier;
        }

        float BloodCoagulation::GetActivatedPartialThromboplastinTime() const
        {
            // aPTT increases as coagulation factors decrease
            float factorActivity = CalculateOverallFactorActivity();
            float apttMultiplier = 1.0f / Math::Max(factorActivity, 0.1f);
            return m_activatedPartialThromboplastinTime * apttMultiplier;
        }

        float BloodCoagulation::GetInternationalNormalizedRatio() const
        {
            // INR is calculated from PT
            float pt = GetProthrombinTime();
            return m_internationalNormalizedRatio * Math::Pow(pt / m_prothrombinTime, 2.0f);
        }

        float BloodCoagulation::GetCurrentTime() const
        {
            // This should be implemented to return actual system time
            // For now, return a placeholder
            return 0.0f;
        }

    } // namespace Blood
} // namespace RedFiles