#include "rf_flow_behavior.h"
#include "../core/rf_memory.h"
#include "../math/rf_math.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Blood {

FlowBehavior::FlowBehavior()
    : m_flowRate(1.0f)
    , m_flowForce(2.0f)
    , m_flowRadius(0.5f)
    , m_flowThickness(0.1f)
    , m_flowLifetime(5.0f)
    , m_flowFadeTime(0.8f)
    , m_flowColor(0.8f, 0.1f, 0.1f, 1.0f)
    , m_flowOpacity(0.7f)
    , m_flowTurbulence(0.05f)
    , m_flowGravity(9.81f)
    , m_flowViscosity(0.8f)
    , m_flowCoagulation(0.1f)
    , m_flowEvaporation(0.02f)
    , m_isEnabled(true)
    , m_flowCount(0)
    , m_maxFlows(30)
{
    m_flows.reserve(m_maxFlows);
    RF_LOG_INFO("FlowBehavior created");
}

FlowBehavior::~FlowBehavior() {
    clear();
    RF_LOG_INFO("FlowBehavior destroyed");
}

void FlowBehavior::initialize(const BloodProperties& properties) {
    m_properties = properties;
    RF_LOG_INFO("FlowBehavior initialized");
}

void FlowBehavior::clear() {
    for (auto& flow : m_flows) {
        if (flow) {
            RF_DELETE(flow);
        }
    }
    
    m_flows.clear();
    m_flowCount = 0;
    RF_LOG_INFO("Cleared all flows");
}

void FlowBehavior::addFlow(const Vec3& position, const Vec3& velocity, float size, 
                       const Color& color, float lifetime) {
    if (m_flowCount >= m_maxFlows) {
        // Remove oldest flow
        RF_DELETE(m_flows.front());
        m_flows.erase(m_flows.begin());
        m_flowCount--;
    }
    
    auto flow = RF_NEW(Flow);
    flow->initialize(position, velocity, size, color, lifetime, m_properties);
    
    m_flows.push_back(flow);
    m_flowCount++;
    
    RF_LOG_INFO("Added flow at position (%.2f, %.2f, %.2f)", 
                position.x, position.y, position.z);
}

void FlowBehavior::update(float deltaTime) {
    if (!m_isEnabled) {
        return;
    }
    
    // Update existing flows
    for (auto& flow : m_flows) {
        if (flow) {
            flow->update(deltaTime);
            
            // Remove completed flows
            if (flow->isCompleted()) {
                RF_DELETE(flow);
                m_flows.erase(std::find(m_flows.begin(), m_flows.end(), flow));
                m_flowCount--;
            }
        }
    }
    
    // Generate new flows based on surface conditions
    updateFlowGeneration(deltaTime);
}

void FlowBehavior::updateFlowGeneration(float deltaTime) {
    // Generate new flows based on surface conditions
    // This would be connected to the surface system
    // For now, just update existing flows
    (void)deltaTime;
}

void FlowBehavior::setFlowRate(float rate) {
    m_flowRate = rate;
    RF_LOG_INFO("Flow rate set to %.2f", rate);
}

void FlowBehavior::setFlowForce(float force) {
    m_flowForce = force;
    RF_LOG_INFO("Flow force set to %.2f", force);
}

void FlowBehavior::setFlowRadius(float radius) {
    m_flowRadius = radius;
    RF_LOG_INFO("Flow radius set to %.2f", radius);
}

void FlowBehavior::setFlowThickness(float thickness) {
    m_flowThickness = thickness;
    RF_LOG_INFO("Flow thickness set to %.2f", thickness);
}

void FlowBehavior::setFlowLifetime(float lifetime) {
    m_flowLifetime = lifetime;
    RF_LOG_INFO("Flow lifetime set to %.2f", lifetime);
}

void FlowBehavior::setFlowFadeTime(float time) {
    m_flowFadeTime = time;
    RF_LOG_INFO("Flow fade time set to %.2f", time);
}

void FlowBehavior::setFlowColor(const Color& color) {
    m_flowColor = color;
    RF_LOG_INFO("Flow color set to (%.2f, %.2f, %.2f, %.2f)", 
                color.r, color.g, color.b, color.a);
}

void FlowBehavior::setFlowOpacity(float opacity) {
    m_flowOpacity = opacity;
    RF_LOG_INFO("Flow opacity set to %.2f", opacity);
}

void FlowBehavior::setFlowTurbulence(float turbulence) {
    m_flowTurbulence = turbulence;
    RF_LOG_INFO("Flow turbulence set to %.2f", turbulence);
}

void FlowBehavior::setFlowGravity(float gravity) {
    m_flowGravity = gravity;
    RF_LOG_INFO("Flow gravity set to %.2f", gravity);
}

void FlowBehavior::setFlowViscosity(float viscosity) {
    m_flowViscosity = viscosity;
    RF_LOG_INFO("Flow viscosity set to %.2f", viscosity);
}

void FlowBehavior::setFlowCoagulation(float coagulation) {
    m_flowCoagulation = coagulation;
    RF_LOG_INFO("Flow coagulation set to %.2f", coagulation);
}

void FlowBehavior::setFlowEvaporation(float evaporation) {
    m_flowEvaporation = evaporation;
    RF_LOG_INFO("Flow evaporation set to %.2f", evaporation);
}

void FlowBehavior::setMaxFlows(int maxFlows) {
    m_maxFlows = maxFlows;
    m_flows.reserve(maxFlows);
    RF_LOG_INFO("Flow max flows set to %d", maxFlows);
}

size_t FlowBehavior::getFlowCount() const {
    return m_flowCount;
}

float FlowBehavior::getFlowRate() const {
    return m_flowRate;
}

float FlowBehavior::getFlowForce() const {
    return m_flowForce;
}

float FlowBehavior::getFlowRadius() const {
    return m_flowRadius;
}

float FlowBehavior::getFlowThickness() const {
    return m_flowThickness;
}

float FlowBehavior::getFlowLifetime() const {
    return m_flowLifetime;
}

float FlowBehavior::getFlowFadeTime() const {
    return m_flowFadeTime;
}

Color FlowBehavior::getFlowColor() const {
    return m_flowColor;
}

float FlowBehavior::getFlowOpacity() const {
    return m_flowOpacity;
}

float FlowBehavior::getFlowTurbulence() const {
    return m_flowTurbulence;
}

float FlowBehavior::getFlowGravity() const {
    return m_flowGravity;
}

float FlowBehavior::getFlowViscosity() const {
    return m_flowViscosity;
}

float FlowBehavior::getFlowCoagulation() const {
    return m_flowCoagulation;
}

float FlowBehavior::getFlowEvaporation() const {
    return m_flowEvaporation;
}

bool FlowBehavior::isEnabled() const {
    return m_isEnabled;
}

const std::vector<Flow*>& FlowBehavior::getFlows() const {
    return m_flows;
}

void FlowBehavior::setProperties(const BloodProperties& properties) {
    m_properties = properties;
}

const BloodProperties& FlowBehavior::getProperties() const {
    return m_properties;
}

} // namespace Blood
} // namespace RedFiles