#pragma once
// state machine to handle all the states
class StateMachine
{
public:
  StateMachine(class AIController* AiController);
  void Update();
  void StateSwap(class State* NewState);
  void RestoreLastState();
  ~StateMachine();
protected:
  class AIController* AiController;
  class State* CurrentState;
  class State* LastState;
  class State* StartState;
};

