/**@author: Stefan Gebauer,M.Sc.Comp.Sc.(TU Berlin matriculation number 361095)
 * Created on 13. Juli 2020, 19:48 */

//TODO Rename this file to "SMARTattrValRtgImpl.hpp" (ATTRibute VALue RaTnG
// IMPLementation)
// This file should then be included by either the source file
// "SMARTattrValRtg.cpp" or in the header file "SMARTattrValRtgDecl.hpp".

#ifndef TU_Bln361095SMARTmon__SMARTValRater_hpp
#define TU_Bln361095SMARTmon__SMARTValRater_hpp

///Stefan Gebauer's(TU Berlin matriculation number:361095) ~"cmnSrc" repository:
 ///enum TU_Bln361095dataCarrierSMARTattrUse(ID)/
 ///enum TU_Bln361095::dataCarrier::SMART::Attr::ID
 #include <hardware/dataCarrier/SMARTattributeNames.h>
 #include <compiler/force_inline.h>///TU_Bln361095frcInln

///Files from SMARTmonitor repository:
 ///TU_Bln361095SMARTmonNmSpcBgn,TU_Bln361095SMARTmonNmSpcEnd
 #include <SMARTmon_ID_prefix.h>
 ///class TU_Bln361095SMARTmonUse(ModelAndFirmware)
 #include <attributes/ModelAndFirmware.hpp>
 ///class TU_Bln361095SMARTmonUse(SMARTuniqueIDandValues)
 #include <attributes/SMARTuniqueIDandValues.hpp>

 ///TU_Bln361095dataCarrier[...]
 #include "SMARTattrValRtngDecl.hpp"///enum TU_Bln361095SMARTmonDef(Rating)

TU_Bln361095SMARTmonNmSpcBgn

/**Mfr=ManuFactureR:http://www.abbreviations.com/abbreviation/Manufacturer
 * Rtg=RaTinG:http://www.abbreviations.com/abbreviation/rating */
namespace MfrRtg{
/**Curr=CURRent:http://www.abbreviations.com/abbreviation/Current
 * Val=VALue: http://www.abbreviations.com/abbreviation/Value
 * Thresh=THRESHold: http://www.allacronyms.com/threshold/abbreviated */
namespace nrmlzdCurrValAndThresh{

/**Gets S.M.A.R.T. attribute value rating in [-1...1] range
 * Mns=MiNuS
 * Rng=RaNGe: http://www.abbreviations.com/abbreviation/range */
TU_Bln361095frcInln TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) Get1ToMns1Rng(
  const TU_Bln361095CPUuse(faststUint) SMARTattrID,
  const TU_Bln361095CPUuse(faststUint) nrmlzdCurrVal,
  const TU_Bln361095CPUuse(faststUint) nrmlzdThresh)
{
  TU_Bln361095CPUuse(faststUint) maxNrmlzdVal = TU_Bln361095::SMARTmon::
    SMARTattr::maxNrmlzdVals[SMARTattrID];
  maxNrmlzdVal = TU_Bln361095::SMARTmon::SMARTattr::getNxtGtrMaxNrmlzdVal(
    maxNrmlzdVal, nrmlzdCurrVal, nrmlzdThresh);
  const TU_Bln361095CPUuse(faststUint) divident = maxNrmlzdVal-nrmlzdThresh;
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
/**@brief GET INTO RaTinG RaNGe
  * @tparam rateRngValTyp should be a signed data type because the result
  *  might be negative? */
#ifdef __cplusplus
template<//typename rateRngValTyp,
  typename realCircaSMARTattrRawValTyp>
#endif
inline
TU_Bln361095SMARTmonUse(RtgValTyp)
  TU_Bln361095SMARTmonDef(GetIntoRtgRng)(
  const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
    realCircaSMARTattrRawValTyp) * p_bestRealCircaSMARTattrRawVal,
  const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
    realCircaSMARTattrRawValTyp) * p_worstRealCircaSMARTattrRawVal,
  const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
    realCircaSMARTattrRawValTyp) * p_realCircaSMARTattrRawVal)
{
  ///1st step: get value range
  //if()
  const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
    realCircaSMARTattrRawValTyp) realCircaSMARTattrRawValRng =
    * p_bestRealCircaSMARTattrRawVal >= * p_worstRealCircaSMARTattrRawVal ?
    * p_bestRealCircaSMARTattrRawVal - * p_worstRealCircaSMARTattrRawVal
    /**For "percent used" the best value (0 %) is the lowest possible value and
      * the worst value is 100 %.
      * So subract best value from worst value in case the worst value is higher
      * than the best value to get the range.*/
    : * p_worstRealCircaSMARTattrRawVal - * p_bestRealCircaSMARTattrRawVal;

  ///2nd step: span value range of real circa value into rating value range
  ///Example: raw value: 75, best value: 100 = 0.75
  const TU_Bln361095SMARTmonUse(RtgValTyp) ratioToBestRealSMARTattrRawVal = 
    * p_bestRealCircaSMARTattrRawVal >= * p_worstRealCircaSMARTattrRawVal ?
    (TU_Bln361095SMARTmonUse(RtgValTyp) ) * p_realCircaSMARTattrRawVal /
    (TU_Bln361095SMARTmonUse(RtgValTyp) ) * p_bestRealCircaSMARTattrRawVal
    /**For "percent used" the best value (0 %) is the lowest possible value and
      * the worst value is 100 %.
      * So divide by the worst value in case the worst value is higher
      * than the best value to get the ratio.
      * If percent used is 7 %, then 100 %-7 %=93 % ;93 %=0.93*/
    : ( (TU_Bln361095SMARTmonUse(RtgValTyp) ) * p_worstRealCircaSMARTattrRawVal
     - (TU_Bln361095SMARTmonUse(RtgValTyp) ) * p_realCircaSMARTattrRawVal )/
     (TU_Bln361095SMARTmonUse(RtgValTyp) ) * p_worstRealCircaSMARTattrRawVal
    ;
    //realCircaSMARTattrRawValRng * TU_Bln361095SMARTmonUse(BestRtgVal)
    ;

  ///Example: real círca S.M.A.R.T. attribute raw value range: [100;0]=100
  /// different values, 
  /// rating range: [1.0;-1.0]=0 to 2.0; 100/2=50
  const TU_Bln361095SMARTmonUse(RtgValTyp) scale =
    (TU_Bln361095SMARTmonUse(RtgValTyp) ) realCircaSMARTattrRawValRng / 
    TU_Bln361095SMARTmonUse(RtgValRng);

  return (//rateRngValTyp
    TU_Bln361095SMARTmonUse(RtgValTyp) )
    /// "ratioToBestRealSMARTattrRawVal" is 0.75 in our example.
    ratioToBestRealSMARTattrRawVal // / scale
    /// "RtgValRng" is 2.0 in our example
    * TU_Bln361095SMARTmonDef(RtgValRng)
    /**Absolute value of negative rating value range may be different than
      *  positive value range.
    (TU_Bln361095SMARTmonDef(RtgValRng)
    + TU_Bln361095SMARTmonUse(WorstRtgVal) ) */
    /// "WorstRtgVal" is -1.0 in our example
    - ( TU_Bln361095SMARTmonUse(WorstRtgVal) > 0 ?
    TU_Bln361095SMARTmonUse(WorstRtgVal) 
    : TU_Bln361095SMARTmonUse(WorstRtgVal) * -1);
}

/** @brief Rates according to {@code p_realCircaSMARTattrValInDegC}. This
 *   function suits at least for Hard Disk Drives because the thresholds are
 *   based on the study for them.
 *  Name comes from:
 *   Acc=ACCording: https://www.abbreviations.com/abbreviation/according
 *  
 * @tparam realCircaSMARTattrValTyp Name comes from S.M.A.R.T. ATTRibute VALue
 *  TYPe
 * @param p_realCircaSMARTattrValInDegC
 *  -Value that may have been transformed from original value/unit.
 *   Like for NVMe the original unit is Kelvin.
 *  -Supposed to be degrees Celsius in order to compare with threshold values
 *   which are in degrees Celsius
 *  -Applies to (S)ATA/NVMe:S.M.A.R.T. attribute raw value or for (S)ATA:
 *   S.M.A.R.T. attribute current normalized value */
#ifdef __cplusplus
  template<typename realCircaSMARTattrValTyp>
#endif
TU_Bln361095frcInln static
  TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) TU_Bln361095SMARTmonDef(
  RateAccDegCelsius)(
  const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
    realCircaSMARTattrValTyp) * p_realCircaSMARTattrValInDegC)
{
///https://harddrivegeek.com/ssd-temperature/

  //TODO Also see GSmartControl(confirmed:version 1.1.3 and data carrier model
  // "Samsung SSD 860 EVO M.2 500GB", firmware "RVT24B6Q") "Statistics" tab:
  // "Description","Value","Flags","Page, Offset"
  // "Specified Maximum Operating Temperature (C)","70","---","0x05, 0x058"
  // This value can be used as "TU_Bln361095dataCarrierMinCritTempInDegC".
  if( *p_realCircaSMARTattrValInDegC > TU_Bln361095dataCarrierMinCritTempInDegC)
    return TU_Bln361095SMARTmonUse(WorstRtgVal);
  if(*p_realCircaSMARTattrValInDegC > TU_Bln361095dataCarrierMinWarnTempInDegC)
  {
    const TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) currTempRtg =
      TU_Bln361095SMARTmonUse(GetIntoRtgRng)(
      /**Set best temperature to warning temperature minus difference between
        * critical and warning temperature so that the temperature rating is
        * inside warning range. */
      & (TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
        realCircaSMARTattrValTyp) )(TU_Bln361095dataCarrierMinWarnTempInDegC -
        (TU_Bln361095dataCarrierMinCritTempInDegC -
        TU_Bln361095dataCarrierMinWarnTempInDegC) ),
      ///Worst value is minimum critical temperature in degrees Celsius.
      & (TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
        realCircaSMARTattrValTyp) )TU_Bln361095dataCarrierMinCritTempInDegC,
      p_realCircaSMARTattrValInDegC
      );
    return currTempRtg;
  }
  //TODO differentiate between hard disk drive and non-spinning data carriers(
  // for example SSD)
  ///Bad if spinning hard disk drive and too cold.
  if(*p_realCircaSMARTattrValInDegC < TU_Bln361095dataCarrierMaxWarnTempInDegC)
    return TU_Bln361095SMARTmonUse(WorstRtgVal);

  return TU_Bln361095SMARTmonUse(BestRtgVal);
}

/**@brief Gets rating for NVMe S.M.A.R.T.'s "percentage used" attribute raw 
 *  value.
 * The more used(in %) the worse the state/rating:
 * -0 % used: fully OK (best rating)
 * -beginning with 50% used: state/rating "warning" till critical
 * -100 % used: fully critical (worst rating) */
#ifdef __cplusplus
  template<typename realCircaSMARTattrRawValTyp>
#endif
//static
  TU_Bln361095frcInln
    TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) TU_Bln361095SMARTmonDef(
      GetPrcntUsdRtng)
  (
    const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
      realCircaSMARTattrRawValTyp) * p_realCircaSMARTattrRawVal
  )
{
  ///Best state:(100-0)/50-1=100/50-1=2.0-1.0=1.0
  ///2nd worst state:(100-99)/50-1=1/50-1=0.02-1.0=-0.98
  const int OneHundredMinusPrcntUsd = 100 - (unsigned)
    *p_realCircaSMARTattrRawVal;
  const TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) prcntUsdRtng =
    TU_Bln361095SMARTmonUse(GetIntoRtgRng)(
    & ( (TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
      realCircaSMARTattrRawValTyp) )0),///Best value is 0 % used.
    & ( (TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
      realCircaSMARTattrRawValTyp) )100),///Worst value is 100 % used.
    p_realCircaSMARTattrRawVal
    );
  return prcntUsdRtng;
}

#ifdef __cplusplus
  template<typename realCircaSMARTattrRawValTyp>
#endif
TU_Bln361095frcInln TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)
  TU_Bln361095SMARTmonDef(AvailSpareRtg)(
  const TU_Bln361095SMARTmonUse(SMARTuniqueIDandValues) *
    p_SMARTuniqueIDandVals,
  const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
    realCircaSMARTattrRawValTyp) * p_realCircaAvailSpareRawVal
  )
{
  const TU_Bln361095SMARTmonUse(SMARTattr) * p_availSpareThreshSMARTattr =
    & p_SMARTuniqueIDandVals->m_SMARTattrs[
    TU_Bln361095dataCarrierNVMeSMARTattrUse(AvailableSpareThreshold) ];
  //TU_Bln361095::ByteArray byteArr;
  TU_Bln361095CPUuse(faststUint) availSpareThresh;
#ifdef __cplusplus
  p_availSpareThreshSMARTattr->getRawVal(/*byteArr*/availSpareThresh,
    TU_Bln361095::dataCarrier::NVMe::SMART::Attr::AvailableSpareThreshold,
    p_SMARTuniqueIDandVals->getSMARTuniqueID() );
#else
  TU_Bln361095SMARTmonUse(GetSMARTattrRawVal)(availSpareThresh,
    TU_Bln361095dataCarrierNVMeSMARTattrUse(AvailableSpareThreshold),
    TU_Bln361095SMARTmonUse(GetSMARTuniqueID)(p_SMARTuniqueIDandVals) );
#endif
    
  if( (TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)) * p_realCircaAvailSpareRawVal
    <= (TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)) availSpareThresh)
  {
    TU_Bln361095SMARTmonUse(WorstRtgVal);
  }
  const TU_Bln361095SMARTmonUse(RtgValTyp) availSpareRtg =
    TU_Bln361095SMARTmonUse(GetIntoRtgRng)(
    & ( (TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
      realCircaSMARTattrRawValTyp) )100),///Best value is 100 % spare available.
    & ( (TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
      realCircaSMARTattrRawValTyp) )availSpareThresh),///Worst value is threshold.
    p_realCircaAvailSpareRawVal
    );
  return availSpareRtg;
}

///rtg=RaTinG: http://www.abbreviations.com/abbreviation/rating
#ifdef __cplusplus
  template<typename realCircaSMARTattrRawValTyp>
#endif
  static TU_Bln361095frcInln
    TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)
  TU_Bln361095SMARTmonDef(NVMe_SMART_rtg)(
  const TU_Bln361095CPUuse(faststUint) SMARTattrID,
  const TU_Bln361095SMARTmonUse(SMARTuniqueIDandValues) *
    p_SMARTuniqueIDandVals,
  const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
    realCircaSMARTattrRawValTyp) * p_realCircaSMARTattrRawVal,
  const TU_Bln361095SMARTmonUse(ModelAndFirmware) * p_modelAndFirmware)
{
  switch( (enum TU_Bln361095dataCarrierNVMeSMARTattrUse(ID) ) SMARTattrID)
  {
  case TU_Bln361095dataCarrierNVMeSMARTattrUse(CompositeTemperature):
    return TU_Bln361095SMARTmonUse(RateAccDegCelsius)(p_realCircaSMARTattrRawVal
      );
  case TU_Bln361095dataCarrierNVMeSMARTattrUse(AvailableSpare):
    return TU_Bln361095SMARTmonUse(AvailSpareRtg)(p_SMARTuniqueIDandVals,
      p_realCircaSMARTattrRawVal);
  case TU_Bln361095dataCarrierNVMeSMARTattrUse(PercentageUsed):
    return TU_Bln361095SMARTmonUse(GetPrcntUsdRtng)(p_realCircaSMARTattrRawVal);
  case TU_Bln361095dataCarrierNVMeSMARTattrUse(MediaErrors):
  case TU_Bln361095dataCarrierNVMeSMARTattrUse(ErrorInfoLogEntryCount):
    if (*p_realCircaSMARTattrRawVal == 0)
      return (TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) )
        TU_Bln361095SMARTmonSMARTattrValRtgRsltUse(OK);
    else
      return (TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) )
        TU_Bln361095SMARTmonSMARTattrValRtgRsltUse(AtLeast1Warn);
  }
  return (TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) )
    TU_Bln361095SMARTmonSMARTattrValRtgRsltUse(Unknown);
}

/**@brief Does the S.M.A.R.T. attribute value rating according to manufacturer
 *  values.
 * Function name comes from:
 * mfr=ManuFactuRer http://www.abbreviations.com/abbreviation/manufacturer
 * Rtg=RaTinG: http://www.abbreviations.com/abbreviation/rating
 * 
 * Use preprocessor macros and pure "C" language (for example pointers instead
 *  of references) in order to be usable with a "C" compiler.*/
//static
TU_Bln361095frcInln TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)
  TU_Bln361095SMARTmonDef(mfrRtg)(
  const TU_Bln361095CPUuse(faststUint) SMARTattrID,
  const TU_Bln361095SMARTmonUse(SMARTuniqueIDandValues) *
    p_SMARTdataCarrierAndVals)
{
  const TU_Bln361095SMARTmonUse(SMARTattr) * p_SMARTattr =
    & p_SMARTdataCarrierAndVals->m_SMARTattrs[SMARTattrID];
  const TU_Bln361095CPUuse(faststUint) nrmlzdCurrVal = p_SMARTattr->
    GetNrmlzdCurrVal();
  const TU_Bln361095CPUuse(faststUint) nrmlzdThresh = p_SMARTattr->
    GetNrmlzdThresh();
  const TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) scaledQuotient =
    TU_Bln361095::SMARTmon::MfrRtg::nrmlzdCurrValAndThresh::Get1ToMns1Rng(
      SMARTattrID, nrmlzdCurrVal, nrmlzdThresh);
  return scaledQuotient;
}

/**@brief does the S.M.A.R.T. attribute value rating for (S)ATA S.M.A.R.T data.
 *  according to \p realCircaValue (from S.M.A.R.T. attribute raw value) or via
 *  \p realCircaValue
 * @param SMARTattrID use either ~"enum TU_Bln361095::dataCarrier::SMART::Attr::
 *  ID" type or (unsigned) integer and cast to this enum
 * @param p_realCircaSMARTattrValInDegC real value (in a unit) gotten from
 *  S.M.A.R.T. attribute raw value(see
http://en.wikipedia.org/wiki/Self-Monitoring,_Analysis_and_Reporting_Technology
#ATA_S.M.A.R.T._attributes) */
#ifdef __cplusplus
  template<typename RealCircaSMARTattrRawValTyp>
#endif
/*static*/ inline
TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)
  TU_Bln361095SMARTmonDef(ATA_SMART_own_Rtg)(
  const TU_Bln361095CPUuse(faststUint) SMARTattrID,
  const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon,
    RealCircaSMARTattrRawValTyp) * p_realCircaSMARTattrRawVal,
  const TU_Bln361095SMARTmonUse(ModelAndFirmware) * p_modelAndFirmware)
{
  switch( (enum TU_Bln361095dataCarrierSMARTattrUse(ID) ) SMARTattrID)
  {
    case TU_Bln361095::dataCarrier::SMART::Attr::ReadErrorRate:
    case TU_Bln361095::dataCarrier::SMART::Attr::ReallocSectorsCnt:
    case TU_Bln361095::dataCarrier::SMART::Attr::SpinUpRetryCnt:
    case TU_Bln361095::dataCarrier::SMART::Attr::RecalibRetriesOrCalibrRetryCnt:
    case TU_Bln361095::dataCarrier::SMART::Attr::SSDprogFailCnt:
    case TU_Bln361095::dataCarrier::SMART::Attr::SSDeraseFailCnt:
    case TU_Bln361095::dataCarrier::SMART::Attr::PwrLossProtectionFailure:
    case TU_Bln361095::dataCarrier::SMART::Attr::EraseFailCnt:
    case TU_Bln361095::dataCarrier::SMART::Attr::CmdTimeout:
    case TU_Bln361095::dataCarrier::SMART::Attr::HighFlyWrites:
    case TU_Bln361095::dataCarrier::SMART::Attr::EndToEndError:
    case TU_Bln361095::dataCarrier::SMART::Attr::ReportedUncorrError:
    case TU_Bln361095::dataCarrier::SMART::Attr::G_senseErrorCnt:
    case TU_Bln361095::dataCarrier::SMART::Attr::ReallocEvtCnt:
    case TU_Bln361095::dataCarrier::SMART::Attr::CurrPendSecCnt:
    case TU_Bln361095::dataCarrier::SMART::Attr::UncorrSecCnt:
    case TU_Bln361095::dataCarrier::SMART::Attr::UDMA_CRCerrorCnt:
    case TU_Bln361095::dataCarrier::SMART::Attr::MultiZoneErrorRate:
    case TU_Bln361095::dataCarrier::SMART::Attr::SoftReadErrorRateOrTACnterDetected:
    case TU_Bln361095::dataCarrier::SMART::Attr::ShockDuringWrite:
    case TU_Bln361095::dataCarrier::SMART::Attr::FreeFallEvtCnt:
      if(*p_realCircaSMARTattrRawVal == 0)
        return TU_Bln361095SMARTmonSMARTattrValRtgRsltUse(OK);
      else
        return TU_Bln361095SMARTmonSMARTattrValRtgRsltUse(AtLeast1Warn);
      break;
    case TU_Bln361095::dataCarrier::SMART::Attr::TotalDataWritten:
      {
      if(p_modelAndFirmware){///may be NULL
        //const ModelAndFirmware::remainingTotalDataWrittenType
          decltype(*p_realCircaSMARTattrRawVal) remainingTotalDataWritten =
            p_modelAndFirmware->
          getRemainingTotalDataWritten(*p_realCircaSMARTattrRawVal);
        if(remainingTotalDataWritten == TU_Bln361095::SMARTmon::ModelAndFirmware::
          maxTeraBytesWrittenNotSet)
          return TU_Bln361095SMARTmonSMARTattrValRtgRsltUse(Unknown);
      }
  //      case TotalDataWritten:
        //TODO if too many data/bytes (depending on SSD cell type (SLC/MLC/TLC)
        // /max TBW) written then show warning.
  //      case PwrCycleCnt:
        //TODO If ratio between power cycle count (diff) and power-on-hours (diff)
        // is too high then warn
      else
        return TU_Bln361095SMARTmonSMARTattrValRtgRsltUse(Unknown);
    }
    case TU_Bln361095dataCarrierSMARTattrUse(DevTemp):
    ///Temperatures for OK or warning depends on whether HDD or SSD too hot/cold.
      return TU_Bln361095SMARTmonUse(RateAccDegCelsius)(
        p_realCircaSMARTattrRawVal);
  }
}

/**@brief does the S.M.A.R.T. attribute value rating for ATA S.M.A.R.T. data
 *  according to manufacturer(S.M.A.R.T. attribute normalized and threshold
 *  value:see
http://en.wikipedia.org/wiki/Self-Monitoring,_Analysis_and_Reporting_Technology
#ATA_S.M.A.R.T._attributes) or via \p p_realCircaSMARTattrValInDegC
 * @tparam realCircaSMARTattrRawValTyp may be uint64_t if only building for
 *  (S)ATA S.M.A.R.T. (maximum 6 bytes for S.M.A.R.T. mattribute raw value)
 * @param p_realCircaSMARTattrValInDegC gotten/transformed from S.M.A.R.T.
 *  attribute raw value: for example raw value as Kelvin("K"), real value in
 *  degrees Celsius (°C) */
#ifdef __cplusplus
  template<typename realCircaSMARTattrRawValTyp>
#endif
//static
  //inline
TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)
  TU_Bln361095SMARTmonDef(ATA_SMART_rtg)(
  const TU_Bln361095CPUuse(faststUint) SMARTattrID,
  const TU_Bln361095SMARTmonUse(SMARTuniqueIDandValues) *
    p_SMARTuniqueIDandVals,
  const realCircaSMARTattrRawValTyp * p_realCircaSMARTattrRawVal,
  const TU_Bln361095SMARTmonUse(ModelAndFirmware) * p_modelAndFirmware
  )
{
  bool useManufacturerRtg = true;
  switch( (enum TU_Bln361095::dataCarrier::SMART::Attr::ID) SMARTattrID)
  {
    ///temperatures for OK or warning depends on whether HDD or SSD
    case TU_Bln361095dataCarrierSMARTattrUse(DevTemp):
/**In regard of temperatures some manufacturers like Intel for model
 * "INTEL SSDSC2BF240A5H REF", firmware "LWDi" don't follow the semantics that
 * lower normalized current values mean a worse rating: for
 * "INTEL SSDSC2BF240A5H REF" the normalized current value is simply the
 * temperature in °C. So use an own rating for temperatures.*/
      useManufacturerRtg = false;
  }
  if(useManufacturerRtg)
    return TU_Bln361095SMARTmonUse(//SMARTattrValRtng::
      mfrRtg)(SMARTattrID, p_SMARTuniqueIDandVals);
  return TU_Bln361095SMARTmonUse(ATA_SMART_own_Rtg)(SMARTattrID,
    p_realCircaSMARTattrRawVal, p_modelAndFirmware);
}

/**Name comes from:GET S.M.A.R.T. ATTRibute VALue RaTiNG
 * Use TU_Bln361095SMARTmon[...] preprocessor macros to enable both C and C++.
 * @brief get rating for @see param SMARTattrID according to current
 *   normalized value or @see realCircaRawValue
 * Is called often: for every S.M.A.R.T. ID of every data carrier in an
 *  interval (every x seconds) -> make as member function in header file (
 *  implicitely inline).
 * @return the higher the less critical.
 *  1.0:fully OK/no warning
 *  < 1.0:above or same as threshold
 *  < 0.0:exceeds threshold->critical/warning/error
 *  -1.0:worst value */
#ifdef __cplusplus
  template <typename realCircaSMARTattrRawValTyp>
#endif
//static
inline TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)
TU_Bln361095SMARTmonDef(GetSMARTattrValRtng)(
  const TU_Bln361095CPUuse(faststUint) SMARTattrID,
  const TU_Bln361095SMARTmonUse(SMARTuniqueIDandValues) *
    p_SMARTuniqueIDandVals,
  const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
    realCircaSMARTattrRawValTyp) realCircaSMARTattrRawVal,
  const TU_Bln361095SMARTmonUse(ModelAndFirmware) * p_modelAndFirmware
  )
{
  const TU_Bln361095CPUuse(faststUint) busType =
#ifdef __cplusplus
    p_SMARTuniqueIDandVals->getSMARTuniqueID().getBusType();
#else
    TU_Bln361095CPUuse(getBusTyp)(p_SMARTuniqueIDandVals);
#endif
  const bool useManufacturerRtg = busType
    /**According to struct "NVME_HEALTH_INFO_LOG" in file "nvme.h" (see
http://learn.microsoft.com/en-us/windows/win32/api/nvme/ns-nvme-nvme_health_info_log
     * ) S.M.A.R.T. via NVMe does not have normalized current, worst and
     * threshold values per attribute ID.*/
    == TU_Bln361095hardwareBusUse(NVMe) ? false : true;
  switch(busType)
  {
    case TU_Bln361095hardwareBusUse(NVMe):
      return TU_Bln361095SMARTmonUse(NVMe_SMART_rtg)(SMARTattrID,
        p_SMARTuniqueIDandVals, & realCircaSMARTattrRawVal,
        p_modelAndFirmware);
      break;
    default:
      return TU_Bln361095SMARTmonUse(ATA_SMART_rtg)(SMARTattrID,
        p_SMARTuniqueIDandVals, & realCircaSMARTattrRawVal,
        p_modelAndFirmware);
      break;
  }
  return TU_Bln361095SMARTmonSMARTattrValRtgRsltUse(Unknown);
}

TU_Bln361095SMARTmonNmSpcEnd

#endif///include guard
