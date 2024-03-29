#include "state_machine.h"

typedef state_machine<std::string,std::string> str_state_machine;
typedef str_state_machine::state_ptr StatePtr;
typedef str_state_machine::state::event_ptr EventPtr;

void my_enter_radio(StatePtr state)
{
    std::cout << __FUNCTION__ << " leaving [" << state->get_id() << "]" << std::endl;
}

void my_exit_radio(StatePtr state)
{
    std::cout << __FUNCTION__ << " leaving [" << state->get_id() << "]" << std::endl;
}

void my_enter_cdplayer(StatePtr state)
{
    std::cout << __FUNCTION__ << " entering [" << state->get_id() << "]" << std::endl;
}

void my_exit_cdplayer(StatePtr state)
{
    std::cout << __FUNCTION__ << " entering [" << state->get_id() << "]" << std::endl;
}

void my_event_action(EventPtr event,
                     StatePtr current_state,
                     StatePtr next_state)
{
    if( current_state == next_state )
        std::cout << "Action: [" << event->get_id() << "] " << "No State Change" << std::endl;
    else
        std::cout << "Action: [" << event->get_id() << "] "  << current_state->get_id() << " --> " << next_state->get_id() << std::endl;
}

int main()
{
    str_state_machine fsm;
    // Setup state transitions for radio
    fsm.add_state("radio")
        ->bind_entry_action(my_enter_radio)
        ->bind_exit_action(my_exit_radio)
            ->add_event("switch_radio");
    fsm.add_state("radio")->add_event("switch_cd","cdplayer")->bind_action(my_event_action);
    fsm.add_state("radio")->add_event("next")->bind_action(my_event_action);
    fsm.add_state("radio")->add_event("previous")->bind_action(my_event_action);
    // Setup state transitions
    fsm.add_state("cdplayer")
        ->bind_entry_action(my_enter_cdplayer)
        ->bind_exit_action(my_exit_cdplayer)
            ->add_event("switch_cd");
    fsm.add_state("cdplayer")->add_event("switch_radio","radio")->bind_action(my_event_action);
    fsm.add_state("cdplayer")->add_event("next")->bind_action(my_event_action);
    fsm.add_state("cdplayer")->add_event("previous")->bind_action(my_event_action);

    fsm.dump_state_table();
    fsm.next_state("next");
    fsm.next_state("previous");

    fsm.next_state("switch_cd");
    fsm.next_state("next");
    fsm.next_state("previous");

    fsm.next_state("switch_radio");
    fsm.next_state("next");
    fsm.next_state("previous");
}