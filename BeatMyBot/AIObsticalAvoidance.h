#pragma once
#include "vector2D.h"



class AIObsticalAvoidance
{
public:
  AIObsticalAvoidance();
  // avoid walls we are really close to
  static Vector2D WallAvoid(const Vector2D &CurrentPosition, const Vector2D &CurrentVelo);
  ~AIObsticalAvoidance();
private:

};

