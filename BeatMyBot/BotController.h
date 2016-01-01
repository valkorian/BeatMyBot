#pragma once
#include "AIController.h"

class BotController : public AIController
{
public :
  void OnRespawn() override ;
  void OnTakeDamage(const int& amount) override ;
  
public:
  bool bTryToShootTarget;
  bool bLeader;

  class Bot* Buddy;
  class Bot* SecondShootTarget;
  class Bot* ShootTarget;
  class Bot* MostDangerousTarget;
};