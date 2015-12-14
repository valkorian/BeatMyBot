#include "NodeList.h"
#include "Renderer.h"
#include "staticmap.h"
#include "NodePlacer.h"
#include <algorithm>

// just a rename of push_back
#define add(n) push_back(n) 


// threat started need to turn class methods into functions 
void* WINAPI GenerateNodes_threadStarter(LPVOID that)
{
  return ((NodeList*)that)->THREAD_GenerateNodeList();
}





NodeList* NodeList::Instance = nullptr;

NodeList::NodeList()
{
  LastID = 0;
  bLoadingMap = false;
}


// starts the node list
NodeList* NodeList::Start()
{
  if (!Instance)
  {
    Instance = new NodeList;
  }
  return Instance;
}


// shuts down the node list 
void NodeList::ShutDown()
{
  if (Instance)
  {
    Instance->TheNodeList.clear();
    delete Instance;
  }
}


// returns the instance of the nodelist
NodeList* NodeList::GetInstance()
{
  return Instance ? Instance : Start();
}


void NodeList::AddNode(const Vector2D& Position)
{
  TheNodeList.add(Node(LastID++, Position));
}


void NodeList::GenerateNodeList()
{
  CreateThread(nullptr, 0,  // can the thread be inherierted and size of the stack.. 
  (LPTHREAD_START_ROUTINE)GenerateNodes_threadStarter, // function to run on the thread
  this, // arguemtns to send to the thread function 
  0, // creation flag 0 == run thread emmediately 
  &NodeGenThread
  ); // threads if
}



void NodeList::GenerateNodes()
{
  NodePlacer::FillMapsWithNodes();
  
}

void* NodeList::THREAD_GenerateNodeList()
{
  bLoadingMap = true;
  GenerateNodes();
  GenerateEdges();
  bLoadingMap = false;
  return nullptr;
}

bool NodeListSorter(const EdgeList &First, const EdgeList &Second)
{
  return First.Cost < Second.Cost;
}


void NodeList::GenerateEdges()
{
  StaticMap* pStaticMap = StaticMap::GetInstance();
  for (auto i = TheNodeList.begin(); i != TheNodeList.end(); i++)
  {
    if (i != TheNodeList.end())
    {
    i++;
      auto f = i;
      i--;
      for (; f != TheNodeList.end(); f++)
      { 
        {
          float Distance = (i->Position - f->Position).magnitude();
		      if (pStaticMap->IsLineOfSight(i->Position, f->Position) && ((Distance < MAX_EDGE_LENGHT) || (i->Edges.size() == 0)))
		      {	
			      i->Edges.add(EdgeList(&(*f), Distance));
			      f->Edges.add(EdgeList(&(*i), Distance));
			 
          }
        }
      }
    }
  }
  // keep the closist nodea 
  for (auto i = TheNodeList.begin(); i != TheNodeList.end(); i++)
  {
    std::sort(i->Edges.begin(), i->Edges.end(), NodeListSorter);
    if (i->Edges.size() > MAX_NUM_OF_EDGES)
    {
      i->Edges.resize(MAX_NUM_OF_EDGES, EdgeList(&(*i), 0));
    }
    
  }
  
}

Node* NodeList::GetRandomNode()
{
  //Node* ReturnNode = nullptr;
  int NumToReturn = rand() % TheNodeList.size();
  int Count = 0;
  for (auto it = TheNodeList.begin(); it != TheNodeList.end(); it++)
  {
    if (Count >= NumToReturn)
    {
      return &*it;
    }
    Count++;
  }
  return nullptr;
}

bool NodeList::IsMapLoading()
{
  return bLoadingMap;
}


void NodeList::DrawAllNodes()
{
  Renderer* TheRender = Renderer::GetInstance();
  for (auto i : TheNodeList)
  {
    TheRender->DrawDot(i.Position,3);
  }
}

void NodeList::DrawAllEdges()
{ 
  Renderer* TheRender = Renderer::GetInstance();
  for (auto i : TheNodeList)
  {
    int color = 1;
    for (auto f : i.Edges)
    {
      if (color > 10)
      {
        color = 10;
      }
        TheRender->DrawLine(i.Position, f.ConnectedTo->Position, color++);
    }
  }
}


std::vector<Node*> NodeList::GetAllVisiableNodes(Vector2D Pos)
{
  std::vector<Node*> VisiableNodes;
  StaticMap* PStMp = StaticMap::GetInstance();

  //for (auto it : TheNodeList)

  for (std::list<Node>::iterator it = TheNodeList.begin(); it != TheNodeList.end(); it++)
  {
    if (PStMp->IsLineOfSight(Pos, it->Position))
    {
      VisiableNodes.push_back(&(*it));
    }
  }
  return VisiableNodes;
}



Node* NodeList::GetClosestNode(Vector2D Pos)
{
  std::vector<Node*> VisiableNodes = GetAllVisiableNodes(Pos);

  float DistSquared = 0.0f;
  Node* Closest = nullptr;
  for (auto it = VisiableNodes.begin(); it != VisiableNodes.end(); it++)
  {
    if (!Closest)
    {     
      Closest = (*it);
      DistSquared = (Pos - (*it)->Position).magnitude();
    }
    else
    {
      float NewDis = (Pos - (*it)->Position).magnitude();
      if (NewDis < DistSquared)
      {
        Closest = (*it);
        DistSquared = NewDis;
      }
    }
  }
  return Closest;

}



NodeList::~NodeList()
{

}
