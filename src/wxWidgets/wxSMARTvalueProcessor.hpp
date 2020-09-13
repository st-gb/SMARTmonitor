/** wxSMARTreader.hpp
 *  Created on: 28.11.2013
 *  Author: Stefan Gebauer, M.Sc.Comp.Sc.*/

#ifndef WXSMARTREADER_HPP_
#define WXSMARTREADER_HPP_

#include <wx/dynlib.h>
//#include <Controller/CPUcontrollerDynLib/calling_convention.h>
#include <preprocessor_macros/Windows_compatible_typedefs.h> //DWORD
#include "dynlib_OnChangedCriticalSMARTparameters_type.hpp"
#include <SMARTvalueProcessorBase.hpp> //base class SMARTvalueProcessorBase

namespace wxWidgets
{

  class DynLibSMARTvalProc
    : public SMARTvalueProcessorBase
  {
    dynlib_OnChangedCriticalSMARTparameters_type m_p_OnChangedCriticalSMARTparameters;
    wxDynamicLibrary m_wxdynlib;
  public:
    DynLibSMARTvalProc();
    virtual
    ~DynLibSMARTvalProc();

    void Init();
    void ExecuteChangedCriticalParametersAction(
      SMARTattrDef & sMARTentry,
      SMARTuniqueID & sMARTuniqueID
      );
    void OnChangedSMARTparams(
      SMARTattrDef & sMARTentry,
      SMARTuniqueID & sMARTuniqueID
      );

	/** @brief template function for ease to assign function pointers from a dyn lib
	 *  @param p_function must be passed in order to get its type */
	template <typename func_type> void PossiblyAssign(
	  const wxString & wxstrFuncName, func_type & p_function//, func_type functyp
	  );
  };
} /* namespace wxWidgets */

#endif /* WXSMARTREADER_HPP_ */
