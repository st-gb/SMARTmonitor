/*
 * wxSMARTreader.cpp
 *
 *  Created on: 28.11.2013
 *      Author: mr.sys
 */

#include <wx/filefn.h>
#include <wx/msgdlg.h>
#include <wxWidgets/wxSMARTvalueProcessor.hpp>

namespace wxWidgets
{

  wxSMARTvalueProcessor::wxSMARTvalueProcessor()
  {
  }

  wxSMARTvalueProcessor::~wxSMARTvalueProcessor()
  {
    // TODO Auto-generated destructor stub
  }

  void wxSMARTvalueProcessor::Init() {
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
      wxString message = wxT("Error: dynamic library \n\"") + wxstrDynLibFileName +
        wxT("\"\n that's informed about changes of SMART attribute values "
        "couldn't be loaded. looked up in this dir:\n");
      message += wxstrCwd;
      /** This function must not be called from wxApp-derived class' constructor
       * because wx stuff hasn't been initialized then.
       * Call it e.g. from wxApp-derived::OnInit() */
      ::wxMessageBox(/*wxstrCwd*/ message );
    }
  }

  void wxSMARTvalueProcessor::ExecuteChangedCriticalParametersAction(
    SMARTentry & sMARTentry,
    SMARTuniqueID & sMARTuniqueID
    )
  {
//    m_wxdynlib.
    OnChangedCriticalSMARTparameters(
      sMARTentry, sMARTuniqueID);
  }

  void wxSMARTvalueProcessor::OnChangedCriticalSMARTparameters(
    SMARTentry & sMARTentry,
    SMARTuniqueID & sMARTuniqueID
    )
  {
    if( m_p_OnChangedCriticalSMARTparameters )
      /** Call function in dynamic library.*/
      m_p_OnChangedCriticalSMARTparameters(
        sMARTentry, sMARTuniqueID);
  }

	template <typename func_type> void wxSMARTvalueProcessor::PossiblyAssign(
	  const wxString & wxstrFuncName, func_type & p_function//, func_type functyp
	  )
	{
	  if( m_wxdynlib.HasSymbol( wxstrFuncName ) )
		p_function = (func_type) m_wxdynlib.GetSymbol( wxstrFuncName );
	  else
		p_function = NULL ;
	}
} /* namespace wxWidgets */
