#include "AIController.h"
#include "bot.h"
#include "AIBehaviors.h"
#include "AStar.h"
#include "NodeList.h"
#include "Renderer.h"
#include "staticmap.h"
#include "AIObsticalAvoidance.h"
#include <iterator>  
#include <time.h>
#include "StateMachine.h"
#include "mydrawengine.h"
#include "TheStates.h"

AIController::AIController():
Owner(nullptr), bFollowingPath(false), bLeader(false), bWantsToStopMoving(false), bSeekingTarget(false),
TheStateMachine(nullptr), ShootTarget(nullptr), MostDangerousTarget(nullptr), SecondShootTarget(nullptr)
{
}

void AIController::SetOwner(Bot* NewOwner)
{
  Owner = NewOwner;
}



void AIController::Initialise(Bot* NewOwner)
{
  // if we have a new owner
  if (NewOwner)
  {
    SetOwner(NewOwner);
  }
  if (Owner)
  {
    // no statemachine lets give the base one
    if (!TheStateMachine)
    {
      TheStateMachine = new StateMachine(this, StartState::GetInstance());
    }
    // Now do we have one let initialize it
    if (TheStateMachine)
    {
      TheStateMachine->Initialize();
    }
  }

}

void AIController::OnRespawn()
{
  if (TheStateMachine)
    TheStateMachine->EnterFirstState();

  bWantsToStopMoving = false;
}


void AIController::OnTakeDamage(const int& amount)
{
  // someone shot me shoot shoot them back if i have the ammo to
  if (TheStateMachine && Owner->GetAmmo() >=0)
    TheStateMachine->StateSwap(CanShootTarget::GetInstance());
}



void AIController::MoveTo(Vector2D Target)
{
  // Can we see the target?
  if (StaticMap::GetInstance()->IsLineOfSight(Owner->GetLocation(), Target))
  {
    bSeekingTarget = true;
    bFollowingPath = false;
    SeekTarget = Target;
  }
  else
  // Nope *Sigh* Time to use A*
  {

    Path = AStar::GeneratePath(NodeList::GetInstance()->GetClosestNode(Owner->m_Position), NodeList::GetInstance()->GetClosestNode(Target));
    // put the target on the end of the path
    Path.push_back(Target);
    AStartTarget = Target;
    bFollowingPath = true;
    bSeekingTarget = false;
  }
  bWantsToStopMoving = false;
}


void AIController::FollowPath()
{
  // Cant see the first point repath
  if (!StaticMap::GetInstance()->IsLineOfSight(Owner->m_Position, Path.front()))
  {
     MoveTo(AStartTarget);
  }
  

 // DrawPath();
  if (!CanSeePathToTarget())
  {
    Owner->m_Acceleration = AIBehaviors::Seek(Owner->m_Position, Owner->m_Velocity, Path.front());
  }
  else
  {
    Owner->m_Acceleration = AIBehaviors::Arrive(Owner->m_Position, Owner->m_Velocity, AStartTarget);
  }


  if (Path.size() > 1)
  {
    if (StaticMap::GetInstance()->IsLineOfSight(Owner->m_Position, *std::next(Path.begin())))
    {
      Path.pop_front();
    }
  }
  else if ((AStartTarget - Owner->m_Position).magnitude() < 20.0f)
  {
    bFollowingPath = false;
    StopMoving();
  }

}


int GetStateAsNumber(State* TheState)
{
  if (TheState == StartState::GetInstance())
  {
    return 1;
  }
  else if (TheState == CapDomPoint::GetInstance())
  {
    return 2;
  }
  else if (TheState == CanShootTarget::GetInstance())
  {
    return 3;
  }
  else if (TheState == ReloadState::GetInstance())
  {
    return 4;
  }
  else if (TheState == GuardDomPoint::GetInstance())
  {
    return 5;
  }
  return 0;
}

void AIController::Update()
{
  if (!Owner) return;
  if (TheStateMachine)
  {
    if (Owner->IsAlive())
    {
      TheStateMachine->Update();
    }
  }
  if (bWantsToStopMoving)
  {
    if (Owner->m_Velocity.magnitude() < 20.0f)
    {
      bWantsToStopMoving = false;
      Owner->m_Velocity = Vector2D(0.0f, 0.0f);
    }
    Owner->m_Acceleration = -Owner->m_Velocity;
  }
  
  
  

  if (bFollowingPath && !Path.empty())
  {
    FollowPath();
  }
  else if (bSeekingTarget)
  {
    SeekToTarget();
  }
  else
  {
    StopMoving();
  }

  Owner->m_Acceleration += Owner->m_Velocity.magnitudeSquared() * AIObsticalAvoidance::WallAvoid(Owner->m_Position, Owner->m_Velocity);


  //MyDrawEngine::GetInstance()->WriteInt(Owner->m_Position - Vector2D(3.0f,3.0f), GetStateAsNumber(TheStateMachine->GetCurrentState()), MyDrawEngine::GREEN);
}

bool AIController::IsCloseToSeekTarget(float Tolerance)
{
  return (Owner->GetLocation() - SeekTarget).magnitude() <= Tolerance;
}

Vector2D AIController::GetSeekTarget()
{
  return SeekTarget;
}

void AIController::StopMoving()
{
  bWantsToStopMoving = true;
  bSeekingTarget = false;
  bFollowingPath = false;
}


void AIController::DrawPath()
{
  Renderer* rend = Renderer::GetInstance();
  for (auto it = Path.begin(); it != Path.end(); it++)
  {
    rend->DrawDot(*it,5);
  }
  // end point
  //rend->DrawDot(*Path.rbegin(), 3);
  MyDrawEngine::GetInstance()->FillCircle(*Path.rbegin(), 50, MyDrawEngine::PURPLE);

}

void AIController::SeekToTarget()
{
  Owner->m_Acceleration = AIBehaviors::Seek(Owner->m_Position, Owner->m_Velocity, SeekTarget);
  if (IsCloseToSeekTarget(10.0f))
  {
    bSeekingTarget = false;
  }
  //MyDrawEngine::GetInstance()->FillCircle(SeekTarget, 30, MyDrawEngine::GREEN);
  //Renderer::GetInstance()->DrawLine(SeekTarget, Owner->m_Position,2);
}

AIController::~AIController()
{
  delete TheStateMachine;
}






bool AIController::CanSeeShootTarget()
{
  return (Owner && ShootTarget) ? StaticMap::GetInstance()->IsLineOfSight(Owner->m_Position, ShootTarget->m_Position) : false;
}
bool AIController::CanSeePathToTarget()
{
  return Owner ? StaticMap::GetInstance()->IsLineOfSight(Owner->m_Position, AStartTarget) : false;
}