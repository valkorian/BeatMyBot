#pragma once
#include "shapes.h"
#include "rules.h"
#include "NetworkFunctions.h"
class Bot 
#ifdef SMARTNETWORK 
  : public NetworkFunctions
{
  GENERATE_NETWORK_BODY()
#else
  {
#endif
    friend class DynamicObjects;
    friend class CanShootTarget;
    friend class ReloadState;
protected:

	Vector2D m_Position;			// Current world coordinates
	Vector2D m_Velocity;			// Current velocity
	Vector2D m_Acceleration;
	float m_dDirection;			// Direction the bot is pointing in. radians anticlockwise from south
	int m_iAmmo;					// Not currently used
	bool m_bFiring;
	bool m_bAiming;					// If true, bot is aiming and cannot move
	float m_dTimeToCoolDown;		// Countdown until the time the bot can shoot again
	float m_dTimeToRespawn;		// Countdown until the bot can respawn. If zero or below, bot is alive
	int m_iAimingAtTeam;			// Team number of the bot being aimed at
	int m_iAimingAtBot;				// Number of the bot being aimed at
	int m_iOwnTeamNumber;
	int m_iOwnBotNumber;
	float m_dAccuracy;				// Accuracy of the current firing solution (1==100%)
	int m_iHealth;					// Health (up to 100)

	Vector2D targetPoint;			// Used only by placeholder AI. Delete this.
  
  friend class AIController;
  class AIController* AiController;
  
public:
  int GetTeamNumber(){ return m_iOwnTeamNumber; }
  int GetBotNumber()  { return m_iOwnBotNumber; }
  int GetAmmo(){ return m_iAmmo; }

	Bot();	
  ~Bot();
	// Runs once each frame. Handles physics, shooting, and calls
	// ProcessAI
	void Update(float frametime);

  void Reload();

  void OnRespawn();
	// Returns the location of the bot
	Vector2D GetLocation();

	// Returns the velocity of the bot
	Vector2D GetVelocity();
	
	// Returns the direction the bot is pointing. In radians anticlockwise from south
	float GetDirection();

	// Restarts the bot in a new location, with full heath, etc
	void PlaceAt(Vector2D position);	
	
	// Returns true if the bot is currently not respawning
	bool IsAlive();					
	
	// This is your function. Use it to set up any states at the beginning of the game
	// and analyse the map.
	// Remember that bots have not spawned yet, so will not be in their
	// starting positions.
	void StartAI();

	// This is your function. Use it to set the orders for the bot.
	// Will be called once each frame from Update
	void ProcessAI();	

	// This is a quick n' dirty AI for team 2.
	// Try to make team 1 better than this.
	// Will be called once each frame from Update
	void ProcessAIBadly();
			
	// Returns the number of the team of the bot being aimed at.
	// Returns a negative number if no bot is being aimed at.
	int GetTargetTeam();	

	// Returns the number of the bot being aimed at.
	// Returns a negative number if no bot is being aimed at.
	int GetTargetBot();

	// Sets the bots own team number and bot number.
	// No need to call this
	void SetOwnNumbers(int teamNo, int botNo);

	// Returns the current health of the bot
	int GetHealth();

	// Returns the current accuracy of the bot.
	// Accuracy is the probability of hitting the current target.
	// If the bot is not aiming, this will be zero.
	double GetAccuracy();

	// Sets the target of the current bot.
	// This will reset the accuracy to zero if the target bot 
	// has changed.
	// It also sets the bot to aiming mode, which means it will stop
	// moving.
	// If you want the bot to stop aiming, just set mbAiming = false
	// or use the StopAiming method
	void SetTarget(int targetTeamNo, int targetBotNo);

	// Call this to set the bot to shoot, if it can.
	// Once a shot it taken, the bot will not shoot again unless told to do so.
	void Shoot();

	// Delivers the specified damage to the bot.
	// If this drops health below zero, the bot will die
	// and will respawn at the spawn point after a specified interval
	void TakeDamage(int amount);

	// Stops the bot from aiming, so it can move again
	void StopAiming();

  // get the data to replicated over network
  void GetData();

  // set the data that was replicated over network
  void SetData();



};