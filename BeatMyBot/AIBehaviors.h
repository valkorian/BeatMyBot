#pragma once
#include "vector2D.h"


class AIBehaviors
{

public:
  AIBehaviors();
  static Vector2D Seek(const Vector2D& CurrentPosition, const Vector2D& CurrrentVelcoity, const Vector2D& TargetPosition);
  static Vector2D Flee(const Vector2D& CurrentPosition, const Vector2D& CurrrentVelcoity, const Vector2D& TargetPosition);
  static Vector2D Arrive(const Vector2D& CurrentPosition, const Vector2D& CurrrentVelcoity, const Vector2D& TargetPosition);
  static Vector2D Pursue(const Vector2D& CurrentPosition, const Vector2D& CurrrentVelcoity, const Vector2D& TargetPosition, const Vector2D& TargetVelocty, const float& LeadPursuitTime = 1.0f);
  static Vector2D Evade(const Vector2D& CurrentPosition, const Vector2D& CurrrentVelcoity, const Vector2D& TargetPosition, const Vector2D& TargetVelocty, const float& LeadPursuitTime = 1.0f);
  ~AIBehaviors();
};




