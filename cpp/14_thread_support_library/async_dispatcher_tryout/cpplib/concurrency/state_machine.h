// Copyright (c) 2019 by Thermo Fisher Scientific
// All rights reserved. This file includes confidential and proprietary information of Thermo Fisher Scientific

#pragma once

#include <map>
#include <stdexcept>
#include <string>
#include "dispatcher.h"
#include "state_variable.h"

namespace cpp
{
namespace concurrency
{

//---------------------------------------------------------------------------------------------------------------------

class statemachine_error :
    public std::exception
{
public:
    statemachine_error(const std::string& msg) :
        std::exception(msg.c_str())
    {
    }
};

class not_allowed_error :
    public statemachine_error
{
public:
    not_allowed_error(const std::string& msg) :
        statemachine_error(msg)
    {
    }

};

class not_unique :
    public statemachine_error
{
public:
    not_unique(const std::string& msg) :
        statemachine_error(msg)
    {
    }
};

class aborted_exception :
    public std::exception
{
};

//---------------------------------------------------------------------------------------------------------------------

template<typename State, typename Event>
class StateMachineT final
{
public:
    StateMachineT() = delete;
    ~StateMachineT()
    {
        m_dispatcher.synchronize();
    }

    StateMachineT(const StateMachineT&) = delete;
    StateMachineT& operator=(const StateMachineT&) = delete;
    StateMachineT(const StateMachineT&&) = delete;

    StateMachineT(concurrency::Dispatcher& dispatcher, State initial, State final) :
        m_engine_state{ EngineState::initializing },
        m_state{ initial },
        m_final_state{ final },
        m_error_state{ final },
        m_dispatcher{ dispatcher },
        m_null_fn{ [] {} }
    {
        static_assert(std::is_enum<State>::value, "State must be an enum type");
        static_assert(std::is_enum<Event>::value, "Event must be an enum type");
    }

    StateMachineT(concurrency::Dispatcher& dispatcher, State initial, State final, State error) :
        m_engine_state(EngineState::initializing),
        m_state{ initial },
        m_error_state{ error },
        m_final_state{ final },
        m_dispatcher{ dispatcher },
        m_null_fn{ [] {} }
    {
        static_assert(std::is_enum<State>::value, "State must be an enum type");
        static_assert(std::is_enum<Event>::value, "Event must be an enum type");
    }

    void start()
    {
        m_dispatcher.invoke([this]
        {
            m_engine_state = EngineState::running;
            enter(m_state.get());
        });
    }

    void requires_engine_state_initializing() const
    {
        if (m_engine_state != EngineState::initializing)
        {
            throw not_allowed_error("Modifications to statemachine can only be done while initializing");
        }
    }

    void add_transition(State from, Event ev, State to)
    {
        requires_engine_state_initializing();

        Reaction r = { ReactionType::Transition,from,ev,to,false,to,m_null_fn };
        auto key = std::make_pair(from, ev);

        if (m_reaction_map.find(key) != m_reaction_map.end())
        {
            throw not_unique("transition not unique!");
        }

        m_reaction_map.emplace(key, r);
    }

    template<typename Fn>
    void add_transition(State from, Event ev, State to, Fn&& fn)
    {
        requires_engine_state_initializing();

        Reaction r = { ReactionType::Transition,from,ev,to,false,to,fn };
        auto key = std::make_pair(from, ev);

        if (m_reaction_map.find(key) != m_reaction_map.end())
        {
            throw not_unique("transition not unique!");
        }

        m_reaction_map.emplace(key, r);
    }

    template<typename Fn>
    void add_transition(State from, Event ev, State to, State error, Fn&& fn)
    {
        requires_engine_state_initializing();

        Reaction r = { ReactionType::Transition,from,ev,to,true,error,fn };
        auto key = std::make_pair(from, ev);

        if (m_reaction_map.find(key) != m_reaction_map.end())
        {
            throw not_unique("transition not unique!");
        }

        m_reaction_map.emplace(key, r);
    }

    template<typename Fn>
    void add_reaction(State state, Event ev, Fn&& fn)
    {
        requires_engine_state_initializing();

        Reaction r = { ReactionType::InState,state,ev,state,false,state,fn };
        auto key = std::make_pair(state, ev);

        if (m_reaction_map.find(key) != m_reaction_map.end())
        {
            throw not_unique("transition not unique!");
        }

        m_reaction_map.emplace(key, r);
    }


    template<typename Fn>
    void add_entry(State state, Fn&& fn)
    {
        requires_engine_state_initializing();

        auto it = m_entry_map.find(state);
        if (it != m_entry_map.end())
        {
            throw not_unique("State already has an entry function");
        }

        m_entry_map.emplace(state, fn);
    }

    template<typename Fn>
    void add_exit(State state, Fn&& fn)
    {
        requires_engine_state_initializing();

        auto it = m_exit_map.find(state);
        if (it != m_exit_map.end())
        {
            throw not_unique("State already has an exit function");
        }

        m_exit_map.emplace(state, fn);
    }

    template<typename Fn>
    void add_activity(State state, Event completion_event, Fn&& fn)
    {
        requires_engine_state_initializing();

        auto it = m_activity_map.find(state);
        if (it != m_activity_map.end())
        {
            throw not_unique("State already has an activity");
        }

        auto lambda = [this, completion_event, fn]
        {
            fn();
            notify(completion_event);
        };

        m_activity_map.emplace(state, lambda);
    }

    template<typename Fn>
    void add_activity(State state, Event completion_event, Event abortion_event, Fn&& fn)
    {
        requires_engine_state_initializing();

        auto it = m_activity_map.find(state);
        if (it != m_activity_map.end())
        {
            throw not_unique("State already has an activity");
        }

        auto lambda = [this, completion_event, abortion_event, fn]
        {
            try
            {
                fn();
                notify(completion_event);
            }
            catch (const aborted_exception&)
            {
                notify(abortion_event);
            }
        };

        m_activity_map.emplace(state, lambda);
    }

    // TODO think about exception handling (for whole machine)
    // different types of exceptions -> events

    void exit(State state)
    {
        m_dispatcher.injected_thread_required(__FUNCTION__);
        auto it = m_exit_map.find(state);
        if (it != m_exit_map.end())
        {
            auto fn = it->second;
            fn();
        }
    }

    State enter(const State state)
    {
        m_dispatcher.injected_thread_required(__FUNCTION__);

        auto it = m_entry_map.find(state);
        if (it != m_entry_map.end())
        {
            auto fn = it->second;
            fn();
        }

        auto activity_it = m_activity_map.find(state);
        if (activity_it != m_activity_map.end())
        {
            auto activity = activity_it->second;
            m_activity_dispatcher.notify(activity);
        }

        return state;
    }

    void process(Event ev)
    {
        m_dispatcher.invoke([this, ev]
        {
            if ((m_engine_state == EngineState::initializing) || (m_engine_state == EngineState::error))
            {
                throw not_allowed_error("Engine not started or in error state");
            }

            if (m_engine_state == EngineState::running)
            {
                auto key = std::make_pair(m_state.get(), ev);
                if (m_reaction_map.find(key) != m_reaction_map.end())
                {
                    auto reaction = m_reaction_map[key];

                    if (reaction.type == ReactionType::Transition)
                    {
                        exit(reaction.from);
                        try
                        {
                            reaction.fn();
                            m_state = enter(reaction.to);
                        }
                        catch (std::exception&)
                        {
                            m_state = reaction.handle_error ? enter(reaction.error_state) : enter(m_error_state);
                            throw;
                        }

                        m_engine_state = (m_state == m_final_state) ? EngineState::stopped : EngineState::running;
                    }
                    else
                    {
                        reaction.fn();
                    }
                }
            }
        });
    }

    void notify(Event ev)
    {
        m_dispatcher.notify([this, ev]() noexcept
        {
            try
            {
                process(ev);
            }
            catch (const std::exception&)
            {
                // do nothing.
            }
        });
    }

    State state()
    {
        return m_dispatcher.invoke([this]
        {
            return m_state.get();
        });
    }

    bool try_wait_for_state(State state, const std::chrono::steady_clock::duration& duration)
    {
        return m_state.try_wait_for(state, duration);
    }

    void wait_for_state(State state)
    {
        m_state.wait_for(state);
    }

    // add transition without reaction
    // add state enter
    // add state leave
    // add doing state + automatic trigger finished

private:
    enum class EngineState
    {
        initializing,
        running,
        stopped,
        error
    };

    enum class ReactionType
    {
        InState,
        Transition
    };

    struct Reaction
    {
        // todo guard std::function<bool()> guard;
        ReactionType type;
        State from;
        Event event;
        State to;
        bool handle_error;
        State error_state;
        std::function<void()> fn;
    };

    EngineState m_engine_state;
    concurrency::StateVariable<State> m_state;
    State m_error_state;
    State m_final_state;
    concurrency::Dispatcher& m_dispatcher;
    concurrency::Dispatcher m_activity_dispatcher;

    std::function<void()> m_null_fn;
    std::function<bool()> m_null_guard;

    std::map<std::pair<State, Event>, Reaction> m_reaction_map;
    std::map<State, std::function<void()>> m_entry_map;

    std::map<State, std::function<void()>> m_activity_map;
    std::map<State, std::function<void()>> m_exit_map;
};

} // concurrency
} // cpp


