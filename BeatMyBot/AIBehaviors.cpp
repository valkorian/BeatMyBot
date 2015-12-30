#include "AIBehaviors.h"
#include "rules.h"

AIBehaviors::AIBehaviors()
{
}

Vector2D AIBehaviors::Seek(const Vector2D& CurrentPosition, const Vector2D& CurrrentVelcoity, const Vector2D& TargetPosition)
{
  return ((TargetPosition - CurrentPosition).unitVector() * MAXBOTSPEED)  -CurrrentVelcoity;
}


Vector2D AIBehaviors::Flee(const Vector2D& CurrentPosition, const Vector2D& CurrrentVelcoity, const Vector2D& TargetPosition)
{
  return ((CurrentPosition - TargetPosition).unitVector() * MAXBOTSPEED) - CurrrentVelcoity;
}


Vector2D AIBehaviors::Arrive(const Vector2D& CurrentPosition, const Vector2D& CurrrentVelcoity, const Vector2D& TargetPosition)
{
  return ((TargetPosition - CurrentPosition).unitVector() * ((TargetPosition - CurrentPosition).magnitude() / 2.0f /*Change Const to get better effect */)) - CurrrentVelcoity;
}


Vector2D AIBehaviors::Pursue(const Vector2D& CurrentPosition, const Vector2D& CurrrentVelcoity, const Vector2D& TargetPosition, const Vector2D& TargetVelocty, const float& LeadPursuitTime)
{
  float time = (TargetPosition - CurrentPosition).magnitude() / MAXBOTSPEED;

  Vector2D PursueTarget = TargetPosition + TargetVelocty * (time * LeadPursuitTime);

  return Seek(CurrentPosition,CurrrentVelcoity,PursueTarget);
}


Vector2D AIBehaviors::Evade(const Vector2D& CurrentPosition, const Vector2D& CurrrentVelcoity, const Vector2D& TargetPosition, const Vector2D& TargetVelocty, const float& LeadPursuitTime)
{
  float time = (TargetPosition - CurrentPosition).magnitude() / MAXBOTSPEED;

  Vector2D PursueTarget = TargetPosition + TargetVelocty * (time * LeadPursuitTime);

  return Flee(CurrentPosition,CurrrentVelcoity,PursueTarget);
}



AIBehaviors::~AIBehaviors()
{
}
