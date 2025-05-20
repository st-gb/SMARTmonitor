///@author:Stefan Gebauer(TU Berlin matriculation number 361095)

///C,C++ standard library header files:
#include <stdio.h>///snprintf(...)
#include <string.h>///strcat(...)
#include <sstream>///class std::ostringstream
#include <time.h>///strftime(...)


///Stefan Gebauer's(TU Berlin mat.#361095) ~"common_sourcecode"repository files:
 #include <hardware/CPU/fastest_data_type.h>///TU_Bln361095::CPU::faststUint
 /**TU_Bln361095timeSecsPerDay,TU_Bln361095timeSecsPerHour,
  * TU_Bln361095timeSecsPerMin */
 #include <time/timeConstants.h>

///_This_ project's (repository) header files:
 #include "UserInterface.hpp"///class TU_Bln361095::SMARTmon::UserInterface

TU_Bln361095SMARTmonNmSpcBgn

/**numChars(abbreviated weekday)=3 + numChars(day)=2 +
 * numChars(abbreviated month)=3 + numChars("HH:MM:SS")=8 + 3 space chars = 19
 * + character string terminating '\0' character = 20 */
#define numCharsForDt_andTm 25

std::string UserInterface::GetTimeAsString(const struct tm & time)
{
  char buffer[numCharsForDt_andTm];
  /**http://pubs.opengroup.org/onlinepubs/009695399/functions/strftime.html ,
   * section "RETURN VALUE" :
   * "If the total number of resulting bytes including the terminating null
   * byte is not more than maxsize, strftime() shall return the number of bytes
   * placed into the array pointed to by s, not including the terminating null
   * byte.Otherwise, 0 shall be returned and the contents of the array are
   * unspecified." */
  size_t numBwoutTerm0written = 
    strftime(buffer, numCharsForDt_andTm,
     /**http://pubs.opengroup.org/onlinepubs/009695399/functions/strftime.html :
       * %a : Abbreviated weekday name
       * %d : Day of the month, zero-padded (01-31)
       * %b : Abbreviated month name
       * %T : ISO 8601 time format (HH:MM:SS) */
      "%a %d %b %T",
      & time);
  if(numBwoutTerm0written == 0)
    return std::string("");
  return std::string(buffer);
}

/**@param timeInMilliSecs should be the uptime (time since the Operating System
 *  was started. 1 day has 86 400 000 milliseconds. With 32 bits (max. integer:
 *  = 2^32 = 4 294 967 296
 *  4 294 967 296 / 86 400 000 = 49,7102696180 period 5 => after ca. 49,7 days
 *  the value wraps to 0 */
typedef uint64_t uptimeInMilliSecsType;

inline void UserInterface::FmtViaOSS(const uptimeInMilliSecsType timeInMilliSecs,
  std::string & str)
{
  if(timeInMilliSecs)
  {
  TU_Bln361095::CPU::faststUint timeInSecs = timeInMilliSecs / 1000;
  std::ostringstream oss;
  ///"number of" is not ambiguous (instead of "count"): only 1 meaning
  TU_Bln361095::CPU::faststUint numberOfDays = timeInSecs/
    TU_Bln361095timeSecsPerDay;
  uptimeInMilliSecsType timeInMilliSecsMinusPrintedTime = timeInMilliSecs;
  ///Philosophy:show as few characters as possible:only time attribute if > 0.
  ///Quality assurance: compare each attribute with Linux' "uptime" command.
  if(numberOfDays > 0)
  {
    oss << numberOfDays << "d";
    timeInMilliSecsMinusPrintedTime -= numberOfDays * 
      TU_Bln361095timeSecsPerDay * 1000;
  }
  const TU_Bln361095::CPU::faststUint timeInSecsMinusDays = timeInSecs -
    (numberOfDays * TU_Bln361095timeSecsPerDay);
  TU_Bln361095::CPU::faststUint timeOfDayNumberOfHoursPart =
    timeInSecsMinusDays / TU_Bln361095timeSecsPerHour;
  if(timeOfDayNumberOfHoursPart > 0)
  {
    oss << timeOfDayNumberOfHoursPart << "h";
    timeInMilliSecsMinusPrintedTime -= timeOfDayNumberOfHoursPart *
      TU_Bln361095timeSecsPerHour * 1000;
  }
//  const TU_Bln361095::CPU::faststUint timeInSecsMinusHours =
//    timeInSecsMinusDays - (timeOfDayNumberOfHoursPart *
//    TU_Bln361095timeSecsPerHour);
//  TU_Bln361095::CPU::faststUint timeOfDayNumberOfMinutesPart =
//    timeInSecsMinusHours/TU_Bln361095timeSecsPerMin;
  const TU_Bln361095::CPU::faststUint timeOfDayMinutesPart = //timeInMilliSecs/
    timeInMilliSecsMinusPrintedTime /
    /** number of milliseconds divided by 1000 = number of seconds
     * number of seconds divided by 1000 = number of minutes */
    60000
    ///
    //%60
    ;
  //TODO minutes part in time of day differs by 26 from "uptime" command (after
  // 9 days)
  if(timeOfDayMinutesPart > 0)
  {
    oss << timeOfDayMinutesPart << "min";
    timeInMilliSecsMinusPrintedTime -= timeOfDayMinutesPart * 60 * 1000;
  }
  /**"timeOfDaySecondsPart" is more precise than "number of seconds" because
   * number of seconds can also mean "whole time in number of seconds".*/
  const TU_Bln361095::CPU::faststUint timeOfDaySecondsPart = timeInSecs % 60;
  if(timeOfDaySecondsPart != 0)
    oss << timeOfDaySecondsPart << "s";
  const TU_Bln361095::CPU::faststUint timeOfDayMilliSecsPart = timeInMilliSecs
    % 1000;
  if(timeOfDayMilliSecsPart != 0)
    oss << timeOfDayMilliSecsPart << "ms";
  str = oss.str();
  }
  else
    str = "unknown";
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
  float fTimeInSecs = (float) timeInMilliSecs / 1000.f;
  TU_Bln361095::CPU::faststUint timeInSecs = (TU_Bln361095::CPU::faststUint)
    fTimeInSecs;
  
  const TU_Bln361095::CPU::faststUint numberOfDays = timeInSecs/
    TU_Bln361095timeSecsPerDay;
  char formatString[maxCharsForFormatStr];
  const void * snprintfPtrs[numTimeAttrs];
  TU_Bln361095::CPU::faststUint fmtStrIdx = 0;
  ///Quality assurance:compare with Linux' "uptime" command.
  ///An if-then-else with snprinf() for each number of time attributes?
  if(numberOfDays > 0)
  {
    /**Concatenate first parameter character string with second parameter
     * character string.*/
    strcat(formatString, "%dd");
    snprintfPtrs[fmtStrIdx++] = & numberOfDays;
    timeInSecs -= numberOfDays * TU_Bln361095timeSecsPerDay;
  }
  const TU_Bln361095::CPU::faststUint numberOfHours = timeInSecs/
    TU_Bln361095timeSecsPerHour;
  if(timeInSecs >= TU_Bln361095timeSecsPerHour)
  {
    strcat(formatString, "%dh");
    snprintfPtrs[fmtStrIdx++] = & numberOfHours;
    timeInSecs -= numberOfHours * TU_Bln361095timeSecsPerHour;
  }
  const TU_Bln361095::CPU::faststUint timeOfDayNumberOfMinutesPart =
    timeInSecs / TU_Bln361095timeSecsPerMin;
  if(timeInSecs >= TU_Bln361095timeSecsPerMin)
  {
    strcat(formatString, "%dmin");
    snprintfPtrs[fmtStrIdx++] = & timeOfDayNumberOfMinutesPart;
    timeInSecs -= timeOfDayNumberOfMinutesPart * TU_Bln361095timeSecsPerMin;
  }
  
  strcat(formatString, "%ds%dms");
  snprintfPtrs[fmtStrIdx++] = & timeInSecs;
  const TU_Bln361095::CPU::faststUint timeOfDayNumberOfMilliSecs =
    timeInMilliSecs % 1000;
  snprintfPtrs[fmtStrIdx] = & timeOfDayNumberOfMilliSecs;
  
  if(0)
  {
    strcat(formatString, "%.3fs");
    const float fSecsPart = timeInSecs + float (timeInMilliSecs % 1000) / 1000.f;
    snprintfPtrs[fmtStrIdx] = & fSecsPart;
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
 * wxWidgets GUI, ...) of this class.
 * Used to format the uptime of last update und time unit range*/
void UserInterface::FormatTime(
  const uint64_t timeInMs, 
  std::string & timeFormatString)
{
  FmtViaOSS(timeInMs, timeFormatString);
//  timeFormatString += " uptime";///" pure runtime after bootup"
}

TU_Bln361095SMARTmonNmSpcEnd
