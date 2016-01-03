#include "Network.h"
#include "ErrorLogger.h"
#include "dynamicObjects.h"
#include "rules.h"
#include "Helpers.h"
#include "NetworkFunctions.h"
#include <algorithm>
#include "DataLogger.h"

#include "Renderer.h"
#include "gametimer.h"
GameTimer NetworkTimer;


Network* Network::Instance = nullptr;

// threat started need to turn class methods into functions 
void* WINAPI WaitforClients_threadStarter(LPVOID that)
{
  return ((Network*)that)->THREAD_WaitforClients();
}



#ifdef SMARTNETWORK

void* WINAPI SmartReceveData_threadStarter(LPVOID that)
{
  return ((Network*)that)->THREAD_SmartReceveData();
}

bool Network::HasReceivedData()
{
  return bHasReceivedData;
}

// get data to be replicated over the network
void Network::GetRepDataFrom(const NetworkFunctions* Object )
{
  Object->GetReplicatedValues(&DataToReplicate, &DataToRepByteSize);
}



class AdressMatches
{

public:
  AdressMatches(void * ObjectToCheck) : Checker(ObjectToCheck){}
  AdressMatches() : Checker(nullptr){}
  bool operator() (ReplicatedData& A){ return Checker == &A.Owner; }
  void* Checker;
};


void Network::OnObjectRemoved(const NetworkFunctions* Object)
{
  AdressMatches MatchFunction((void* )Object);
  DataToReplicate.erase(std::remove_if(DataToReplicate.begin(), DataToReplicate.end(), MatchFunction), DataToReplicate.end());
}





void Network::GenerateRepData()
{
  if (RepData != nullptr)
  {
    ClearRepData();
  }
  
  RepData = new char[DataToRepByteSize];
  int count = 0;
  for (std::vector<ReplicatedData>::iterator it = DataToReplicate.begin(); it != DataToReplicate.end(); it++)
  {
    NetworkDataType* DataToRead = (NetworkDataType*)it->Data;
    for (unsigned int i = 0; i < it->Size; ++i)
    {
      RepData[count] = DataToRead[i];
      
      ++count;
    }
  }

}


void Network::ClearRepData()
{
  delete[] RepData;
  RepData = nullptr;
}


void Network::SmartSendToAllClients()
{
  GenerateRepData();
  SendToAllClients(RepData, DataToRepByteSize);
  ClearRepData();
  // reset the frame count since last update
  FramesSinceLastNetworkUpdate = 0;
}


void Network::SmartReceveData()
{
  CreateThread(nullptr, 0,  // can the thread be inherierted and size of the stack.. 
    (LPTHREAD_START_ROUTINE)SmartReceveData_threadStarter, // function to run on the thread
    this, // arguemtns to send to the thread function 
    0, // creation flag 0 == run thread emmediately 
    &NetworkThread
    ); // threads if
}


void Network::MarkHasRecievedDataAsRead()
{
  bHasReceivedData = false;
  if (!bPendingShutDown)
  {
    // start listing for more data
    SmartReceveData();
  }

}

void* Network::THREAD_SmartReceveData()
{
  // make sure repdata is clear
  ClearRepData();

  RepData = new char[DataToRepByteSize];
  CopyReceivedDataInto(&RepData[0], DataToRepByteSize);
  bHasReceivedData = true;
  return nullptr;
}


void Network::WriteRepData()
{
  // do we have data to write
  if (RepData)
  {
    int count = 0;
    for (std::vector<ReplicatedData>::iterator it = DataToReplicate.begin(); it != DataToReplicate.end(); it++)
    {
      NetworkDataType* DataToWriteTo = (NetworkDataType*)it->Data;
      for (unsigned int i = 0; i < it->Size; ++i)
      {
        DataToWriteTo[i] = RepData[count];
        ++count;
      }
    }
  }
  ClearRepData();
  MarkHasRecievedDataAsRead();
}



void Network::ClearDataToReplicate()
{
  DataToReplicate.clear();
}

void Network::SmartHandleNetworkRep()
{
  HandleFrameCounter();
  if (bPendingNetworkUpdate && Role == ConnectionType::SERVER)
  {
    SmartSendToAllClients();
    // clear the update request since we just filled it 
    bPendingNetworkUpdate = false;
  }
  else
  {
    if (HasReceivedData())
    {
      // if we got data from the server read it into rep data
      WriteRepData();
    }
  }
  // log the size size of the packet and the time between frames
  NetworkTimer.mark();
  DataLogger::WriteTimeWithValue(NetworkTimer.m_fFrameTime, DataToRepByteSize);
}

#endif // end of smart network functions 



Network::Network()
{
  NetworkTimer.mark();
  NetworkTimer.mark();
  ActiveSocket = -1;
  bPendingShutDown = false;
  Role = ConnectionType::UNCONNECTED;
  bPendingConnection = false;
  bIsConnectedToServer = false;
  bPendingNetworkUpdate = false;
  FramesSinceLastNetworkUpdate = 0;
#ifdef SMARTNETWORK
  bHasReceivedData = false;
#endif
  RepData = nullptr;
  DataToRepByteSize = 0;
}


void Network::HandleFrameCounter()
{
  FramesSinceLastNetworkUpdate++;
  if (FramesSinceLastNetworkUpdate >= MAXTICKBEFOREUPDATE)
  {
    RequestNetworkUpdate();
  }
}


void Network::RequestNetworkUpdate()
{
  bPendingNetworkUpdate = true;
}

void Network::CancelNetworkUpdate()
{
  bPendingNetworkUpdate = false;
}


void Network::HandleNetWorkReplication(void* Object, const int& size)
{

  if (IsServer())
  {
    SendToAllClients(Object, size);
  }
  else
  {
    CopyReceivedDataInto(Object, size);
  }


  NetworkTimer.mark();
  DataLogger::WriteTimeWithValue(NetworkTimer.m_fFrameTime, size);


}

Network* Network::Start()
{
  if (!Instance)
  {
    Instance = new Network();
  }
  return Instance;
}


Network* Network::GetInstance()
{
  // if we have a instance return it if not make one
  return Instance ? Instance : Start();
}


ConnectionType Network::GetConnectionType() const
{
  return Role;
}

bool Network::IsServer() const
{
  return Role == ConnectionType::SERVER;
}


bool Network::IsClient() const
{
  return Role == ConnectionType::CLIENT;
}



bool Network::IsUnconnected() const
{
  return Role == ConnectionType::UNCONNECTED;
}

int Network::NumberOfConnectedClients()
{
  return Connections.size();
}


bool Network::SwapFromClientToServer()
{
  // lets make sure we are a client
  if (IsClient())
  {
    // store the current port number needed later but close active socket will clear it
    int CurrentPort = ActivePort;
    // close the connection to the server
    CloseActiveSocket();

    // host the current game as a server host game returns true if it worked
    return HostGame(CurrentPort);
  }
  return false;
}



bool Network::HostGame(const unsigned short &PortNumber)
{
  if (!bPendingConnection)
  {
    // create the server as a socket returns the socket number if success 
    if (CreateServerSocket(PortNumber) > 0)
    {
      // sit waiting for clients 
      WaitforClients();
      bPendingConnection = false;
      return true;
    }
  }
  return false;
}

bool Network::JoinGame(const char* ServerIP, unsigned short const &PortNumber)
{
  if (!bPendingConnection)
  {
    if (CreateClientSocket(PortNumber) != INVALID_SOCKET)
    {
      bool Connected = ConnectToServer(ServerIP);
      bPendingConnection = false;
      return Connected;
    }
  }


	return false;
}


int Network::StartWSA()
{
  WSADATA wsaData;
  return WSAStartup(MAKEWORD(2, 0), &wsaData);
}

int Network::CreateClientSocket(const unsigned short &PortNumber)
{
  bPendingConnection = true;
  if (StartWSA() != 0)
  {
    ErrorLogger::Writeln(L"Error in started StartWSA");
    return INVALID_SOCKET;
  }

  // socket to create
  int TheSocket;
  
  TheSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (TheSocket == INVALID_SOCKET)
  {
    ErrorLogger::Writeln(L"Error in starting socket");
    return INVALID_SOCKET;
  }
  ActivePort = PortNumber;
  ActiveSocket = TheSocket;
  return TheSocket;
}





bool Network::ConnectToServer(const char* ServerIP)
{
  bool Connected = false;
  // local server adress
  sockaddr_in ServerAdress;
  // zero off all the data
  memset(&ServerAdress, 0, sizeof(ServerAdress));

  ServerAdress.sin_family = AF_INET;
  ServerAdress.sin_addr.s_addr = inet_addr(ServerIP);
 
  ServerAdress.sin_port = htons(ActivePort);

  if (connect(ActiveSocket, (sockaddr*)&ServerAdress, sizeof(ServerAdress)) < 0)
  {
    std::wstring ERR = L"Error Connecting to = ";
    wchar_t ErrBuff[10];
    _itow_s(WSAGetLastError(), ErrBuff, 10);
    ERR += ErrBuff;
    ErrorLogger::Writeln(ERR.c_str());
  }
  else
  {
    // if we connected set role to client 
    Role = ConnectionType::CLIENT;
    bIsConnectedToServer = true;
    Connected = true;
#ifdef SMARTNETWORK
    // start looking for data
    SmartReceveData();
#endif
  }
  // we are no longer trying to connect
  bPendingConnection = false;
  return Connected;
}


int Network::CreateServerSocket(const unsigned short &PortNumber)
{
  bPendingConnection = true;
  // if startWSA fails bind() wont work so no point in keeping going
  if (StartWSA() != 0)
  {
    ErrorLogger::Writeln(L"Error in started StartWSA");
    return -1;
  }

  // socket to create
  int TheSocket; 
  // local server adress
  struct sockaddr_in ServerAdress;
  // zero off all the data
  memset(&ServerAdress, 0, sizeof(ServerAdress));

  
  // create the socket 
  TheSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  // turn off Nagle's algorithm
  int flag = 1;
  setsockopt(TheSocket, IPPROTO_TCP, TCP_NODELAY, (char*)(&flag), sizeof(int));
  if (TheSocket == INVALID_SOCKET)
  {
    ErrorLogger::Writeln(L"Error in creating Server Socket");
    return -1;
  }
  ServerAdress.sin_family = AF_INET;
  ServerAdress.sin_port = htons(PortNumber);
  ServerAdress.sin_addr.s_addr = htonl(INADDR_ANY);

  // bind the socket to the local adress
  if (bind(TheSocket, (struct sockaddr*)&ServerAdress, sizeof(ServerAdress)) < 0)
  {
    std::wstring ERR = L"Error in binding socket = ";
    wchar_t ErrBuff[10];
    _itow_s(WSAGetLastError(), ErrBuff, 10);
    ERR += ErrBuff;
    ErrorLogger::Writeln(ERR.c_str());
    return -1;
  }
  
  // set the socket to listen for an connection 
  if (listen(TheSocket, MAXPENDINGCONNECTIONS) != 0)
  {
    ErrorLogger::Writeln(L"Error in setting up socket to listen");
    return -1;
  }
  Role = ConnectionType::SERVER;
  ActiveSocket = TheSocket;
  bPendingConnection = false;
  return TheSocket;
}

bool Network::IsConnectedToServer()
{
  return bIsConnectedToServer;
}


void Network::DropAllConnections()
{
  // do we have clients 
  if (Connections.empty())
  {
    for (std::vector<DWORD>::iterator it = Connections.begin(); it != Connections.end(); ++it)
    {
      // close the connection with the current client
      closesocket(*it);
    }
    // remove all connections from the list
    Connections.clear();
  }
}

void Network::CloseActiveSocket()
{

  // drop all the connected clients
  DropAllConnections();

  // if socket if no longer the default me must be using one 
  if (ActiveSocket != -1)
  {
    ActivePort = -1;
    closesocket(ActiveSocket);
    Role = ConnectionType::UNCONNECTED;
    ActiveSocket = -1;
  }
  
}

void Network::WaitforClients()
{
  CreateThread(nullptr, 0,  // can the thread be inherierted and size of the stack.. 
	  (LPTHREAD_START_ROUTINE)WaitforClients_threadStarter, // function to run on the thread
    this, // arguemtns to send to the thread function 
    0, // creation flag 0 == run thread emmediately 
    &NetworkThread
    ); // threads if
}

void* Network::THREAD_WaitforClients()
{

  while (!bPendingShutDown)
    // sit and wait for clients 
  {
    int NewCilent = AcceptClientConnection();
    // if NewCilent is <= 0 its means an error code got returned not an client 
    if (NewCilent > 0)
    {
      // Add the new client to the list of clients
      Connections.push_back(NewCilent);
      // we have a new client update the network so they have the up to date game
      RequestNetworkUpdate();
    }
  }
  return nullptr;
}


int Network::AcceptClientConnection()
{
  struct sockaddr_in ClientAdress;
  int ClientSocket = -1;
  int clntLen = sizeof(ClientAdress);

  ClientSocket = accept(ActiveSocket, (sockaddr *)&ClientAdress, (int*) &clntLen);

  if (ClientSocket == INVALID_SOCKET)
  {
    std::wstring ERR = L"Error in Accepting client = ";
    wchar_t ErrBuff[10];
    _itow_s(WSAGetLastError(), ErrBuff, 10);
    ERR += ErrBuff;
    ErrorLogger::Writeln(ERR.c_str());
    return -1;
  }
  return ClientSocket;
}






void Network::SendToAllClients(void* Object, const int& size)
{
  
  // only send if we are the server
  if (Role == ConnectionType::SERVER)
  {

    Renderer::GetInstance()->DrawTextAt(Vector2D(10, 500), TEXT("PacketSize = "));
    wchar_t Numbuff[10];
    _itow_s(size, Numbuff, 10);
    Renderer::GetInstance()->DrawTextAt(Vector2D(120, 500), Numbuff);

#ifdef SMARTNETWORK
    Renderer::GetInstance()->DrawTextAt(Vector2D(10, 600), TEXT("Vector Size = "));
    _itow_s(DataToReplicate.size() , Numbuff, 10);
    Renderer::GetInstance()->DrawTextAt(Vector2D(120, 600), Numbuff);
#endif

    

    bool HasDroppedClient = false;

    
   

    // used to store a the client that has dropped if one is found 
    std::vector<DWORD>::iterator DroppedClient;

    // send the buffer to every client
    for (std::vector<DWORD>::iterator it = Connections.begin(); it != Connections.end(); it++)
    {
      if (send(*it, (char*)Object, size, 0) == SOCKET_ERROR)
      {
        HasDroppedClient = true;
        // Error with this client time to drop them
        DroppedClient = it;
      }
    }
    if (HasDroppedClient)
    {
      Connections.erase(DroppedClient);
    }
  }
}





void Network::CopyReceivedDataInto(void* Object, const int& size)
{
  
  if (Role == ConnectionType::CLIENT)
  {
    char* Arr = new char[size];
    if (recv(ActiveSocket, Arr, size, 0) <= 0)
    {
      // some thing went wrong with reading.. most times its not connected to server
      bIsConnectedToServer = false;
    }
    else
    {
      // only copy the data if the receive worked unless data will be replaced by junk
      memcpy(Object, Arr, size);
    }
    delete[] Arr;
  }
  
}


void Network::ShutDown()
{

  if (Instance)
  {
    Instance->CloseActiveSocket();
#ifdef SMARTNETWORK
    Instance->ClearRepData();
    Instance->ClearDataToReplicate();
#endif
    delete Instance;
  }
}


Network::~Network()
{
}
