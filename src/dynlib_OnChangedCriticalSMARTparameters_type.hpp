/*
 * dynlib_OnChangedCriticalSMARTparameters_type.hpp
 *
 *  Created on: 30.07.2016
 *      Author: sg
 */

#ifndef DYNLIB_ONCHANGEDCRITICALSMARTPARAMETERS_TYPE_HPP_
#define DYNLIB_ONCHANGEDCRITICALSMARTPARAMETERS_TYPE_HPP_

//#include <atasmart.h> //SkIdentifyParsedData
#include <attributes/SMARTattrDef.hpp>///class SMARTattrDef
#include <attributes/SMARTuniqueID.hpp> //class SMARTuniqueID

#define DYN_LIB_CALLING_CONVENTION

typedef DWORD (
  DYN_LIB_CALLING_CONVENTION
  * dynlib_OnChangedCriticalSMARTparameters_type)(
      SMARTattrDef & sMARTattrDef,
      SMARTuniqueID & sMARTuniqueID
  );

#endif /* DYNLIB_ONCHANGEDCRITICALSMARTPARAMETERS_TYPE_HPP_ */
