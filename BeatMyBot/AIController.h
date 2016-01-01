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
  virtual void OnRespawn();
  void SeekToTarget();
  virtual void OnTakeDamage(const int& amount);
  Vector2D GetSeekTarget();
  bool IsCloseToSeekTarget(float Tolerance = 40.0f);
  ~AIController();

  void DrawPath();
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
  bool bSeekingTarget;
  bool bTryToShootTarget;
  bool bLeader;

  class Bot* Buddy;
  class Bot* SecondShootTarget;
  class Bot* ShootTarget;
  class Bot* MostDangerousTarget;
  class Bot* Owner;
  
};

