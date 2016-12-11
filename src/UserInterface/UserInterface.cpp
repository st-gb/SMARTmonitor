/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "UserInterface.hpp" //class UserInterface
#include <fastest_data_type.h> //fastestUnsignedDataType
#include <stdio.h> //snprintf(...)

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
    const char * formatString = "%.0fmin %.3fs pure runtime after bootup";
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