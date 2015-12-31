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
  void MoveTo(Vector2D Target);
  void StopMoving();
  bool CanSeeShootTarget();
  bool CanSeePathToTarget();
  void OnRespawn();
  void SeekToTarget();
  void OnTakeDamage(const int& amount);
  // DEBUGGING
  void DrawPath();
  Vector2D GetSeekTarget();
  bool IsCloseToSeekTarget(float Tolerance = 40.0f);
  ~AIController();
private:
  void FollowPath();
  
private:
  class StateMachine* TheStateMachine;
  bool bWantsToStopMoving;
  Vector2D AStartTarget;
  Vector2D SeekTarget;
  std::list<Vector2D> Path;

public:
  bool bFollowingPath;
  bool bTryToShootTarget;
  bool bLeader;
  bool bSeekingTarget;
  class Bot* Buddy;
  class Bot* SecondShootTarget;
  class Bot* Owner;
  class Bot* ShootTarget;
  class Bot* MostDangerousTarget;
};

