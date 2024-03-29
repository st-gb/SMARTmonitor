/*  wxSMARTmonitorDialog.hpp
 *  Created on: 26.11.2013
 *  Author: Stefan Gebauer, M.Sc. Comp. Sc. */

#ifndef WXSMARTMONITORDIALOG_HPP_
#define WXSMARTMONITORDIALOG_HPP_

///wxWidgets files:
#include <wx/defs.h>///wxID_HIGHEST
#include <wx/dialog.h>
#include <wx/timer.h> //class wxTimer
#include <wx/thread.h>///class wxCondition
//#ifdef _WINDOWS
//  #include "SmartReader.h" //class CSmartReader
//#endif

///This repository's files:
#include "PerDataCarrierPanel.hpp"///class PerDataCarrierPanel
#include "wxSMARTmonitorTaskBarIcon.hpp"
#include "client/SMARTmonitorClient.h" //enum SMARTmonitorClient::state

class TaskBarIcon;
class wxListCtrl;
using namespace wxWidgets;

class SMARTdialog: public wxDialog
{
  //enum IDs { CONNECT = wxID_HIGHEST + 1};
//  wxWidgets::wxSMARTvalueProcessor & m_SMARTvalueProcessor;
  enum IDs {TIMER_ID = 0, options, showSupportedSMART_IDs, CONNECT,
    directSMARTdata};
  /** Called "data carrier" and not "drive" because SMART info affect data 
   *  carriers / media and not "drives" (where they are inserted) */
  wxTextCtrl * m_p_wxMessageTextCtrl;
//  wxTimer m_timer;
  wxSizer * p_sMARTinfoSizer;
  wxButton * m_p_directSMARTaccesBtn;
public:
  typedef std::map<SMARTuniqueID, PerDataCarrierPanel *>
    SMARTuniqueID2perDataCarrierPanelType;
  SMARTuniqueID2perDataCarrierPanelType m_SMARTuniqueID2perDataCarrierPanel;
//  SMARTaccess_type & m_SMARTaccess;
  wxCondition * m_p_wxCloseCondition;
  wxMutex m_wxCloseMutex;
#ifdef TU_Bln361095useClntSrv
  ///cnct=connect:http://www.allacronyms.com/connect/abbreviated
  ///Btn=button:http://www.abbreviations.com/abbreviation/Button
  wxButton * m_p_cnctAndDiscnctBtn;
#endif
  SMARTdialog(const wxString & title//,
    //const wxWidgets::wxSMARTvalueProcessor & wxSMARTvalueProcessor
    //const SMARTvalueProcessorBase & SMARTvalueProcessor
    );
  virtual ~SMARTdialog();
  
  void buildUI();
  void DrctSMARTaccssUIctrls();
  void NoDrctSMARTaccessUIctrls();
#ifdef TU_Bln361095useClntSrv
  void UnCnnctdToSrvUIctrls();
  void DisableCnnctAndDiscnnctBtn(){
    m_p_cnctAndDiscnctBtn->Enable(false); }
#endif
  void EnableShowSupportedSMART_IDs();
  void CreatePerDiskUIctrls(wxSizer * p_sizer);
  void disableDrctSMARTaccss(const wxString & cause){
  #ifdef directSMARTaccess
    ///Is NULL if directSMARTaccess is not build into
    if(m_p_directSMARTaccesBtn){
      m_p_directSMARTaccesBtn->Disable();
      m_p_directSMARTaccesBtn->SetToolTip(cause);
    }
  #endif
  }
  void RemovePerDataCarrierPanels();
  void SetStatus(const wxString &);
  void ShowCurrentAction(const enum SMARTmonitorClient::CurrentAction);
  void StartAsyncDrctUpd8Thread();
  void EndAllThreadsAndCloseAllOtherTopLevelWindows();
  void UpdateSMARTvaluesUI();
  void UpdateUIregarding1DataCarrierOnly();
  void SetSMARTdriveID(const SMARTuniqueID &);
  void SetState(enum SMARTmonitorClient::serverConnectionState newState);
  void ReBuildUserInterface();
  void setUI(const enum SMARTmonitorClient::serverConnectionState);
  void InformAboutTerminationOfUpdateThread();
  void ShowText(/*const char * const*/ wxString & );
  void ShowMessage(
    wxString & text, 
    enum UserInterface::MessageType::messageTypes msgType);
protected:
  void OnAbout(wxCommandEvent& event);
  void OnOptions(wxCommandEvent& event);
  void OnDrctSMARTaccss(wxCommandEvent &);
  void OnOK(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnShowSupportedSMART_IDs(wxCommandEvent& WXUNUSED(event));
#ifdef TU_Bln361095useClntSrv
  void OnCnnctToSrvOrDiscnnct(wxCommandEvent& WXUNUSED(event));
#endif
  void OnReBuildUI(wxCommandEvent &);
  void OnUpdateSMARTparameterValuesInGUI(wxCommandEvent& event);
  void OnCloseWindow(wxCloseEvent& event);
  void OnTimer(wxTimerEvent& event);
  void ReadSMARTvaluesAndUpdateUI();

#if defined(__WXOSX__) && wxOSX_USE_COCOA
    TaskBarIcon   *m_dockIcon;
#endif

  DECLARE_EVENT_TABLE()
};

DECLARE_LOCAL_EVENT_TYPE(UpdateSMARTparamValsInGUIevtType, wxNewEventType())
 
#endif /* WXSMARTMONITORDIALOG_HPP_ */
