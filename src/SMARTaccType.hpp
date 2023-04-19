/**File name: acc=ACCess: http://www.allacronyms.com/access/abbreviated
 * @author: Stefan Gebauer(TU Berlin matriculation number 361095)
 * Created on 29. Mai 2020, 20:37 */

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
/**Bln=BerLiN: http://www.acronymfinder.com/Berlin-(Bln).html
 * mon=MONitor: http://www.abbreviations.com/abbreviation/monitor */\
 ! defined TU_Bln361095SMARTmon_SMARTaccType_hpp
   #define TU_Bln361095SMARTmon_SMARTaccType_hpp

///Stefan Gebauer's(TU Berlin mat.#361095)~"common_sourcecode" repository files:
 ///TU_Bln361095MicroSoftWindows
#include <OperatingSystem/Windows/MicrosoftWindows.h>

#ifdef /*_WIN32*/TU_Bln361095MicroSoftWindows
  ///class TU_Bln361095::(SMARTmon::)OpSys::Windows::SMARTaccess
  #include <Windows/SMARTaccess.h>
  typedef TU_Bln361095::SMARTmon::OpSys::Windows::SMARTaccess SMARTaccess_type;
#endif
#ifdef __linux__
  #include <libATA_SMART/SMARTaccess.hpp>
  typedef libatasmart::SMARTaccess SMARTaccess_type;
#endif

#endif///include guard
