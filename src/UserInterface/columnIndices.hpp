/** File: columnIndices.hpp
 * Author:Stefan Gebauer,Computer Science Master(TU Berlin matric.number 361095)
 * Created on 11.Mar 2017, 17:39 UTC+1*/
/**File location rationale:May also be used in a test or service that might show
 * something (although a service usually does not). So place this file in
 * directory "UserInterface"*/

///Include guard, see http://en.wikipedia.org/wiki/Include_guard
/**Non-standard include guard:supported by many, but not all industry compilers,
 * see http://en.wikipedia.org/wiki/Pragma_once#Portability */
#ifdef TU_Bln361095usePrgmInclGrd
  #pragma once
#endif
#if defined TU_Bln361095usePrgmInclGrd || \
  /**Include guard supported by (nearly) all industry compilers:*/\
  ! defined TU_Bln361095SMARTmon_UI_ColIndices_hpp
  #define TU_Bln361095SMARTmon_UI_ColIndices_hpp

#include <SMARTmon_ID_prefix.h>///TU_Bn361095SMARTmonNmSpcBgn

#ifdef __cplusplus
  /**Col=COLumn:http://www.abbreviations.com/abbreviation/Column*/
  #define TU_Bln361095SMARTmonColIdxsNmSpcBgn namespace colIndices{
  #define TU_Bln361095SMARTmonColIdxsNmSpcEnd }
  #define TU_Bln361095SMARTmonColIdxsDef(suffix) suffix
  #define TU_Bn361095SMARTmonColIdxsUse(suffix) colIndices :: suffix
#else
  #define TU_Bln361095SMARTmonColIdxsNmSpcBgn
  #define TU_Bln361095SMARTmonColIdxsNmSpcEnd
  #define TU_Bln361095SMARTmonColIdxsDef(suffix) \
    TU_Bn361095SMARTmonColIdxs##suffix
  #define TU_Bn361095SMARTmonColIdxsUse(suffix) \
    TU_Bn361095SMARTmonColIdxs##suffix
#endif

TU_Bln361095SMARTmonNmSpcBgn
TU_Bln361095SMARTmonColIdxsNmSpcBgn

/**Declare in a seperate header file and not in class's SMARTmonClient header
 * file because the whole class declaration of "SMARTmonitorClient" is not
 * needed when this enum is used by source code not derived from
 * "SMARTmonitorClient" such as wxWidgets::SMARTtableListCtrl.*/
enum columnIndices{
  TU_Bln361095SMARTmonColIdxsDef(SMART_ID) = 0,
  TU_Bln361095SMARTmonColIdxsDef(SMARTparameterName),
///Curr=CURRent:http://www.abbreviations.com/abbreviation/Current
  TU_Bln361095SMARTmonColIdxsDef(nrmlzdCurrVal),
///thresh=threshold:http://www.allacronyms.com/threshold/abbreviated
  TU_Bln361095SMARTmonColIdxsDef(nrmlzdThresh),
  TU_Bln361095SMARTmonColIdxsDef(rawValue),
  TU_Bln361095SMARTmonColIdxsDef(humanReadableRawVal),
  TU_Bln361095SMARTmonColIdxsDef(unit),
  TU_Bln361095SMARTmonColIdxsDef(unitRange),
  TU_Bln361095SMARTmonColIdxsDef(lastUpdate),
  TU_Bln361095SMARTmonColIdxsDef(byndLast)};

TU_Bln361095SMARTmonColIdxsNmSpcEnd
TU_Bln361095SMARTmonNmSpcEnd

#endif///! defined TU_Bln361095SMARTmon_UI_ColIndices_hpp
