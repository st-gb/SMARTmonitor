/*
 * wxSMARTreader.cpp
 *
 *  Created on: 28.11.2013
 *      Author: mr.sys
 */

#include "wxSMARTreader.hpp"
#include <wx/filefn.h>
#include <wx/msgdlg.h>

namespace wxWidgets
{

  wxSMARTreader::wxSMARTreader()
  {
    wxString wxstrDynLibFileName = wxString(
      wxT("OnChangedCriticalSMARTparameters") ) +
      wxDynamicLibrary::GetDllExt();
    m_wxdynlib.Load( wxstrDynLibFileName );
    if( m_wxdynlib.IsLoaded() )
    {
      PossiblyAssign(
        wxT("OnChangedCriticalSMARTparameters"),
        m_p_OnChangedCriticalSMARTparameters);
    }
    else
    {
      wxString wxstrCwd = ::wxGetCwd();
      ::wxMessageBox(wxstrCwd);
    }
  }

  wxSMARTreader::~wxSMARTreader()
  {
    // TODO Auto-generated destructor stub
  }

  void wxSMARTreader::ExecuteChangedCriticalParametersAction(
    ST_SMART_INFO* pCurrentSMARTparameter,
    const ST_DRIVE_INFO & st_drive_info
    )
  {
//    m_wxdynlib.
    OnChangedCriticalSMARTparameters(pCurrentSMARTparameter, st_drive_info);
  }

  void wxSMARTreader::OnChangedCriticalSMARTparameters(
    ST_SMART_INFO * pCurrentSMARTparameter,
    const ST_DRIVE_INFO & st_drive_info
    )
  {
    if( m_p_OnChangedCriticalSMARTparameters )
      m_p_OnChangedCriticalSMARTparameters(pCurrentSMARTparameter, st_drive_info);
  }

	template <typename func_type> void wxSMARTreader::PossiblyAssign(
	  const wxString & wxstrFuncName, func_type & p_function//, func_type functyp
	  )
	{
	  if( m_wxdynlib.HasSymbol( wxstrFuncName ) )
		p_function = (func_type) m_wxdynlib.GetSymbol( wxstrFuncName );
	  else
		p_function = NULL ;
	}
} /* namespace wxWidgets */
