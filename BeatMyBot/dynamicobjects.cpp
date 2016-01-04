#include "dynamicobjects.h"
#include "renderer.h"
#include "errorlogger.h"
#include "rules.h"
#define NULL 0




DynamicObjects::DynamicObjects()
{
	m_iNumPlacedDominationPoints =0;
	// Tell each bot its own team and number
	// (I hate this)
	for(int i=0;i<NUMTEAMS;i++)
	{
		for(int f=0;f<NUMBOTSPERTEAM;f++)
		{
			m_rgTeams[i].m_rgBots[f].SetOwnNumbers(i,f);
		}
	}
  // run a 3 mins game
  RemainingTime = 60*3;
#ifdef SMARTNETWORK
  this->OnSpawnedObject(this);
#endif

}
#ifdef SMARTNETWORK
void DynamicObjects::GetReplicatedValues(std::vector<ReplicatedData>* TheVector, int* DataRepByteSize) const
{
  for (unsigned int i = 0; i < NUMTEAMS; ++i)
  {
   RepData(this, m_rgTeams[i].m_iScore);
  }
  for (unsigned int i = 0; i < NUMDOMINATIONPOINTS; ++i)
  {
   RepData(this, m_rgDominationPoints[i].m_OwnerTeamNumber);
   RepData(this, m_rgDominationPoints[i].m_Location);
  }

  RepData(this, RemainingTime);

  
}
#endif

void DynamicObjects::Initialise()
{
	// Start the countdown for points
	m_dNextScorePoint=0.0;

	for(int i=0;i<NUMTEAMS;i++)
	{
		for(int f=0;f<NUMBOTSPERTEAM;f++)
		{
			// So the AI programmer can do any startup stuff
			m_rgTeams[i].m_rgBots[f].StartAI();	
		}
	}
}

DynamicObjects::~DynamicObjects()
{
#ifdef SMARTNETWORK
  this->OnRemovedObject(this);
#endif
}



DynamicObjects* DynamicObjects::pInst = NULL;

DynamicObjects* DynamicObjects::GetInstance()
{
	if(pInst == NULL)
	{
		pInst= new DynamicObjects();
	}

	return pInst;
}

void  DynamicObjects::Reset()
{
  for (unsigned int i = 0; i < MAXDOMINATIONPOINTS; ++i)
  {
    m_rgDominationPoints[i].m_OwnerTeamNumber = -1;
  }
  for (unsigned int i = 0; i < MAXTEAMS; ++i)
  {
    m_rgTeams[i].m_iScore = 0;
    for (unsigned int f = 0; f < MAXBOTSPERTEAM; ++f)
    { 
      m_rgTeams[i].m_rgBots[f].m_dTimeToRespawn = 1;
    }
  }
  
}

void DynamicObjects::Release()		// Static
{
	if(pInst)
	{
		delete pInst;
		pInst = NULL;
	}
}

ErrorType DynamicObjects::Update(float frametime)
{
 
	
 
  static float Timer = 1;
  Timer -= frametime;
  if (Timer <= 0.0f)
  {
    Timer = 1.0f;
    RemainingTime--;
  }
  if (RemainingTime <= 0)
  {
    // hack way to shut down game
    return ErrorType::FAILURE;
  }
  Renderer* TheRender = Renderer::GetInstance();
  TheRender->DrawTextAt(Vector2D(0, 300),L"Remaining Time: ");
  TheRender->DrawNumberAt(Vector2D(150, 300), RemainingTime);
 

	// Check for alliance changes to Domination points
	// Cycle through each domination point
	for(int i=0;i<NUMDOMINATIONPOINTS;i++)
	{
		int numTeamsInRange =0;				// How many teams are close enough to claim the point
		int lastTeamFound = -1;				


		// Cycle through each team
		for(int f=0;f<NUMTEAMS;f++)
		{	
			double dominationRange = DOMINATIONRANGE;	// How close you need to be to claim the point	
			// If this team already owns the point
			if(f == m_rgDominationPoints[i].m_OwnerTeamNumber)
			{
				// ... it doesn't need to be as close to retain it.
				dominationRange*=4;	
			}


			// Cycle through each bot in the team
			for(int g=0;g<NUMBOTSPERTEAM;g++)
			{		
				// How far is this bot from the point under study?
				double range = (m_rgDominationPoints[i].m_Location - m_rgTeams[f].m_rgBots[g].GetLocation()).magnitude();
				// If it is close enough and the bot is still alive
				// add this team to the list of candidate teams
				if(range<dominationRange && m_rgTeams[f].m_rgBots[g].IsAlive())
				{
					numTeamsInRange++;
					lastTeamFound=f;
					break;
				}
			}
		}
		// If only one team can claim it, they claim it.
		// Otherwise, it does not change.
		if(numTeamsInRange==1)
		{
			if(m_rgDominationPoints[i].m_OwnerTeamNumber != lastTeamFound)
			{
				// Special FX
        Renderer::GetInstance()->ShowDominationPointClaimed(m_rgDominationPoints[i].m_Location, lastTeamFound);
				m_rgDominationPoints[i].m_OwnerTeamNumber = lastTeamFound;
			}
		}
	}

	// Add scores to each team for domination points
	m_dNextScorePoint+=frametime;
	// If it is time for the next points to be awarded
	if(m_dNextScorePoint>1.0)
	{
		// Cycle through each domination point
		for(int i=0;i<NUMDOMINATIONPOINTS;i++)
		{	

			int team = m_rgDominationPoints[i].m_OwnerTeamNumber;
			if(team>=0 )		// If the domination point is owned by someone
			{
				// Give a point to the team
				m_rgTeams[team].m_iScore++;
			}
		}
		// Next points will be awarded in one second
		m_dNextScorePoint-=1.0;
	}

  // Update all bots
  for (int i = 0; i<NUMTEAMS; i++)
  {
    for (int f = 0; f<NUMBOTSPERTEAM; f++)
    {
      m_rgTeams[i].m_rgBots[f].Update(frametime);
    }
  }

	return SUCCESS;
}

int DynamicObjects::GetScore(int teamNumber) const
{
	return m_rgTeams[teamNumber].m_iScore;
}

ErrorType DynamicObjects::Render()
{
	ErrorType answer = SUCCESS;

	Renderer* pRenderer = Renderer::GetInstance();

	// Draw each Bot
	for(int i=0;i<NUMTEAMS;i++)
	{
		for(int f=0;f<NUMBOTSPERTEAM;f++)
		{
			Bot& currentBot = m_rgTeams[i].m_rgBots[f];
			// If it's alive, draw a circle and a line.
			if(currentBot.IsAlive())
			{
				if(pRenderer->DrawBot(currentBot.GetLocation(), currentBot.GetDirection() , i) == FAILURE)
				{
					ErrorLogger::Writeln(L"Renderer failed to draw a bot");
					answer=FAILURE;
				}
			}
			else	// Current Bot is dead
			{
				if(pRenderer->DrawDeadBot(currentBot.GetLocation() , i) == FAILURE)
				{
					ErrorLogger::Writeln(L"Renderer failed to draw a dead bot");
					// Non critical error
				}
			}
		}
	}

	// Draw domination points - just a circle with the right colour.
	for(int i=0;i<NUMDOMINATIONPOINTS;i++)
	{
		if(pRenderer->DrawDominationPoint( m_rgDominationPoints[i].m_Location, m_rgDominationPoints[i].m_OwnerTeamNumber)==FAILURE)
		{
				ErrorLogger::Writeln(L"Renderer failed to draw a domination point");
				answer=FAILURE;
		}
	}

	return SUCCESS;
}


Bot& DynamicObjects::GetBot(int teamNumber, int botNumber)
{
	teamNumber= teamNumber%NUMTEAMS;
	botNumber= botNumber%NUMBOTSPERTEAM;
	return m_rgTeams[teamNumber].m_rgBots[botNumber];
}

DominationPoint DynamicObjects::GetDominationPoint(int pointNumber) const
{
	return m_rgDominationPoints[pointNumber];
}

void DynamicObjects::PlaceDominationPoint(Vector2D location)
{
	if(m_iNumPlacedDominationPoints<NUMDOMINATIONPOINTS)
	{
		m_rgDominationPoints[m_iNumPlacedDominationPoints].m_Location=location;
		m_rgDominationPoints[m_iNumPlacedDominationPoints].m_OwnerTeamNumber=-1; 
		++m_iNumPlacedDominationPoints;
	}
}