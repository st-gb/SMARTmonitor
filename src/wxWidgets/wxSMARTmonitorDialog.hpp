/*  wxSMARTmonitorDialog.hpp
 *  Created on: 26.11.2013
 *      Author: mr.sys  */

#ifndef WXSMARTMONITORDIALOG_HPP_
#define WXSMARTMONITORDIALOG_HPP_

#include <wx/dialog.h>
#include <wx/timer.h> //class wxTimer
//#ifdef _WINDOWS
//  #include "SmartReader.h" //class CSmartReader
//#endif
#include <wxWidgets/wxSMARTvalueProcessor.hpp> //class wxSMARTreader
#include "wxSMARTmonitorTaskBarIcon.hpp"
#include "client/SMARTmonitorClient.h" //enum SMARTmonitorClient::state
#include <wx/thread.h> //class wxCondition
#include "SMARTtableListCtrl.hpp"
#include <wx/defs.h> //wxID_HIGHEST

class TaskBarIcon;
class wxListCtrl;
using namespace wxWidgets;

class SMARTdialog: public wxDialog
{
  //enum IDs { CONNECT = wxID_HIGHEST + 1};
//  wxWidgets::wxSMARTvalueProcessor & m_SMARTvalueProcessor;
  enum IDs {TIMER_ID = 0, showSupportedSMART_IDs, CONNECT};
  /** Called "data carrier" and not "drive" because SMART info affect data 
   *  carriers / media and not "drives" (where they are inserted) */
  wxTextCtrl * m_p_wxDataCarrierIDtextCtrl;
  wxTextCtrl * m_p_wxMessageTextCtrl;
  wxButton * m_p_showSupportedSMART_IDs;
//  wxTimer m_timer;
public:
  SMARTtableListCtrl * m_pwxlistctrl;
//  SMARTaccess_type & m_SMARTaccess;
  wxCondition * m_p_wxCloseCondition;
  wxMutex m_wxCloseMutex;
  wxButton * m_p_ConnectAndDisconnectButton;
  SMARTdialog(const wxString & title//,
    //const wxWidgets::wxSMARTvalueProcessor & wxSMARTvalueProcessor
    //const SMARTvalueProcessorBase & SMARTvalueProcessor
    );
  virtual ~SMARTdialog();
  
  wxSizer * CreatePerDiskUIctrls();
  void SetStatus(const wxString &);
  void StartAsyncDrctUpd8Thread();
  void EndAllThreadsAndCloseAllOtherTopLevelWindows();
  void UpdateSMARTvaluesUI();
  void UpdateUIregarding1DataCarrierOnly();
  void SetSMARTdriveID();
  void SetState(enum SMARTmonitorClient::serverConnectionState newState);
  void ReBuildUserInterface();
  void EnableServerInteractingControls(int );
  void InformAboutTerminationOfUpdateThread();
  void ShowText(/*const char * const*/ wxString & );
  void ShowMessage(
    wxString & text, 
    enum UserInterface::MessageType::messageTypes msgType);
protected:
  void OnAbout(wxCommandEvent& event);
  void OnOK(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnShowSupportedSMART_IDs(wxCommandEvent& WXUNUSED(event));
  void OnConnectToServer(wxCommandEvent& WXUNUSED(event));
  void OnUpdateSMARTparameterValuesInGUI(wxCommandEvent& event);
  void OnCloseWindow(wxCloseEvent& event);
  void OnTimer(wxTimerEvent& event);
  void ReadSMARTvaluesAndUpdateUI();

#if defined(__WXOSX__) && wxOSX_USE_COCOA
    TaskBarIcon   *m_dockIcon;
#endif

  DECLARE_EVENT_TABLE()
};

DECLARE_LOCAL_EVENT_TYPE(UpdateSMARTparameterValuesInGUIEventType, wxNewEventType())
 
#endif /* WXSMARTMONITORDIALOG_HPP_ */
