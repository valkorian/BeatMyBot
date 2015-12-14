#pragma once
#include "State.h"


class StartState : public State
{
public:
  void Enter(class AIController* Controller);
                                                                                        
  void Exit(class AIController* Controller) ;
                                            
  void Update(class AIController* Controller);
 

  static StartState* Start();
  
  static void ShutDown();
 

  static State* GetInstance();
  ~StartState() override;
private:
  StartState();
  static StartState* Instance;
};


class CapDomPoint : public State
{
public:
  void Enter(class  AIController*) override;
  void Exit(class AIController*)   override;
  void Update(class AIController*) override;

  static CapDomPoint* Start();

  static void ShutDown();


  static State* GetInstance();
  ~CapDomPoint() override;
private:
  CapDomPoint();
  static CapDomPoint* Instance;
};

