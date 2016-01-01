#include "StateMachine.h"
#include "State.h"
#include "TheStates.h"




StateMachine::StateMachine(class AIController* _AiController,State* FirstState)
  :LastState(nullptr), CurrentState(nullptr), EnterState(FirstState),
  AiController(_AiController)
{
  
}

void StateMachine::EnterFirstState()
{
  StateSwap(EnterState);
}

void StateMachine::Initialize()
{
  // no state to enter in
  if (!EnterState)
  {
    // right they set the current state... ok lets fix this
    if (CurrentState)
    {
      EnterState = CurrentState;
    }
    else
      // Humm no current or enter state .. Last state?
    {
      // Why why would this be set and not the others but hey i got a state
      if (LastState)
      {
        EnterState = LastState;
      }
      // not supprised im here if i got this low ... humm its all null.. ok update can handle it been null lets hope someone swaps states
    }
  }
  
  // now enter the start state state
  StateSwap(EnterState);
  

}
void StateMachine::Update()
{
  if (!CurrentState) return;
  CurrentState->Update(AiController);
  CurrentState->CheckCanSwaps(AiController);
}


void StateMachine::StateSwap(class State* NewState)
{
  // no new state leave 
  if (!NewState) return;

  if (CurrentState)
  {
    CurrentState->Exit(AiController);
  }
  //we are entering the same state just call enter and exit
  if (NewState != CurrentState)
  {
    LastState = CurrentState;
    CurrentState = NewState;
  }
  CurrentState->Enter(AiController);
}

void StateMachine::RestoreLastState()
{
  // if we have a last state go into it if not go into start state
  if (LastState && LastState != CurrentState)
  {
    StateSwap(LastState);
  }
  else
  {
    StateSwap(EnterState);
  }
}

StateMachine::~StateMachine()
{
  // shut down al of the states
  StartState::ShutDown();
  CapDomPoint::ShutDown();
  CanShootTarget::ShutDown();
  ReloadState::ShutDown();
  GuardDomPoint::ShutDown();
}
