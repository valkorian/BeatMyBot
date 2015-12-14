#include "TheStates.h"
#include "AIController.h"
#include "StateMachine.h"
#include "staticmap.h"
#include "dynamicObjects.h"
StartState* StartState::Instance = nullptr;

void StartState::Enter(class AIController* Controller) 
{
 
}

void StartState::Exit(class AIController* Controller) 
{

}

void StartState::Update(class AIController* Controller) 
{

}



StartState* StartState::Start()
{
  if (!Instance)
  {
    Instance = new StartState;
  }
  return Instance;
}
void StartState::ShutDown()
{
 delete Instance;
 Instance = nullptr;
}

State* StartState::GetInstance() 
{
  return Instance ? Instance : Start();
}


StartState::~StartState() 
{
  
}

StartState::StartState()
{
  StateTransistion.push_back(StateTransistionType(CapDomPoint::GetInstance(), [](AIController* Controller) {return true; }));
}



CapDomPoint* CapDomPoint::Instance = nullptr;

void CapDomPoint::Enter(class AIController* Controller)
{
  DynamicObjects* DynObj = DynamicObjects::GetInstance();
  
  // count num of dom points we dont own 
  int count = 0;
  DominationPoint* DomPoint = nullptr;
  for (unsigned int i = 0; i < NUMDOMINATIONPOINTS; ++i)
  {
    if (DynObj->GetDominationPoint(i).m_OwnerTeamNumber != Controller->GetOwner()->GetTeamNumber())
    {
      DomPoint = &DynObj->GetDominationPoint(i);
      count++;
    }
  }
  if (count == 0)
  {
    // No dom points to cap Enter new state
  }
  // only one point go cap it 
  else if (count == 1)
  {
    Controller->PathTo(DomPoint->m_Location);
  }
  // go to random point
  else
  {
    Controller->PathTo(DynObj->GetDominationPoint(Controller->GetOwner()->GetBotNumber() % 3).m_Location);
  }
  
}

void CapDomPoint::Exit(class AIController* Controller)
{

}

void CapDomPoint::Update(class AIController* Controller)
{

}



CapDomPoint* CapDomPoint::Start()
{
  if (!Instance)
  {
    Instance = new CapDomPoint;
  }
  return Instance;
}
void CapDomPoint::ShutDown()
{
  delete Instance;
  Instance = nullptr;
}

State* CapDomPoint::GetInstance()
{
  return Instance ? Instance : Start();
}


CapDomPoint::~CapDomPoint()
{

}

CapDomPoint::CapDomPoint()
{
  
}