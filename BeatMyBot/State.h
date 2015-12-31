#pragma once
#include <vector>

class StateTransistionType
{
public:
  StateTransistionType(class State* _NextState, bool(*_CanSwap)(class AIController*))
  {
    NextState = _NextState;
    CanSwap = _CanSwap;
  }
  class State* GetNextState() { return NextState; }
  bool CanSwapInTo(class AIController* Con){ return CanSwap(Con); }
protected:  
  class State* NextState;
  bool (*CanSwap)(class AIController*);
   
};




class State
{
public:
  
  virtual void Enter(class  AIController*) = 0;
  virtual void Exit(class AIController*) = 0;
  virtual void Update(class AIController*) = 0;
  void CheckCanSwaps(class AIController*);
  

  State* GetInstance();
  virtual ~State(){}
protected:
  std::vector<StateTransistionType> StateTransistion;

  State(){}
};

  


