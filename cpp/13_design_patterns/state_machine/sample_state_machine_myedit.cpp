#include "state_machine_myedit.h"

typedef StateMachine<std::string,std::string> StringStateMachine;
 
void my_enter_radio(StringStateMachine::StatePtr state){
    std::cout << __FUNCTION__ << " leaving [" << state->GetId() << "]" << std::endl;
}
 
void my_exit_radio(StringStateMachine::StatePtr state){
std::cout << __FUNCTION__ << " leaving [" << state->GetId() << "]" << std::endl;
}
 
void my_enter_cdplayer(StringStateMachine::StatePtr state){
    std::cout << __FUNCTION__ << " entering [" << state->GetId() << "]" << std::endl;
}
 
void my_exit_cdplayer(StringStateMachine::StatePtr state){
    std::cout << __FUNCTION__ << " entering [" << state->GetId() << "]" << std::endl;
}
 
void my_event_action(StringStateMachine::State::EventPtr event,
                     StringStateMachine::StatePtr current_state,
                     StringStateMachine::StatePtr next_state)
{
    if( current_state == next_state )
        std::cout << "Action: [" << event->GetId() << "] " << "No State Change" << std::endl;
    else
        std::cout << "Action: [" << event->GetId() << "] "  << current_state->GetId() << " --> " << next_state->GetId() << std::endl;
}

int main() {
    StringStateMachine fsm;
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