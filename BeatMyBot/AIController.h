#pragma once
#include <list>
#include "vector2D.h"
class AIController
{
public:
  AIController();
  void SetOwner(class Bot* Owner);
  void Initialise(class Bot* Owner = nullptr);
  void Update();
  class StateMachine* GetStateMahcine(){ return TheStateMachine; }
  class Bot* GetOwner(){ return Owner; }
  void PathTo(Vector2D Target);
  void StopMoving();
  void ShootAt(class Bot* ShootTarget);
  bool CanSeeShootTarget();
  bool CanSeePathToTarget();
  void StopShooting();

  // DEBUGGING
  void DrawPath();

  ~AIController();
private:
  void FollowPath();
  
private:
  class StateMachine* TheStateMachine;
  class Bot* Owner;
  class Bot* ShootTarget;
  Vector2D AStartTarget;
  std::list<Vector2D> Path;


  bool bFollowingPath;
  bool bTryToShootTarget;
};

