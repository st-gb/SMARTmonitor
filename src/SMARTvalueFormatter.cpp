/** File:   SMARTvalueFormatter.cpp
 * Author: Stefan Gebauer, M.Sc.Comp.Sc./Informatik (TU Berlin)
 * Created on 2. Januar 2017, 23:13 */

/** Files from Stefan Gebauer's common_sourcecode repository: */
///convertToStdString(...)
#include <dataType/charStr/convertFromAndToStdString.hpp>
#include <hardware/dataCarrier/SMARTattributeNames.h>///enum SMARTattributeNames
///enum TU_Bln361095::dataCarrier::NVMe::SMART::Attr::ID
#include <hardware/dataCarrier/NVMe/NVME_SMART_attr.h>
#include <preprocessor_macros/logging_preprocessor_macros.h>///LOGN[...](...)

#include "SMARTvalueFormatter.hpp"
#include <UserInterface/UserInterface.hpp>///UserInterface::FormatTime(...)
#include <attributes/ModelAndFirmware.hpp>///class ModelAndFirmware
///_This_ (Stefan Gebauer's) ("SMARTmon") repository's (header) files:
 #include <attributes/SMARTattr.hpp>///class TU_Bln361095::SMARTmon::SMARTattr

TU_Bln361095SMARTmonNmSpcBgn

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

std::string SMARTvalueFormatter::GetDegCfromCurr(
  const uint64_t & SMARTrawValue)
{
  std::ostringstream stdoss;
  stdoss << (SMARTrawValue & 0xFFFF) << "°C";  
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


TU_Bln361095SMARTmonNmSpcEnd
