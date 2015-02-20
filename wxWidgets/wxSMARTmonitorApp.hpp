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
#include "wxSMARTreader.hpp" //class wxSMARTreader
#include <UserInterface/UserInterface.hpp> //base class UserInterface

class MyTaskBarIcon;

class wxSMARTmonitorApp
  : public wxApp, UserInterface
{
  wxWidgets::wxSMARTreader smartReader;
  static unsigned s_numberOfMilliSecondsToWaitBetweenSMARTquery;
public:
  static const wxString appName;
  MyTaskBarIcon * m_taskBarIcon;
  wxSMARTmonitorApp();
  virtual
  ~wxSMARTmonitorApp();
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
