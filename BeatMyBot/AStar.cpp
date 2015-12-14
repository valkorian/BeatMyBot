#include "AStar.h"
#include "NodeList.h"
#include <algorithm>

// just a rename of push_back
#define add(n) push_back(n) 


std::list<Node*> AStar::OpenSet;
std::list<Node*> AStar::ClosedSet;

AStar::AStar()
{
}

// return it to the smallest fvalue
Node* FindSmallistFValueAndPop(std::list<Node*> &TheList)
{
  // if list is empty just leave
  if (TheList.empty()){ return nullptr; }

  auto it = TheList.begin();
  std::list<Node*>::iterator Smallist = it;
  it++;
  for (; it != TheList.end(); it++)
  {  
    {
      if ((*it)->AStarVal.GuessedCostToEnd < (*Smallist)->AStarVal.GuessedCostToEnd)
      {
        Smallist = it;
      }
    }
  }
  
  Node* SmallistNode = *Smallist;
  TheList.erase(Smallist);
  return SmallistNode;
}


void AStar::ClearOpenAndCloseSet()
{
	ClearOpenSet();
	ClearClosedSet();
}

void AStar::ClearOpenSet()
{
	// set all the Astart data in the node back to 0 for next time
	std::for_each(OpenSet.begin(), OpenSet.end(), [](Node* TheNode){memset(&TheNode->AStarVal, 0, sizeof(AStarValues)); });
	// remove them all from the list
	OpenSet.clear();
}

void AStar::ClearClosedSet()
{
	// set all the Astart data in the node back to 0 for next time
	std::for_each(ClosedSet.begin(), ClosedSet.end(), [](Node* TheNode){memset(&TheNode->AStarVal, 0, sizeof(AStarValues)); });
	// remove them all from the list
	ClosedSet.clear();
}

float AStar::Heuristic(Node* Start, Node* End)
{
  return (Start->Position - End->Position).magnitude();
}

std::list<Vector2D> AStar::GeneratePath(Node* StartNode, Node* EndNode)
{
	// only path find if we have a start and a end node and if they are not the same node
	if ((StartNode && EndNode) && StartNode != EndNode)
	{
		// if the open or closed set are not empty clear them out
		if (!OpenSet.empty())   { ClearOpenSet(); }
		if (!ClosedSet.empty()) { ClearClosedSet(); }


		StartNode->AStarVal.AStarCamefrom = nullptr;
		StartNode->AStarVal.CostFromStart = 0.0f;
	  StartNode->AStarVal.GuessedCostToEnd = StartNode->AStarVal.CostFromStart + Heuristic(StartNode, EndNode);

		OpenSet.add(StartNode);

    while (!OpenSet.empty())
    {

      // find the smallest f value in the open set
      Node* CurrentNode = FindSmallistFValueAndPop(OpenSet);

      if (CurrentNode == EndNode)
      {
        return GetPath();
      }

      ClosedSet.add(CurrentNode);
      if (!CurrentNode->Edges.empty())
      {
        for (auto it : CurrentNode->Edges)
        {
          if (std::find(ClosedSet.begin(), ClosedSet.end(), it.ConnectedTo) == ClosedSet.end())
          {
            // get cost from start to this neighbour
            float NewDistToFromStart = CurrentNode->AStarVal.CostFromStart + it.Cost;

            if ((NewDistToFromStart < it.ConnectedTo->AStarVal.CostFromStart) ||
              (std::find(OpenSet.begin(), OpenSet.end(), it.ConnectedTo) == OpenSet.end()))
            {
              it.ConnectedTo->AStarVal.AStarCamefrom = CurrentNode;
              it.ConnectedTo->AStarVal.CostFromStart = NewDistToFromStart;
              it.ConnectedTo->AStarVal.GuessedCostToEnd = NewDistToFromStart + Heuristic(it.ConnectedTo, EndNode);

              // this is not in the openset 
              if ((std::find(OpenSet.begin(), OpenSet.end(), it.ConnectedTo) == OpenSet.end()))
              {
                OpenSet.add(it.ConnectedTo);
              }

            }
          }
        }
      }
		}
	}
  return GetPath();
}

std::list<Vector2D> AStar::GetPath()
{
  std::list<Vector2D> Path;
  //ClosedSet.reverse();
  return ClosedSet.empty() ? Path : GetPath(*ClosedSet.rbegin(), Path);

}


std::list<Vector2D>& AStar::GetPath(Node* ThisNode, std::list<Vector2D> &Path)
{
  if (ThisNode->AStarVal.AStarCamefrom == nullptr)
  {
    return Path;
  }
  else
  {
    GetPath(ThisNode->AStarVal.AStarCamefrom, Path);
    Path.add(ThisNode->AStarVal.AStarCamefrom->Position);
    
    return Path;
    
  }
}



AStar::~AStar()
{
}
