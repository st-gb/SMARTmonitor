/* Author:Stefan Gebauer */
#include "UserInterface.hpp" //class UserInterface
#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType
#include <stdio.h> //snprintf(...)
#include <string.h>///strcat(...)
#include <sstream>///class std::ostringstream

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

///86400s=60s/min*60min/h*24h/d=3600s/h*24h=1 day
#define sPerDay 86400
#define sPerH 3600
#define sPerMin 60

inline void FmtViaOSS(const unsigned long timeInMs, std::string & str)
{
  fastestUnsignedDataType timeInS = timeInMs / 1000;
  std::ostringstream oss;
  fastestUnsignedDataType days = timeInS/sPerDay;
  ///Philosophy:show as few characters as possible:only time attribute if > 0.
  ///Quality assurance: compare each attribute with Linux' "uptime" command.
  if(days > 0)
  {
    oss << days << "d";
  }
  const fastestUnsignedDataType timeInSminusDays = timeInS - (days * sPerDay);
  fastestUnsignedDataType hours = timeInSminusDays/sPerH;
  if(hours > 0)
  {
    oss << hours << "h";
  }
//  const fastestUnsignedDataType timeInSminusH = timeInSminusDays - (hours * sPerH);
//  fastestUnsignedDataType min = timeInSminusH/sPerMin;
  fastestUnsignedDataType min = timeInMs/60000%60;
  if(min > 0)//TODO min differs by 26 from "uptime" command (after 9 days)
  {
    oss << min << "min";
  }
  oss << (timeInS % 60) << "s" << (timeInMs % 1000) << "ms";
  str = oss.str();
}

/**"seconds" in struct "timeval" has type "long int": 2^64=1,8447×10^19=
* 2,135069444×10^14 days (2^64/86400)
* ->  15 digits
* max. 6 chars for time units: "d h min s"
* max  9 chars:max.999 ms max.60 s/min max.60 min/h max.24h/d,
*      7 chars for literal " uptime"
*  -----
* max.38 chars incl. ("\0") */
#define maxCharsForFormattedStr 40
/** 3 characters for "%dd" (days)
 *  3 characters for "%dh" (hours)
 *  5 characters for "%dmin" (minutes)
 *  7 characters for "%ds%dms" (seconds, milliseconds)
 *  1 character for "\0"
 * ----
 * max. 19 characters */
#define maxCharsForFormatStr 20

///5 time attribute:days,hours,minutes,seconds,milliseconds
#define numTimeAttrs 5

void FmtVia_snprintf(const unsigned long timeInMs, std::string & stdstrTimeFormat)
{
  char formattedString[maxCharsForFormattedStr];
  float fTimeInS = (float) timeInMs / 1000.f;
  fastestUnsignedDataType timeInS = (fastestUnsignedDataType) fTimeInS;
  
  const fastestUnsignedDataType days = timeInS/sPerDay;
  char formatString[maxCharsForFormatStr];
  const void * snprintfPtrs[numTimeAttrs];
  fastestUnsignedDataType fmtStrIdx = 0;
  ///Quality assurance:compare with Linux' "uptime" command.
  ///An if-then-else with snprinf() for each number of time attributes?
  if(days > 0)
  {
    strcat(formatString, "%dd");
    snprintfPtrs[fmtStrIdx++] = & days;
  }
  timeInS -= days * sPerDay;
  const fastestUnsignedDataType hours = timeInS/sPerH;
  if(timeInS >= sPerH)
  {
    strcat(formatString, "%dh");
    snprintfPtrs[fmtStrIdx++] = & hours;
  }
  timeInS -= hours * sPerH;
  const fastestUnsignedDataType min = timeInS / sPerMin;
  if(timeInS >= sPerMin)
  {
    strcat(formatString, "%dmin");
    snprintfPtrs[fmtStrIdx++] = & min;
  }
  timeInS -= min * sPerMin;
  
  strcat(formatString, "%ds%dms");
  snprintfPtrs[fmtStrIdx++] = & timeInS;
  const fastestUnsignedDataType ms = timeInMs % 1000;
  snprintfPtrs[fmtStrIdx] = & ms;
  
  if(0){
  strcat(formatString, "%.3fs");
  const float fSec = timeInS + float (timeInMs % 1000) / 1000.f;
  snprintfPtrs[fmtStrIdx] = & fSec;
  }
  snprintf(formattedString, maxCharsForFormattedStr, formatString,
    snprintfPtrs[0],
    snprintfPtrs[1],
    snprintfPtrs[2],
    snprintfPtrs[3],
    snprintfPtrs[4]);
  stdstrTimeFormat = formattedString;
}

/** This function is intended to be used by subclasses (e.g. TUI via (n)curses, 
 * wxWidgets GUI, ...) of this class.*/
void UserInterface::FormatTimeOfLastUpdate(
  long int timeStampOfRetrievalInMs, 
  std::string & timeFormatString)
{
  FmtViaOSS(timeStampOfRetrievalInMs, timeFormatString);
//  timeFormatString += " uptime";///" pure runtime after bootup"
}
