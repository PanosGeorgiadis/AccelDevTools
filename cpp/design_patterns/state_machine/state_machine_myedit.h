/*******************************************************************************************************************************
    Author: Panagiotis Georgiadis
    Date: 25 October 2019

    Defined Classes: StateMachine
    Purpose: Provide a state machine template class

    History:
        20191025 - Initial Release

*******************************************************************************************************************************/

#pragma once

#include <memory>
#include <iostream>
#include <set>
#include <functional>
#include <iomanip>

template <typename StateIdType, typename EventIdType>
class StateMachine
{
private:
    struct StateComparer;
public:
    typedef std::shared_ptr<StateMachine<StateIdType, EventIdType>> StateMachinePtr;
    // State Class Definition
    class State
    {
        friend StateMachine;
        typedef std::shared_ptr<State> StatePtr;
        struct EventComparer;
    public:
        // Transition Class Definition
        class Event
        {
            friend State;
            typedef std::shared_ptr<Event> EventPtr;
            // Public Member Functions
        public:
            // Returns the current event id
            EventIdType GetId() const;
            // Returns the state transition associated with the event
            StatePtr GetState() const;
            // Return true if action has been defined
            bool HasAction() const;
            // Bind action to this event
            void BindAction(std::function<void(EventPtr,StatePtr,StatePtr)> action);
        // Private Member Functions
        private:
            Event(EventIdType event_id, StatePtr target_state);
            ~Event();
        // Private Types
        private:
            EventIdType _id;
            StatePtr _state;
            std::function<void(EventPtr,StatePtr,StatePtr)> _action;
        };
    typedef std::shared_ptr<Event> EventPtr;
    // Private Member Type Definitions
    private:
        struct EventComparer
        {
            bool operator()(const EventPtr lhs, const EventPtr rhs) const { return rhs->_id < lhs->_id; }
        };
        typedef std::set<EventPtr,EventComparer> EventSet;
    // Public Member Functions
    public:
        StateIdType GetId() const;
        bool IsAccepted() const;
        bool IsTerminal() const;
        // Creates a null next event
        EventPtr AddEvent(EventIdType event_id);
        EventPtr AddEvent(EventIdType event_id, StateIdType next_state, bool accepted = false);
        EventPtr GetEvent(EventIdType event_id) const;
        bool HasEntryAction() const;
        bool HasExitAction() const;
        StatePtr BindEntryAction(std::function<void(StatePtr/*current_state*/)> entry_action);
        StatePtr BindExitAction(std::function<void(StatePtr/*next_state*/)> exit_action);
        const EventSet& GetEvents() const;
    // Private Member Function
    private:
        State(const std::shared_ptr<StateMachine>& parent, StateIdType state_id, bool accepted);
        ~State();
        void _do_action(EventPtr event);
        typename EventSet::const_iterator _find_event(EventIdType event_id) const;
        typename EventSet::iterator _find_event(EventIdType event_id);
    // Private Types
    private:
        std::shared_ptr<StateMachine> _parent; // Pointer to parent
        StateIdType _id;
        bool _accepted;
        EventSet _events;
        std::function<void(StatePtr/*current_state*/)> _entry_action;
        std::function<void(StatePtr/*next_state*/)> _exit_action;
};
typedef State* StatePtr;
// Private Member Type Definitions
private:
    struct StateComparer
    {
        bool operator()(const StatePtr lhs, const StatePtr rhs) const { return rhs->_id < lhs->_id; }
    };
    typedef std::set<StatePtr,StateComparer> StateSet;
// Public Member Functions
public:
    StateMachine();
    ~StateMachine();
    StatePtr AddState(StateIdType state_id, bool accepted = false);
    StatePtr GetState(StateIdType state_id) const;
    StatePtr GetCurrentState() const;
    StatePtr GetInitialState() const;
    StatePtr NextState(EventIdType event_id) throw(std::runtime_error);
    StatePtr Reset();

    const StateSet& GetStates() const;
    void dump_state_table();
// Private Member Functions
private:
    typename StateSet::const_iterator _find_state(StateIdType state_id) const;
    typename StateSet::iterator _find_state(StateIdType state_id);
// Private Types
private:
    StateSet _states;
    StatePtr _initialState;
    StatePtr _currentState;
};

/*************************************************************************
*
*   Event Member Function Definitions
*
*************************************************************************/

template <typename StateIdType,typename EventIdType>
EventIdType StateMachine<StateIdType, EventIdType>::State::Event::GetId() const
{
    return _id;
}

template <typename StateIdType,typename EventIdType>
typename  StateMachine<StateIdType, EventIdType>::State::StatePtr
StateMachine<StateIdType, EventIdType>::State::Event::GetState() const
{
    return _state;
}

template <typename StateIdType,typename EventIdType>
bool StateMachine<StateIdType, EventIdType>::State::Event::HasAction() const
{
    return (bool)_action;
}

template <typename StateIdType,typename EventIdType>
void StateMachine<StateIdType, EventIdType>::State::Event::BindAction(std::function<void(EventPtr,StatePtr,StatePtr)> action)
{
    _action = action;
}

template <typename StateIdType,typename EventIdType>
StateMachine<StateIdType, EventIdType>::State::Event::Event(EventIdType event_id, StatePtr target_state)
: _id(event_id) , _state(target_state) { }

template <typename StateIdType,typename EventIdType>
StateMachine<StateIdType, EventIdType>::State::Event::~Event() {}



/*************************************************************************
*
*   State Member Function Definitions
*
*************************************************************************/

template <typename StateIdType,typename EventIdType>
StateIdType StateMachine<StateIdType, EventIdType>::State::GetId() const
{
    return _id;
}

template <typename StateIdType,typename EventIdType>
bool StateMachine<StateIdType, EventIdType>::State::IsAccepted() const
{
    return _accepted;
}

template <typename StateIdType,typename EventIdType>
bool StateMachine<StateIdType, EventIdType>::State::IsTerminal() const
{
    return _events.size() == 0;
}

template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::State::EventPtr
StateMachine<StateIdType, EventIdType>::State::AddEvent(EventIdType event_id)
{
    typename EventSet::const_iterator itr = _find_event( event_id );
    if( itr == _events.end() )
    {
        // No state change...
        EventPtr t = std::make_shared<Event>(event_id, this);
        _events.insert(t);
        return t;
    }
    else
        throw std::runtime_error("Attemted state transiton mangling");
}

template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::State::EventPtr
StateMachine<StateIdType, EventIdType>::State::AddEvent(EventIdType event_id, StateIdType next_state, bool accepted)
{
    typename EventSet::const_iterator itr = _find_event( event_id );
    // Get the state or create it
    StatePtr s = _parent->AddState(next_state,accepted);
    if( itr == _events.end() )
    {
        EventPtr t = std::make_shared<Event>(event_id, s);
        _events.insert(t);
        return t;
    }
    else
        throw std::runtime_error("Attemted state transiton mangling");
}

template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::State::EventPtr
StateMachine<StateIdType, EventIdType>::State::GetEvent(EventIdType event_id) const
{
    typename EventSet::const_iterator itr = _find_event( event_id );
    if( itr != _events.end() )
        return *itr;
    return nullptr;
}

template <typename StateIdType,typename EventIdType>
void StateMachine<StateIdType, EventIdType>::State::_do_action(EventPtr event)
{
    event->_action(event,this,event->GetState());
}

template <typename StateIdType,typename EventIdType>
StateMachine<StateIdType, EventIdType>::State::State(const std::shared_ptr<StateMachine>& parent, StateIdType state_id, bool accepted)
: _parent(parent) , _id(state_id), _accepted(accepted) , _events() { }

template <typename StateIdType,typename EventIdType>
StateMachine<StateIdType, EventIdType>::State::~State()
{
    for( auto t : _events )
        delete t;
}

template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::State::EventSet::const_iterator
StateMachine<StateIdType, EventIdType>::State::_find_event(EventIdType event_id) const
{
    Event bogus(event_id,nullptr);
    return _events.find( &bogus );
}

template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::State::EventSet::iterator
StateMachine<StateIdType, EventIdType>::State::_find_event(EventIdType event_id)
{
    Event bogus(event_id,nullptr);
    return _events.find( &bogus );
}

template <typename StateIdType,typename EventIdType>
bool StateMachine<StateIdType, EventIdType>::State::HasEntryAction() const
{
    return (bool)_entry_action;
}

template <typename StateIdType,typename EventIdType>
bool StateMachine<StateIdType, EventIdType>::State::HasExitAction() const
{
    return (bool)_exit_action;
}

template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::State::StatePtr
StateMachine<StateIdType, EventIdType>::State::BindEntryAction(std::function<void(StatePtr/*current_state*/)> entry_action)
{
    _entry_action = entry_action;
    return this;
}

template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::State::StatePtr
StateMachine<StateIdType, EventIdType>::State::BindExitAction(std::function<void(StatePtr/*next_state*/)> exit_action)
{
    _exit_action = exit_action;
    return this;
}

template <typename StateIdType,typename EventIdType>
const typename StateMachine<StateIdType, EventIdType>::State::EventSet &
StateMachine<StateIdType, EventIdType>::State::GetEvents() const
{
    return _events;
}

/*************************************************************************
*
*   State Machine Member Function Definitions
*
*************************************************************************/

// State machine construstor
template <typename StateIdType,typename EventIdType>
StateMachine<StateIdType, EventIdType>::StateMachine()
: _states(), _initialState(nullptr) , _currentState(nullptr) {}

// State machine destructor
template <typename StateIdType,typename EventIdType>
StateMachine<StateIdType, EventIdType>::~StateMachine()
{
    for( auto s : _states )
        delete s;
}

template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::StatePtr
StateMachine<StateIdType, EventIdType>::AddState(StateIdType state_id, bool accepted )
{
    typename StateSet::const_iterator itr = _find_state( state_id );
    if( itr == _states.end() )
    {
        StatePtr s = std::make_shared<State>(this,state_id,accepted);
        // First state added is set to initial state by default
        if( _states.size() == 0 )
        {
            _initialState = s;
            _currentState = _initialState;
        }
        _states.insert(s);
        return s;
    }
    else
        return *itr;
}


template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::StatePtr
StateMachine<StateIdType, EventIdType>::GetState(StateIdType state_id) const
{
    typename StateSet::const_iterator itr = _find_state( state_id );
    if( itr != _states.end() )
        return *itr;
    return nullptr;
}

template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::StatePtr
StateMachine<StateIdType, EventIdType>::GetCurrentState() const
{
return _currentState;
}

template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::StatePtr
StateMachine<StateIdType, EventIdType>::GetInitialState() const
{
return _initialState;
}


template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::StatePtr
StateMachine<StateIdType, EventIdType>::NextState(EventIdType event_id) throw(std::runtime_error)
{
    // From the current state get the event by id
    typename State::EventPtr event = _currentState->GetEvent(event_id);
    if( event == nullptr ) throw std::runtime_error("No event for this event");

    StatePtr next_state = event->GetState();
    if( event->HasAction() )
        _currentState->_do_action(event);

    if( _currentState != next_state )
    {
        if( _currentState->HasExitAction() )
            _currentState->_exit_action( next_state );
        if( next_state->HasEntryAction() )
            next_state->_entry_action( _currentState );
        _currentState = next_state;
    }
    return _currentState;
}

template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::StatePtr
StateMachine<StateIdType, EventIdType>::Reset() {
    _currentState = _initialState;
    return _currentState;
}

template <typename StateIdType,typename EventIdType>
const typename StateMachine<StateIdType, EventIdType>::StateSet&
StateMachine<StateIdType, EventIdType>::GetStates() const
{
    return _states;
}

template <typename StateIdType,typename EventIdType>
void StateMachine<StateIdType, EventIdType>::dump_state_table()
{
    std::cout << std::left << std::setfill(' ')
              << std::setw(16) << "STATE"
              << std::setw(16) << "ACCEPTED"
              << std::setw(16) << "TERMINAL"
              << std::setw(16) << "EVENT"
              << std::setw(16) << "ACTON"
              << std::setw(16) << "NEXT_STATE" << std::endl;
    for( auto state : _states )
    {
        if( state->GetEvents().size() == 0 )
            std::cout << std::left << std::setfill(' ')
                      << std::setw(16) << state->GetId()
                      << std::setw(16) << std::boolalpha << state->IsAccepted()
                      << std::setw(16) << std::boolalpha << state->IsTerminal()
                      << std::setw(16) << "---"
                      << std::setw(16) << "---"
                      << std::setw(16) << "---" << std::endl;
        for( auto event : state->GetEvents() )
        {
            std::cout << std::left << std::setfill(' ')
                      << std::setw(16) << state->GetId()
                      << std::setw(16) << std::boolalpha << state->IsAccepted()
                      << std::setw(16) << std::boolalpha << state->IsTerminal()
                      << std::setw(16) << event->GetId()
                      << std::setw(16) << std::boolalpha << event->HasAction()
                      << std::setw(16) << ((event->GetState() == nullptr ) ? "---" : event->GetState()->GetId() ) << std::endl;
        }
    }
    std::cout << std::setfill('-') << std::setw(80) << "" << std::endl;
}

template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::StateSet::const_iterator
StateMachine<StateIdType, EventIdType>::_find_state(StateIdType state_id) const
{
    state bogus(nullptr,state_id,false);
    return _states.find( &bogus );
}

template <typename StateIdType,typename EventIdType>
typename StateMachine<StateIdType, EventIdType>::StateSet::iterator
StateMachine<StateIdType, EventIdType>::_find_state(StateIdType state_id)
{
    state bogus(nullptr,state_id,false);
    return _states.find( &bogus );
}
