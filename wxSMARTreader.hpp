/*
 * wxSMARTreader.hpp
 *
 *  Created on: 28.11.2013
 *      Author: mr.sys
 */

#ifndef WXSMARTREADER_HPP_
#define WXSMARTREADER_HPP_

#include <wx/dynlib.h>
#include "SmartReader.h" //class CSmartReader
//#include <Controller/CPUcontrollerDynLib/calling_convention.h>

#define DYN_LIB_CALLING_CONVENTION

typedef DWORD (
  DYN_LIB_CALLING_CONVENTION
  * dynlib_OnChangedCriticalSMARTparameters_type)(
	ST_SMART_INFO * pCurrentSMARTparameter,
    const ST_DRIVE_INFO & st_drive_info );

namespace wxWidgets
{

  class wxSMARTreader : public CSmartReader
  {
    dynlib_OnChangedCriticalSMARTparameters_type m_p_OnChangedCriticalSMARTparameters;
    wxDynamicLibrary m_wxdynlib;
  public:
    wxSMARTreader();
    virtual
    ~wxSMARTreader();

    void ExecuteChangedCriticalParametersAction(
      ST_SMART_INFO * pCurrentSMARTparameter,
      const ST_DRIVE_INFO & st_drive_info
      );
    void OnChangedCriticalSMARTparameters(
      ST_SMART_INFO * pCurrentSMARTparameter,
      const ST_DRIVE_INFO & st_drive_info);

	/** @brief template function for ease to assign function pointers from a dyn lib
	 *  @param p_function must be passed in order to get its type */
	template <typename func_type> void PossiblyAssign(
	  const wxString & wxstrFuncName, func_type & p_function//, func_type functyp
	  );
  };
} /* namespace wxWidgets */

#endif /* WXSMARTREADER_HPP_ */
