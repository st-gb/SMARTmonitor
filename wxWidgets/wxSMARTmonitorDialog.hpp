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
#include <multithread/nativeThreadType.hpp>
#include <wx/thread.h> //class wxCondition
#include "SMARTtableListCtrl.hpp"

class MyTaskBarIcon;
class wxListCtrl;
using namespace wxWidgets;

class SMARTdialog: public wxDialog
{
  //  CSmartReader m_oSmartReader;
  wxWidgets::wxSMARTvalueProcessor & m_SMARTvalueProcessor;
  SMARTaccess_type & m_SMARTaccess;
  enum IDs {TIMER_ID = 0, showSupportedSMART_IDs};
  SMARTtableListCtrl * m_pwxlistctrl;
  wxTextCtrl * m_p_wxTextCtrl;
  wxButton * m_p_showSupportedSMART_IDs;
//  wxTimer m_timer;
  nativeThread_type m_updateSMARTparameterValuesThread;
public:
  wxCondition m_wxCloseCondition;
  wxMutex m_wxCloseMutex;
  fastestUnsignedDataType m_updateUI;
  SMARTdialog(const wxString& title,
    //const wxWidgets::wxSMARTvalueProcessor & wxSMARTvalueProcessor
    const SMARTvalueProcessorBase & SMARTvalueProcessor);
  virtual ~SMARTdialog();
  void StartAsyncUpdateThread();
  void EndUpdateUIthread();
  void UpdateSMARTvaluesUI();
  void UpdateUIregarding1DataCarrierOnly();
  void SetSMARTattribIDandNameLabel();
  void SetSMARTdriveID();

protected:
    void OnAbout(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnShowSupportedSMART_IDs(wxCommandEvent& WXUNUSED(event));
    void OnUpdateSMARTparameterValuesInGUI(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void OnTimer(wxTimerEvent& event);
    void ReadSMARTvaluesAndUpdateUI();

#if defined(__WXOSX__) && wxOSX_USE_COCOA
    MyTaskBarIcon   *m_dockIcon;
#endif

    DECLARE_EVENT_TABLE()
};


#endif /* WXSMARTMONITORDIALOG_HPP_ */
