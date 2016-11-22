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
//#include "warning.xpm"
//#include "smile.xpm"
GCC_DIAG_ON(write-strings)
#include <wx/listctrl.h> //class wxListCtrl
#include <hardware/CPU/atomic/AtomicExchange.h>
#include <Controller/time/GetTickCount.hpp>
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
#include <wxWidgets/SupportedSMARTIDsDialog.hpp>
#include <sstream> //ostringstream
#include <wx/textdlg.h> //wxGetTextFromUser

//extern wxSMARTmonitorApp theApp;

// ----------------------------------------------------------------------------
// MyDialog implementation
// ----------------------------------------------------------------------------

//wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
BEGIN_EVENT_TABLE(SMARTdialog, wxDialog)
    EVT_TIMER(TIMER_ID, SMARTdialog::OnTimer)
    EVT_BUTTON(wxID_ABOUT, SMARTdialog::OnAbout)
    EVT_BUTTON(wxID_OK, SMARTdialog::OnOK)
    EVT_BUTTON(wxID_EXIT, SMARTdialog::OnExit)
    EVT_BUTTON(showSupportedSMART_IDs, SMARTdialog::OnShowSupportedSMART_IDs)
    EVT_BUTTON(CONNECT, SMARTdialog::ConnectToServer)
    EVT_CLOSE(SMARTdialog::OnCloseWindow)
    EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED,
      SMARTdialog::OnUpdateSMARTparameterValuesInGUI)
END_EVENT_TABLE()

DWORD THREAD_FUNCTION_CALLING_CONVENTION wxUpdateSMARTparameterValuesThreadFunc(void * p_v)
{
  SMARTdialog * p_myDialog = (SMARTdialog *) p_v;
  const unsigned numberOfMilliSecondsToWaitBetweenSMARTquery =
    wxGetApp().GetNumberOfMilliSecondsToWaitBetweenSMARTquery();
  fastestUnsignedDataType numberOfSecondsToWaitBetweenSMARTquery;
  //TODO set to false when connected
  bool getSMARTvaluesDirectly = false;
  if(p_myDialog)
  {
    std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValuesSet = 
      p_myDialog->m_SMARTaccess.GetSMARTuniqueIDandValues();
    do
    {
      if( getSMARTvaluesDirectly )
      {
        /*p_myDialog->*/wxGetApp().UpdateSMARTvaluesThreadSafe();

        numberOfSecondsToWaitBetweenSMARTquery =
          numberOfMilliSecondsToWaitBetweenSMARTquery / 1000;
        while( numberOfSecondsToWaitBetweenSMARTquery -- && p_myDialog->m_updateUI)
        {
          //TODO handle closing of window / app
          wxSleep(1);
        }
        wxMilliSleep(numberOfMilliSecondsToWaitBetweenSMARTquery % 1000 );
      }
      else
      {
        int res = wxGetApp().GetSMARTvaluesFromServer(sMARTuniqueIDandValuesSet);
        if( res == 0 )
          p_myDialog->UpdateSMARTvaluesUI();
      }
      
      //https://wiki.wxwidgets.org/Custom_Events_in_wx2.8_and_earlier#.22But_I_don.27t_need_a_whole_new_event_class....22
      wxCommandEvent MyEvent( wxEVT_COMMAND_BUTTON_CLICKED );
      wxPostEvent(p_myDialog, MyEvent);
    }while(p_myDialog->m_updateUI);
    LOGN("locking close mutex");
    /** see http://docs.wxwidgets.org/trunk/classwx_condition.html */
    wxMutexLocker lock(p_myDialog->m_wxCloseMutex);
    LOGN("sending close event");
    p_myDialog->m_wxCloseCondition.Broadcast();
  }
  return 0;
}

SMARTdialog::SMARTdialog(
  const wxString & title,
   //const wxWidgets::wxSMARTvalueProcessor & SMARTvalueProcessor
    const SMARTvalueProcessorBase & SMARTvalueProcessor
  )
  : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, //wxDefaultSize,
      wxSize(400,400),
      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
//    , m_timer(this, TIMER_ID)
  , m_SMARTvalueProcessor( (wxWidgets::wxSMARTvalueProcessor &) SMARTvalueProcessor)
  , m_SMARTaccess(SMARTvalueProcessor.getSMARTaccess() )
  , m_updateUI(1)
  , m_wxCloseCondition(m_wxCloseMutex)

{
  m_wxCloseMutex.Lock();
    wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

    wxSizerFlags flags;
    flags.Border(wxALL, 10);

    m_p_wxTextCtrl = new wxTextCtrl(this, wxID_ANY, wxT("dd") );
    m_p_wxTextCtrl->SetEditable(false);

    wxSizer * const diskIDsizer = new wxBoxSizer(wxHORIZONTAL);

    m_p_showSupportedSMART_IDs = new wxButton(this, showSupportedSMART_IDs, wxT("show supported SMART IDs"));

    diskIDsizer->Add( m_p_wxTextCtrl,
      /** "in the main orientation of the wxBoxSizer - where 0 stands for not
       * changeable" */
      1,
      wxEXPAND, 0);
    diskIDsizer->Add( m_p_showSupportedSMART_IDs, 0, wxSTRETCH_NOT, 0);
    sizerTop->Add( diskIDsizer, 0, wxEXPAND, 0);

    m_pwxlistctrl = new SMARTtableListCtrl(this//, wxID_ANY, wxDefaultPosition,
      //wxDefaultSize, wxLC_REPORT
      );

    sizerTop->Add( m_pwxlistctrl,
      /** "in the main orientation of the wxBoxSizer - where 0 stands for not changeable" */
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
    sizerBtns->Add(new wxButton(this, CONNECT, wxT("&Connect")), flags);
    sizerBtns->Add(new wxButton(this, wxID_OK, wxT("&Hide")), flags);
    sizerBtns->Add(new wxButton(this, wxID_EXIT, wxT("E&xit")), flags);

    sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));
    SetSizerAndFit(sizerTop);
    Centre();

    wxIcon icon;
    if( wxGetApp().GetSMARTokayIcon(icon) )
    {
      if ( wxGetApp().m_taskBarIcon == NULL || ! wxGetApp().m_taskBarIcon->SetIcon(
        icon, wxT("no S.M.A.R.T. problem") ) )
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
}

SMARTdialog::~SMARTdialog()
{
  if( wxGetApp().m_taskBarIcon )
    delete wxGetApp().m_taskBarIcon;
#if defined(__WXCOCOA__)
    delete m_dockIcon;
#endif
}

void SMARTdialog::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    static const wxChar * const title = wxT("About wxS.M.A.R.T.monitor");
    static const wxChar * const message
      = _T("a tool to monitor CRITICAL S.M.A.R.T. parameters\n"
        "\nsee http://en.wikipedia.org/wiki/S.M.A.R.T."
        "\n(C) 2013-2016 by Trilobyte Software Engineering GmbH, Germany\n");

#if defined(__WXMSW__) && wxUSE_TASKBARICON_BALLOONS
    m_taskBarIcon->ShowBalloon(title, message, 15000, wxICON_INFORMATION);
#else // !__WXMSW__
    wxMessageBox(message, title, wxICON_INFORMATION | wxOK, this);
#endif // __WXMSW__/!__WXMSW__
}

void SMARTdialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    Show(false);
}

void SMARTdialog::EndUpdateUIthread()
{
  LOGN("disabling update UI (ends update UI thread)");
  /** Inform the SMART values update thread about we're going to exit,*/
  AtomicExchange( (long *) & m_updateUI, 0);
  LOGN("waiting for close event");
  m_wxCloseCondition.Wait();
}

void SMARTdialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
  EndUpdateUIthread();
  Close(true);
}

void SMARTdialog::ConnectToServer(wxCommandEvent& WXUNUSED(event))
{
  wxString wxstrServerAddress = wxGetTextFromUser(wxT("input SMART values server address") //message,
    , wxGetTextFromUserPromptStr// const wxString & 	caption = wxGetTextFromUserPromptStr,
    , wxT("localhost")//const wxString & 	default_value = wxEmptyString,
    , NULL //wxWindow * 	parent = NULL,
    );
  std::string stdstrServerAddress = wxWidgets::GetStdString_Inline(wxstrServerAddress);
  const fastestUnsignedDataType res = wxGetApp().ConnectToServer(stdstrServerAddress.c_str());
  if( res == 0)
  {
    SetTitle(wxT("data from ") + wxstrServerAddress);
    //SMARTaccess_type & sMARTaccess = m_SMARTaccess.;
    std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValues = m_SMARTaccess.
      GetSMARTuniqueIDandValues();
    /** Get # of attributes to in order build the user interface (write 
     *  attribute ID an name into the table--creating the UI needs to be done 
     *  only once because the attribute IDs received usually do not change).*/
    const int res = wxGetApp().GetSMARTvaluesFromServer(sMARTuniqueIDandValues);
    if( res == 0)
    {
      ReBuildUserInterface();
      UpdateSMARTvaluesUI();
    }
    StartAsyncUpdateThread();
  }
  else
  {
    //wxGetApp().ShowMessage("");
  }
}

void SMARTdialog::ReBuildUserInterface()
{
  SetSMARTdriveID();
  SetSMARTattribIDandNameLabel();
}

void SMARTdialog::OnShowSupportedSMART_IDs(wxCommandEvent& WXUNUSED(event))
{
  std::vector<SMARTattributeNameAndID> SMARTattributeNamesAndIDs;
  libatasmart::getSupportedSMART_IDs("/dev/sda", SMARTattributeNamesAndIDs);
  SupportedSMART_IDsDialog * p_SupportedSMART_IDsDialog = new
    SupportedSMART_IDsDialog(SMARTattributeNamesAndIDs);
  p_SupportedSMART_IDsDialog->Show(true);
}

void SMARTdialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
  EndUpdateUIthread();
  Destroy();
}

void SMARTdialog::UpdateUIregarding1DataCarrierOnly()
{
  const int listItemCount = m_pwxlistctrl->GetItemCount();
//#endif
  DWORD tickCountOfCurrentTimeInMilliSeconds;
  DWORD numberOfMilliSecondsPassedSinceLastSMARTquery;
  wxString currentTime;

  const std::set<SkSmartAttributeParsedData> & SMARTattributesToObserve =
    m_SMARTaccess.getSMARTattributesToObserve();
  std::set<SkSmartAttributeParsedData>::const_iterator
    SMARTattributesToObserveIter = SMARTattributesToObserve.begin();
  for( unsigned index = 0; index < listItemCount; ++ index,
    SMARTattributesToObserveIter ++)
  {
    m_pwxlistctrl->SetItem(
      index,
      SMARTtableListCtrl::COL_IDX_rawValue /** column #/ index */,
      wxString::Format(wxT("%u"), wxGetApp().m_arSMARTrawValue[index]) );
    LOGN( "raw value:" << wxGetApp().m_arSMARTrawValue[index] )

//    const SkSmartAttributeParsedData & skSmartAttributeParsedData = * SMARTattributesToObserveIter;
//    wxListItem listItem;
//    listItem.SetId(index);
//    listItem.SetColumn(SMARTtableListCtrl::COL_IDX_rawValue);

    if( //SMARTattributesToObserveIter->threshold < SMARTattributesToObserveIter->worst_value
        wxGetApp().m_arSMARTrawValue[index] > 0 )
//      wxListItemAttr;
      m_pwxlistctrl->SetItemBackgroundColour(index, * wxRED);
      //listItem.SetBackgroundColour(* wxRED);
    else
      m_pwxlistctrl->SetItemBackgroundColour(index, * wxGREEN);
      //listItem.SetBackgroundColour(* wxGREEN);
//    m_pwxlistctrl->SetItem(listItem);

//    tickCountOfCurrentTimeInMilliSeconds = GetTickCount();
//    numberOfMilliSecondsPassedSinceLastSMARTquery =
//      tickCountOfCurrentTimeInMilliSeconds -
//      m_arTickCountOfLastQueryInMilliSeconds[index];

    //TODO save time in UpdateSMARTvaluesThreadSafe and use this time here.
    currentTime = wxNow();

//    strftime(buffer, 80,"Now it's %I:%M%p.",timeinfo);
//    m_arTickCountOfLastQueryInMilliSeconds[index]

    m_pwxlistctrl->SetItem(
      index,
      SMARTtableListCtrl::COL_IDX_lastUpdate /** column #/ index */,
      //wxString::Format(wxT("%u ms ago"), numberOfMilliSecondsPassedSinceLastSMARTquery )
      currentTime
      );
  }
}

void SMARTdialog::UpdateSMARTvaluesUI()
{
  unsigned lineNumber = 0;
  bool atLeast1NonNullValue = false;

  const fastestUnsignedDataType numberOfDifferentDrives = m_SMARTaccess.
    GetNumberOfDifferentDrives();
  std::set<SMARTuniqueIDandValues> & SMARTuniqueIDsAndValues = m_SMARTaccess.
    GetSMARTuniqueIDandValues();
  std::set<SMARTuniqueIDandValues>::const_iterator SMARTuniqueIDandValuesIter =
    SMARTuniqueIDsAndValues.begin();
  fastestUnsignedDataType SMARTattributeID;
  uint64_t SMARTrawValue;
  const std::set<SkSmartAttributeParsedData> & SMARTattributesToObserve =
    wxGetApp().mp_SMARTaccess->getSMARTattributesToObserve();
  wxString wxSMARTattribName;
  wxString currentTime;
#ifdef DEBUG
  int itemCount = m_pwxlistctrl->GetItemCount();
#endif
  for(fastestUnsignedDataType currentDriveIndex = 0;
    SMARTuniqueIDandValuesIter != SMARTuniqueIDsAndValues.end() ;
    SMARTuniqueIDandValuesIter ++)
  {
    std::set<SkSmartAttributeParsedData>::const_iterator
      SMARTattributesToObserveIter = SMARTattributesToObserve.begin();
    for( ; SMARTattributesToObserveIter != SMARTattributesToObserve.end();
        SMARTattributesToObserveIter ++)
    {
      const SkSmartAttributeParsedData & SMARTattributeToObserve =
        *SMARTattributesToObserveIter;
      SMARTattributeID = SMARTattributeToObserve.id;
      SMARTrawValue = SMARTuniqueIDandValuesIter->m_SMARTrawValues[SMARTattributeID];

      if( SMARTuniqueIDandValuesIter->m_successfullyReadSMARTrawValue[SMARTattributeID] )
      {
        m_pwxlistctrl->SetItem(lineNumber,
          SMARTtableListCtrl::COL_IDX_rawValue /** column #/ index */,
          wxString::Format(wxT("%i"),
          SMARTrawValue) );
        if( SMARTrawValue > 0)
          m_pwxlistctrl->SetItemBackgroundColour(lineNumber, * wxRED);
        else
          m_pwxlistctrl->SetItemBackgroundColour(lineNumber, * wxGREEN);
        if( /*SMARTattributesToObserveIter->pretty_value*/ SMARTrawValue )
          atLeast1NonNullValue = true;

        currentTime = wxNow();
        m_pwxlistctrl->SetItem(
          lineNumber,
          SMARTtableListCtrl::COL_IDX_lastUpdate /** column #/ index */,
          //wxString::Format(wxT("%u ms ago"), numberOfMilliSecondsPassedSinceLastSMARTquery )
          currentTime
          );
      }
      else
      {
        m_pwxlistctrl->SetItem(lineNumber,
          SMARTtableListCtrl::COL_IDX_rawValue /** column #/ index */,
          wxT("N/A") );
      }
//            m_pwxlistctrl->SetItem( item );
      ++ lineNumber;
    }
  }
  if( atLeast1NonNullValue )
  {
    wxIcon icon;
    if( wxGetApp().GetSMARTwarningIcon(icon) )
    {
      if ( ! wxGetApp().m_taskBarIcon->SetIcon(
        icon,
        wxT("warning:\nraw value for at least 1 critical SMART parameter is > 0"))
        )
        wxMessageBox(wxT("Could not set new icon."), wxT("wxSMARTmonitor") );
      SetIcon(icon);
    }
  }
}

void SMARTdialog::OnUpdateSMARTparameterValuesInGUI(wxCommandEvent& event)
{
//#ifdef _DEBUG
//  ReadValuesRegarding1DataCarrierOnly();
  UpdateSMARTvaluesUI();
}

void SMARTdialog::StartAsyncUpdateThread()
{
  //    m_timer.Start(1000); // 1 second interval
  if( wxGetApp().mp_SMARTaccess->GetNumSMARTattributesToObserve() > 0 )
  {
    ReadSMARTvaluesAndUpdateUI();
    m_updateSMARTparameterValuesThread.start(
      wxUpdateSMARTparameterValuesThreadFunc, this);
  }
}

void SMARTdialog::SetSMARTdriveID()
{
  LOGN("begin")
  std::set<SMARTuniqueIDandValues> & SMARTuniqueIDsAndValues = m_SMARTaccess.
    GetSMARTuniqueIDandValues();
  std::set<SMARTuniqueIDandValues>::const_iterator SMARTuniqueIDandValuesIter =
    SMARTuniqueIDsAndValues.begin();
  for(fastestUnsignedDataType currentDriveIndex = 0 /*, ucT4 = 0*/;
//    currentDriveIndex < numberOfDifferentDrives; /*++ currentDriveIndex*/
    SMARTuniqueIDandValuesIter != SMARTuniqueIDsAndValues.end() ;
    SMARTuniqueIDandValuesIter ++)
  {
    const SMARTuniqueID & SMARTuniqueID = SMARTuniqueIDandValuesIter->getSMARTuniqueID();
//    SMARTuniqueID.
    std::ostringstream oss;
    oss << "model:" << SMARTuniqueID.m_modelName;
    oss << " firmware:" << SMARTuniqueID.m_firmWareName;
    oss << " serial:" << SMARTuniqueID.m_serialNumber;
    std::string mediaInfo = oss.str();
    m_p_wxTextCtrl->SetValue( wxWidgets::GetwxString_Inline(mediaInfo ) );
//    pDriveInfo = m_SMARTvalueProcessor.GetDriveInfo(currentDriveIndex);

//    std::set<SkSmartAttributeParsedData>::const_iterator
//      SMARTattributesToObserveIter = SMARTattributesToObserve.begin();
  }
}

void SMARTdialog::SetSMARTattribIDandNameLabel()
{
  LOGN("begin")
  const std::set<SkSmartAttributeParsedData> & SMARTattributesToObserve =
    m_SMARTaccess.getSMARTattributesToObserve();
  LOGN("begin " << & SMARTattributesToObserve)
  unsigned lineNumber = 0;
  wxString wxSMARTattribName;

  std::set<SkSmartAttributeParsedData>::const_iterator
    SMARTattributesToObserveIter = SMARTattributesToObserve.begin();
  fastestUnsignedDataType SMARTattributeID;
  for( ; SMARTattributesToObserveIter != SMARTattributesToObserve.end();
      SMARTattributesToObserveIter ++)
  {
    const SkSmartAttributeParsedData & SMARTattributeToObserve =
      *SMARTattributesToObserveIter;
    SMARTattributeID = SMARTattributeToObserve.id;

    wxListItem item;
    item.SetId(lineNumber); //item line number
    item.SetText( wxString::Format(wxT("%u"),
      /*pSmartInfo->m_ucAttribIndex*/ SMARTattributeID)
      );
//            m_pwxlistctrl->SetItem( item );
    m_pwxlistctrl->InsertItem( item );

    wxSMARTattribName = wxWidgets::GetwxString_Inline(
      SMARTattributesToObserveIter->name);
    m_pwxlistctrl->SetItem(lineNumber, 1, /*wxString::Format( wxT("%s"),
      wxSMARTattribName.c_str() )*/ wxSMARTattribName
      );
//            item.SetId(2);
  }
  LOGN("end")
}

void SMARTdialog::ReadSMARTvaluesAndUpdateUI()
{
  DWORD dwRetVal = wxGetApp().mp_SMARTaccess->ReadSMARTValuesForAllDrives();
  if( dwRetVal == SMARTaccessBase::accessDenied )
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

//  ST_DRIVE_INFO * pDriveInfo = NULL;

  m_pwxlistctrl->DeleteAllItems();

  ReBuildUserInterface();
  UpdateSMARTvaluesUI();
}

void SMARTdialog::OnTimer(wxTimerEvent& event)
{
//  ReadSMARTvaluesAndUpdateUI();
}

