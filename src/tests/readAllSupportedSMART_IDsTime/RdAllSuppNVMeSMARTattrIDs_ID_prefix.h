/**(c)from 2025 Stefan Gebauer,Computer Science Master(from TU Berlin)
 * @author Stefan Gebauer(TU Berlin matriculation number 361095)
 * File name should contain "SMARTmon" to easier chose the correct file if
 * multiple "[...]ID_prefix.h" files are open in the same application.*/

///Include guard,see http://en.wikipedia.org/wiki/Include_guard :

/**Bln=BerLiN:http://www.acronymfinder.com/Berlin-(Bln).html
 * Prgm=PRaGMa
 * Incl=INCLude:http://www.abbreviations.com/abbreviation/include
 * Grd=GuaRD:http://www.abbreviations.com/abbreviation/guard */
#ifdef TU_Bln361095usePrgmInclGrd
/**Non-standard include guard:supported by many, but not all industry compilers:
 * see http://en.wikipedia.org/wiki/Pragma_once#Portability */
  #pragma once
#endif
#if defined TU_Bln361095usePrgmInclGrd ||\
/**Include guard supported by (nearly) all industry compilers:*/\
/**Bln=BerLiN:http://www.acronymfinder.com/Berlin-(Bln).html
 * mon=MONitor: http://www.abbreviations.com/abbreviation/monitor
 * ID=identifier:http://www.abbreviations.com/abbreviation/identifier */\
 !defined TU_Bln361095SMARTmon__tests__RdAllSuppNVMeSMARTattrIDs_ID_prefix_h
  #define TU_Bln361095SMARTmon__tests__RdAllSuppNVMeSMARTattrIDs_ID_prefix_h

#include "../SMARTmon_tests_ID_prefix.h"

#ifdef __cplusplus
  #define TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsNmSpc \
    TU_Bln361095SMARTmonTstsNmSpc :: RdAllSuppNVMeSMARTattrIDs
  #define TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsNmSpcBgn \
    TU_Bln361095SMARTmonTstsNmSpcBgn namespace RdAllSuppNVMeSMARTattrIDs{
  #define TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsNmSpcEnd \
    TU_Bln361095SMARTmonTstsNmSpcEnd }
  #define TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsDef(l) l
  #define TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsUse(suffix) \
    TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsNmSpc :: suffix
  
  #define TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdDef(exitCd) \
    /* progExit ## ExitCd :: */ exitCd
  #define TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdUse(exitCd) \
    TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsNmSpc :: ExitCd :: exitCd
  #define TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdNmSpcBgn \
    TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsNmSpcBgn namespace ExitCd{
  #define TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdNmSpcEnd \
    TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsNmSpcEnd }
#else
#endif

TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdNmSpcBgn
enum TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsDef(progExitCds) {
  TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdDef(Success) = 0,
  TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdDef(
    GetSuppSMART_IDsFaild),
  TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdDef(OpnDtCarrierFaild)
};
TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdNmSpcEnd

#endif///include guard