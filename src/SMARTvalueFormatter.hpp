/**(c) from 2017 by Stefan Gebauer(Computer Science Master from TU Berlin)
 * @author Stefan Gebauer(TU Berlin matriculation number 361095)
 * Created on 2. Januar 2017, 23:13 */

#ifndef TU_Bln361095SMARTmon_SMARTvalFormatter_hpp
#define TU_Bln361095SMARTmon_SMARTvalFormatter_hpp

///C,C++ standard header files:
 #include <stdint.h>///uint64_t
 #include <string>///class std::string

///Stefan Gebauer's(TU Berlin matricul.numb.361095) ~"cmnSrc" repository files:
 #include <hardware/CPU/fastest_data_type.h>///TU_Bln361095::CPU::faststUint
 ///TU_Bln361095dataCarrierATA_SMARTgetCurrTemp()
 #include <hardware/dataCarrier/ATA3Std.h>
 #include <time/timeConstants.h>///TU_Bln361095timeMilliSecsPerHour

///_this_ repository's (header) files:
 ///TU_Bln361095SMARTmonNmSpcBgn, TU_Bln361095SMARTmonNmSpcEnd
 #include <SMARTmon_ID_prefix.h>
 ///TU_Bln361095SMARTmonNmSpc::SMARTattr::rawValTyp
 #include "attributes/SMARTattr.hpp"
 #include <UserInterface/UserInterface.hpp>///UserInterface::FormatTime(...)

///Forward declarations:
TU_Bln361095SMARTmonNmSpcBgn
class ModelAndFirmware;
class SMARTuniqueID;
TU_Bln361095SMARTmonNmSpcEnd

TU_Bln361095SMARTmonNmSpcBgn

extern char unitPrefixes[];

namespace ATA{///ATA and SATA
namespace SMART{
namespace AttrVal{///"Attr"="ATTRibute" "Val"="VALue"

//TODO move to "cmnSrc"?!
///Inline to avoid multiple definitions compiler error.
/**Specific to (S)ATA S.M.A.R.T. */
inline std::string TU_Bln361095hardwareATA_SMARTattrValDef(GetDegCfromCurrMinMax)(
  const uint64_t & SMARTrawValue)
{
  std::ostringstream stdoss;
  const TU_Bln361095::CPU::faststUint maxVal = (SMARTrawValue >> 32) & 0xFFFF;
  const TU_Bln361095::CPU::faststUint minVal = (SMARTrawValue >> 16) & 0xFFFF;

  stdoss << (SMARTrawValue & 0xFFFF) << "�C";
  /** Does not work for a model:ST9500420AS firmware:0003SDM1 (serial:5VJ1WXTF) :
   * highmost byte is 12, lowmost byte: �C, other bytes 0. So check if both
   * values are non-0. */
  if(minVal && maxVal)
    stdoss << "["
      << minVal << "�C..."
      << maxVal << "�C]";
  return stdoss.str();
}

/**@brief formats ATA S.M.A.R.T. value human readable
 * Make this method into a distinct class So it can be used by tests with
 *  only a little payload (code bloat)
 * \param SMARTattrRawVal may be raw value or real (ca.) value
 *  can be > 6 bytes when it is the real value
 *  (use for example boost::multiprecision::cpp_int) */
 template<typename SMARTattrRawValTyp>
   static std::string
     ///ForMaT HUMAN READABLE
     FmtHumanReadable(
       TU_Bln361095::CPU::faststUint SMARTattributeID,
       const SMARTattrRawValTyp
       //TU_Bln361095::SMARTmon::SMARTattr::rawValTyp
        & SMARTattrRawVal,
       const bool unitKnown,
       const TU_Bln361095::SMARTmon::ModelAndFirmware * p_modelAndFirmware)//;
 //template<typename SMARTattrRawValTyp>
///*static*/ std::string SMARTvalueFormatter::FmtATAvalHumanReadable(
//  const TU_Bln361095::CPU::faststUint SMARTattributeID,
//  const /*uint64_t*//*boost::multiprecision::cpp_int*/ /*SMARTattrRawValTyp*/
//  TU_Bln361095::SMARTmon::SMARTattr::rawValTyp
//  & SMARTattrRawVal,
//  const bool unitKnown,
//  const ModelAndFirmware* p_modelAndFirmware)
{
  switch (SMARTattributeID)
  {
    /**https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes
    * "The raw value of this attribute shows total count of hours (or minutes,
    * or seconds, depending on manufacturer) in power-on state."*/
  case TU_Bln361095::dataCarrier::SMART::Attr::PowerOnTime:
  case TU_Bln361095::dataCarrier::SMART::Attr::HeadFlyingHours: {
    //TODO could do a diff of OS uptime (GetTickCount(...) under MS Windows)
     // and values to determine the time unit.
//      return GetTimeFrom_ms(SMARTrawVal);
    std::string timeFmt;
    //return GetTimeFrom_h(SMARTrawVal);

    TU_Bln361095SMARTmonNmSpc::UserInterface::FormatTime(
      *TU_Bln361095SMARTmonNmSpc::SMARTattr::getDataPtr(SMARTattrRawVal),
      timeFmt);
    return timeFmt;
  }
  case TU_Bln361095::dataCarrier::SMART::Attr::TempDiffOrAirflowTemp:
    //     ///If temp. in Celsius: and 100-temp. �C: maximum is 100-(-273)=373
    //     if(SMARTrawVal > 1000)///Assume unit milliKelvin if value is large
    //      /** http://en.wikipedia.org/wiki/S.M.A.R.T.: "some older drives may 
    //      * instead report raw Temperature (identical to 0xC2)"*/
    //      return GetDegCfrom_mK(SMARTrawVal);
        /** http://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes
         *  : "Lowest byte of the raw value contains the exact temperature value (
         *   Celsius degrees)."*/
         //      return SMARTvalueFormatter::GetDegCfromCurr(SMARTattrRawVal);
    return SMARTvalueFormatter::TempDiffOrAirflowTemp_DegCforCurrMinMax(
      *TU_Bln361095::SMARTmon::SMARTattr::getDataPtr(SMARTattrRawVal));

  case TU_Bln361095::dataCarrier::SMART::Attr::DevTemp:
    //     if(SMARTrawVal > 1000)///Assume unit milliKelvin if value is large
    return TU_Bln361095::SMARTmon::ATA::SMART::AttrVal::GetDegCfromCurrMinMax(
      *TU_Bln361095::SMARTmon::SMARTattr::getDataPtr(SMARTattrRawVal));
  case TU_Bln361095::dataCarrier::SMART::Attr::TotalDataWritten:
  case TU_Bln361095::dataCarrier::SMART::Attr::TotalDataRead:
    return SMARTvalueFormatter::fmtTotalDataRdWr( //(uint8_t *) &SMARTattrRawVal
      SMARTattrRawVal, //6, 
      unitKnown, p_modelAndFirmware);
  default:
  {
    return SMARTvalueFormatter::GetNumberWithSIprefix(
      /**Get data as 64 bit/8 byte number if data type is
       * "boost::multiprecision::cpp_int" */
      *TU_Bln361095::SMARTmon::SMARTattr::getDataPtr(SMARTattrRawVal)
    );
  }
  //      break;
  }
}

/**"SMARTattrRawValTyp" may be "boost::multiprecision::cpp_int" or "uint64_t"
 *  for example. */
template <typename SMARTattrRawValTyp> inline void fmtRawVal(
  const TU_Bln361095::CPU::FaststUint SMARTattrID, 
  const SMARTattrRawValTyp SMARTattrRawVal,
  std::ostringstream & std_ossRawSMARTval)
{
  switch(SMARTattrID)
  {
    case TU_Bln361095::dataCarrier::SMART::Attr::GiB_Erased:
  /** https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes
    *  : "Value is equal to (100-temp. �C)"*/
    case TU_Bln361095::dataCarrier::SMART::Attr::TempDiffOrAirflowTemp:
  /**https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes
    * 9 Jul 2020: "Lowest byte of the raw value contains the exact temperature
    * value (Celsius degrees)"*/
    case TU_Bln361095::dataCarrier::SMART::Attr::DevTemp:
    case TU_Bln361095::dataCarrier::SMART::Attr::HW_ECC_Recovered:
  /**https://en.wikipedia.org/wiki/S.M.A.R.T.#Known_ATA_S.M.A.R.T._attributes
    * 9 Jul 2020: "Decoded as: byte 0-1-2 = average erase count (big endian)
    * and byte 3-4-5 = max erase count (big endian).*/
    case TU_Bln361095::dataCarrier::SMART::Attr::AvgEraseCntAndMaxEraseCnt:
    case TU_Bln361095::dataCarrier::SMART::Attr::HeadFlyingHours:
      std_ossRawSMARTval << std::hex << /**To better differentiate between number and
      * base*/std::uppercase << SMARTattrRawVal << "h";
      break;
    default:
      std_ossRawSMARTval << SMARTattrRawVal;
  }
}

/**@brief Uses the default unit to get the _real_ S.M.A.R.T. attribute raw value
 * from the S.M.A.R.T. attribute raw value.
 * -sets the unit as human readable character string
 * -determines the real raw value according to the raw value
 * @param numForHumanReadableFormat output the (unchanged) raw value */
template<typename realCircaSMARTattrRawValTyp, typename SMARTattrRawValTyp>
 inline void useDefaultUnit(
  const TU_Bln361095::CPU::faststUint SMARTattrID,
  realCircaSMARTattrRawValTyp & realCircaSMARTattrRawVal,
  const SMARTattrRawValTyp & SMARTattrRawVal,
  std::ostringstream & std_ossUnit,
  SMARTattrRawValTyp & numForHumanReadableFormat)
{
  switch(SMARTattrID)
  {
  case TU_Bln361095::dataCarrier::SMART::Attr::DevTemp:
    numForHumanReadableFormat = SMARTattrRawVal;
      std_ossUnit << "�C?";
    realCircaSMARTattrRawVal = TU_Bln361095dataCarrierATA_SMARTgetCurrTemp(
      SMARTattrRawVal);
    break;
  case TU_Bln361095::dataCarrier::SMART::Attr::PowerOnTime:
    numForHumanReadableFormat = SMARTattrRawVal * /**h to ms*/
      TU_Bln361095timeMilliSecsPerHour;
    std_ossUnit << "~h?";
    break;
  case TU_Bln361095::dataCarrier::SMART::Attr::HeadFlyingHours:
    numForHumanReadableFormat = (SMARTattrRawVal & 0xFFFFFF) * /**h to ms*/
      TU_Bln361095timeMilliSecsPerHour;
    std_ossUnit << "~h?";
    break;
  case TU_Bln361095::dataCarrier::SMART::Attr::SpinUpTime:
    std_ossUnit << "ms?";
    numForHumanReadableFormat = SMARTattrRawVal;
    break;
  default:
    numForHumanReadableFormat = SMARTattrRawVal;
    switch(SMARTattrID)
    {
      ///S.M.A.R.T. parameter ID 4
    case TU_Bln361095::dataCarrier::SMART::Attr::StrtStpCnt:
      ///S.M.A.R.T. parameter ID 5
    case TU_Bln361095::dataCarrier::SMART::Attr::ReallocSectorsCnt:
      ///S.M.A.R.T. parameter ID 10
    case TU_Bln361095::dataCarrier::SMART::Attr::SpinUpRetryCnt:
    ///S.M.A.R.T. parameter ID 11
    case TU_Bln361095::dataCarrier::SMART::Attr::
      RecalibRetriesOrCalibrRetryCnt:
      ///S.M.A.R.T. parameter ID 12
    case TU_Bln361095::dataCarrier::SMART::Attr::PwrCycleCnt:
      ///S.M.A.R.T. parameter ID 196
    case TU_Bln361095::dataCarrier::SMART::Attr::ReallocEvtCnt:
      ///S.M.A.R.T. parameter ID 197
    case TU_Bln361095::dataCarrier::SMART::Attr::CurrPendSecCnt:
      ///S.M.A.R.T. parameter ID 198
    case TU_Bln361095::dataCarrier::SMART::Attr::UncorrSecCnt:
      ///S.M.A.R.T. parameter ID 199
    case TU_Bln361095::dataCarrier::SMART::Attr::UDMA_CRCerrorCnt:
    case TU_Bln361095::dataCarrier::SMART::Attr::FreeFallEvtCnt:
      ///S.M.A.R.T. param ID 200
    case TU_Bln361095::dataCarrier::SMART::Attr::MultiZoneErrorRate:
    ///S.M.A.R.T. param ID 201
    case TU_Bln361095::dataCarrier::SMART::Attr::
      SoftReadErrorRateOrTACnterDetected:
      std_ossUnit << "#?";
      break;
    case TU_Bln361095::dataCarrier::SMART::Attr::TotalDataWritten:
    case TU_Bln361095::dataCarrier::SMART::Attr::TotalDataRead:
      std_ossUnit << ">=1sector?";
      break;
    default:
      std_ossUnit << "?";
    }
    realCircaSMARTattrRawVal = SMARTattrRawVal;
  }
}
}///namespace AttrVal
}///namespace SMART
}///namespace ATA

namespace NVMe
{
  /**The name is NVMe SMART:
https://en.wikipedia.org/wiki/Self-Monitoring,_Analysis_and_Reporting_Technology
#Known_NVMe_S.M.A.R.T._attributes */
namespace SMART{
namespace AttrVal{
/**"SMARTattrRawValTyp" may be "boost::multiprecision::cpp_int" or "uint64_t"
 *  for example. */
/**@brief Uses the default unit for S.M.A.R.T. attribute identifier
 *  \p SMARTattrID. Also sets \p realCircaSMARTattrRawVal and \p std_ossUnit
 *  according to this unit.
 * @tparam realCircaSMARTattrRawValTyp may be "boost::multiprecision::cpp_int"
 *  (from boost.org) or "uint64_t" () for example.
 * @tparam SMARTattrRawValTyp may be "boost::multiprecision::cpp_int" or "uint64_t"
 *  for example.
 * @param[in] SMARTattrRawVal : The raw value retrieved (unchanged) from data
 *  carrier
 * @param[out] realCircaSMARTattrRawVal The real raw value to calculate/set for
 *  \p SMARTattrID
 * @param[out] std_ossUnit : The unit to set for \p SMARTattrID */
template<typename realCircaSMARTattrRawValTyp, typename SMARTattrRawValTyp>
 inline void
 useDefaultUnit(
  const TU_Bln361095::CPU::faststUint SMARTattrID,
  /*SMARTattr::rawValTyp*/ realCircaSMARTattrRawValTyp & realCircaSMARTattrRawVal,
  const /*SMARTattr::rawValTyp*/ SMARTattrRawValTyp & SMARTattrRawVal,
  std::ostringstream & std_ossUnit,
  /*SMARTattr::rawValTyp*/ SMARTattrRawValTyp & numForHumanReadableFormat
  )
{
  ///http://en.wikipedia.org/wiki/Self-Monitoring,_Analysis_and_Reporting_Technology
  switch(SMARTattrID)
  {
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::CompositeTemperature:
    numForHumanReadableFormat = SMARTattrRawVal;
    std_ossUnit << "�C?";
    {
    const TU_Bln361095::CPU::faststUint tempInKelvin = *SMARTattr::getDataPtr(
      SMARTattrRawVal);
    //realCircaSMARTattrRawVal = TU_Bln361095::KelvinToDegC(tempInKelvin);
    //const unsigned tempInDegC
    realCircaSMARTattrRawVal = TU_Bln361095::dataCarrier::NVMe::SMART::Attr::
      GetTempInDegC( (uint8_t *) & tempInKelvin);
    }
    break;
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::PowerOnHours:
      numForHumanReadableFormat = SMARTattrRawVal * /**h to ms*/
        TU_Bln361095timeMilliSecsPerHour;
      std_ossUnit << "~h?";
      break;
      ///Unit can be determined with DISK_PERFORMANCE's members LARGE_INTEGER
      // ReadTime, LARGE_INTEGER WriteTime, LARGE_INTEGER IdleTime, QueryTime?
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::ControllerBusyTime:
      numForHumanReadableFormat = SMARTattrRawVal * /**h to ms*/
        TU_Bln361095timeMilliSecsPerHour;
      /**For model:KINGSTON OM8SBP3512K-AH firmware:HPS2 serial:0026B7684E283630
       * it increases more than 1 in an hour -> unit less than hour*/
      std_ossUnit << "s?";
      numForHumanReadableFormat = SMARTattrRawVal;
      break;
    break;
    default:
    numForHumanReadableFormat = SMARTattrRawVal;
    switch(SMARTattrID)
    {
      ///S.M.A.R.T. parameter ID 4
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::AvailableSpare:
      ///S.M.A.R.T. parameter ID 5
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::AvailableSpareThreshold:
      ///S.M.A.R.T. parameter ID 10
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::PercentageUsed:
      std_ossUnit << "%?";
      break;
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::TotalDataWritten:
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::TotalDataRead:
      if(std::is_same_v<decltype(numForHumanReadableFormat), uint64_t>)
        /**Only multipy if there are enough bits for multiplying.
         * 2^64/512000=36,028,797,018,963.968
         * =0010 0000 1100 0100 1001 1011 1010 0101 1110 0011 0101 0011 binary
         * (5*8+2=42 bits/6 bytes used) */
        if (SMARTattr::highBytesSet((const uint8_t*) & SMARTattrRawVal, 6))
          numForHumanReadableFormat *= 512000;
#ifdef TU_Bln361095useBoostMultiprecisionCppInt
      //if(std::is_same_v<decltype(numForHumanReadableFormat),
      //  boost::multiprecision:://cpp_int
      //  number<boost::multiprecision::cpp_int_backend<> > >)
        numForHumanReadableFormat *= 512000;
#endif
      std_ossUnit << "512000B?";
      break;
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::HostReadCmds:
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::HostWriteCmds:
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::PowerCycle:
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::UnsafeShutdowns:
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::MediaErrors:
    case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::ErrorInfoLogEntryCount:
      std_ossUnit << "#?";
      break;
default:
      std_ossUnit << "?";
    }
    realCircaSMARTattrRawVal = SMARTattrRawVal;
  }
}

/**"Fmt"="ForMaT"
 * @param SMARTattrVal pointer to 1 byte type because this value can be up to 16
 *  bytes long */
template<typename SMARTattrValTyp>
std::string FmtHumanReadable(
  const TU_Bln361095::CPU::faststUint SMARTattrID,
  const /* uint8_t* */ SMARTattrValTyp
   //TU_Bln361095SMARTmonNmSpc::SMARTattr::rawValTyp
   & SMARTattrVal,
  const bool unitKnown,
  const TU_Bln361095::SMARTmon::ModelAndFirmware * p_modelAndFirmware)
{
  switch(SMARTattrID)
  {
  case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::CompositeTemperature:
    //if(std::is_same_v<decltype(SMARTattrVal), boost::multiprecision::cpp_int>)
    {
#ifdef TU_Bln361095useBoostMultiprecisionCppInt
    const TU_Bln361095::CPU::faststUint tempInDegC =
      TU_Bln361095::dataCarrier::NVMe::SMART::Attr::GetTempInDegC(
        (uint8_t*)SMARTattr::getDataPtr(SMARTattrVal)
      );
#endif
      std::ostringstream oss;
      oss << tempInDegC;
      return oss.str();
    }
    ///see nvme.h: 16 byte value.
  case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::PowerOnHours:
  {
    std::string timeFmt;
    const uint64_t * dataPtr = TU_Bln361095::SMARTmon::SMARTattr::getDataPtr(
      SMARTattrVal);
    TU_Bln361095::SMARTmon::UserInterface::FormatTime(
      /**For "KINGSTON OM8SBP3512K-AH", firmware "HPS2" 512,1 GB this is given
       * in hours and calculated to milliseconds */
      *dataPtr
      , timeFmt);
    return timeFmt;
  }
  case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::TotalDataWritten:
  case TU_Bln361095::dataCarrier::NVMe::SMART::Attr::TotalDataRead:
    return SMARTvalueFormatter::fmtTotalDataRdWr(
      SMARTattrVal, //16, 
      unitKnown, p_modelAndFirmware);
  default:
    return SMARTvalueFormatter::GetNumberWithSIprefix(SMARTattrVal);
  }
  return "";
}

}///namespace AttrVal
}///namespace SMART
}///Namespace NVMe

class SMARTvalueFormatter
{
public:
  SMARTvalueFormatter();
  SMARTvalueFormatter(const SMARTvalueFormatter& orig);
  virtual ~SMARTvalueFormatter();

  /**@brief Formats \p SMARTattrRawVal human readable, for example large number
   *  with SI metric prefixes like "k" for thousand, "M" for million.
   * @param SMARTattrRawVal raw value read from S.M.A.R.T. attribute */
  template<typename SMARTattrRawValTyp>
inline static std::string
    ///Fmt=ForMaT
    FmtHumanReadable(
      const SMARTuniqueID & sMARTuniqueID,
      TU_Bln361095::CPU::faststUint SMARTattrID,
      const SMARTattrRawValTyp
       //TU_Bln361095::SMARTmon::SMARTattr::rawValTyp
       & SMARTattrRawVal,
      const bool unitKnown,
      const ModelAndFirmware * p_modelAndFirmware)
  {
    switch(sMARTuniqueID.getBusType() )
    {
    case TU_Bln361095::hardware::bus::NVMe:
      return TU_Bln361095::SMARTmon::NVMe::SMART::AttrVal::FmtHumanReadable(
        SMARTattrID,
        SMARTattrRawVal,
        unitKnown,
        p_modelAndFirmware
        );
      break;
    default:
      return TU_Bln361095::SMARTmon::NVMe::SMART::AttrVal::FmtHumanReadable(
        SMARTattrID,
        SMARTattrRawVal,
        unitKnown,
        p_modelAndFirmware
        );
    }
  }

/**@brief formats S.M.A.R.T. attribute value for "total data read" or "total data
 *  written". Can be used for ATA or NVMe S.M.A.R.T. */
template <typename SMARTattrRawValTyp> static std::string fmtTotalDataRdWr(
  //const uint8_t SMARTattrRawVal[],
  //unsigned numB,
  const SMARTattrRawValTyp & SMARTattrRawVal,
  const bool unitKnown,
  const TU_Bln361095::SMARTmon::ModelAndFirmware* p_modelAndFirmware)
{
  //if(/*TU_Bln361095::SMARTmon::*/SMARTattr::highBytesSet(SMARTattrRawVal, numB) )
  //  return "";
  std::string numWithSIprefix = /*SMARTvalueFormatter::*/GetNumberWithSIprefix(
    SMARTattrRawVal);
  if (unitKnown)
    numWithSIprefix += "B";///Only append "B" for "Bytes" if unit is known!
  if (p_modelAndFirmware)
  {
    const unsigned maxTeraBytesWritten = p_modelAndFirmware->
      GetMaxTeraBytesWritten();
  //  if (maxTeraBytesWritten > 0)
  //  {
  //    const double percent = /* (double)*( (uint64_t*)SMARTattrRawVal)*/
  //      SMARTattrRawVal / (double)
  //      maxTeraBytesWritten
  //      / 10000000000.0 /** 1 tera / 100 */;

  //    //TODO idea: use an enum "unitDeterminationType" with values as in
  //    // the following "case" statements rather than evaluating
  //    // the "greater" bit etc. in the unit -> clearer / easier:
  //    /*switch(unitDeterminationType){
  //      case getLowerUnitBound:///Only lower unit bound available
  //        numWithSIprefix +=
  //          ///real unit value is probably higher, so percentage of "value /
  //          /// max. TBW" is
  //          " >=~" + convertToStdString(percent) + "%";
  //      case getValRespMakeMoreAccurate:*/

  //    numWithSIprefix += " " + TU_Bln361095::charStr::convertToStdString(
  //      percent) + "%";
  ////          }
  //  }
  }
  return numWithSIprefix;
}

/**@tparam divisionDataTyp may be boost::multiprecision::cpp_int for 16 byte
 *  NVMe S.M.A.R.T. attribute values */
template<typename divisionDataTyp> inline static char
 calcUnitPrefixFactorAndRemainder(
  divisionDataTyp & remainder,
  divisionDataTyp & unitPrefixFactor)
{
  LOGN_DEBUG("begin--number:" << remainder)
  divisionDataTyp remainderCopy = remainder;
  divisionDataTyp one1kPower = 1;
  TU_Bln361095::CPU::faststUint One_kExp = 0;
  while(remainderCopy > 1000ULL)
  {
    remainderCopy /= 1000ULL;
    one1kPower *= 1000ULL;
    One_kExp++;
  }
  unsigned One_kExpCopy = One_kExp;
  remainderCopy = 1ULL;
  while(One_kExpCopy > 0)
  {
    remainderCopy *= 1000ULL;
    One_kExpCopy--;
  }
  if(remainderCopy > 1)
  {
    unitPrefixFactor = remainder / /*remainderCopy*/ one1kPower;
    remainder = remainder - unitPrefixFactor * remainderCopy;
  }
  else
  {
    unitPrefixFactor = remainder;
    remainder = 0;
  }
  LOGN_DEBUG("prefix factor:" << unitPrefixFactor
    << " remainder:" << remainder << " 1k's exponent:" << One_kExp)
  return unitPrefixes[One_kExp];
}

/**@tparam divisionDataTyp may be boost::multiprecision::cpp_int for 16 byte
 *  NVMe S.M.A.R.T. attribute values */
template<typename divisionDataTyp> static std::string GetNumberWithSIprefix(
  const divisionDataTyp & SMARTattrRawVal)
{
  divisionDataTyp remainder = SMARTattrRawVal;
  divisionDataTyp unitPrefixFactor = SMARTattrRawVal;
  char unitPrefixChar;
  std::ostringstream stdoss;
  do
  {
    unitPrefixChar = calcUnitPrefixFactorAndRemainder(remainder,
      unitPrefixFactor);
    if(unitPrefixChar == ' ')
      stdoss << unitPrefixFactor;
    else
      stdoss << unitPrefixFactor << unitPrefixChar;
  } while(unitPrefixChar != ' ');
  return stdoss.str();
}

  ///ForMaT NVMe VALue HUMAN READABLE
  ///For (S)ATA (and not NVMe) S.M.A.R.T.:
  static std::string GetDegCfromCurr(const uint64_t & SMARTrawValue);
  //static std::string GetDegCfromCurrMinMax(const uint64_t & SMARTrawValue);
  static std::string GetDegCfrom_mK(const uint64_t & SMARTrawValue);
  static std::string GetTimeFrom_h(const uint64_t & SMARTrawVal);
  static std::string GetTimeFrom_ms(const uint64_t & SMARTrawValue);
  static std::string TempDiffOrAirflowTemp_DegCforCurrMinMax(const uint64_t &);
private:

};

#endif///include guard

TU_Bln361095SMARTmonNmSpcEnd
