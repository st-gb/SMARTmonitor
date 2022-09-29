/** SMARTparamTblHdrWStr.c
 *  Created on: 25.09.2022 by Stefan Gebauer*/

#include <wchar.h>///wchar_t

///Stefan Gebauer's(TU Berlin matr.#361095)"common_sourcecode" repository files:
#include <preprocessor_macros/stringize.h>///stringize(...)

///_This_ repository's files:
#include <UserInterface/enLiterals.h>///English literal defines
#include <UserInterface/columnIndices.hpp>///enum columnIndices

///English Column Header Wide Strings
wchar_t * enColHdrWstrs[] = {
  (wchar_t *)MakeWcharStr(enID_ASCIIlit),
  (wchar_t *)MakeWcharStr(enParamNmASCIIlit),
  (wchar_t *)MakeWcharStr(enNrmlzdCurrValASCIIlit),
  (wchar_t *)MakeWcharStr(enNrmlzdThresValASCIIlit),
  (wchar_t *)MakeWcharStr(enRawValASCIIlit),
  (wchar_t *)MakeWcharStr(enHumanReadableRawValASCIIlit),
  (wchar_t *)MakeWcharStr(enRawValUnitASCIIlit),
  (wchar_t *)MakeWcharStr(enRawValUnitRngASCIIlit),
  (wchar_t *)MakeWcharStr(enLastUpdateAsUptimeASCIIlit)
  };

unsigned maxColValStrWdthInPx[/*10*/TU_Bn361095SMARTmonColIdxsUse(byndLast)];
wchar_t * widestColValStr[] ={
  L"000",///ID
  L""/*p_SMARTattrNmWithMaxPx*/,///parameter name
  L"200",///normalized current value
  L"200",///normalized threshold
  L"AABBCCDDEEFFh",///raw value
  L">=~9999d3h59min59s999ms",///human readable raw value
  L">=~1h59min59s999ms",///human readable raw value unit
  L"~1h09min59s999ms-~1h59min59s999ms",///human readable raw value unit range
  L"99d23h59min59s999ms"///last update as uptime
  };
