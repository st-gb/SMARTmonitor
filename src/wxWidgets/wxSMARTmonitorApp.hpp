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
#include <wx/icon.h> //class wxIcon
#include <wx/timer.h> //class wxTimer

//#include "wxSMARTmonitorTaskBarIcon.hpp"
//#include <SMARTmonitorBase.hpp> //base class SMARTmonitorBase
#include <client/SMARTmonitorClient.h> //base class SMARTmonitorClient
//#include <libATA_SMART/SMARTaccess.hpp>
#include <wxWidgets/wxSMARTvalueProcessor.hpp> //class wxWidgets::wxSMARTvalueProcessor
//typedef libatasmart::SMARTaccess SMARTaccess_type;

/** Forward declarations: */
class TaskBarIcon;
class ConnectToServerDialog;

class wxSMARTmonitorApp
  : public wxApp, public /*SMARTmonitorBase*/ SMARTmonitorClient
{
  wxWidgets::wxSMARTvalueProcessor m_wxSMARTvalueProcessor;
  ConnectToServerDialog * m_pConnectToServerDialog/*(pch, timeOut )*/;
  wxTimer m_wxtimer;
public:
  enum IDs {TIMER_ID = 0};
  static wxIcon s_SMARTokIcon;
  static wxIcon s_SMARTwarningIcon;
  //SMARTaccess_type & m_SMARTaccess;
  //static const wxString appName;
  TaskBarIcon * m_taskBarIcon;
  static fastestUnsignedDataType s_GUIthreadID;
  wxSMARTmonitorApp();
  virtual
  ~wxSMARTmonitorApp();

  std::set<wxTopLevelWindow *> openTopLevelWindows;
  void AfterConnectToServer(int connectResult);
  void BeforeWait();
  void ChangeState(enum serverConnectionState newState);
  void CreateCommandLineArgsArrays();
  void CreateTaskBarIcon();
  void GetTextFromUser(const char * label, std::string & );
  bool OnInit();
  void OnTimer(wxTimerEvent& event);
//  int OnRun();
  bool GetIcon(wxIcon & icon, wxString iconFileName, char * inMemoryIcon [] );
  bool GetSMARTokayIcon(wxIcon & icon);
  bool GetSMARTwarningIcon(wxIcon & icon);
  void SetAttribute(
    const SMARTuniqueID &, 
    fastestUnsignedDataType SMARTattributeID,
    const enum ColumnIndices::columnIndices &,
    const std::string &);
  void ShowConnectionState(const char * const pch, int timeOut);
  void ShowMessage(const char * const ) const;
  wxIcon ShowSMARTokIcon();
  wxIcon ShowSMARTwarningIcon();
  void ShowIcon(const wxIcon & icon, const wxString & message );
  void ShowStateAccordingToSMARTvalues(bool b);
  void StartServiceConnectionCountDown(const fastestUnsignedDataType);
  void ReBuildUserInterface();
  
  DECLARE_EVENT_TABLE()
  void OnAfterConnectToServer(wxCommandEvent & );
  void OnShowMessage(wxCommandEvent & event);
  void OnStartServiceConnectionCountDown(wxCommandEvent & event);
};

DECLARE_APP(wxSMARTmonitorApp) //wxGetApp()

#endif /* WXSMARTMONITORAPP_HPP_ */
