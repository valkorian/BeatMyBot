#include "State.h"
#include "AIController.h"
#include "StateMachine.h"

#include "mydrawengine.h"



void State::CheckCanSwaps(AIController* Controller)
{
  for (auto it : StateTransistion)
  {
    if (it.CanSwapInTo(Controller))
    {
      Controller->GetStateMahcine()->StateSwap(it.GetNextState());
      break;
    }
  }
}
