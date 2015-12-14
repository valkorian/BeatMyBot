#pragma once
#define ASTAR
#include <list>
#include "vector2D.h"
// stuct of values only used in the A* path finding
struct AStarValues
{
	float CostFromStart;
	float GuessedCostToEnd;
	struct Node* AStarCamefrom;
};

class AStar
{
public:
	AStar();
  static std::list<Vector2D>  GeneratePath(struct Node* StartNode, struct Node* EndNode);
	static void ClearOpenAndCloseSet();
	static void ClearOpenSet();
	static void ClearClosedSet();
	static float Heuristic(struct Node* Start, struct Node* End);
  static std::list<Vector2D>  GetPath();
  static std::list<Vector2D>& GetPath(Node* ThisNode, std::list<Vector2D> &Path);
private:
	// list of nodes been checked
  static std::list<struct Node*> OpenSet;
	// list of nodes on the tree
	static std::list<struct Node*> ClosedSet;
	~AStar();
};

