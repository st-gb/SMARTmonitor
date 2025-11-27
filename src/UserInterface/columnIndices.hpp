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
  #define TU_Bln361095SMARTmonATA_SMARTattrColIdxNmSpcBgn \
    TU_Bln361095SMARTmonNmSpcBgn namespace ATA_SMARTattrTblColIdx{
  #define TU_Bln361095SMARTmonATA_SMARTattrColIdxNmSpcEnd \
    TU_Bln361095SMARTmonNmSpcEnd }
  #define TU_Bln361095SMARTmonATA_SMARTattrColIdxDef(suffix) suffix
  #define TU_Bln361095SMARTmonATA_SMARTattrColIdxUse(suffix) \
    TU_Bln361095SMARTmonNmSpc :: ATA_SMARTattrTblColIdx :: suffix

  #define TU_Bln361095SMARTmonNVMeSMARTattrColIdxNmSpc \
    TU_Bln361095SMARTmonNmSpc :: NVMeSMARTattrColIdx
  #define TU_Bln361095SMARTmonNVMeSMARTattrColIdxNmSpcBgn \
    TU_Bln361095SMARTmonNmSpcBgn namespace NVMeSMARTattrColIdx{
  #define TU_Bln361095SMARTmonNVMeSMARTattrColIdxNmSpcEnd \
    TU_Bln361095SMARTmonNmSpcEnd }
  #define TU_Bln361095SMARTmonNVMeSMARTattrColIdxDef(suffix) suffix
  #define TU_Bln361095SMARTmonNVMeSMARTattrColIdxUse(suffix) \
    TU_Bln361095SMARTmonNVMeSMARTattrColIdxNmSpc :: suffix
#else
  #define TU_Bln361095SMARTmonATA_SMARTattrColIdxNmSpcBgn
  #define TU_Bln361095SMARTmonATA_SMARTattrColIdxNmSpcEnd
  #define TU_Bln361095SMARTmonATA_SMARTattrColIdxDef(suffix) \
    TU_Bln361095SMARTmonColIdxs##suffix
  #define TU_Bln361095SMARTmonATA_SMARTattrColIdxUse(suffix) \
    TU_Bln361095SMARTmonColIdxs##suffix

  #define TU_Bln361095SMARTmonNVMeSMARTattrColIdxNmSpcBgn
  #define TU_Bln361095SMARTmonNVMeSMARTattrColIdxNmSpcEnd
  #define TU_Bln361095SMARTmonNVMeSMARTattrColIdxDef(suffix) \
    TU_Bln361095SMARTmonNVMeSMARTattrColIdx ## suffix
  ///ATTRibute TaBLe COLumn InDeX
  #define TU_Bln361095SMARTmonNVMeSMARTattrTblColIdxUse(suffix) \
    TU_Bln361095SMARTmonNVMeSMARTattrColIdxDef(suffix)
#endif

TU_Bln361095SMARTmonNmSpcBgn
TU_Bln361095SMARTmonATA_SMARTattrColIdxNmSpcBgn

/**Declare in a seperate header file and not in class's SMARTmonClient header
 * file because the whole class declaration of "SMARTmonitorClient" is not
 * needed when this enum is used by source code not derived from
 * "SMARTmonitorClient" such as wxWidgets::SMARTtableListCtrl.*/
enum TU_Bln361095SMARTmonDef(ATA_SMARTattrTblColIdcs)
  TU_Bln361095SMARTmonATA_SMARTattrColIdxDef(ID) = 0,
  TU_Bln361095SMARTmonATA_SMARTattrColIdxDef(Name),
///Curr=CURRent:http://www.abbreviations.com/abbreviation/Current
  TU_Bln361095SMARTmonATA_SMARTattrColIdxDef(NrmlzdCurrVal),
///thresh=threshold:http://www.allacronyms.com/threshold/abbreviated
  TU_Bln361095SMARTmonATA_SMARTattrColIdxDef(NrmlzdThresh),
  TU_Bln361095SMARTmonATA_SMARTattrColIdxDef(RawVal),
  TU_Bln361095SMARTmonATA_SMARTattrColIdxDef(HumanReadableRawVal),
  TU_Bln361095SMARTmonATA_SMARTattrColIdxDef(Unit),
  TU_Bln361095SMARTmonATA_SMARTattrColIdxDef(UnitRange),
  TU_Bln361095SMARTmonATA_SMARTattrColIdxDef(LastUpdate),
  TU_Bln361095SMARTmonATA_SMARTattrColIdxDef(ByndLast)
  };

TU_Bln361095SMARTmonATA_SMARTattrColIdxNmSpcEnd

TU_Bln361095SMARTmonNVMeSMARTattrColIdxNmSpcBgn

enum TU_Bln361095SMARTmonDef(NVMeSMARTattrTblColIdcs)
{
  TU_Bln361095SMARTmonNVMeSMARTattrColIdxDef(ID) = 0,
  TU_Bln361095SMARTmonNVMeSMARTattrColIdxDef(Name),
  TU_Bln361095SMARTmonNVMeSMARTattrColIdxDef(RawVal),
  TU_Bln361095SMARTmonNVMeSMARTattrColIdxDef(RawValUnit),
  TU_Bln361095SMARTmonNVMeSMARTattrColIdxDef(HumanReadableRawVal),
  TU_Bln361095SMARTmonNVMeSMARTattrColIdxDef(UnitRange),
  TU_Bln361095SMARTmonNVMeSMARTattrColIdxDef(LastUpdate),
  TU_Bln361095SMARTmonNVMeSMARTattrColIdxDef(ByndLast)
};

TU_Bln361095SMARTmonNVMeSMARTattrColIdxNmSpcEnd

#endif///! defined TU_Bln361095SMARTmon_UI_ColIndices_hpp
