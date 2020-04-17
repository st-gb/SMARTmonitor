/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SMARTvalueFormatter.cpp
 * Author: sg
 * 
 * Created on 2. Januar 2017, 23:13
 */

#include "SMARTvalueFormatter.hpp"
#include <preprocessor_macros/logging_preprocessor_macros.h>
#include <SMARTattributeNames.h>

SMARTvalueFormatter::SMARTvalueFormatter() {
}

SMARTvalueFormatter::SMARTvalueFormatter(const SMARTvalueFormatter& orig) {
}

SMARTvalueFormatter::~SMARTvalueFormatter() {
}

/** See https://en.wikipedia.org/wiki/Unit_prefix */
char unitPrefixes [] = {' ', 'k', 'M', 'G', 'T', 'P'};

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

std::string SMARTvalueFormatter::GetDegCfrom_mK(
  const uint64_t & SMARTrawValue)
{
  std::ostringstream stdoss;
  stdoss.precision(1);
  //stdoss << std::fixed << (float) SMARTrawValue / 3600000 << " h";
  stdoss << std::fixed << (float) SMARTrawValue/1000.0f - 273.1f << "°C";
  return stdoss.str();
}

std::string SMARTvalueFormatter::GetTimeFrom_ms(
  const uint64_t & SMARTrawValue)
{
  std::ostringstream stdoss;
  stdoss.precision(1);
  //stdoss << std::fixed << (float) SMARTrawValue / 3600000 << " h";
  const float hours = (float) SMARTrawValue / 3600000.0f;
  const fastestUnsignedDataType numDays = hours / 24;
  const float hoursRemainder = hours - (numDays * 24);
  stdoss << std::fixed << numDays << "d" << hoursRemainder << "h";
  return stdoss.str();
}

/** Make this method into a distinct class So it can be used by tests with
 *  only a little payload (code bloat) */
std::string SMARTvalueFormatter::FormatHumanReadable(
  fastestUnsignedDataType SMARTattributeID,
  const uint64_t & SMARTrawValue)
{
  switch (SMARTattributeID)
  {
    /**https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes
    * "The raw value of this attribute shows total count of hours (or minutes,
    * or seconds, depending on manufacturer) in power-on state."*/
    case SMARTattributeNames::PowerOnTime :
    case SMARTattributeNames::HeadFlyingHours :
      return GetTimeFrom_ms(SMARTrawValue);
    /**https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes.
    *"Value is equal to (100-temp. °C), allowing manufacturer to set a minimum
    * threshold which corresponds to a maximum temperature. This also follows
    * the convention of 100 being a best-case value and lower values being
    * undesirable. However, some older drives may instead report raw Temperature
    * (identical to 0xC2) or Temperature minus 50 here.*/
    case SMARTattributeNames::TempDiffOrAirflowTemp:
     ///If temp. in Celsius: and 100-temp. °C: maximum is 100-(-273)=373
     if(SMARTrawValue > 1000)///Assume unit milliKelvin if value is large
      /**thtps://en.wikipedia.org/wiki/S.M.A.R.T.: "some older drives may 
      * instead report raw Temperature (identical to 0xC2)"*/
      return GetDegCfrom_mK(SMARTrawValue);
    /**https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes:
     * "Lowest byte of the raw value contains the exact temperature value (
     *   Celsius degrees)."*/
    case SMARTattributeNames::DevTemp :
     if(SMARTrawValue > 1000)///Assume unit milliKelvin if value is large
      return GetDegCfrom_mK(SMARTrawValue);
    default:
    {
      uint64_t remainder = SMARTrawValue;
      uint64_t unitPrefixFactor = SMARTrawValue;
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
//      break;
  }
}
