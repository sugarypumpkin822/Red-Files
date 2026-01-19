#include "rf_animation_state.h"
#include "rf_animation.h"
#include "../core/rf_memory.h"
#include "../utils/rf_logger.h"

namespace RedFiles {
namespace Animation {

AnimationState::AnimationState()
    : m_animation(nullptr)
    , m_stateTime(0.0f)
    , m_stateDuration(0.0f)
    , m_isActive(false)
    , m_canTransition(true)
{
    RF_LOG_INFO("AnimationState created");
}

AnimationState::~AnimationState() {
    RF_LOG_INFO("AnimationState destroyed");
}

void AnimationState::setAnimation(Animation* animation) {
    m_animation = animation;
}

Animation* AnimationState::getAnimation() const {
    return m_animation;
}

void AnimationState::enter() {
    m_isActive = true;
    m_stateTime = 0.0f;
    
    onEnter();
    
    RF_LOG_INFO("AnimationState entered");
}

void AnimationState::exit() {
    if (!m_isActive) {
        return;
    }
    
    m_isActive = false;
    
    onExit();
    
    RF_LOG_INFO("AnimationState exited");
}

void AnimationState::update(float time, float duration) {
    if (!m_isActive || !m_animation) {
        return;
    }
    
    m_stateTime = time;
    m_stateDuration = duration;
    
    onUpdate(time, duration);
}

bool AnimationState::isActive() const {
    return m_isActive;
}

bool AnimationState::canTransition() const {
    return m_canTransition;
}

void AnimationState::setCanTransition(bool canTransition) {
    m_canTransition = canTransition;
}

float AnimationState::getStateTime() const {
    return m_stateTime;
}

float AnimationState::getStateDuration() const {
    return m_stateDuration;
}

float AnimationState::getProgress() const {
    if (m_stateDuration <= 0.0f) {
        return 0.0f;
    }
    return m_stateTime / m_stateDuration;
}

// IdleState implementation
IdleState::IdleState() {
    RF_LOG_INFO("IdleState created");
}

IdleState::~IdleState() {
    RF_LOG_INFO("IdleState destroyed");
}

void IdleState::onEnter() {
    RF_LOG_INFO("Entering idle state");
}

void IdleState::onExit() {
    RF_LOG_INFO("Exiting idle state");
}

void IdleState::onUpdate(float time, float duration) {
    // Idle state doesn't do much, just wait for transitions
    (void)time;
    (void)duration;
}

// PlayingState implementation
PlayingState::PlayingState() {
    RF_LOG_INFO("PlayingState created");
}

PlayingState::~PlayingState() {
    RF_LOG_INFO("PlayingState destroyed");
}

void PlayingState::onEnter() {
    RF_LOG_INFO("Entering playing state");
}

void PlayingState::onExit() {
    RF_LOG_INFO("Exiting playing state");
}

void PlayingState::onUpdate(float time, float duration) {
    if (!m_animation) {
        return;
    }
    
    // Update animation properties based on current time
    float progress = time / duration;
    
    // Update any animation-specific properties
    onAnimationProgress(progress);
}

void PlayingState::onAnimationProgress(float progress) {
    // Override in derived classes for specific behavior
    (void)progress;
}

// PausedState implementation
PausedState::PausedState() {
    RF_LOG_INFO("PausedState created");
}

PausedState::~PausedState() {
    RF_LOG_INFO("PausedState destroyed");
}

void PausedState::onEnter() {
    RF_LOG_INFO("Entering paused state");
}

void PausedState::onExit() {
    RF_LOG_INFO("Exiting paused state");
}

void PausedState::onUpdate(float time, float duration) {
    // Paused state doesn't update time
    (void)time;
    (void)duration;
}

// StoppedState implementation
StoppedState::StoppedState() {
    RF_LOG_INFO("StoppedState created");
}

StoppedState::~StoppedState() {
    RF_LOG_INFO("StoppedState destroyed");
}

void StoppedState::onEnter() {
    RF_LOG_INFO("Entering stopped state");
}

void StoppedState::onExit() {
    RF_LOG_INFO("Exiting stopped state");
}

void StoppedState::onUpdate(float time, float duration) {
    // Stopped state doesn't update time
    (void)time;
    (void)duration;
}

// TransitioningState implementation
TransitioningState::TransitioningState()
    : m_targetState(nullptr)
    , m_transitionDuration(0.0f)
    , m_transitionTime(0.0f)
{
    RF_LOG_INFO("TransitioningState created");
}

TransitioningState::~TransitioningState() {
    RF_LOG_INFO("TransitioningState destroyed");
}

void TransitioningState::setTargetState(AnimationState* targetState, float duration) {
    m_targetState = targetState;
    m_transitionDuration = duration;
    m_transitionTime = 0.0f;
}

void TransitioningState::onEnter() {
    RF_LOG_INFO("Entering transitioning state");
    m_transitionTime = 0.0f;
}

void TransitioningState::onExit() {
    RF_LOG_INFO("Exiting transitioning state");
    
    // Enter the target state
    if (m_targetState) {
        m_targetState->enter();
    }
}

void TransitioningState::onUpdate(float time, float duration) {
    (void)time;
    (void)duration;
    
    if (!m_targetState) {
        return;
    }
    
    m_transitionTime += 0.016f; // Assuming 60 FPS
    
    float progress = m_transitionTime / m_transitionDuration;
    
    if (progress >= 1.0f) {
        // Transition complete
        exit();
    } else {
        // Update transition
        onTransitionProgress(progress);
    }
}

void TransitioningState::onTransitionProgress(float progress) {
    // Override in derived classes for specific transition behavior
    (void)progress;
}

// StateMachine implementation
StateMachine::StateMachine()
    : m_currentState(nullptr)
    , m_previousState(nullptr)
    , m_globalState(nullptr)
{
    RF_LOG_INFO("StateMachine created");
}

StateMachine::~StateMachine() {
    clear();
    RF_LOG_INFO("StateMachine destroyed");
}

void StateMachine::update(float time, float duration) {
    // Update global state first
    if (m_globalState) {
        m_globalState->update(time, duration);
    }
    
    // Update current state
    if (m_currentState) {
        m_currentState->update(time, duration);
    }
}

void StateMachine::changeState(AnimationState* newState) {
    if (!newState || newState == m_currentState) {
        return;
    }
    
    // Exit current state
    if (m_currentState) {
        m_currentState->exit();
        m_previousState = m_currentState;
    }
    
    // Enter new state
    m_currentState = newState;
    newState->enter();
    
    RF_LOG_INFO("State changed to: %s", getStateName(newState));
}

void StateMachine::setGlobalState(AnimationState* globalState) {
    m_globalState = globalState;
    if (globalState) {
        globalState->enter();
    }
}

AnimationState* StateMachine::getCurrentState() const {
    return m_currentState;
}

AnimationState* StateMachine::getPreviousState() const {
    return m_previousState;
}

AnimationState* StateMachine::getGlobalState() const {
    return m_globalState;
}

bool StateMachine::canTransitionTo(AnimationState* newState) const {
    if (!m_currentState) {
        return true;
    }
    
    return m_currentState->canTransition();
}

const char* StateMachine::getStateName(AnimationState* state) const {
    if (!state) {
        return "Null";
    }
    
    // Use dynamic cast or type checking to identify state type
    if (dynamic_cast<IdleState*>(state)) {
        return "Idle";
    } else if (dynamic_cast<PlayingState*>(state)) {
        return "Playing";
    } else if (dynamic_cast<PausedState*>(state)) {
        return "Paused";
    } else if (dynamic_cast<StoppedState*>(state)) {
        return "Stopped";
    } else if (dynamic_cast<TransitioningState*>(state)) {
        return "Transitioning";
    }
    
    return "Unknown";
}

void StateMachine::clear() {
    if (m_currentState) {
        m_currentState->exit();
    }
    
    if (m_globalState) {
        m_globalState->exit();
    }
    
    m_currentState = nullptr;
    m_previousState = nullptr;
    m_globalState = nullptr;
}

} // namespace Animation
} // namespace RedFiles