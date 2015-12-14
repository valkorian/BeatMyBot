#pragma once
#include "vector2D.h"
#include <list>
#include <vector>
#include <Windows.h>
#include "AStar.h"

#define MAX_NUM_OF_EDGES 10
#define MAX_EDGE_LENGHT  1000.0f
#define MIN_NODE_PLACE_RECTANGLE_SIZE 2900.f



struct EdgeList
{
  EdgeList(struct Node* _ConnectedTo, const float& _Cost) : ConnectedTo(_ConnectedTo), Cost(_Cost){}
  struct Node* ConnectedTo;
  float Cost;
  
  
};


struct Node
{
  Node(int _NodeID, Vector2D _Position) : NodeID(_NodeID), Position(_Position)
  {
#ifdef ASTAR
    memset(&AStarVal, 0, sizeof(AStarVal));
#endif
  }
  int NodeID;
  Vector2D Position;
  std::vector<EdgeList> Edges;
#ifdef ASTAR
  AStarValues AStarVal;
#endif
};


class NodeList
{
public:
  friend void* WINAPI GenerateNodes_threadStarter(LPVOID that);
  // starts the node list
  static NodeList* Start();
  // shuts down the node list 
  static void ShutDown();
  // returns the instance of the nodelist
  static NodeList* GetInstance();
  void AddNode(const Vector2D& Position);
  void GenerateNodeList();
  // draws all the nodes on the map
  void DrawAllNodes();
  void DrawAllEdges();
  bool IsMapLoading();
  Node* GetClosestNode(Vector2D Pos);
  std::vector<Node*> GetAllVisiableNodes(Vector2D Pos);
  Node* GetRandomNode();
  std::list<Node>& GetNodeList(){ return TheNodeList; }
  ~NodeList();
private:
  void GenerateNodes();
  void GenerateEdges();
  void* THREAD_GenerateNodeList();
  static NodeList* Instance;
  std::list<Node> TheNodeList;
  bool bLoadingMap;
  DWORD NodeGenThread;
  NodeList();
  int LastID;
};

