/* wxSMARTmonitorApp.hpp
 *  Created on: 26.11.2013
 *  Author: Stefan Gebauer, M. Sc. Comp. Sc. */

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
//typedef libatasmart::SMARTaccess SMARTaccess_type;
#include "wxSMARTmonitorDialog.hpp"///class SMARTdialog

/*static*/ extern SMARTdialog * gs_dialog;

/** Forward declarations: */
class TaskBarIcon;
class ConnectToServerDialog;

class wxSMARTmonitorApp
  : public wxApp, public /*SMARTmonitorBase*/ SMARTmonitorClient
{
public:
  wxTimer m_wxtimer;
  ConnectToServerDialog * m_p_cnnctToSrvDlg/*(pch, timeOut )*/;
  enum IDs {TIMER_ID = 0};
  static wxIcon s_SMARTokIcon;
  static wxIcon s_SMARTstatusUnknownIcon;
  static wxIcon s_SMARTwarningIcon;
  //SMARTaccess_type & m_SMARTaccess;
  //static const wxString appName;
  TaskBarIcon * m_taskBarIcon;
  std::map<fastestUnsignedDataType,const SMARTuniqueID *> m_evtID2SMARTuniqueID;
  wxSMARTmonitorApp();
  virtual
  ~wxSMARTmonitorApp();

  std::set<wxTopLevelWindow *> openTopLevelWindows;
  void AfterConnectToServer(int connectResult);
  void BeforeConnectToServer() {};
  void BeforeWait();
  void ChangeConnectionState(enum serverConnectionState newState);
  void CreateCommandLineArgsArrays();
  void CreateTaskBarIcon();
  void DisableSrvUIctrls();
  void EnableSrvUIctrls();
  void EndWaitTillCnnctTimer(){
    m_wxtimer.Stop();//stop connect timer in main window
    gs_dialog->SetTitle(GetAppDisplayName() );
  }
  void GetTextFromUser(const char * label, std::string & );
  inline wxString GetTitleInclDataSrc();
  bool OnInit();
  void OnTimer(wxTimerEvent& event);
//  int OnRun();
  bool GetIcon(wxIcon & icon, wxString iconFileName, char * inMemoryIcon [] );
  bool GetSMARTokayIcon(wxIcon & icon);
  bool GetSMARTstatusUnknownIcon(wxIcon & icon);
  bool GetSMARTwarningIcon(wxIcon & icon);
  void OnStartCntDown(wxCommandEvent &);
  void SetAttribute(
    const SMARTuniqueID &,
    fastestUnsignedDataType SMARTattributeID, /**Usually the line (number) */
    //TODO exchange enum with fastestUnsignedDataType for performance?
    const enum ColumnIndices::columnIndices &,/**Usually the column (number) */
    const std::string &,
    const enum SMARTvalueRating, void * data);
  void SetCurrentAction(enum CurrentAction currAction);
  void SetGetDirectSMARTvals();
  void SetGetSMARTvalsMode(const enum GetSMARTvalsMode);
  void ShowConnectionState(const char * const pch, int timeOut);
  void ShwCnnctToSrvrDlg(const std::string &);
  void ShowMessage(const char * const ) const;
  void ShowMessage(const char * const, enum MessageType::messageTypes msg) const;
  wxIcon ShowSMARTokIcon();
  wxIcon ShowSMARTstatusUnknownIcon();
  wxIcon ShowSMARTwarningIcon();
  void ShowIcon(const wxIcon & icon, const wxString & message );
  void startCnnctCountDown();
  void ShowStateAccordingToSMARTvalues(const SMARTvalueRating
    entireSMARTvalRating);
  void StartServiceConnectionCountDown(const fastestUnsignedDataType);
  void ReBuildUserInterface();
  
  DECLARE_EVENT_TABLE()
  void OnAfterConnectToServer(wxCommandEvent &);
  void OnChangeState(wxCommandEvent &);
  void OnCnnctToSrvr(wxCommandEvent &);
  void OnShowMessage(wxCommandEvent & event);
  void OnShowCurrentAction(wxCommandEvent &);
  void OnStartServiceConnectionCountDown(wxCommandEvent & event);
};

wxString wxSMARTmonitorApp::GetTitleInclDataSrc()
{
  const wxString wxstrServiceAddress = wxWidgets::GetwxString_Inline(
    m_stdstrServiceHostName);
  return wxString::Format("wxSMARTmonitor--data from %s:%u in ca. %u ms "
    "interval",
    wxstrServiceAddress,
    m_socketPortNumber,
    GetNumberOfMilliSecondsToWaitBetweenSMARTquery() );
}

DECLARE_APP(wxSMARTmonitorApp) //wxGetApp()

#endif /* WXSMARTMONITORAPP_HPP_ */
