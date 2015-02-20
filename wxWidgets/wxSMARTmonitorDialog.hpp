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
#include "SmartReader.h" //class CSmartReader
#include "wxSMARTreader.hpp" //class wxSMARTreader
#include "wxSMARTmonitorTaskBarIcon.hpp"
#include <multithread/nativeThreadType.hpp>

class MyTaskBarIcon;
class wxListCtrl;

class MyDialog: public wxDialog
{
  enum columnIndices { COL_IDX_SMART_ID = 0 , COL_IDX_SMARTparameterName,
    COL_IDX_rawValue, COL_IDX_lastUpdate };
  //  CSmartReader m_oSmartReader;
  wxWidgets::wxSMARTreader & m_oSmartReader;
  enum IDs {TIMER_ID = 0};
  DWORD m_arRawValue[255];
  long int m_arTickCountOfLastQueryInMilliSeconds[255];
  wxListCtrl * m_pwxlistctrl;
//  wxTimer m_timer;
  nativeThread_type m_updateSMARTparameterValuesThread;
public:
    MyDialog(const wxString& title, const wxWidgets::wxSMARTreader & smartReader);
    virtual ~MyDialog();
    void UpdateSMARTvaluesThreadSafe();

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
