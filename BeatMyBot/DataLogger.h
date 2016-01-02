#pragma once
#include <fstream>
class DataLogger
{
public:
  DataLogger();
  // creates the file and writes in the time and date 
  static void StartLogging(const wchar_t* Type, bool SmartNetwork);
  // writes the value with the frame time nexto it with a | between
  static void WriteTimeWithValue(float Time, int Value);
  // close the file and finish any other jobs needed
  static void StopLogging(); 

  static bool IsRunning();
  ~DataLogger();
private:
  static std::fstream TheFile;
  // is the logger ready to start logging 
  static bool bIsRunning;
};

