/*
 * wxSMARTmonitorApp.hpp
 *
 *  Created on: 26.11.2013
 *      Author: mr.sys
 */

#ifndef WXSMARTMONITORAPP_HPP_
#define WXSMARTMONITORAPP_HPP_

#include <wx/app.h> //Base class wxApp
#include <wx/dialog.h> //class wxDialog

//#include "wxSMARTmonitorTaskBarIcon.hpp"
#include <SMARTmonitorBase.hpp> //base class SMARTmonitorBase
//#include <libATA_SMART/SMARTaccess.hpp>
#include <wxWidgets/wxSMARTvalueProcessor.hpp> //class wxWidgets::wxSMARTvalueProcessor
//typedef libatasmart::SMARTaccess SMARTaccess_type;

class MyTaskBarIcon;

class wxSMARTmonitorApp
  : public wxApp, public SMARTmonitorBase
{
  wxWidgets::wxSMARTvalueProcessor m_wxSMARTvalueProcessor;
  const wchar_t ** m_cmdLineArgStrings;
  std::wstring * m_ar_stdwstrCmdLineArgs;
  static unsigned s_numberOfMilliSecondsToWaitBetweenSMARTquery;
public:
  //SMARTaccess_type & m_SMARTaccess;
  static const wxString appName;
  MyTaskBarIcon * m_taskBarIcon;
  wxSMARTmonitorApp();
  virtual
  ~wxSMARTmonitorApp();
 
  void CreateCommandLineArgsArrays();
  void CreateTaskBarIcon();
  bool OnInit();
  bool GetSMARTokayIcon(wxIcon & icon);
  bool GetSMARTwarningIcon(wxIcon & icon);
  static unsigned GetNumberOfMilliSecondsToWaitBetweenSMARTquery() {
    return s_numberOfMilliSecondsToWaitBetweenSMARTquery;
  }
  void ShowMessage(const char * const ) const;
};

DECLARE_APP(wxSMARTmonitorApp) //wxGetApp()

#endif /* WXSMARTMONITORAPP_HPP_ */
