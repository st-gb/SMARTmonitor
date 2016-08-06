/*
 * dynlib_OnChangedCriticalSMARTparameters_type.hpp
 *
 *  Created on: 30.07.2016
 *      Author: sg
 */

#ifndef DYNLIB_ONCHANGEDCRITICALSMARTPARAMETERS_TYPE_HPP_
#define DYNLIB_ONCHANGEDCRITICALSMARTPARAMETERS_TYPE_HPP_

#include <atasmart.h> //SkIdentifyParsedData

#define DYN_LIB_CALLING_CONVENTION

typedef DWORD (
  DYN_LIB_CALLING_CONVENTION
  * dynlib_OnChangedCriticalSMARTparameters_type)(
//  ST_SMART_INFO * pCurrentSMARTparameter,
  SkSmartAttributeParsedData & SkSmartAttributeParsedData,
    const /*ST_DRIVE_INFO*/ SkIdentifyParsedData & st_drive_info );

#endif /* DYNLIB_ONCHANGEDCRITICALSMARTPARAMETERS_TYPE_HPP_ */
