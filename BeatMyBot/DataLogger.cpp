#include "DataLogger.h"
#include <ctime>
#include <string>


std::fstream DataLogger::TheFile;
bool DataLogger::bIsRunning = false;
DataLogger::DataLogger()
{
  
}

#define CastToString(Thing) std::to_string(CurrentTime.Thing)
// creates the file and writes in the time and date 
void DataLogger::StartLogging(const wchar_t* Type,bool SmartNetwork)
{
  // no point starting logging again
  if (bIsRunning) return;
  // make the file name the date and the type 

  // current time
  time_t t = time(0);
  tm CurrentTime;
  localtime_s(&CurrentTime, &t);
  std::wstring ws(Type);
  std::string Buffer = SmartNetwork ? "Smart-" : "Simple-";
  Buffer += std::string(ws.begin(), ws.end()) + '-';
  Buffer += CastToString(tm_mday)  +'-';
  Buffer += CastToString(tm_mon + 1) + '-';
  Buffer += CastToString(tm_year + 1900) + "--";
  Buffer += CastToString(tm_hour) + '-';
  Buffer += CastToString(tm_min) + '-';
 // Buffer += CastToString(tm_sec);
  TheFile.open(("Logs//" + Buffer), std::fstream::in | std::fstream::out | std::fstream::app);

  // set the logger to running if the file is open 
  bIsRunning = TheFile.is_open();
  
  
   TheFile << Buffer << "\n\n";
   TheFile << "  FrameTime" << "\t\t\t\t\t|\t\t\t" << "PacketSize" << '\n';
   TheFile << "  ------------------------------------------------" << std::endl;
  bIsRunning = true; 
}

// writes the value with the frame time nexto it with a | between
void DataLogger::WriteTimeWithValue(float Time, int Value)
{
  if (bIsRunning)
  {
    TheFile << "  ";
    TheFile.seekp(0);
    TheFile << Time << ".ms"   "\t"; 
    TheFile << "\t\t\t|\t\t\t";
    TheFile << Value << ".bytes" << '\n';
  }
}
// close the file and finish any other jobs needed
void DataLogger::StopLogging()
{
  TheFile.close();
}

bool DataLogger::IsRunning()
{
  return bIsRunning;
}

DataLogger::~DataLogger()
{
  // close the file incase of early shutdown
  TheFile.close();
  bIsRunning = false;
}
