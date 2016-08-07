/*
 * wxSMARTmonitorApp.hpp
 *
 *  Created on: 26.11.2013
 *      Author: mr.sys
 */

#ifndef WXSMARTMONITORAPP_HPP_
#define WXSMARTMONITORAPP_HPP_

#include <wx/app.h>
#include <wx/dialog.h> //class wxDialog
//#include "wxSMARTmonitorTaskBarIcon.hpp"
#include <UserInterface/UserInterface.hpp> //base class UserInterface

#include <libATA_SMART/SMARTaccess.hpp>
#include <wxWidgets/wxSMARTvalueProcessor.hpp> //class wxSMARTreader
//typedef libatasmart::SMARTaccess SMARTaccess_type;

class MyTaskBarIcon;

class wxSMARTmonitorApp
  : public wxApp, UserInterface
{
  wxWidgets::wxSMARTvalueProcessor m_wxSMARTvalueProcessor;
  static unsigned s_numberOfMilliSecondsToWaitBetweenSMARTquery;
public:
  SMARTaccess_type & m_SMARTaccess;
  static const wxString appName;
  MyTaskBarIcon * m_taskBarIcon;
  wxSMARTmonitorApp();
  virtual
  ~wxSMARTmonitorApp();
  bool OnInit();
  void ConstructConfigFilePath(std::wstring & stdwstrConfigPathWithoutExtension);
  bool GetSMARTokayIcon(wxIcon & icon);
  bool GetSMARTwarningIcon(wxIcon & icon);
  static unsigned GetNumberOfMilliSecondsToWaitBetweenSMARTquery() {
    return s_numberOfMilliSecondsToWaitBetweenSMARTquery;
  }
  void ShowMessage(const char * const ) const;
};

DECLARE_APP(wxSMARTmonitorApp) //wxGetApp()

#endif /* WXSMARTMONITORAPP_HPP_ */
