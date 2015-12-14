#include "StateMachine.h"
#include "State.h"
#include "TheStates.h"




StateMachine::StateMachine(class AIController* _AiController)
  :LastState(nullptr), CurrentState(StartState::GetInstance()),
  AiController(_AiController)
{
  CurrentState->Enter(AiController);
}

void StateMachine::Update()
{
  CurrentState->Update(AiController);
  CurrentState->CheckCanSwaps(AiController);
}


void StateMachine::StateSwap(class State* NewState)
{
  CurrentState->Exit(AiController);
  LastState = CurrentState;
  CurrentState = NewState;
  CurrentState->Enter(AiController);
}

void StateMachine::RestoreLastState()
{
  // if we have a last state go into it if not go into start state
  if (LastState)
  {
    StateSwap(LastState);
  }
  else
  {
    StateSwap(StartState::GetInstance());
  }
}

StateMachine::~StateMachine()
{
  StartState::ShutDown();
  CapDomPoint::ShutDown();
}
