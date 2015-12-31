#include "TheStates.h"
#include "AIController.h"
#include "StateMachine.h"
#include "staticmap.h"
#include "dynamicObjects.h"
#include "rules.h"
#define SafeCircleSize 500
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
  /*
  StateTransistion.push_back(StateTransistionType(CanShootTarget::GetInstance(), [](AIController* Controller)
  {
    Circle2D SafeCircle = Circle2D(Controller->Owner->GetLocation(), SafeCircleSize);
    DynamicObjects* DyObj = DynamicObjects::GetInstance();

    for (unsigned int i = 0; i < MAXBOTSPERTEAM; ++i)
    {
      Bot& TheBot = DyObj->GetBot(Controller->Owner->GetTeamNumber() + 1, i);
      if (TheBot.IsAlive() && SafeCircle.Intersects(TheBot.GetLocation()))
      {
        // Someone is in my safe circle Shoot it 
        return true;
      }
    }
    //MyDrawEngine::GetInstance()->FillCircle(SafeCircle.GetCentre(), SafeCircleSize, MyDrawEngine::DARKGREEN);
    return false;
  }
  ));
  */
  StateTransistion.push_back(StateTransistionType(CapDomPoint::GetInstance(), [](AIController* Controller) {return (Controller->GetOwner()->GetAmmo() > 0); }));
  StateTransistion.push_back(StateTransistionType(ReloadState::GetInstance(), [](AIController* Controller) {return (Controller->GetOwner()->GetAmmo() <= 0); }));

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
    Controller->GetStateMahcine()->StateSwap(GuardDomPoint::GetInstance());
  }
  if (count == 1)
  {
   Controller->MoveTo(DomPoint->m_Location);
  }
  // go to your point
  else
  {
 
    Controller->MoveTo(DynObj->GetDominationPoint(Controller->GetOwner()->GetBotNumber() % 3).m_Location);
  }
  
}

void CapDomPoint::Exit( AIController* Controller)
{

}

void CapDomPoint::Update( AIController* Controller)
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
  /*
  StateTransistion.push_back(StateTransistionType(CanShootTarget::GetInstance(), [](AIController* Controller)
  {return (DynamicObjects::GetInstance()->GetDominationPoint(Controller->GetOwner()->GetBotNumber() % 3).m_OwnerTeamNumber == Controller->Owner->GetTeamNumber()) &&
          ((Controller->Owner->GetLocation() - DynamicObjects::GetInstance()->GetDominationPoint(Controller->GetOwner()->GetBotNumber() % 3).m_Location).magnitude() < 300.0f); }));
  */
  StateTransistion.push_back(StateTransistionType(GuardDomPoint::GetInstance(), [](AIController* Controller)
  {return (DynamicObjects::GetInstance()->GetDominationPoint(Controller->GetOwner()->GetBotNumber() % 3).m_OwnerTeamNumber == Controller->Owner->GetTeamNumber()); }));
  
  
  StateTransistion.push_back(StateTransistionType(ReloadState::GetInstance(), [](AIController* Controller) {return ((Controller->GetOwner()->GetAmmo() <= 0) ||
  (DynamicObjects::GetInstance()->GetDominationPoint(Controller->GetOwner()->GetBotNumber() % 3).m_OwnerTeamNumber == Controller->GetOwner()->GetTeamNumber() && (Controller->GetOwner()->GetAmmo() <= 0))); }));

  
}



CanShootTarget* CanShootTarget::Instance = nullptr;

void CanShootTarget::Enter(class AIController* Controller)
{

  if (Controller->Owner->GetAmmo() <= 0)
  {
    // I have ammo I cant shoot s**t just run
    Controller->GetStateMahcine()->StateSwap(ReloadState::GetInstance());
    return;
  }
  Controller->StopMoving();
  Bot* CloseBot = nullptr;
  Controller->bFollowingPath = false;
  if (Controller->bLeader || Controller->Buddy->AiController->SecondShootTarget == nullptr)
  {
    int OtherTeamNum = Controller->GetOwner()->GetTeamNumber() == 0 ? 1 : 0;
    DynamicObjects* DyObj = DynamicObjects::GetInstance();
    StaticMap* STMap = StaticMap::GetInstance();
    CloseBot = &DyObj->GetBot(OtherTeamNum, 0);
    Bot* SecondCloseBot = &DyObj->GetBot(OtherTeamNum, 0);
    float CloseistDest = (CloseBot->GetLocation() - Controller->GetOwner()->GetLocation()).magnitude();



    for (unsigned int i = 1; i < MAXBOTSPERTEAM; ++i)
    {
      if (DyObj->GetBot(OtherTeamNum, i).IsAlive()) 
      {
        float NewDest = (DyObj->GetBot(OtherTeamNum, i).GetLocation() - Controller->GetOwner()->GetLocation()).magnitude();
        if (NewDest < CloseistDest)
        {
          CloseistDest = NewDest;
          SecondCloseBot = CloseBot;
          CloseBot = &DyObj->GetBot(OtherTeamNum, i);
        }
      }
    }


  }
  else
  {
    CloseBot = Controller->Buddy->AiController->SecondShootTarget;
  }

  // can i see the bot
  if (CloseBot != nullptr && StaticMap::GetInstance()->IsLineOfSight(Controller->GetOwner()->GetLocation(), CloseBot->GetLocation()))
  {
    Controller->MostDangerousTarget = CloseBot;
  }
  else
  {
    Controller->MostDangerousTarget = nullptr;
  }
}

void CanShootTarget::Exit(AIController* Controller)
{
  Controller->MostDangerousTarget = nullptr;
  Controller->SecondShootTarget = nullptr;
  Controller->GetOwner()->StopAiming();
}

void CanShootTarget::Update(AIController* Controller)
{
  // no target leave 
  if (!Controller->MostDangerousTarget)
  {
    Controller->GetStateMahcine()->StateSwap(StartState::GetInstance());
    return;
  }
  // out of ammo i cant shoot a thing go reload
  if (Controller->Owner->GetAmmo() <= 0)
  {
    Controller->GetStateMahcine()->StateSwap(ReloadState::GetInstance());
  }
  else if (!Controller->MostDangerousTarget->IsAlive() ||
           !StaticMap::GetInstance()->IsLineOfSight(Controller->Owner->GetLocation(), Controller->MostDangerousTarget->GetLocation()))
  {
    Controller->GetStateMahcine()->StateSwap(StartState::GetInstance());
  }
  else if (Controller->Owner->GetAccuracy() == 0.0f)
  {
    Controller->GetOwner()->SetTarget(Controller->MostDangerousTarget->GetTeamNumber(), Controller->MostDangerousTarget->GetBotNumber());
  }
  else if (Controller->Owner->GetAccuracy() > 0.7f || (Controller->MostDangerousTarget->m_bAiming && Controller->MostDangerousTarget->m_dTimeToCoolDown<0.1 && Controller->GetOwner()->GetAccuracy() > 0.3f))
  {
    
    Controller->GetOwner()->Shoot();
  }
  
}



CanShootTarget* CanShootTarget::Start()
{
  if (!Instance)
  {
    Instance = new CanShootTarget;
  }
  return Instance;
}
void CanShootTarget::ShutDown()
{
  delete Instance;
  Instance = nullptr;
}

State* CanShootTarget::GetInstance()
{
  return Instance ? Instance : Start();
}


CanShootTarget::~CanShootTarget()
{

}

CanShootTarget::CanShootTarget()
{
  StateTransistion.push_back(StateTransistionType(ReloadState::GetInstance(), [](AIController* Controller) {return (Controller->GetOwner()->GetAmmo() <= 0); }));

 
}



ReloadState* ReloadState::Instance = nullptr;

void ReloadState::Enter(class AIController* Controller)
{
  Controller->MoveTo(StaticMap::GetInstance()->GetClosestResupplyLocation(Controller->Owner->GetLocation()));
}

void ReloadState::Exit(class AIController* Controller)
{
  //Controller->bFollowingPath = false;
}

void ReloadState::Update(class AIController* Controller)
{
  
  if (Controller->GetOwner()->GetAmmo() >= MAXAMMO)
  {
    Controller->GetStateMahcine()->StateSwap(StartState::GetInstance());
  }
  
  
}



ReloadState* ReloadState::Start()
{
  if (!Instance)
  {
    Instance = new ReloadState;
  }
  return Instance;
}
void ReloadState::ShutDown()
{
  delete Instance;
  Instance = nullptr;
}

State* ReloadState::GetInstance()
{
  return Instance ? Instance : Start();
}


ReloadState::~ReloadState()
{

}

ReloadState::ReloadState()
{

}



GuardDomPoint* GuardDomPoint::Instance = nullptr;

void GuardDomPoint::Enter(class AIController* Controller)
{
  StaticMap* STM = StaticMap::GetInstance();
  Vector2D SeekTarget;
  Vector2D DomPointPos = DynamicObjects::GetInstance()->GetDominationPoint(Controller->Owner->GetBotNumber() % 3).m_Location;
  
  // pick a random point near that we can see and get too
  do 
  {
    SeekTarget = DomPointPos + Vector2D(float((rand() % 300) - (rand() % 300)), float((rand() % 300) - (rand() % 300)));

  } while ((STM->IsInsideBlock(Circle2D(SeekTarget, 120))) && (!STM->IsLineOfSight(Controller->Owner->GetLocation(),SeekTarget)));
  Controller->MoveTo(SeekTarget);
}

void GuardDomPoint::Exit(class AIController* Controller)
{
  
}
void GuardDomPoint::Update(class AIController* Controller)
{
  Circle2D SafeCircle = Circle2D(Controller->Owner->GetLocation(), SafeCircleSize);
  DynamicObjects* DyObj = DynamicObjects::GetInstance();

  for (unsigned int i = 0; i < MAXBOTSPERTEAM; ++i)
  {
    Bot& TheBot = DyObj->GetBot(Controller->Owner->GetTeamNumber() + 1, i);
    if (TheBot.IsAlive() && SafeCircle.Intersects(TheBot.GetLocation()))
    {
      // Someone is in my safe circle Shoot it 
      Controller->GetStateMahcine()->StateSwap(CanShootTarget::GetInstance());
      return;
    }
  }
  //MyDrawEngine::GetInstance()->FillCircle(SafeCircle.GetCentre(), SafeCircleSize,MyDrawEngine::CYAN);

  if (Controller->IsCloseToSeekTarget(80.0f))
  {
    // Reenter the state to find a new target to seek too
    Controller->GetStateMahcine()->StateSwap(GuardDomPoint::GetInstance());
  }
  
}



GuardDomPoint* GuardDomPoint::Start()
{
  if (!Instance)
  {
    Instance = new GuardDomPoint;
  }
  return Instance;
}
void GuardDomPoint::ShutDown()
{
  delete Instance;
  Instance = nullptr;
}

State* GuardDomPoint::GetInstance()
{
  return Instance ? Instance : Start();
}


GuardDomPoint::~GuardDomPoint()
{

}

GuardDomPoint::GuardDomPoint()
{
  StateTransistion.push_back(StateTransistionType(ReloadState::GetInstance(), [](AIController* Controller){return Controller->Owner->GetAmmo() <= MAXAMMO * 0.25; }));
}