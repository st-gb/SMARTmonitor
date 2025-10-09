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
 !defined TU_Bln361095SMARTmon__tests__ID_prefix_h
  #define TU_Bln361095SMARTmon__tests__ID_prefix_h

#include "../SMARTmon_ID_prefix.h"

#define TU_Bln361095SMARTmonTstsNmSpcBgn \
  TU_Bln361095SMARTmonNmSpcBgn namespace Tsts{
#define TU_Bln361095SMARTmonTstsNmSpc TU_Bln361095SMARTmonNmSpc :: Tsts
#define TU_Bln361095SMARTmonTstsNmSpcEnd TU_Bln361095SMARTmonNmSpcEnd }

#endif///include guard