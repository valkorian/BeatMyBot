#pragma once
// state machine to handle all the states
class StateMachine
{
 // friend class CanShootTarget;
public:
  StateMachine(class AIController* AiController, class State* FirstState);
  void Initialize();
  void Update();
  void StateSwap(class State* NewState);
  void RestoreLastState();
  class State* GetCurrentState(){ return CurrentState; }
  void EnterFirstState();
  ~StateMachine();
protected:
  class AIController* AiController;
private:
  class State* CurrentState;
  class State* LastState;
  class State* EnterState;
};

