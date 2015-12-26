// Turn off the safety warning becuse i like to live dangerously
#define _CRT_SECURE_NO_WARNINGS
#include "Network.h"
#include "game.h"
#include "staticmap.h"
#include "Renderer.h"
#include "errorlogger.h"
#include "myinputs.h"
#include "dynamicobjects.h"
#include "mydrawengine.h"
#include "NodeList.h"
#include <vector>

#ifdef _DEBUG
#include "Debug.h"
#endif


class NumberKeyBoard
{
public:
  NumberKeyBoard()
    :bKeyLastPress(false)
  {
    Keyinput.push_back('\0');
    KeyInPutCharBuff = nullptr;
  }
  ~NumberKeyBoard(){ delete[] KeyInPutCharBuff; }
  void UpDateKeyBoard()
  {
    MyInputs* pInputs = MyInputs::GetInstance();
    pInputs->SampleKeyboard();
    char* KeyState = pInputs->GetKeyboardState();

    // check to see if any number keys are pressed
    for (unsigned int i = 0; i < 11; ++i)
    {
      // number 1-9
      if ((KeyState[i] & 0x80) > 0)
      {
        PushKey(48 + i - 1);
        bKeyLastPress = true;
        break;
      }
      // number0
      else if ((KeyState[DIK_0] & 0x80) > 0)
      {
        PushKey(48);
        bKeyLastPress = true;
        break;
      }
      // is . pressed
      else if ((KeyState[DIK_PERIOD] & 0x80) > 0)
      {
        PushKey(46);
        bKeyLastPress = true;
        break;
      }
      // if backspace is pressed pop last char
      else if ((KeyState[DIK_BACKSPACE] & 0x80) > 0)
      {
        PopLast();
        bKeyLastPress = true;
        break;
      }
      // no key pressed this frame
      if (i == 10)
        bKeyLastPress = false;
    }
  }

  void PushKey(const wchar_t &Key)
  {
    if (!bKeyLastPress)
    {
      // remove the null
      Keyinput.pop_back();
      Keyinput.push_back(Key);
      Keyinput.push_back('\0');
    
    
    }
  }

  void PopLast()
  {
    if (!bKeyLastPress)
    {
      if (Keyinput.size() > 1)
      {
        // pop null
        Keyinput.pop_back();
        // pop the key
        Keyinput.pop_back();
        // stick null back in
        Keyinput.push_back('\0');
      }
    }
  }

  char* GetTextAsChar()
  {
    if (KeyInPutCharBuff) { delete[] KeyInPutCharBuff; KeyInPutCharBuff = nullptr; };

    KeyInPutCharBuff = new char [GetNumOfLetter()];

    wcstombs(KeyInPutCharBuff, &Keyinput[0], GetNumOfLetter());

    return KeyInPutCharBuff;
  }

 
  void FlushKeyBoard()
  {
    Keyinput.clear();
    Keyinput.push_back('\0');
  }

  wchar_t* GetText()
  {
    return &Keyinput[0];
  }
  bool TextIsEmpty()
  {
    return Keyinput.empty();
  }
  int GetNumOfLetter()
  {
    return Keyinput.size();
  }
private:
  char* KeyInPutCharBuff;
  bool bKeyLastPress;
  std::vector<wchar_t> Keyinput;
};



Game* Game::pInst = NULL;

Game* Game::GetInstance()
{
	if(pInst == NULL)
	{
		pInst= new Game();
	}

	return pInst;
}


void Game::Release()		// Static
{
	if(pInst)
	{
		delete pInst;
		pInst = NULL;
	}
}


Game::Game()
{
	InitialiseScript();
}

Game::~Game()
{
	
}

void Game::ServerAdressMenu(Renderer* pTheRenderer)
{
  static bool DisplayConnectError = false;
  MyDrawEngine* pDrawEngine = MyDrawEngine::GetInstance();
  Vector2D ScreenCenter = Vector2D(pDrawEngine->GetScreenWidth() / 2.0f, pDrawEngine->GetScreenHeight() / 2.0f);

  if (DisplayConnectError)
  {
    pTheRenderer->DrawTextAt(ScreenCenter + Vector2D(0, -500), L"Failed to connect to server");
  }

  static NumberKeyBoard IpAdressKeyboard;
  IpAdressKeyboard.UpDateKeyBoard();
  pTheRenderer->DrawTextAt(ScreenCenter + Vector2D(-150,0), L"Enter Server Address and Press 'Enter' To Connect");
  if (!IpAdressKeyboard.TextIsEmpty())
    pTheRenderer->DrawTextAt(ScreenCenter + Vector2D(0.0f,50.0f), IpAdressKeyboard.GetText());

  if (MyInputs::GetInstance()->KeyPressed(DIK_RETURN))
  {
    DisplayConnectError = false;
    if (Network::GetInstance()->JoinGame(550, IpAdressKeyboard.GetTextAsChar()))
    {
      CurrentLevelState = LevelState::Playing;
    }
    else
    {
      DisplayConnectError = true;
      IpAdressKeyboard.FlushKeyBoard();
    }
  }
  

}

void Game::DrawMenu(Renderer* pTheRenderer)
{
  MyDrawEngine* DrawEngine = MyDrawEngine::GetInstance();
  int ScreenY =  DrawEngine->GetScreenHeight();
  int ScreenX =  DrawEngine->GetScreenWidth();
  pTheRenderer->DrawTextAt(Vector2D(ScreenX / 2.0f, ScreenY / 2.0f), L"Press 'H' host a game");
  pTheRenderer->DrawTextAt(Vector2D(ScreenX / 2.0f, ScreenY / 2 + 50.0f), L"Press 'J' Join a game");

  if (NodeList::GetInstance()->IsMapLoading())
  {
    pTheRenderer->DrawTextAt(Vector2D(ScreenX / 2.0f, 150.f), L"Loading Map");
  }

  MyInputs* pInputs = MyInputs::GetInstance();
  pInputs->SampleKeyboard();
  if (pInputs->KeyPressed(DIK_H))
  {

    if (Network::GetInstance()->HostGame(550))
    {
      CurrentLevelState = Playing;
    }
  }
  else if (pInputs->KeyPressed(DIK_J))
  {
    CurrentLevelState = LevelState::MenuIPPick;
  }
}


ErrorType Game::Start()
{
  CurrentLevelState = Menu;

  Network::Start();
  NodeList::Start();
  NodeList::GetInstance()->GenerateNodeList();

	m_timer.mark();
	m_timer.mark();
	m_State = RUNNING;

	return SUCCESS;
}

ErrorType Game::RunInterface()
{
	ErrorType answer=SUCCESS;

	Renderer* pTheRenderer = Renderer::GetInstance();

	// Handle mouse
	static int mouseX = 0;
	static int mouseY = 0;
	static float screenCentreX = 0.0f;
	static float screenCentreY = 0.0f;
	static float zoom = 0.5f;
  if (CurrentLevelState == LevelState::Playing)
  {
	  MyInputs* pInputs = MyInputs::GetInstance();
	  pInputs->SampleMouse();

	  mouseX += int(pInputs->GetMouseDX()/zoom);
	  mouseY -= int(pInputs->GetMouseDY()/zoom);

	  if(mouseX<screenCentreX-200/zoom)
		  screenCentreX-=((screenCentreX-200/zoom)-mouseX)/(50*zoom);
	  if(mouseX>screenCentreX+200/zoom)
		  screenCentreX+=(mouseX-(screenCentreX+200/zoom))/(50*zoom);
	  if(mouseY>screenCentreY+200/zoom)
      screenCentreY += (mouseY - (screenCentreY+200/zoom)) / (50 * zoom);
	  if(mouseY<screenCentreY-200/zoom)
      screenCentreY -= ((screenCentreY-200/zoom)-mouseY) / (50 * zoom);

  

  
      // Handle viewport
      pTheRenderer->SetViewScale(zoom);
      pTheRenderer->SetViewCentre(Vector2D(screenCentreX, screenCentreY));

      pTheRenderer->DrawMousePointer(Vector2D(float(mouseX), float(mouseY)));



    /*
    if (!Keyinput.empty())
    {
      Keyinput.push_back('\0');
      pTheRenderer->DrawTextAt(Vector2D(100, 100), &Keyinput[0]);
      Keyinput.pop_back();
    }
    */


    

    if (pInputs->GetMouseDZ() > 0)
      zoom *= 1.05f;
    if (pInputs->GetMouseDZ() < 0)
      zoom /= 1.05f;

    // Display the scores
    for (int i = 0; i < NUMTEAMS; i++)
    {
      int score = (DynamicObjects::GetInstance()->GetScore(i));
      Vector2D pos(10.0f, i*25.0f + 10.0f);
      pTheRenderer->DrawTextAt(pos, L"Team ");
      pos.set(75.0f, i*25.0f + 10.0f);
      pTheRenderer->DrawNumberAt(pos, i + 1);
      pos.set(105.0f, i*25.0f + 10.0f);
      pTheRenderer->DrawNumberAt(pos, score);
    }

    ConnectionType Role = Network::GetInstance()->GetConnectionType();

    // we only care about how many clents are connected to if we are the server 
    if (Role == ConnectionType::SERVER)
    {
      pTheRenderer->DrawTextAt(Vector2D(10.0f, 60.0f), L"Number Of Connected Clients :");
      wchar_t buf[10];
      _itow_s(Network::GetInstance()->NumberOfConnectedClients(), buf, 10);
      pTheRenderer->DrawTextAt(Vector2D(275.0f, 60.0f), buf);
                        
       
      #ifdef SMARTNETWORK 
      pTheRenderer->DrawTextAt(Vector2D(10.0f, 85.0f), L"Smart Network");
      #else 
      pTheRenderer->DrawTextAt(Vector2D(10.0f, 85.0f), L"Simple Network" );
      #endif);
    }

    
    

    std::wstring NetworkRole = L"Unconnected";
    if (Role != ConnectionType::UNCONNECTED)
    {
      NetworkRole = Network::GetInstance()->IsServer() ? L"Server" : L"Client";
    }

    float ScreenXMiddle = MyDrawEngine::GetInstance()->GetScreenWidth() / 2.0f;
    pTheRenderer->DrawTextAt(Vector2D(ScreenXMiddle, 10), NetworkRole.c_str());
  }
  else if (CurrentLevelState == LevelState::MenuIPPick)
  {
    ServerAdressMenu(pTheRenderer);
  }
  else
  {
    DrawMenu(pTheRenderer);
  }

	return answer;
}

ErrorType Game::Update()
{
	static float DeltaTimeAdder = 0.0f;
	static int FrameCounter = 0;
	static int Currentfps = 20;
	m_timer.mark();
	// Cap the frame time
	if(m_timer.m_fFrameTime>0.05f)
		m_timer.m_fFrameTime=0.05f;
	ErrorType answer=SUCCESS;

	Renderer* pTheRenderer = Renderer::GetInstance();

	DeltaTimeAdder += m_timer.m_fFrameTime;
	FrameCounter++;
	if (DeltaTimeAdder >= 1.0f)
	{
		DeltaTimeAdder = 0;
		Currentfps = FrameCounter;
		FrameCounter = 0;
	}
	wchar_t Buff[10];
	_itow_s(Currentfps, Buff, 10);
	pTheRenderer->DrawTextAt(Vector2D(MyDrawEngine::GetInstance()->GetScreenWidth() * 0.95f, 0), Buff);
	
  if (CurrentLevelState == Playing)
  {
    Network* pNetwork = Network::GetInstance();
    // If not paused or minimised
    if (m_State == RUNNING)
    {
#ifdef SMARTNETWORK
      if (pNetwork->IsClient() && pNetwork->HasReceivedData())
      {
        // if we got data from the server read it into rep data
        pNetwork->WriteRepData();
      }
      else if (pNetwork->IsServer())
#endif
      {
        // Update Dynamic objects
        DynamicObjects::GetInstance()->Update(m_timer.m_fFrameTime);
      }

    }

    // Render
    if (pTheRenderer->GetIsValid() == false)
    {
      m_State = MINIMISED;			// Or some other reason why invalid
    }
    else
    {
      if (m_State == MINIMISED)
        m_State = PAUSED;			// No longer minimised. Return to pause state.
      if (StaticMap::GetInstance()->Render() == FAILURE)
      {
        ErrorLogger::Writeln(L"Static map failed to render. Terminating.");
        answer = FAILURE;
      }
      if (DynamicObjects::GetInstance()->Render() == FAILURE)
      {
        ErrorLogger::Writeln(L"Dynamic objects failed to render. Terminating.");
        answer = FAILURE;
      }

      if (pTheRenderer->DrawFX(m_timer.m_fFrameTime) == FAILURE)
      {
        ErrorLogger::Writeln(L"Render failed to draw FX.");
        // Continue. Non-critical error.
      }

      
      if (pNetwork)
      {
        // we are a client but no longer have an connection to the server
        if (pNetwork->IsClient() && !pNetwork->IsConnectedToServer())
        {
          // swap to become an server 
          if (!pNetwork->SwapFromClientToServer())
            // failed to swap from client to server
          {
            ErrorLogger::Writeln(L"Failed to swap from client to server Terminating.");
            //TODO Throw Error Screen
            answer = FAILURE;
          }
        }
#ifdef SMARTNETWORK
        pNetwork->SmartHandleNetworkRep();
#else 
        // handle replicating the dynamic object over the network.. will send if server will receive if client
        pNetwork->HandleNetWorkReplication(DynamicObjects::GetInstance(), sizeof(DynamicObjects));
#endif
      }
    }
    
#ifdef _DEBUG
	MyInputs::GetInstance()->SampleKeyboard();
	static bool bLastPress = false;
	if (MyInputs::GetInstance()->KeyPressed(DIK_D) && !bLastPress)
	{
		bShowDebug = !bShowDebug;
		bLastPress = true;
	}
	else if (!MyInputs::GetInstance()->KeyPressed(DIK_D))
	{
		bLastPress = false;
	}
	
	
	if (bShowDebug)
	{
		NodeList::GetInstance()->DrawAllNodes();
		NodeList::GetInstance()->DrawAllEdges();
	}
#endif


    
  } // End CurrentLevelState == Playing

  if (pTheRenderer->EndScene() == FAILURE)
  {
    ErrorLogger::Writeln(L"Render failed to end scene. Terminating.");
    answer = FAILURE;
  }
    


    if (RunInterface() == FAILURE)
    {
      ErrorLogger::Writeln(L"Interface reports failure.");
      // Non-critical error
    }
  

	return answer;
}

ErrorType Game::End()
{
  
	Renderer::Release();
	StaticMap::Release();
	DynamicObjects::Release();
  NodeList::ShutDown();
  Network::ShutDown();
	return SUCCESS;
}


ErrorType Game::InitialiseScript()
{
	// Hard code for now
	StaticMap* pMap = StaticMap::GetInstance();

	// Add some rectangles
	Rectangle2D r;
	r.PlaceAt(1000, -1000, 700, -700);
	pMap->AddBlock(r);
  r.PlaceAt(-700, 700, -1000, 1000);
  pMap->AddBlock(r);

  r.PlaceAt(1000, -700, 900, 0);
  pMap->AddBlock(r);
  r.PlaceAt(-900, 0, -1000, 700);
  pMap->AddBlock(r);

  r.PlaceAt(-700, -1000, -1400, -700);
  pMap->AddBlock(r);
  r.PlaceAt(1400, 700, 700, 1000);
  pMap->AddBlock(r);

  r.PlaceAt(300, -1000, -300, -850);
  pMap->AddBlock(r);
  r.PlaceAt(300, 850, -300, 1000);
  pMap->AddBlock(r);

  r.PlaceAt(500, -450, 200, -150);
  pMap->AddBlock(r);
  r.PlaceAt(-200, 150, -500, 450);
  pMap->AddBlock(r);

  r.PlaceAt(-200, -500, -750, -400);
  pMap->AddBlock(r);
  r.PlaceAt(750, 400, 200, 500);
  pMap->AddBlock(r);

  r.PlaceAt(700, -200, 600, 200);
  pMap->AddBlock(r);
  r.PlaceAt(-600, -200, -700, 200);
  pMap->AddBlock(r);


	// Add two spawn points
	pMap->AddSpawnPoint(Vector2D(-1250, 0));
	pMap->AddSpawnPoint(Vector2D(1250, 00));

  // Add four supply points
  pMap->AddSupplyPoint(Vector2D(900, 500));
  pMap->AddSupplyPoint(Vector2D(900, -500));
  pMap->AddSupplyPoint(Vector2D(-900, 500));
  pMap->AddSupplyPoint(Vector2D(-900, -500));

  
    // Place the domination points
    DynamicObjects* pDynObjects = DynamicObjects::GetInstance();

    pDynObjects->PlaceDominationPoint(Vector2D(-200, -850));
    pDynObjects->PlaceDominationPoint(Vector2D(0, 0));
    pDynObjects->PlaceDominationPoint(Vector2D(200, 850));

    pDynObjects->Initialise();
  
	return SUCCESS;
}