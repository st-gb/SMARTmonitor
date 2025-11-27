/** SMARTparamTblHdrWStr.c
 *  Created on: 25.09.2022 by Stefan Gebauer*/

///Standard/ISO C++ header files:
 #include <wchar.h>///wchar_t

///Stefan Gebauer's(TU Berlin matr.#361095)"common_sourcecode" repository files:
 #include <dataType/charStr/stringize.h>///TU_Bln361095XpndAndMakeWcharStr(...)

///_This_ repository's files:
 #include <SMARTmon_ID_prefix.h>///TU_Bln361095SMARTmonDef(...)
 #include "SMARTparamTblHdrWStr.h"///TU_Bln361095SMARTmonWidestAttrATA[...]
 /** English S.M.A.R.T. attributes(-related) values column header ASCII literal
  *  preprocessor definitions. ("TU_Bln361095SMARTmonSMARTattrTblEn[...]") */
 #include <UserInterface/enLiterals.h>
 /**enum TU_Bln361095SMARTmonUse(ATA_SMARTattrTblColIdcs),
  * TU_Bn361095SMARTmonATA_SMARTattrColIdxsUse(ByndLast) */
 #include <UserInterface/columnIndices.hpp>

/**Also for C++: if a C++ source(.cpp, .cxx,...) file (indirectly) includes
 * _this_ file. */
TU_Bln361095SMARTmonNmSpcBgn

/**These character string apply to (S)ATA S.M.A.R.T., not to NVMe S.M.A.R.T.!
 * Name comes from: ENglish ATA SMART ATTRibute TaBLe COLumn HeaDeR Wide STRings
 */
const wchar_t * const TU_Bln361095SMARTmonDef(EnATA_SMARTattrTblColHdrWstrs)[] =
{
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnID_ASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnParamNmASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnNrmlzdCurrValASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnNrmlzdThresValASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnRawValASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnHumanReadableRawValASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnRawValUnitASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnRawValUnitRngASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnLastUpdateAsUptimeASCIIlit)
};

/**These character string apply to NVMe S.M.A.R.T., not to (S)ATA S.M.A.R.T.!
 * Name comes from:ENglish NVMe SMART ATTRibute TaBLe COLumn HeaDeR Wide STRings
 */
const wchar_t * const TU_Bln361095SMARTmonDef(EnNVMeSMARTattrTblColHdrWchrStrs)
  [] =
{
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnID_ASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnParamNmASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnRawValASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnHumanReadableRawValASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnRawValUnitASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnRawValUnitRngASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonSMARTattrTblEnLastUpdateAsUptimeASCIIlit)
};

///MAXimum COLumn VALue STRing WiDTH IN PiXels
unsigned TU_Bln361095SMARTmonDef(MaxSMARTattrTblColValStrWdthInPx)[/*10*/
  TU_Bln361095SMARTmonATA_SMARTattrColIdxUse(ByndLast) ];
unsigned TU_Bln361095SMARTmonDef(MaxNVMeSMARTattrTblColValChrStrWdthInPx)[
  TU_Bln361095SMARTmonNVMeSMARTattrTblColIdxUse(ByndLast) ];

/**Identifier mame comes from: WIDEST ATA S.M.A.R.T. ATTRibute COLumn Wide
 *  CHaRacter STRingS */
const wchar_t * const TU_Bln361095SMARTmonDef(WidestATA_SMARTattrTblColWchrStrs)
  [] =
{
  TU_Bln361095XpndAndMakeWcharStr(TU_Bln361095SMARTmonWidestSMARTattrIDvalASCIIlit),
  L""/*p_SMARTattrNmWithMaxPx*/,///parameter name
  L"200",///normalized current value
  L"200",///normalized threshold
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonWidestAttrATArawValASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonWidestAttrATAhumanReadableRawValASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonWidestSMARTattrHumanReadableRawValUnitASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonWidestSMARTattrHumanReadableRawValUnitRngASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonMax2DecDigitsDaysOpSysUptimeASCIIlit)
};

/**Identifier mame comes from: WIDEST NVMe S.M.A.R.T. ATTRibute TaBLe COLumn
 *  Wide CHaRacter STRingS */
const wchar_t * const TU_Bln361095SMARTmonDef(WidestNVMeSMARTattrTblColWchrStrs)[] =
{
  TU_Bln361095XpndAndMakeWcharStr(TU_Bln361095SMARTmonWidestSMARTattrIDvalASCIIlit),
  L""/*p_SMARTattrNmWithMaxPx*/,///parameter name
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonWidestNVMeSMARTattrRawValASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonWidestNVMeSMARTattrHumanReadableRawValASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonWidestSMARTattrHumanReadableRawValUnitASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonWidestSMARTattrHumanReadableRawValUnitRngASCIIlit),
  TU_Bln361095XpndAndMakeWcharStr(
    TU_Bln361095SMARTmonMax2DecDigitsDaysOpSysUptimeASCIIlit)
  };

TU_Bln361095SMARTmonNmSpcEnd
