///@author Stefan Gebauer(TU Berlin matriculation number 361095)
///All stuff that is not an implementation(=no prgram code) into this file.
/// This file can then be 

///Include guard,see http://en.wikipedia.org/wiki/Include_guard
#ifdef TU_Bln361095usePrgmInclGrd
/**Non-standard include guard:supported by many, but not all industry compilers:
 * see http://en.wikipedia.org/wiki/Pragma_once#Portability */
  #pragma once
#endif
#if defined TU_Bln361095usePrgmInclGrd ||\
/**Include guard supported by (nearly) all industry compilers:*/\
/**Bln=BerLiN: http://www.acronymfinder.com/Berlin-(Bln).html
 * mon=MONitor
 * attr=ATTRibut
 * Val=VALue
 * Rtg=RaTinG
 * Decl=DECLaration */\
 ! defined TU_Bln361095SMARTmon__SMARTattrValRtgDecl_h
   #define TU_Bln361095SMARTmon__SMARTattrValRtgDecl_h

///Stefan Gebauer's(TU Berlin matriculation number:361095) ~"cmnSrc" repository:
 #include <compiler/C,C++/C,C++_unified.h>///TU_Bln361095TypenameOrTypedef(...)

///This project's files:
 #include <SMARTmon_ID_prefix.h>///TU_Bln361095SMARTmonDef(...)
 ///TU_Bln361095SMARTmonSMARTattrVal[...] preprocessor macros
 #include <attributes/SMARTattr.hpp>

#ifdef __cplusplus
  ///Rtg=RaTinG: http://www.abbreviations.com/abbreviation/rating
  #define TU_Bln361095SMARTmonSMARTattrValRtgNmSpc \
    TU_Bln361095SMARTmonSMARTattrValNmSpc :: Rtg
  #define TU_Bln361095SMARTmonSMARTattrValRtgNmSpcBgn \
    TU_Bln361095SMARTmonNmSpcBgn namespace SMARTattrVal{ namespace Rtg{
  #define TU_Bln361095SMARTmonSMARTattrValRtgNmSpcEnd \
    TU_Bln361095SMARTmonNmSpcEnd }}
  #define TU_Bln361095SMARTmonSMARTattrValRtgDef(suffix) suffix
  #define TU_Bln361095SMARTmonSMARTattrValRtgUse(suffix) \
    TU_Bln361095SMARTmonSMARTattrValRtgNmSpc :: suffix
  #define TU_Bln361095SMARTmonSMARTattrValRtgRsltDef(suffix) suffix
  #define TU_Bln361095SMARTmonSMARTattrValRtgRsltUse(suffix) \
    TU_Bln361095SMARTmonSMARTattrValRtgNmSpc :: Rslt :: suffix
#else///Compiled with a "C" compiler.
  
#endif

///Encapsulate in own namespace
//TU_Bln361095SMARTmonNmSpcBgn

#ifndef __cplusplus
 ///If using "C" we need a "typedef" because template data types can't be used.
 typedef uint64_t TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
   realCircaSMARTattrRawValTyp);
#endif

///Use plural because the rating affects multiple S.M.A.R.T. values.
TU_Bln361095SMARTmonSMARTattrValRtgNmSpcBgn

#if __cplusplus >= 201103L///"enum class" not before C++ 11  
  enum class //Rating
    Rslt
#else
#if __cplusplus
  //namespace Rating{
#endif
  enum TU_Bln361095SMARTmonSMARTattrValRtgDef(Rslt)
#endif
  {
    /**The rating as floating point value is from [1...-1] where 1 is best
     * rating (optimal), 0 is warning and -1 worst rating (error).*/
    ///first letter should be uppercase?
    TU_Bln361095SMARTmonSMARTattrValRtgRsltDef(Error)=-1,
    TU_Bln361095SMARTmonSMARTattrValRtgRsltDef(AtLeast1Warn)=0,
    TU_Bln361095SMARTmonSMARTattrValRtgRsltDef(OK)=1,
    TU_Bln361095SMARTmonSMARTattrValRtgRsltDef(NoCriticalVal),
    TU_Bln361095SMARTmonSMARTattrValRtgRsltDef(Unknown)
  };
#if __cplusplus < 201103L
  //}
#endif
TU_Bln361095SMARTmonSMARTattrValRtgNmSpcEnd

TU_Bln361095SMARTmonNmSpcBgn

/**val=VALue: http://www.abbreviations.com/abbreviation/Value
 * rtg=RaTinG: http://www.abbreviations.com/abbreviation/Rating
 * Typ=TYPe: http://www.allacronyms.com/type/abbreviated
 * Type for either current or threshold or worst attribute value [only (S)ATA
 *  S.M.A.R.T.] or raw value (all S.M.A.R.T. bus types)  */
typedef/**If type is quotient between current value and threshold:use floating
 * point type for example*/
  float TU_Bln361095SMARTmonDef(SMARTattrValRtgTyp);

/**Mfr=ManuFactureR:http://www.abbreviations.com/abbreviation/Manufacturer
 * Rtg=RaTinG:http://www.abbreviations.com/abbreviation/rating */
namespace MfrRtg{
/**nrmlzd=NoRMaLiZeD
 * Curr=CURRent:http://www.abbreviations.com/abbreviation/Current
 * Val=VALue: http://www.abbreviations.com/abbreviation/Value
 * Thresh=THRESHold: http://www.allacronyms.com/threshold/abbreviated */
namespace nrmlzdCurrValAndThresh{

TU_Bln361095frcInln TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) Get1ToMns1Rng(
  const TU_Bln361095::CPU::faststUint SMARTattrID,
  const TU_Bln361095::CPU::faststUint nrmlzdCurrVal,
  const TU_Bln361095::CPU::faststUint nrmlzdThresh);

}}///End namespaces

/**The rating value type can for example be set to float for ranges from 1 to -1
 * or for example to unsigned for 24 bit Red Green Blue(RGB) color range(only
 * red and green color compontents are needed for colors red, orange/yellow and
 * green).*/
typedef //float
  TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp) TU_Bln361095SMARTmonDef(RtgValTyp);
extern TU_Bln361095SMARTmonUse(RtgValTyp) TU_Bln361095SMARTmonDef(BestRtgVal);
extern TU_Bln361095SMARTmonUse(RtgValTyp) TU_Bln361095SMARTmonDef(
  AtLeastWarnRtgVal);
extern TU_Bln361095SMARTmonUse(RtgValTyp) TU_Bln361095SMARTmonDef(WorstRtgVal);
extern TU_Bln361095SMARTmonUse(RtgValTyp) TU_Bln361095SMARTmonDef(RtgValRng);

/**@brief Name comes from: RATE ACCording DEGrees CELSIUS */
#ifdef __cplusplus
  template<typename realCircaSMARTattrRawValTyp>
#endif
  TU_Bln361095frcInln //static
  TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)
  TU_Bln361095SMARTmonDef(RateAccDegCelsius)(
    const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
      realCircaSMARTattrRawValTyp) * p_realCircaSMARTattrRawVal);

///@brief Name comes from: GET PeRCeNT USeD RaTiNG
#ifdef __cplusplus
  template<typename realCircaSMARTattrRawValTyp>
#endif
  //static
  TU_Bln361095frcInln
  TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)
  TU_Bln361095SMARTmonDef(GetPrcntUsdRtng)(
    const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
      realCircaSMARTattrRawValTyp) * p_realCircaSMARTattrRawVal);

//static
TU_Bln361095frcInln TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)
  TU_Bln361095SMARTmonDef(mfrRtg)(
  const TU_Bln361095CPUuse(faststUint) SMARTattrID,
  const TU_Bln361095SMARTmonUse(SMARTuniqueIDandValues) *
    p_SMARTdataCarrierAndVals);

/**@brief Does a rating not based on manufacturer (S.M.A.R.T.) data/information
 *  (like (S)ATA S.M.A.R.R.T. attribute threshold data) but on this application.
 */
#ifdef __cplusplus
  template<typename realCircaSMARTattrRawValTyp>
#endif
  //static
  TU_Bln361095frcInln TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)
  TU_Bln361095SMARTmonDef(ATA_SMART_own_Rtg)(
    const TU_Bln361095CPUuse(faststUint) SMARTattrID,
    const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
      realCircaSMARTattrRawValTyp) * p_realCircaSMARTattrRawVal,
    const TU_Bln361095::SMARTmon::ModelAndFirmware * p_modelAndFirmware);

///@brief Does rating for ATA and SATA S.M.A.R.T. data carriers.
#ifdef __cplusplus
  template<typename realCircaSMARTattrRawValTyp>
#endif
  //static
  TU_Bln361095frcInln TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)
  TU_Bln361095SMARTmonDef(ATA_SMART_rtg)(
    const TU_Bln361095CPUuse(faststUint) SMARTattrID,
    const TU_Bln361095SMARTmonUse(SMARTuniqueIDandValues) * SMARTuniqueIDandVals,
    const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
      realCircaSMARTattrRawValTyp) * p_realCircaSMARTattrRawVal,
    const TU_Bln361095SMARTmonUse(ModelAndFirmware) * p_modelAndFirmware);

///@brief Does the rating for NVMe S.M.A.R.T. data carriers.
#ifdef __cplusplus
  template<typename realCircaSMARTattrRawValTyp>
#endif
  //static
  TU_Bln361095frcInln TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)
  TU_Bln361095SMARTmonDef(NVMe_SMART_rtg)(
    const TU_Bln361095CPUuse(faststUint) SMARTattrID,
    const TU_Bln361095SMARTmonUse(SMARTuniqueIDandValues) * SMARTuniqueIDandVals,
    const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_, 
      realCircaSMARTattrRawValTyp) * p_realCircaSMARTattrRawVal,
    const TU_Bln361095SMARTmonUse(ModelAndFirmware) * p_modelAndFirmware
  );

/**@brief Gets the S.M.A.R.T. attribute value rating for (S)ATA and NVMe data
 *  carriers.
 * @param p_realCircaSMARTattrRawVal should be a pointer because its type may
 *   consume much space as for example a 128 bit int from "Boost" library */
#ifdef __cplusplus
  template <typename realCircaSMARTattrRawValTyp>
#endif
  //static
  TU_Bln361095frcInln TU_Bln361095SMARTmonUse(SMARTattrValRtgTyp)
  TU_Bln361095SMARTmonDef(GetSMARTattrValRtng)(
    const TU_Bln361095CPUuse(faststUint) SMARTattrID,
    const TU_Bln361095SMARTmonUse(SMARTuniqueIDandValues) *
      p_SMARTuniqueIDandVals,
    const TU_Bln361095TypenameOrTypedef(TU_Bln361095SMARTmon_,
      realCircaSMARTattrRawValTyp) * p_realCircaSMARTattrRawVal,
    const TU_Bln361095SMARTmonUse(ModelAndFirmware) * p_modelAndFirmware);

TU_Bln361095SMARTmonNmSpcEnd

///Middle between fully OK and threshold
#define TU_Bln361095SMARFTmonSMARTattrValAbs1RngWarnThresh 0.5f

/** https://de.wikipedia.org/wiki/Self-Monitoring,_Analysis_and_Reporting_Technology#%C3%9Cbliche_Parameter
 * :Only applies to HDDs?: "Über alle Alter gemittelt, sind Temperaturen
 * unter 25 °C weit gefährlicher als solche über 40 °C."*/
/**"Failure Trends in a Large Disk Drive Population" by "Google Inc." 2007,
 *  -page 5:"In fact, there is a clear trend showing that lower temperatures are
 *   associated with higher failure rates."
 *  -page 6:"Figure 4: Distribution of average temperatures and failures rates."
 *   failures increase at ca. < 25 °C */
#define TU_Bln361095dataCarrierMaxWarnTempInDegC 25
/**http://en.wikipedia.org/wiki/Solid-state_drive#Hard_disk_drives 24 Sep 2022:
 * "A Facebook study found that at operating temperatures above 40 °C (104 °F),
 * the failure rate among SSDs increases with temperature."*/
/**"Failure Trends in a Large Disk Drive Population" by "Google Inc." 2007,
 *  -page 5:"Only at very high temperatures is there a slight reversal of this
 *   trend"
 *  -page 6:"Figure 4: Distribution of average temperatures and failures rates."
 *   failures increase at ca. > 46/47 °C */
#define TU_Bln361095dataCarrierMinWarnTempInDegC 46
#define TU_Bln361095dataCarrierMinCritTempInDegC 69

#endif///Include guard