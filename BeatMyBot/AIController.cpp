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
Owner(nullptr), bFollowingPath(false),
TheStateMachine(nullptr), ShootTarget(nullptr), MostDangerousTarget(nullptr)
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
    TheStateMachine = new StateMachine(this);
  }

}


void AIController::PathTo(Vector2D Target)
{
  Path = AStar::GeneratePath(NodeList::GetInstance()->GetClosestNode(Owner->m_Position), NodeList::GetInstance()->GetClosestNode(Target));
  // put the target on the end of the path
  Path.push_back(Target);
  AStartTarget = Target;

  bFollowingPath = true;

}


void AIController::FollowPath()
{
  // Cant see the first point repath
  if (!StaticMap::GetInstance()->IsLineOfSight(Owner->m_Position, Path.front()))
  {
     PathTo(AStartTarget);
  }
  

  DrawPath();
  if (!CanSeePathToTarget())
  {
    Owner->m_Acceleration = AIBehaviors::Seek(Owner->m_Position, Owner->m_Velocity, Path.front());
  }
  else
  {
    Owner->m_Acceleration = AIBehaviors::Arrive(Owner->m_Position, Owner->m_Velocity, AStartTarget);
  }

  Owner->m_Acceleration += Owner->m_Velocity.magnitudeSquared() * AIObsticalAvoidance::WallAvoid(Owner->m_Position, Owner->m_Velocity);

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




void AIController::Update()
{
  if (!Owner) return;
  if (TheStateMachine)
  {
    if (Owner->IsAlive())
    {
      TheStateMachine->Update();
    }
    else
    {
      TheStateMachine->StateSwap(StartState::GetInstance());
    }
  }

  if (bFollowingPath && !Path.empty())
  {
    FollowPath();
  }
  else
  {
    StopMoving();
  }
 
}


void AIController::StopMoving()
{
  if (Owner)
  {
    Owner->m_Acceleration = Vector2D(0, 0);
    Owner->m_Velocity = Vector2D(0, 0);
  }
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


AIController::~AIController()
{
  delete TheStateMachine;
}



void AIController::ShootAt(Bot* ShootTarget)
{

}

bool AIController::CanSeeShootTarget()
{
  return (Owner && ShootTarget) ? StaticMap::GetInstance()->IsLineOfSight(Owner->m_Position, ShootTarget->m_Position) : false;
}
bool AIController::CanSeePathToTarget()
{
  return Owner ? StaticMap::GetInstance()->IsLineOfSight(Owner->m_Position, AStartTarget) : false;
}