#pragma once

#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace RFAnimation {

// State machine states
enum class StateType {
    IDLE,
    PLAYING,
    PAUSED,
    STOPPED,
    TRANSITIONING,
    COMPLETED,
    ERROR
};

// Transition types
enum class TransitionType {
    NONE,
    FADE_IN,
    FADE_OUT,
    FADE_CROSS,
    SLIDE,
    SCALE,
    ROTATE,
    CUSTOM
};

// State transition
struct StateTransition {
    StateType fromState;
    StateType toState;
    TransitionType type;
    float duration;
    std::map<std::string, std::any> parameters;
    std::function<void()> onEnter;
    std::function<void()> onExit;
    std::function<void()> onUpdate;
    
    StateTransition() : fromState(StateType::IDLE), toState(StateType::IDLE), type(TransitionType::NONE), 
                   duration(0.0f), onEnter(nullptr), onExit(nullptr), onUpdate(nullptr) {}
};

// State machine for managing animation states
class AnimationState {
public:
    AnimationState();
    virtual ~AnimationState() = default;
    
    // State management
    void addState(const std::string& name, StateType type);
    void removeState(const std::string& name);
    void setCurrentState(const std::string& name);
    void setCurrentState(StateType type);
    std::string getCurrentStateName() const;
    StateType getCurrentStateType() const;
    bool hasState(const std::string& name) const;
    
    // Transition management
    void addTransition(const std::string& fromState, const std::string& toState, 
                     TransitionType type, float duration = 0.0f);
    void addTransition(const StateTransition& transition);
    void removeTransition(const std::string& fromState, const std::string& toState);
    void removeTransitionsFrom(const std::string& stateName);
    
    // State machine control
    void update(float deltaTime);
    void reset();
    void pause();
    void resume();
    void stop();
    
    // State queries
    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;
    bool isTransitioning() const;
    bool isCompleted() const;
    bool hasError() const;
    
    // Event handling
    void addStateEventListener(const std::string& stateName, const std::string& eventType, 
                              std::function<void()> callback);
    void removeStateEventListener(const std::string& stateName, const std::string& eventType, 
                                 std::function<void()> callback);
    void clearStateEventListeners();
    
    // Utility methods
    void cloneFrom(const AnimationState& other);
    std::unique_ptr<AnimationState> clone() const;
    
    // Data access
    const std::map<std::string, StateType>& getStates() const;
    const std::vector<StateTransition>& getTransitions() const;
    const std::map<std::string, std::vector<std::function<void()>>>& getStateEventListeners() const;
    
protected:
    // Protected members
    std::map<std::string, StateType> states_;
    std::vector<StateTransition> transitions_;
    std::map<std::string, std::vector<std::function<void()>>> stateEventListeners_;
    
    StateType currentState_;
    StateType previousState_;
    float stateTime_;
    bool isPaused_;
    bool isTransitioning_;
    bool hasError_;
    
    // Protected helper methods
    virtual void updateState(float deltaTime);
    virtual void processTransitions(float deltaTime);
    virtual void triggerStateEvent(const std::string& stateName, const std::string& eventType);
    virtual void executeTransition(const StateTransition& transition);
    virtual void onStateEnter(const std::string& stateName);
    virtual void onStateExit(const std::string& stateName);
    virtual void onStateUpdate(const std::string& stateName, float deltaTime);
    
    // Helper methods for transitions
    StateTransition* findTransition(const std::string& fromState, const std::string& toState);
    void removeTransitionsToFrom(const std::string& stateName);
    std::vector<StateTransition*> findTransitionsFrom(const std::string& stateName);
    void removeTransitionsTo(const std::string& stateName);
    
    // Helper methods for state management
    StateType getStateType(const std::string& stateName) const;
    std::string getStateName(StateType type) const;
    void setCurrentStateInternal(StateType type);
    void transitionToState(StateType type);
};

// Specialized state classes
class IdleState : public AnimationState {
public:
    IdleState();
    void onEnter(const std::string& stateName) override;
    void onExit(const std::string& stateName) override;
    void onStateUpdate(const std::string& stateName, float deltaTime) override;
};

class PlayingState : public AnimationState {
public:
    PlayingState();
    void onEnter(const std::string& stateName) override;
    void onExit(const std::string& stateName) override;
    void onStateUpdate(const std::string& stateName, float deltaTime) override;
};

class PausedState : public AnimationState {
public:
    PausedState();
    void onEnter(const std::string& stateName) override;
    void onExit(const std::string& stateName) override;
    void onStateUpdate(const std::string& stateName, float deltaTime) override;
};

class TransitioningState : public AnimationState {
public:
    TransitioningState();
    void onEnter(const std::string& stateName) override;
    void onExit(const std::string& stateName) override;
    void onStateUpdate(const std::string& stateName, float deltaTime) override;
};

class ErrorState : public AnimationState {
public:
    ErrorState();
    void onEnter(const std::string& stateName) override;
    void onExit(const std::string& stateName) override;
    void onStateUpdate(const std::string& stateName, float deltaTime) override;
};

// State machine factory
class StateMachineFactory {
public:
    static std::unique_ptr<AnimationState> createState(StateType type);
    static std::unique_ptr<AnimationState> createIdleState();
    static std::unique_ptr<AnimationState> createPlayingState();
    static std::unique_ptr<AnimationState> createPausedState();
    static std::unique_ptr<AnimationState> createTransitioningState();
    static std::unique_ptr<AnimationState> createErrorState();
    static std::vector<StateType> getAvailableStates();
};

} // namespace RFAnimation