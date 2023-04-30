/** File:   SMARTvalueFormatter.cpp
 * Author: Stefan Gebauer, M.Sc.Comp.Sc./Informatik (TU Berlin)
 * Created on 2. Januar 2017, 23:13 */

/** Files from Stefan Gebauer's common_sourcecode repository: */
///convertToStdString(...)
#include <dataType/charStr/convertFromAndToStdString.hpp>
#include <hardware/dataCarrier/SMARTattributeNames.h>///enum SMARTattributeNames
#include <preprocessor_macros/logging_preprocessor_macros.h>///LOGN[...](...)

#include "SMARTvalueFormatter.hpp"
#include <UserInterface/UserInterface.hpp>///UserInterface::FormatTime(...)
#include <attributes/ModelAndFirmware.hpp>///class ModelAndFirmware

SMARTvalueFormatter::SMARTvalueFormatter() {
}

SMARTvalueFormatter::SMARTvalueFormatter(const SMARTvalueFormatter& orig) {
}

SMARTvalueFormatter::~SMARTvalueFormatter() {
}

/** See https://en.wikipedia.org/wiki/Unit_prefix */
/** 6 B for a SMART value->max. value is 2^(6*8)=2^48~=2,8*10^14
 * but together with a unit the value may get larger: e.g. 1 MB as unit for
 * "total data written" and the max value for the 6 B S.M.A.R.T. value.
 * So use max value of uint64 for the max. unit prefix.
 *  2^(8*8)=2^64~=1,8*10^19 -> max. SI unit prefix is E[xa] with 10^18 as
 *  the max. value can be output as ~ 18E[...] */
char unitPrefixes [] = {' ', 'k', 'M', 'G', 'T', 'P', 'E'};

char calcUnitPrefixFactorAndRemainder(
  uint64_t & remainder, 
  uint64_t & unitPrefixFactor)
{
  LOGN_DEBUG("begin--number:" << remainder)
  uint64_t remainderCopy = remainder;
  uint64_t one1kPower = 1;
  fastestUnsignedDataType One_kExp = 0;
  while (remainderCopy > 1000ULL) {
    remainderCopy /= 1000ULL;
    one1kPower *= 1000ULL;
    One_kExp++;
  }
  unsigned One_kExpCopy = One_kExp;
  remainderCopy = 1ULL;
  while (One_kExpCopy > 0) {
    remainderCopy *= 1000ULL;
    One_kExpCopy--;
  }
  if (remainderCopy > 1) {
    unitPrefixFactor = remainder / /*remainderCopy*/ one1kPower;
    remainder = remainder - unitPrefixFactor * remainderCopy;
  } else {
    unitPrefixFactor = remainder;
    remainder = 0;
  }
  LOGN_DEBUG("prefix factor:" << unitPrefixFactor
          << " remainder:" << remainder << " 1k's exponent:" << One_kExp)
  return unitPrefixes[One_kExp];
}

std::string SMARTvalueFormatter::GetDegCfromCurr(
  const uint64_t & SMARTrawValue)
{
  std::ostringstream stdoss;
  stdoss << (SMARTrawValue & 0xFFFF) << "°C";  
  return stdoss.str();
}

std::string SMARTvalueFormatter::GetDegCfromCurrMinMax(
  const uint64_t & SMARTrawValue)
{
  std::ostringstream stdoss;
  const fastestUnsignedDataType maxVal = (SMARTrawValue >> 32) & 0xFFFF;
  const fastestUnsignedDataType minVal = (SMARTrawValue >> 16) & 0xFFFF;

  stdoss << (SMARTrawValue & 0xFFFF) << "°C";
  /** Does not work for a model:ST9500420AS firmware:0003SDM1 (serial:5VJ1WXTF) :
   * highmost byte is 12, lowmost byte: °C, other bytes 0. So check if both
   * values are non-0. */
  if(minVal && maxVal)
    stdoss << "["
      << minVal << "°C..."
      << maxVal << "°C]";
  return stdoss.str();
}

std::string SMARTvalueFormatter::TempDiffOrAirflowTemp_DegCforCurrMinMax(
  const uint64_t & SMARTrawValue)
{
  std::ostringstream stdoss;
  /** model:ST9500420AS firmware:0003SDM1 (serial:5VJ1WXTF) :
   * lowmost byte: current temperature in °C
   * 3rd lowmost byte: min. value in °C
   * 4th lowmost byte: max. value in °C */
  const fastestUnsignedDataType maxVal = (SMARTrawValue >> 24) & 0xFF;
  const fastestUnsignedDataType minVal = (SMARTrawValue >> 16) & 0xFF;

  stdoss << (SMARTrawValue & 0xFF) << "°C";
  /** Does not work for a model:Samsung SSD 860 EVO M.2 500GB firmware:RVT24B6Q
   *  (serial:S5GCNJ0N506884T) : lowmost byte is current temperature, all other
   *  bytes 0. So check if both values are non-0. */
  if(minVal && maxVal){
    stdoss << "["
      << minVal << "°C..."
      << maxVal << "°C]";
  }
  return stdoss.str();
}

std::string SMARTvalueFormatter::GetDegCfrom_mK(
  const uint64_t & SMARTrawValue)
{
  std::ostringstream stdoss;
  stdoss.precision(1);
  //stdoss << std::fixed << (float) SMARTrawValue / 3600000 << " h";
  stdoss << std::fixed << (float) SMARTrawValue/1000.0f - 273.1f << "°C";
  return stdoss.str();
}

std::string SMARTvalueFormatter::GetTimeFrom_h(const uint64_t & SMARTrawValue)
{
  std::ostringstream stdoss;
  const fastestUnsignedDataType numDays = SMARTrawValue / 24;
  stdoss << std::fixed << numDays << "d" << SMARTrawValue % 24 << "h";
  return stdoss.str();
}

std::string SMARTvalueFormatter::GetTimeFrom_ms(
  const uint64_t & SMARTrawValue)
{
  std::ostringstream stdoss;
  stdoss.precision(1);///1 digit after comma
  //stdoss << std::fixed << (float) SMARTrawValue / 3600000 << " h";
  ///1h=60min,1min=60s=>1h=3600s, 1s=1000ms <=> 1h=3,600,000 ms
  const float hours = (float) SMARTrawValue / 3600000.0f;
  const unsigned minRemainder = SMARTrawValue % 60000;
  const fastestUnsignedDataType numDays = hours / 24;
  //TODO when min is "0" no output is neccessary?
  const float hoursRemainder = hours - (numDays * 24);
  stdoss << std::fixed << numDays << "d" << hoursRemainder << "h" << 
    minRemainder << "min";
  return stdoss.str();
}

std::string SMARTvalueFormatter::GetNumberWithSIprefix(const uint64_t & SMARTrawVal)
{
  uint64_t remainder = SMARTrawVal;
  uint64_t unitPrefixFactor = SMARTrawVal;
  char unitPrefixChar;
  std::ostringstream stdoss;
  do {
    unitPrefixChar = calcUnitPrefixFactorAndRemainder(remainder, unitPrefixFactor);
    if( unitPrefixChar == ' ' )
      stdoss << unitPrefixFactor;
    else
      stdoss << unitPrefixFactor << unitPrefixChar;
  } while (unitPrefixChar != ' ');
  return stdoss.str();
}

/** Make this method into a distinct class So it can be used by tests with
 *  only a little payload (code bloat)
 * \param SMARTval may be raw value or real (ca.) value */
std::string SMARTvalueFormatter::FormatHumanReadable(
  const fastestUnsignedDataType SMARTattributeID,
  const uint64_t & SMARTval,
  const bool unitKnown,
  const ModelAndFirmware * p_modelAndFirmware)
{
  switch (SMARTattributeID)
  {
    /**https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes
    * "The raw value of this attribute shows total count of hours (or minutes,
    * or seconds, depending on manufacturer) in power-on state."*/
    case TU_Bln361095::dataCarrier::SMART::Attr::PowerOnTime :
    case TU_Bln361095::dataCarrier::SMART::Attr::HeadFlyingHours:{
    //TODO could do a diff of OS uptime (GetTickCount(...) under MS Windows)
     // and values to determine the time unit.
//      return GetTimeFrom_ms(SMARTrawVal);
      std::string timeFmt;
      //return GetTimeFrom_h(SMARTrawVal);
      UserInterface::FormatTime(SMARTval, timeFmt);
      return timeFmt;
    }
    case TU_Bln361095::dataCarrier::SMART::Attr::TempDiffOrAirflowTemp:
//     ///If temp. in Celsius: and 100-temp. °C: maximum is 100-(-273)=373
//     if(SMARTrawVal > 1000)///Assume unit milliKelvin if value is large
//      /** http://en.wikipedia.org/wiki/S.M.A.R.T.: "some older drives may 
//      * instead report raw Temperature (identical to 0xC2)"*/
//      return GetDegCfrom_mK(SMARTrawVal);
    /** http://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes
     *  : "Lowest byte of the raw value contains the exact temperature value (
     *   Celsius degrees)."*/
//      return SMARTvalueFormatter::GetDegCfromCurr(SMARTval);
      return SMARTvalueFormatter::TempDiffOrAirflowTemp_DegCforCurrMinMax(
        SMARTval);

    case TU_Bln361095::dataCarrier::SMART::Attr::DevTemp :
//     if(SMARTrawVal > 1000)///Assume unit milliKelvin if value is large
      return GetDegCfromCurrMinMax(SMARTval);
    case TU_Bln361095::dataCarrier::SMART::Attr::TotalDataWritten:
    case TU_Bln361095::dataCarrier::SMART::Attr::TotalDataRead:{
      std::string numWithSIprefix = GetNumberWithSIprefix(SMARTval);
      if(unitKnown)
        numWithSIprefix += "B";///Only append "B" if unit is known!
      if(p_modelAndFirmware){
        const unsigned maxTeraBytesWritten = p_modelAndFirmware->
          GetMaxTeraBytesWritten();
        if(maxTeraBytesWritten > 0){
          const double percent = (double) SMARTval / (double)
            maxTeraBytesWritten / 10000000000.0 /** 1 tera / 100 */;
          //TODO idea: use an enum "unitDeterminationType" with values as in
          // the following "case" statements rather than evaluating
          // the "greater" bit etc. in the unit -> clearer / easier:
          /*switch(unitDeterminationType){
           case getLowerUnitBound:///Only lower unit bound available
             numWithSIprefix +=
               ///real unit value is probably higher, so percentage of "value /
               /// max. TBW" is
               " >=~" + convertToStdString(percent) + "%";
           case getValRespMakeMoreAccurate:*/
            numWithSIprefix += " " + TU_Bln361095::charStr::convertToStdString(
              percent) + "%";
//          }
        }
      }
      return numWithSIprefix;
    }
    default:
    {
      return GetNumberWithSIprefix(SMARTval);
    }
//      break;
  }
}
