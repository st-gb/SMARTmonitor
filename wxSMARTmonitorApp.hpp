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

class MyTaskBarIcon;

class wxSMARTmonitorApp
  : public wxApp
{
  wxWidgets::wxSMARTreader smartReader;
public:
  MyTaskBarIcon * m_taskBarIcon;
  wxSMARTmonitorApp();
  virtual
  ~wxSMARTmonitorApp();
  bool OnInit();
  bool GetSMARTokayIcon(wxIcon & icon);
  bool GetSMARTwarningIcon(wxIcon & icon);
};

DECLARE_APP(wxSMARTmonitorApp) //wxGetApp()

#endif /* WXSMARTMONITORAPP_HPP_ */
