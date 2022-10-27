/** File:   SMARTvalueRater.hpp
 * Author: Stefan Gebauer,M.Sc.Comp.Sc.
 * Created on 13. Juli 2020, 19:48 */

#ifndef SMARTVALUERATER_HPP
#define SMARTVALUERATER_HPP

///Stefan Gebauer's(TU Berlin matr.#361095)"common_sourcecode" repository files:
#include <hardware/dataCarrier/SMARTattributeNames.h>///enum SMARTattributeNames
#include <compiler/force_inline.h>///TU_Bln361095frcInln

///Files from SMARTmonitor repository:
#include <attributes/ModelAndFirmware.hpp>///class ModelAndFirmware
#include <attributes/SMARTuniqueIDandValues.hpp>///class SMARTuniqueIDandValues

//TODO encapsulate in own namespace?
enum SMARTvalueRating{SMARTvalueWarning=0,SMARTvalueOK=1,noCriticalValue,unknown};

using namespace SMARTattributeNames;///enum SMARTattributeNames

/**val=VALue: http://www.abbreviations.com/abbreviation/Value
 * rtg=rating: http://www.abbreviations.com/abbreviation/Rating
 * Typ=type: http://www.allacronyms.com/type/abbreviated */
typedef/**If type is quotient between current value and threshold:use floating
 * point type for example*/
  float SMARTvalRatngTyp;

#define SMARTvalAbs1RngWarnThresh 0.5f///Middle between fully OK and threshold

/** https://de.wikipedia.org/wiki/Self-Monitoring,_Analysis_and_Reporting_Technology#%C3%9Cbliche_Parameter
 * :Only applies to HDDs?: "Über alle Alter gemittelt, sind Temperaturen
 * unter 25 °C weit gefährlicher als solche über 40 °C."*/
/**"Failure Trends in a Large Disk Drive Population" by "Google Inc." 2007,
 *  -page 5:"In fact, there is a clear trend showing that lower temperatures are
 *   associated with higher failure rates."
 *  -page 6:"Figure 4: Distribution of average temperatures and failures rates."
 *   failures increase at ca. < 25 °C */
#define maxWarnTempInDegC 25
/**http://en.wikipedia.org/wiki/Solid-state_drive#Hard_disk_drives 24 Sep 2022:
 * "A Facebook study found that at operating temperatures above 40 °C (104 °F),
 * the failure rate among SSDs increases with temperature."*/
/**"Failure Trends in a Large Disk Drive Population" by "Google Inc." 2007,
 *  -page 5:"Only at very high temperatures is there a slight reversal of this
 *   trend"
 *  -page 6:"Figure 4: Distribution of average temperatures and failures rates."
 *   failures increase at ca. > 46/47 °C */
#define minWarnTempInDegC 46
#define minCritTempInDegC 69

///Mfr=ManuFactureR:http://www.abbreviations.com/abbreviation/Manufacturer
///Rtg=RaTinG:http://www.abbreviations.com/abbreviation/rating
namespace MfrRtg{
/**Curr=CURRent:http://www.abbreviations.com/abbreviation/Current
 * Val=VALue: http://www.abbreviations.com/abbreviation/Value
 * Thresh=THRESHold: http://www.allacronyms.com/threshold/abbreviated */
namespace nrmlzdCurrValAndThresh{
///Mns=MiNuS
///Rng=RaNGe:http://www.abbreviations.com/abbreviation/range
inline float Get1ToMns1Rng(
  const unsigned SMARTattrID,
  const long int nrmlzdCurrVal,
  const long int nrmlzdThresh)
{
  long maxNrmlzdVal = SMARTvalue::maxNrmlzdVals[SMARTattrID];
  maxNrmlzdVal = SMARTvalue::getNxtGtrMaxNrmlzdVal(maxNrmlzdVal,
    nrmlzdCurrVal, nrmlzdThresh);
  const long int divident = maxNrmlzdVal-nrmlzdThresh;
  if(divident == 0)///Avoid division by 0
    return 0.0f;
  else{
    const long int divisor = nrmlzdCurrVal - nrmlzdThresh;
    /**Use gradation (percent value) to make color between light red and
     * dark red or red and dark violet.
     * examples:
     * (100-10)/(100-10)=90/90=1
     * (99-10)/(100-10)=89/90~=0,98
     * (50-0)/(100-0)=50/100=0.5
     * (40-10)/(100-10)=30/90=0.period3
     * (11-10)/(100-10)=1/90=0,0period1
     * (10-10)/(100-10)=0/90=0.0
     * (5-10)/(100-10)=-5/90=−0,0556
     * (0-10)/(100-10)=-10/90=−0,period1 */
    const float quotient =
     (float)divisor///Many normalized current S.M.A.R.T. values start at 100
      / (float) divident;
    float scaledQuotient;
    if(quotient > 0.0f){
      /**Happened when maximal normalized value was assumed to be 100 and
       * the normalized current value was 104. Then the quotient was 104-2/100-2=102/98 */
      if(quotient > 1.0f)
      	 scaledQuotient = 1.0f;
      else
        scaledQuotient = quotient;///To test errors: multiply with * -1.0f
    }
  /** http://en.wikipedia.org/wiki/S.M.A.R.T.#ATA_S.M.A.R.T._attributes :
   * "threshold values beyond which attributes should not pass"
   * Some normalized threshold values are 0 and the normalized current value
   * can't be < 0. So use <= for indicating errors.*/
    else///Normalized current value <= normalized threshold (this only holds if max normalized value is correct?)
    {
      if(quotient == 0.0f)
    	scaledQuotient = -1.0f;///=threshold=>error
      else
    	scaledQuotient = quotient
          ///Span to range 0--1.0
          * (float)divident/(float)nrmlzdThresh;
    }
    return scaledQuotient;
  }
}
}}///End namespaces

class SMARTvalueRater
{
public:
  /**@brief Rates according to {@code realCircaValue}. This function suits at
   *  least for Hard Disk Drives because the thresholds are based on the study
   *  for them.
   * @param realCircaValue supposed to be degrees Celsius.*/
  TU_Bln361095frcInln static SMARTvalRatngTyp rateAccDegCelsius(
    const uint64_t realCircaValue)
  {
  ///https://harddrivegeek.com/ssd-temperature/
    if(realCircaValue < maxWarnTempInDegC)
      return (float)realCircaValue / (float)maxWarnTempInDegC;///=> [0...1[ =warning
    else if(realCircaValue >= minWarnTempInDegC){
      if(realCircaValue > minCritTempInDegC)
        return (float)(realCircaValue-minCritTempInDegC)/-30.0f;///=> ]0...-1] =error
      else///[minWarnTempInDegC-1...minCritTempInDegC]
        return (float)(minCritTempInDegC-realCircaValue)/
          (float)(minCritTempInDegC-minWarnTempInDegC);///=> [0...1[ =warning
    }
    else///Optimal temperature range
      return 1.0f;
  }
  /** @brief get rating for @see param SMARTattrID according to current
   *   normalized value or @see realCircaRawValue
   * Is called often: for every S.M.A.R.T. ID of every data carrier in an
   *  interval (every x seconds) -> make as member function in header file (
   *  implicitely inline).
   * @return the higher the less critical.
   *  1.0:fully OK/no warning
   *  < 1.0:above or same as threshold
   *  < 0.0:exceeds threshold->critical/warning/error
   *  -1.0:worst value*/
  SMARTvalRatngTyp GetSMARTvalueRating(
    fastestUnsignedDataType SMARTattrID,
    const SMARTuniqueIDandValues & SMARTuniqueIDandVals,
    const uint64_t realCircaValue,
    const ModelAndFirmware * p_modelAndFirmware)
  {
	bool useManufacturerRtg = true;
	switch((enum SMARTattributeNames) SMARTattrID)
	{
     case DevTemp:///temperatures for OK or warning depends on whether HDD or SSD
/**In regard of temperatures some manufacturers like Intel for model
 * "INTEL SSDSC2BF240A5H REF", firmware "LWDi" don't follow the semantics that
 * lower normalized current values mean a worse rating: for
 * "INTEL SSDSC2BF240A5H REF" the normalized current value is simply the
 * temperature in °C. So use an own rating for temperatures.*/
       useManufacturerRtg = false;
	}
	if(useManufacturerRtg){
    const SMARTvalue & currSMARTval = SMARTuniqueIDandVals.m_SMARTvalues[
      SMARTattrID];
    const long int nrmlzdCurrVal = currSMARTval.GetNrmlzdCurrVal();
    const long int nrmlzdThresh = currSMARTval.GetNrmlzdThresh();
    const float scaledQuotient = MfrRtg::nrmlzdCurrValAndThresh::Get1ToMns1Rng(
      SMARTattrID, nrmlzdCurrVal, nrmlzdThresh);
    return scaledQuotient;
	}
    switch( (enum SMARTattributeNames) SMARTattrID)
    {
    case ReadErrorRate:
    case ReallocSectorsCnt:
    case SpinUpRetryCnt:
    case RecalibRetriesOrCalibrRetryCnt:
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
    case MultiZoneErrorRate:
    case SoftReadErrorRateOrTACnterDetected:
    case ShockDuringWrite:
    case FreeFallEvtCnt:
      if(realCircaValue == 0)
        return SMARTvalueOK;
      else
        return SMARTvalueWarning;
      break;
    case TotalDataWritten:
      {
      if(p_modelAndFirmware){///may be NULL
        const ModelAndFirmware::remainingTotalDataWrittenType
          remainingTotalDataWritten = p_modelAndFirmware->
          getRemainingTotalDataWritten(realCircaValue);
        if(remainingTotalDataWritten == ModelAndFirmware::
          maxTeraBytesWrittenNotSet)
          return unknown;
        else{
          if(remainingTotalDataWritten > 10000000000 )///> 10 GB left
            return SMARTvalueOK;
          else///< 10 GB to write left or max. TeraBytesWritten exceeded
            return SMARTvalueWarning;
        }
      }
      else
        return unknown;
    }
    case DevTemp:///temperatures for OK or warning depends on whether HDD or SSD
      return rateAccDegCelsius(realCircaValue);
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
