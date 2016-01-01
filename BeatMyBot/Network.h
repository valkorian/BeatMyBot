#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#define NETWORK
#include <vector>
#include "NetworkFunctions.h"

// mex number of pending connections allowed
#define MAXPENDINGCONNECTIONS 5

// number of frames before the network updated
#define MAXTICKBEFOREUPDATE   1

#define DefaultPort 550

typedef char NetworkDataType;




enum class ConnectionType
{
  SERVER,
  CLIENT,
  UNCONNECTED
};



class Network
{
	friend void* WINAPI WaitforClients_threadStarter(LPVOID that);
  friend void* WINAPI SmartReceveData_threadStarter(LPVOID that);


public:
  static Network* Start();
  static Network* GetInstance();
  static void ShutDown();

  // host an network game returns true if success 
  bool HostGame(const unsigned short &PortNumber = DefaultPort);

  // join a network game returns true if success
  bool JoinGame(const char* ServerIP, const unsigned short &PortNumber = DefaultPort);

  // closed and removed all connections 
  void DropAllConnections();

#ifdef SMARTNETWORK
  void SmartSendToAllClients();
 
  void SmartReceveData();

  void MarkHasRecievedDataAsRead();
  
  void* THREAD_SmartReceveData();

  void WriteRepData();

  // add data to be replicated over the network
  void GetRepDataFrom(const class NetworkFunctions* Object);

  // removes all the replicated data from an object when its removed 
  void OnObjectRemoved(const NetworkFunctions* Object);

  void SmartHandleNetworkRep();

  bool HasReceivedData();
#endif

  // send the buffer to all connected clients only had effect if we are the server 
  void SendToAllClients(void* Object, const int& size);

  void CopyReceivedDataInto(void* Object, const int& size);

  ConnectionType GetConnectionType() const;

  // are we a server
  bool IsServer() const;

  // are we a client
  bool IsClient() const;

  // returns if connected to the network as a client or server
  bool IsConnectedAsClientOrServer() const;

  // are we running the game but failed to set up an connect or an server
  bool IsUnconnected() const;

  // returns if we area connected to an server.. data is invalid if we are a server
  bool IsConnectedToServer();

  // turns a client into an server allowing connect to them will keep all current game data 
  bool SwapFromClientToServer();

  void HandleNetWorkReplication(void* Object, const int& size);

  int NumberOfConnectedClients();

  // request the network to send data to the clents 
  void RequestNetworkUpdate();

  // cancels the last reqest to update. Has no effect if the network code is currently running
  void CancelNetworkUpdate();

  

  ~Network();
private:

  // network sits here waiting for client to connect runs in a seprate thread 
  void* THREAD_WaitforClients();

  // helperfunction for starting WSAStartup returns WSAStartup return value
  int StartWSA();

  // create a server running on the given socket
  int CreateServerSocket(const unsigned short &PortNumber);

  // create a client 
  int CreateClientSocket(const unsigned short &PortNumber);

  // tells the server to wait for clients will make a thread and get THEAD_Waitforclients to do the job
  void WaitforClients();

  void HandleFrameCounter();

  int AcceptClientConnection();

  void CloseActiveSocket();

  // connects to the given server ip using the active socket
  bool ConnectToServer(const char* ServerIP);

  // clears the array DataToReplicate
  void  ClearDataToReplicate();

  void GenerateRepData();

  void ClearRepData();

  void WriteFromRepData();

  

private:

  

  
#ifdef SMARTNETWORK
  // server is the data to send over the network, client is the data sent from the server
  std::vector<ReplicatedData> DataToReplicate;

  // did we receive data
  bool bHasReceivedData;
#endif
  // size of the data been sent
  int DataToRepByteSize;


  NetworkDataType* RepData;

  int FramesSinceLastNetworkUpdate;

  // role of the network for this machine 
  ConnectionType Role;

  // has as request for a network been asked for 
  bool bPendingNetworkUpdate;

  // is the network been asked to shut down
  bool bPendingShutDown;

  // is the network currently connecting 
  bool bPendingConnection;

  // are we connected 
  bool bIsConnectedToServer;

  // the main thread the network will run in after it has been set up both client and server
  DWORD NetworkThread;

  // list of connection to the network
  std::vector<DWORD> Connections;
  // the instance of this network
  static Network* Instance;
  
  // socket number this network is working on set to -1 if no sockets are been used 
  int ActiveSocket;

  // current port the network is running on
  int ActivePort;

  // constructor hidden due to network singleton 
  Network();

};


