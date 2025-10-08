/**SMARTparamTblHdrWStr.h: File name comes from:
 * param=PARAMeter
 * Tbl=TaBLe:
 * Hdr=HeaDeR
 * WStr=Wide STRing
 *
 * Created 25.09.2022 by Stefan Gebauer*/

#ifndef TU_Bln361095SMARTmon__UserInterface__SMARTattrTblHdrWstr_h
#define TU_Bln361095SMARTmon__UserInterface__SMARTattrTblHdrWstr_h

///This repository's files:
 #include <SMARTmon_ID_prefix.h>///TU_Bln361095SMARTmonDef(...)

///For both (S)ATA and NVMe S.M.A.R.T.:
#define TU_Bln361095SMARTmonWidestSMARTattrIDvalASCIIlit "000"

/**identifier comes from: ATTRibute ATA RAW VALue
 * Only applies to (S)ATA S.M.A.R.T. because there the raw value has 6 bytes.*/
#define TU_Bln361095SMARTmonWidestAttrATArawValASCIIlit "AABBCCDDEEFFh"

/**identifier comes from: ATTRibute ATA RAW VALue
 * Only applies to NVMe S.M.A.R.T. because there the raw value has 16 bytes.
 * "AABBCCDDEEFF"=6 bytes hexadecimal, "AABBCCDD"=4 bytes hexadecimal*/
#define TU_Bln361095SMARTmonWidestNVMeSMARTattrRawValASCIIlit \
  "AABBCCDDEEFFAABBCCDDEEFFAABBCCDDh"

///ATTRibute ATA HUMAN READABLE RAW VALue LITeral 2^^18 = 3,4 * 10^38
#define TU_Bln361095SMARTmonWidestNVMeSMARTattrHumanReadableRawValASCIIlit \
  "124123T123G123M123k123 MB"

///ATTRibute ATA HUMAN READABLE RAW VALue LITeral
#define TU_Bln361095SMARTmonWidestAttrATAhumanReadableRawValASCIIlit \
  ">=~9999d3h59min59s999ms"
/**ATTRibute human readable raw VALue unit
 * The widest human readable character string for all S.M.A.R.T. attributes
 * is the time. The following applies to (S)ATA and NVMe: */
#define TU_Bln361095SMARTmonWidestSMARTattrHumanReadableRawValUnitASCIIlit \
  ">=~1h59min59s999ms"
/**identifier comes from: human readable raw VALue unit RaNGe
 * The widest human readable character string for all S.M.A.R.T. attributes
 * is the time. The following applies to (S)ATA and NVMe: */
#define TU_Bln361095SMARTmonWidestSMARTattrHumanReadableRawValUnitRngASCIIlit \
  "~1h09min59s999ms-~1h59min59s999ms"
#define TU_Bln361095SMARTmonMax2DecDigitsDaysOpSysUptimeASCIIlit \
  "99d23h59min59s999ms"///last update as uptime

TU_Bln361095SMARTmonNmSpcBgn

#ifdef __cplusplus
/**Must declare as extern "C" if compiling as C++, else Microsoft Visual Studio
 * gives linker errors.
 * With "extern "C": no namespace prefixes in name?? */
//extern "C"{
#endif

///For ATA and SATA S.M.A.R.T.:
extern const wchar_t * const //TU_Bln361095SMARTmonEnATA_SMARTattrTblColHdrWstrs
  ///If called from an C++ file than it has NO prefix.
  TU_Bln361095SMARTmonDef(EnATA_SMARTattrTblColHdrWstrs)
  [];
///ENglish NVMe S.M.A.R.T. ATTRibute TaBLe COLumn HeaDeR Wide CHaRacter STRingS
extern const wchar_t * const
  //TU_Bln361095SMARTmonEnNVMeSMARTattrTblColHdrWchrStrs
  TU_Bln361095SMARTmonDef(EnNVMeSMARTattrTblColHdrWchrStrs)[];
extern unsigned TU_Bln361095SMARTmonDef(MaxSMARTattrTblColValStrWdthInPx)[];
extern const wchar_t * const TU_Bln361095SMARTmonDef(
  WidestATA_SMARTattrTblColWchrStrs)[];

#ifdef __cplusplus
//}///extern "C"
#endif

TU_Bln361095SMARTmonNmSpcEnd

#endif///Include guard
