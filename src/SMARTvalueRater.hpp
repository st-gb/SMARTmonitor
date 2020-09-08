/** File:   SMARTvalueRater.hpp
 * Author: Stefan Gebauer,M.Sc.Comp.Sc.
 * Created on 13. Juli 2020, 19:48 */

#ifndef SMARTVALUERATER_HPP
#define SMARTVALUERATER_HPP

#include <hardware/dataCarrier/SMARTattributeNames.h>///enum SMARTattributeNames

enum SMARTvalueRating{noCriticalValue,SMARTvalueOK,SMARTvalueWarning,unknown};

using namespace SMARTattributeNames;///enum SMARTattributeNames

class SMARTvalueRater
{
public:
  enum SMARTvalueRating GetSMARTvalueRating(
    fastestUnsignedDataType SMARTattrID,
    const SMARTuniqueIDandValues & SMARTuniqueIDandVals,
    const uint64_t realCircaValue)
  {
    switch( (enum SMARTattributeNames) SMARTattrID)
    {
    case ReadErrorRate:
    case ReallocSectorsCnt:
    case SpinUpRetryCnt:
    case SSDprogFailCnt:
    case SSDeraseFailCnt:
    case PwrLossProtectionFailure:
    case EraseFailCnt:
    case CmdTimeout:
    case HighFlyWrites:
    case EndToEndError:
    case ReportedUncorrError:
    case G_senseErrorCnt:
    case ReallocEvtCnt:
    case CurrPendSecCnt:
    case UncorrSecCnt:
    case UDMA_CRCerrorCnt:
    case ShockDuringWrite:
    case FreeFallEvtCnt:
      if(realCircaValue == 0)
        return SMARTvalueOK;
      else
        return SMARTvalueWarning;
      break;
    case DevTemp:///temperatures for OK or warning depends on whether HDD or SSD
      /** https://de.wikipedia.org/wiki/Self-Monitoring,_Analysis_and_Reporting_Technology#%C3%9Cbliche_Parameter
       * :Only applies to HDDs?: "Über alle Alter gemittelt, sind Temperaturen
       * unter 25 °C weit gefährlicher als solche über 40 °C."*/
      ///https://harddrivegeek.com/ssd-temperature/
      ///https://en.wikipedia.org/wiki/Solid-state_drive
      if(realCircaValue >= 25 && realCircaValue < 45)///Values for HDD
        return SMARTvalueOK;
      else
        return SMARTvalueWarning;
//      case TotalDataWritten:
      //TODO if too many data/bytes (depending on SSD cell type (SLC/MLC/TLC)
      // /max TBW) written then show warning.
//      case PwrCycleCnt:
      //TODO If ratio between power cycle count (diff) and power-on-hours (diff)
      // is too high then warn
    }
    return unknown;
  }
};

#endif /* SMARTVALUERATER_HPP */
