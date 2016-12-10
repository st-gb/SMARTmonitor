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
//#include <SMARTmonitorBase.hpp> //base class SMARTmonitorBase
#include <client/SMARTmonitorClient.h> //base class SMARTmonitorClient
//#include <libATA_SMART/SMARTaccess.hpp>
#include <wxWidgets/wxSMARTvalueProcessor.hpp> //class wxWidgets::wxSMARTvalueProcessor
//typedef libatasmart::SMARTaccess SMARTaccess_type;

class MyTaskBarIcon;

class wxSMARTmonitorApp
  : public wxApp, public /*SMARTmonitorBase*/ SMARTmonitorClient
{
  wxWidgets::wxSMARTvalueProcessor m_wxSMARTvalueProcessor;
  std::string m_stdstrServerAddress;
public:
  //SMARTaccess_type & m_SMARTaccess;
  //static const wxString appName;
  MyTaskBarIcon * m_taskBarIcon;
  static fastestUnsignedDataType s_GUIthreadID;
  wxSMARTmonitorApp();
  virtual
  ~wxSMARTmonitorApp();
 
  void ChangeState(enum state newState);
  void CreateCommandLineArgsArrays();
  void CreateTaskBarIcon();
  bool OnInit();
  bool GetSMARTokayIcon(wxIcon & icon);
  bool GetSMARTwarningIcon(wxIcon & icon);
  void ShowMessage(const char * const ) const;
  void SetServiceAddress(const std::string & str) {
    m_stdstrServerAddress = str;
  }
  wxIcon ShowSMARTwarningIcon();
};

DECLARE_APP(wxSMARTmonitorApp) //wxGetApp()

#endif /* WXSMARTMONITORAPP_HPP_ */
