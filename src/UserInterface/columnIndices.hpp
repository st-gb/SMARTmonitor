/** File: columnIndices.hpp
 * Author:Stefan Gebauer,Computer Science Master(TU Berlin matric.number 361095)
 * Created on 11.Mar 2017, 17:39 UTC+1*/
/**File location rationale:May also be used in a test or service that might show
 * something (although a service usually does not). So place this file in
 * directory "UserInterface"*/

///Include guard, see http://en.wikipedia.org/wiki/Include_guard
/**Non-standard include guard:supported by many, but not all industry compilers,
 * see http://en.wikipedia.org/wiki/Pragma_once#Portability */
#ifdef TU_Bln361095usePrgmInclGuard
#pragma once
#endif
///Include guard supported by (nearly) all industry compilers:
#ifndef TU_Bln361095SMARTmon_UI_ColumnIndices_hpp
#define TU_Bln361095SMARTmon_UI_ColumnIndices_hpp

namespace ColumnIndices
{
/**Declare in a seperate header file and not in class's SMARTmonClient header
 * file because the whole class declaration of "SMARTmonitorClient" is not
 * needed when this enum is used by source code not derived from
 * "SMARTmonitorClient" such as wxWidgets::SMARTtableListCtrl.*/
enum columnIndices{SMART_ID = 0, SMARTparameterName,
///Curr=CURRent:http://www.abbreviations.com/abbreviation/Current
  nrmlzdCurrVal,
///thresh=threshold:http://www.allacronyms.com/threshold/abbreviated
  nrmlzdThresh,
  rawValue, humanReadableRawValue, unit, unitRange, lastUpdate, beyondLast};
}

#endif///TU_Bln361095SMARTmon_UI_ColumnIndices_hpp
