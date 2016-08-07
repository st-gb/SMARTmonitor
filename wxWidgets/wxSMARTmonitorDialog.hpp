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
#include "SMARTtableListCtrl.hpp"

class MyTaskBarIcon;
class wxListCtrl;
using namespace wxWidgets;

class MyDialog: public wxDialog
{
  //  CSmartReader m_oSmartReader;
  wxWidgets::wxSMARTvalueProcessor & m_SMARTvalueProcessor;
  SMARTaccess_type & m_SMARTaccess;
  enum IDs {TIMER_ID = 0};
  DWORD m_arRawValue[255]; //provide space for up t 255 SMART attribute values
  long int m_arTickCountOfLastQueryInMilliSeconds[255];
  SMARTtableListCtrl * m_pwxlistctrl;
//  wxTimer m_timer;
  nativeThread_type m_updateSMARTparameterValuesThread;
public:
    MyDialog(const wxString& title,
     const wxWidgets::wxSMARTvalueProcessor & wxSMARTvalueProcessor);
    virtual ~MyDialog();
    void UpdateSMARTvaluesThreadSafe();
    void StartAsyncUpdateThread();

protected:
    void OnAbout(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
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
