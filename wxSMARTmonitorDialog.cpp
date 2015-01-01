/*
 * wxSMARTmonitiorDialog.cpp
 *
 *  Created on: 26.11.2013
 *      Author: mr.sys
 */

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wxSMARTmonitorDialog.hpp"
#include "wxSMARTmonitorApp.hpp" //wxGetApp()
#include "wxSMARTmonitorTaskBarIcon.hpp"
#include <compiler/GCC/enable_disable_warning.h>
GCC_DIAG_OFF(write-strings)
#include "warning.xpm"
#include "smile.xpm"
GCC_DIAG_ON(write-strings)
#include <wx/listctrl.h> //class wxListCtrl
#include <hardware/CPU/atomic/AtomicExchange.h>
#include <Controller/time/GetTickCount.hpp>

// ----------------------------------------------------------------------------
// MyDialog implementation
// ----------------------------------------------------------------------------

//wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_TIMER(TIMER_ID, MyDialog::OnTimer)
    EVT_BUTTON(wxID_ABOUT, MyDialog::OnAbout)
    EVT_BUTTON(wxID_OK, MyDialog::OnOK)
    EVT_BUTTON(wxID_EXIT, MyDialog::OnExit)
    EVT_CLOSE(MyDialog::OnCloseWindow)
    EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED,
      MyDialog::OnUpdateSMARTparameterValuesInGUI)
END_EVENT_TABLE()

DWORD THREAD_FUNCTION_CALLING_CONVENTION UpdateSMARTparameterValuesThreadFunc(void * p_v)
{
  MyDialog * p_myDialog = (MyDialog *) p_v;
  if(p_myDialog)
  {
    do
    {
      p_myDialog->UpdateSMARTvaluesThreadSafe();
      //https://wiki.wxwidgets.org/Custom_Events_in_wx2.8_and_earlier#.22But_I_don.27t_need_a_whole_new_event_class....22
      wxCommandEvent MyEvent( wxEVT_COMMAND_BUTTON_CLICKED );
      wxPostEvent(p_myDialog, MyEvent);
      wxMilliSleep(1000);
    }while(1);
  }
  return 0;
}

MyDialog::MyDialog(const wxString& title, const wxWidgets::wxSMARTreader & smartReader)
  : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, //wxDefaultSize,
      wxSize(400,400),
      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
//    , m_timer(this, TIMER_ID)
  , m_oSmartReader( (wxWidgets::wxSMARTreader &) smartReader)
{
    wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

    wxSizerFlags flags;
    flags.Border(wxALL, 10);

    m_pwxlistctrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition,
      wxDefaultSize, wxLC_REPORT);
    const wxFont & textControlFont = m_pwxlistctrl->GetFont();
//    this->GetDC
//    unsigned numpixels = textControlFont.GetWidth("255");

    /** Adapted from http://wiki.wxwidgets.org/WxListCtrl#Minimal_example_to_get_started*/
    // Add first column
    wxListItem col0;
    col0.SetId(0);
    col0.SetText( _("ID") );
    col0.SetWidth(30);
    m_pwxlistctrl->InsertColumn(0, col0);

    // Add second column
    wxListItem col1;
    col1.SetId(1);
    col1.SetText( _("parameter name") );
    col1.SetWidth(200);
    m_pwxlistctrl->InsertColumn(1, col1);

    // Add third column
    col1.SetId(2);
    col1.SetText( wxT("raw value") );
    col1.SetWidth(70);
    m_pwxlistctrl->InsertColumn(2, col1);

    col1.SetId(3);
    col1.SetText( wxT("last update") );
    col1.SetWidth(100);
    m_pwxlistctrl->InsertColumn(3, col1);

    sizerTop->Add( m_pwxlistctrl,
      //"in the main orientation of the wxBoxSizer - where 0 stands for not changeable"
      1 //proportion
      , wxEXPAND //int flag=0
      , /*int border*/ 0);
//    wxSizer * const sizerListCtrl = new wxBoxSizer(wxHORIZONTAL);
//    sizerListCtrl->Add( m_pwxlistctrl, 0,
//        wxEXPAND //int flag=0
//        , /*int border*/ 0);
//    sizerTop->Add(sizerListCtrl /*, flags.Align(wxALIGN_CENTER_HORIZONTAL) */);


//    sizerTop->Add(new wxStaticText
//              (
//                this,
//                wxID_ANY,
//                wxT("Press 'Hide me' to hide this window, Exit to quit.")
//              ), flags);
//
//    sizerTop->Add(new wxStaticText
//                      (
//                        this,
//                        wxID_ANY,
//                        wxT("Double-click on the taskbar icon to show me again.")
//                      ), flags);

//    sizerTop->AddStretchSpacer()->SetMinSize(200, 50);

    wxSizer * const sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    sizerBtns->Add(new wxButton(this, wxID_ABOUT, wxT("&About")), flags);
    sizerBtns->Add(new wxButton(this, wxID_OK, wxT("&Hide")), flags);
    sizerBtns->Add(new wxButton(this, wxID_EXIT, wxT("E&xit")), flags);

    sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));
    SetSizerAndFit(sizerTop);
    Centre();

    wxIcon icon;
    if( wxGetApp().GetSMARTokayIcon(icon) )
    {
      if ( ! wxGetApp().m_taskBarIcon->SetIcon(icon, wxT("no S.M.A.R.T. problem") ) )
        wxMessageBox(wxT("Could not set new icon."), wxT("wxSMARTmonitor") );
      SetIcon(icon);
    }
//    // we should be able to show up to 128 characters on recent Windows versions
//    // (and 64 on Win9x)
//    if ( ! m_taskBarIcon->SetIcon(
//        wxICON(sample),
//         wxT("wxTaskBarIcon Sample\n"
//         "With a very, very, very, very\n"
//         "long tooltip whose length is\n"
//         "greater than 64 characters.")
//        )
//       )
//    {
//        wxLogError(wxT("Could not set icon."));
//    }

#if defined(__WXOSX__) && wxOSX_USE_COCOA
    m_dockIcon = new MyTaskBarIcon(wxTBI_DOCK);
    if ( !m_dockIcon->SetIcon(wxICON(sample)) )
    {
        wxLogError(wxT("Could not set icon."));
    }
#endif
//    m_timer.Start(1000); // 1 second interval
    ReadSMARTvaluesAndUpdateUI();
    m_updateSMARTparameterValuesThread.start(
      UpdateSMARTparameterValuesThreadFunc, this);
}

MyDialog::~MyDialog()
{
    delete wxGetApp().m_taskBarIcon;
#if defined(__WXCOCOA__)
    delete m_dockIcon;
#endif
}

void MyDialog::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    static const TCHAR * const title = _T("About wxS.M.A.R.T.monitor");
    static const TCHAR * const message
        = _T("a tool to monitor critical S.M.A.R.T. parameters\n"
          "\nsee http://en.wikipedia.org/wiki/S.M.A.R.T."
          "\n(C) 2013 by Trilobyte Software Engineering GmbH, Germany\n");

#if defined(__WXMSW__) && wxUSE_TASKBARICON_BALLOONS
    m_taskBarIcon->ShowBalloon(title, message, 15000, wxICON_INFORMATION);
#else // !__WXMSW__
    wxMessageBox(message, title, wxICON_INFORMATION | wxOK, this);
#endif // __WXMSW__/!__WXMSW__
}

void MyDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    Show(false);
}

void MyDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}

void MyDialog::OnUpdateSMARTparameterValuesInGUI(wxCommandEvent& event)
{
  for( unsigned index = 0; index < 5; ++ index)
  {
    m_pwxlistctrl->SetItem(index, 2,  wxString::Format(wxT("%u"),
        m_arRawValue[index]) );
  }
}

void MyDialog::UpdateSMARTvaluesThreadSafe()
{
  DWORD dwRetVal = m_oSmartReader.ReadSMARTValuesForAllDrives();
  if( dwRetVal == ERROR_ACCESS_DENIED )
    ;
  else
  {
    ST_DRIVE_INFO * pDriveInfo = NULL;
    ST_SMART_INFO * pSmartInfo = NULL;
    ST_SMART_DETAILS * pSmartDetails = NULL;
    unsigned ucT3;
    unsigned lineNumber = 0;
    bool atLeast1NonNullValue = false;
    //loop over drives
    long double timeInS;
    for(unsigned ucT1 = 0, ucT4 = 0; ucT1 < m_oSmartReader.m_ucDrivesWithInfo; ++ ucT1)
    {
      pDriveInfo = m_oSmartReader.GetDriveInfo(ucT1);
      for(unsigned SMARTattributeID = ucT3 = 0; SMARTattributeID < 255; ++ SMARTattributeID)
      {
        pSmartInfo = m_oSmartReader.GetSMARTValue(pDriveInfo->m_ucDriveIndex, SMARTattributeID);
        if(pSmartInfo)
        {
          pSmartDetails = m_oSmartReader.GetSMARTDetails(
            pSmartInfo->m_ucAttribIndex);
          if(pSmartDetails)
          {
            if(pSmartDetails->m_bCritical)
            {
              AtomicExchange( (long int *) & m_arRawValue[lineNumber] //long * Target
                , pSmartInfo->m_dwAttribValue /*long val*/ );
              //FileTi
//              OperatingSystem::GetTimeCountInSeconds(timeInS);

              if( pSmartInfo->m_dwAttribValue )
                atLeast1NonNullValue = true;
              ++ lineNumber;
            }
          }
        }
      }
    }
  }
}

void MyDialog::ReadSMARTvaluesAndUpdateUI()
{
  DWORD dwRetVal = m_oSmartReader.ReadSMARTValuesForAllDrives();
  if( dwRetVal == ERROR_ACCESS_DENIED )
  {
    SetTitle( wxT("wxS.M.A.R.T.monitor--access to SMART denied--restart as admin") );
    wxIcon icon;
    if( wxGetApp().GetSMARTwarningIcon(icon) )
    {
      SetIcon(icon);
    }
  }
  else
    SetTitle( wxT("wxSMARTmonitor") );

  ST_DRIVE_INFO * pDriveInfo = NULL;
  ST_SMART_INFO * pSmartInfo = NULL;
  ST_SMART_DETAILS * pSmartDetails = NULL;
  unsigned ucT3;
  m_pwxlistctrl->DeleteAllItems();
  unsigned lineNumber = 0;
  bool atLeast1NonNullValue = false;
  for(unsigned ucT1 = 0, ucT4 = 0; ucT1 < m_oSmartReader.m_ucDrivesWithInfo; ++ ucT1)
  {
    pDriveInfo = m_oSmartReader.GetDriveInfo(ucT1);
    for(unsigned SMARTattributeID = ucT3 = 0; SMARTattributeID < 255; ++ SMARTattributeID)
    {
      pSmartInfo = m_oSmartReader.GetSMARTValue(pDriveInfo->m_ucDriveIndex, SMARTattributeID);
      if(pSmartInfo)
      {
        pSmartDetails = m_oSmartReader.GetSMARTDetails(
          pSmartInfo->m_ucAttribIndex);
        if(pSmartDetails)
        {
          if(pSmartDetails->m_bCritical)
          {
            wxListItem item;
            item.SetId(lineNumber); //item line number
            item.SetText( wxString::Format(wxT("%u"),
              pSmartInfo->m_ucAttribIndex)
              );
//            m_pwxlistctrl->SetItem( item );
            m_pwxlistctrl->InsertItem( item );

            m_pwxlistctrl->SetItem(lineNumber, 1, wxString::Format( wxT("%s"),
                pSmartDetails->m_csAttribName.c_str() ) );
//            item.SetId(2);
            m_pwxlistctrl->SetItem(lineNumber, 2,  wxString::Format(wxT("%u"),
                pSmartInfo->m_dwAttribValue) );

            if( pSmartInfo->m_dwAttribValue )
              atLeast1NonNullValue = true;
//            m_pwxlistctrl->SetItem( item );
            ++ lineNumber;
          }
        }
      }
    }
  }
  if( atLeast1NonNullValue )
  {
    wxIcon icon;
    if( wxGetApp().GetSMARTwarningIcon(icon) )
    {
      if ( ! wxGetApp().m_taskBarIcon->SetIcon(icon,
        wxT("warning:\nraw value for at least 1 critical SMART parameter is > 0")))
          wxMessageBox(wxT("Could not set new icon."), wxT("wxSMARTmonitor") );
      SetIcon(icon);
    }
  }
}

void MyDialog::OnTimer(wxTimerEvent& event)
{
  ReadSMARTvaluesAndUpdateUI();
}

