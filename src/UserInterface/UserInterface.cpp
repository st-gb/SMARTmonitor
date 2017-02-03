/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "UserInterface.hpp" //class UserInterface
#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType
#include <stdio.h> //snprintf(...)

std::string UserInterface::GetTimeAsString(const struct tm & timeOfLastSMARTvaluesUpdate)
{
  char buffer[80];
  //"%a" : Abbreviated weekday name
  // %d : Day of the month, zero-padded (01-31)
  // %b : Abbreviated month name
  // %T : ISO 8601 time format (HH:MM:SS),
  size_t strLen = strftime(buffer, 80, "%a %d %b %T", 
    & timeOfLastSMARTvaluesUpdate );
  return std::string(buffer);
}

/** This function is intended to be used by subclasses (e.g. TUI via (n)curses, 
 * wxWidgets GUI, ...) of this class.*/
void UserInterface::FormatTimeOfLastUpdate(
  long int timeStampOfRetrievalIn1ks, 
  std::string & timeFormatString)
{
  float timeOfSMARTvalueRetrievelInS;
  char formattedString[80];
  //currentTime = wxNow();
  timeOfSMARTvalueRetrievelInS = (float) timeStampOfRetrievalIn1ks / 1000.f;
  if( timeOfSMARTvalueRetrievelInS >= 3600.0 )
  {
    const fastestUnsignedDataType timeOfSMARTvalueRetrievelInH = 
      timeOfSMARTvalueRetrievelInS / 3600.0;
    const fastestUnsignedDataType timeOfSMARTvalueRetrievelInMin = 
      timeOfSMARTvalueRetrievelInS / 60 - timeOfSMARTvalueRetrievelInH * 60;
    
    timeOfSMARTvalueRetrievelInS = timeOfSMARTvalueRetrievelInS -
      timeOfSMARTvalueRetrievelInH * 3600 - timeOfSMARTvalueRetrievelInMin * 60;
    const char * formatString = "%dh%dmin%.3fs pure runtime after bootup";
    snprintf(formattedString, 80, formatString,
      timeOfSMARTvalueRetrievelInH, 
      timeOfSMARTvalueRetrievelInMin,
      timeOfSMARTvalueRetrievelInS);
  }
  else if( timeOfSMARTvalueRetrievelInS >= 60.0 )
  {
    const fastestUnsignedDataType timeOfSMARTvalueRetrievelInMin = 
      timeOfSMARTvalueRetrievelInS / 60.0;
    timeOfSMARTvalueRetrievelInS = timeOfSMARTvalueRetrievelInS -
      timeOfSMARTvalueRetrievelInMin * 60;
    const char * formatString = "%dmin %.3fs pure runtime after bootup";
    char formattedString[80];
    snprintf(formattedString, 80, formatString,
      timeOfSMARTvalueRetrievelInMin,
      timeOfSMARTvalueRetrievelInS);
  }
  else
  {
    const char * formatString = "%.3f s pure runtime after bootup";
    snprintf(formattedString, 80, formatString,
      timeOfSMARTvalueRetrievelInS);
  }
  timeFormatString = formattedString;
}