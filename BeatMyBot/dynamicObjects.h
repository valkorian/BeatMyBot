#pragma once
#include "errortype.h"
#include "bot.h"
#include "NetworkFunctions.h"
#include "rules.h"
const int MAXTEAMS = NUMTEAMS;
const int MAXBOTSPERTEAM = NUMBOTSPERTEAM;
const int MAXDOMINATIONPOINTS = NUMDOMINATIONPOINTS;



struct DominationPoint
{
	int m_OwnerTeamNumber;
	Vector2D m_Location;

	DominationPoint()
	{
		m_OwnerTeamNumber=-1;
	}
};

struct Team
{
	Bot m_rgBots[MAXBOTSPERTEAM];
	int m_iScore;

	void Reset()
	{
		m_iScore = 0;
   
	}

	Team()
	{
		Reset();
	}
};


class DynamicObjects 
#ifdef SMARTNETWORK 
  : public NetworkFunctions
{
  GENERATE_NETWORK_BODY()
#else
{
#endif
private:
	DynamicObjects();
	~DynamicObjects();
	DynamicObjects(const DynamicObjects & other);	//Disable
	Team m_rgTeams[MAXTEAMS];
	DominationPoint m_rgDominationPoints[MAXDOMINATIONPOINTS];
	static DynamicObjects* pInst;
	int m_iNumPlacedDominationPoints;
	float m_dNextScorePoint;						// Time until next points get added
  //int RemainingTime;
public:
	static DynamicObjects* GetInstance();			// Thread-safe singleton
  void Reset();
	// Deletes pInst
	static void Release();

	// Runs most of the game - tells each Bot to update
	// and checks for domination point capture
	ErrorType Update(float frametime);

	// Places all dynamic objects (Bots and dom points)
	// in the right location
	ErrorType Render();

	// Sets the score timer to zero and initialises the AI 
	// for each Bot
	void Initialise();

	// Returns a reference to the requested bot
	Bot& GetBot(int teamNumber, int botNumber);	

	// Returns the score for the specified team
	int GetScore(int teamNumber) const;

	// Returns the specified Domination point
	DominationPoint GetDominationPoint(int pointNumber) const;

	// Adds a new domination point at the specified location
	// (up to the maximum limit in MAXDOMINATIONPOINTS
	void PlaceDominationPoint(Vector2D location);
};