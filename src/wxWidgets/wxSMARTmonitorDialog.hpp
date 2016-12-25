/*
 * wxSMARTmonitorDialog.hpp
 *
 *  Created on: 26.11.2013
 *      Author: mr.sys
 */

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
#include <OperatingSystem/multithread/nativeThreadType.hpp>
#include <wx/thread.h> //class wxCondition
#include "SMARTtableListCtrl.hpp"
#include <wx/defs.h> //wxID_HIGHEST

class TaskBarIcon;
class wxListCtrl;
using namespace wxWidgets;

class SMARTdialog: public wxDialog
{
  //enum IDs { CONNECT = wxID_HIGHEST + 1};
  wxWidgets::wxSMARTvalueProcessor & m_SMARTvalueProcessor;
  enum IDs {TIMER_ID = 0, showSupportedSMART_IDs, CONNECT};
  SMARTtableListCtrl * m_pwxlistctrl;
  wxTextCtrl * m_p_wxTextCtrl;
  wxButton * m_p_showSupportedSMART_IDs;
//  wxTimer m_timer;
  nativeThread_type m_updateSMARTparameterValuesThread;
  static bool s_atLeast1CriticalNonNullValue;
public:
  SMARTaccess_type & m_SMARTaccess;
  wxCondition * m_p_wxCloseCondition;
  wxMutex m_wxCloseMutex;
  wxButton * m_p_ConnectAndDisconnectButton;
  fastestUnsignedDataType m_updateUI;
  SMARTdialog(const wxString & title,
    //const wxWidgets::wxSMARTvalueProcessor & wxSMARTvalueProcessor
    const SMARTvalueProcessorBase & SMARTvalueProcessor);
  virtual ~SMARTdialog();
  
  void StartAsyncUpdateThread();
  void EndUpdateUIthread();
  void UpdateSMARTvaluesUI();
  void UpdateUIregarding1DataCarrierOnly();
  void SetSMARTattribIDandNameLabel();
  void SetSMARTdriveID();
  void SetState(enum SMARTmonitorClient::state newState);
  void ReBuildUserInterface();
  void InformAboutTerminationOfUpdateThread();

protected:
  void OnAbout(wxCommandEvent& event);
  void OnOK(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnShowSupportedSMART_IDs(wxCommandEvent& WXUNUSED(event));
  void ConnectToServer(wxCommandEvent& WXUNUSED(event));
  void OnUpdateSMARTparameterValuesInGUI(wxCommandEvent& event);
  void OnCloseWindow(wxCloseEvent& event);
  void OnTimer(wxTimerEvent& event);
  void ReadSMARTvaluesAndUpdateUI();
  void ShowStateAccordingToSMARTvalues(bool b);
  inline void UpdateTimeOfSMARTvalueRetrieval(
    unsigned lineNumber,
    long int timeStampOfRetrieval);

#if defined(__WXOSX__) && wxOSX_USE_COCOA
    TaskBarIcon   *m_dockIcon;
#endif

    DECLARE_EVENT_TABLE()
};


#endif /* WXSMARTMONITORDIALOG_HPP_ */
