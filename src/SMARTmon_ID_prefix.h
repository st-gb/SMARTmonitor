/**(c)from 2022 Stefan Gebauer,Computer Science Master(from TU Berlin)
 * Created 28.09.2022
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
 !defined TU_Bln361095SMARTmon_ID_prefix_h
  #define TU_Bln361095SMARTmon_ID_prefix_h

#ifdef __cplusplus
/**The following definitions can/should also be used in C++ because so one can
 * see whether the namespace exists(in an IDE via highlighting) and ensure that
 * everywhere the same namespace is used without a typo.*/

  #define TU_Bln361095SMARTmonDef(suffix) suffix
  #define TU_Bln361095SMARTmonNmSpc TU_Bln361095::SMARTmon
  #define TU_Bln361095SMARTmonNmSpcBgn namespace TU_Bln361095{namespace \
    SMARTmon{
  #define TU_Bln361095SMARTmonNmSpcEnd }}
  #define TU_Bln361095SMARTmonUse(suffix) TU_Bln361095SMARTmonNmSpc :: suffix
#else///File is compiled with a C compiler.
  #define TU_Bln361095SMARTmonDef(suffix) TU_Bln361095SMARTmon ## suffix
  #define TU_Bln361095SMARTmonNmSpcBgn
  #define TU_Bln361095SMARTmonNmSpcEnd
  /**Using the preprocessor macro for definition in the usage ensures that both
   * declaration and usage have the identical name.*/
  #define TU_Bln361095SMARTmonUse(suffix) TU_Bln361095SMARTmonDef(suffix)
#endif

#endif///include guard
