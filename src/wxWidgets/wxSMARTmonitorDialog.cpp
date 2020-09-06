/* wxSMARTmonitiorDialog.cpp
 *  Created on: 26.11.2013
 *      Author: mr.sys  */

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
#include <preprocessor_macros/logging_preprocessor_macros.h>
typedef double TimeCountInSecType;///for Windows' GetTimeCountInSeconds(...)
#include <Controller/time/GetTickCount.hpp>
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
#include <wxWidgets/SupportedSMARTIDsDialog.hpp>
#include <sstream> //ostringstream
#include <wx/textdlg.h> //wxGetTextFromUser
#include <hardware/CPU/atomic/memory_barrier.h>

//extern wxSMARTmonitorApp theApp;
/** Static (class) variables. */

//from https://wiki.wxwidgets.org/Custom_Events_in_wx2.8_and_earlier#The_Normal_Case
//const wxEventType UpdateSMARTparameterValuesInGUIEventType = wxNewEventType();
DEFINE_LOCAL_EVENT_TYPE(UpdateSMARTparameterValuesInGUIEventType)

//wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
BEGIN_EVENT_TABLE(SMARTdialog, wxDialog)
//    EVT_TIMER(TIMER_ID, SMARTdialog::OnTimer)
    EVT_BUTTON(wxID_ABOUT, SMARTdialog::OnAbout)
    EVT_BUTTON(wxID_OK, SMARTdialog::OnOK)
    EVT_BUTTON(wxID_EXIT, SMARTdialog::OnExit)
    EVT_BUTTON(showSupportedSMART_IDs, SMARTdialog::OnShowSupportedSMART_IDs)
    EVT_BUTTON(CONNECT, SMARTdialog::OnConnectToServer)
    EVT_CLOSE(SMARTdialog::OnCloseWindow)
    EVT_COMMAND(wxID_ANY, UpdateSMARTparameterValuesInGUIEventType,
      SMARTdialog::OnUpdateSMARTparameterValuesInGUI)
END_EVENT_TABLE()

//TODO delete this function? Because UpdateSMARTparameterValuesThreadFunc is 
// more elaborated.
DWORD THREAD_FUNCTION_CALLING_CONVENTION wxUpdateSMARTparameterValuesThreadFunc(void * p_v)
{
  LOGN_DEBUG("begin")
  SMARTdialog * p_myDialog = (SMARTdialog *) p_v;
  const unsigned numberOfMilliSecondsToWaitBetweenSMARTquery =
    wxGetApp().GetNumberOfMilliSecondsToWaitBetweenSMARTquery();
  fastestUnsignedDataType numberOfSecondsToWaitBetweenSMARTquery;
  //TODO set to false when connected
  bool getSMARTvaluesDirectly = false;
  if(p_myDialog)
  {
    std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValuesSet = 
      wxGetApp().GetSMARTuniqueIDsAndVals();
    do
    {
#ifdef directSMARTaccess
      if( getSMARTvaluesDirectly )
      {
        /*p_myDialog->*/wxGetApp().Upd8SMARTvalsDrctlyThreadSafe();
      }
      else
#endif
      {
        int res = wxGetApp().GetSMARTattrValsFromSrv(/*sMARTuniqueIDandValuesSet*/);
        if( res == 0 )
        {
          //p_myDialog->UpdateSMARTvaluesUI();
//          wxGetApp().BeforeWait();
        }
        else
          break;
      }
      //https://wiki.wxwidgets.org/Custom_Events_in_wx2.8_and_earlier#.22But_I_don.27t_need_a_whole_new_event_class....22
      wxCommandEvent UpdateUIevent( wxEVT_COMMAND_BUTTON_CLICKED );
      ///To update in the user interface thread.
      wxPostEvent(p_myDialog, UpdateUIevent);
      numberOfSecondsToWaitBetweenSMARTquery =
        numberOfMilliSecondsToWaitBetweenSMARTquery / 1000;
      while( numberOfSecondsToWaitBetweenSMARTquery -- && wxGetApp().
        s_updateSMARTvalues)
      {
        //TODO handle closing of window / app
        wxSleep(1);
      }
      wxMilliSleep(numberOfMilliSecondsToWaitBetweenSMARTquery % 1000 );
    }while(wxGetApp().s_updateSMARTvalues);
    LOGN("after update UI loop")
    //p_myDialog->InformAboutTerminationOfUpdateThread();
  }
  LOGN("end")
  return 0;
}

void SMARTdialog::SetStatus(const wxString & status)
{
  /** Create title as local variable for easier debugging. */
  wxString title = wxGetApp().GetAppDisplayName() + wxT("-") + status;
  SetTitle(title);
}

void SMARTdialog::EnableServerInteractingControls(int n)
{
//  if( n != 0)
//    m_p_ConnectAndDisconnectButton->SetLabelText(wxT("&Disconnect") );
  m_p_ConnectAndDisconnectButton->Enable();
  m_p_showSupportedSMART_IDs->Enable();
}

void SMARTdialog::InformAboutTerminationOfUpdateThread()
{
  LOGN("locking close mutex")
  /** see http://docs.wxwidgets.org/trunk/classwx_condition.html */
  wxMutexLocker lock(m_wxCloseMutex);
  LOGN("sending close event")
  m_p_wxCloseCondition->Broadcast();
}

///Call for every disk.
wxSizer * SMARTdialog::CreatePerDiskUIctrls()
{
  wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

//  flags.Border(wxALL, 0);

  m_p_wxDataCarrierIDtextCtrl = new wxTextCtrl(this, wxID_ANY, wxT("") );
  m_p_wxDataCarrierIDtextCtrl->SetEditable(false);
  m_p_wxDataCarrierIDtextCtrl->SetToolTip(wxT("data carrier ID"));

  wxSizer * const diskIDsizer = new wxBoxSizer(wxHORIZONTAL);

  m_p_showSupportedSMART_IDs = new wxButton(this, showSupportedSMART_IDs, 
    wxT("supported SMART IDs"));
  
  diskIDsizer->Add( m_p_wxDataCarrierIDtextCtrl,
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
  return sizerTop;
}

SMARTdialog::SMARTdialog(
  const wxString & title//,
   //const wxWidgets::wxSMARTvalueProcessor & SMARTvalueProcessor
//    const SMARTvalueProcessorBase & SMARTvalueProcessor
  )
  : wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, //wxDefaultSize,
      wxSize(400,400),
      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
//    , m_timer(this, TIMER_ID)
//  , m_SMARTvalueProcessor( (wxWidgets::wxSMARTvalueProcessor &) SMARTvalueProcessor)
//  , m_SMARTaccess(SMARTvalueProcessor.getSMARTaccess() )
  , m_wxCloseMutex()
  //, m_wxCloseCondition(m_wxCloseMutex)

{
  /** Must create the wxCondition on heap. When it was a member of this class 
     and created on stack then an error ~"wxCondtion not initialzed" occured.*/
  //TODO test if this is possible: in initalizer list: m_wxCloseMutex(), 
  //  [...] m_wxCloseCondition(m_wxCloseMutex) (exactly this order))
  m_p_wxCloseCondition = new wxCondition(m_wxCloseMutex);
  /** http://docs.wxwidgets.org/trunk/classwx_condition.html :
      "the mutex should be initially locked" */
  LOGN("locking the mutex")
  m_wxCloseMutex.Lock();
  LOGN("Mutex is OK?:" << m_wxCloseMutex.IsOk() << " cond.OK?:" << 
    m_p_wxCloseCondition->IsOk() )
  
  wxSizerFlags flags;
  wxSizer * const sizerTop = CreatePerDiskUIctrls();
//    wxSizer * const sizerListCtrl = new wxBoxSizer(wxHORIZONTAL);
//    sizerListCtrl->Add( m_pwxlistctrl, 0,
//        wxEXPAND //int flag=0
//        , /*int border*/ 0);
//    sizerTop->Add(sizerListCtrl /*, flags.Align(wxALIGN_CENTER_HORIZONTAL) */);

//    sizerTop->AddStretchSpacer()->SetMinSize(200, 50);

  wxSizer * const sizerBtns = new wxBoxSizer(wxHORIZONTAL);
  sizerBtns->Add(new wxButton(this, wxID_ABOUT, wxT("&About")), flags);
  m_p_ConnectAndDisconnectButton = new wxButton(this, CONNECT, wxT("&Connect") );
  sizerBtns->Add(m_p_ConnectAndDisconnectButton, flags);
  /** Needs to compile with wxWidgets headers > 2.8.x */
  if( wxTaskBarIcon::IsAvailable() )
  {
    /** Only show the hide button if re-showing this dialog is possible. */
    wxButton * hideButton = new wxButton(this, wxID_OK, wxT("&Hide") );
    hideButton->SetToolTip( wxT("Click on the taskbar icon to show this dialog again.") );
    sizerBtns->Add(hideButton, flags);
  }
  sizerBtns->Add(new wxButton(this, wxID_EXIT, wxT("E&xit")), flags);

  sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));
  m_p_wxMessageTextCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""), 
    wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
  m_p_wxMessageTextCtrl->SetEditable(false);
  sizerTop->Add(
    m_p_wxMessageTextCtrl,
    /** "in the main orientation of the wxBoxSizer - where 0 stands for not changeable" */
    0 //proportion
    , wxEXPAND //int flag=0
    , /*int border*/ 0);
;
  SetSizerAndFit(sizerTop);
  Centre();

//  wxIcon okIcon = wxGetApp().ShowSMARTokIcon();
//  SetIcon(wxGetApp().s_SMARTokIcon);
}

SMARTdialog::~SMARTdialog()
{
  LOGN("unlocking mutex")
  m_wxCloseMutex.Unlock();
  delete m_p_wxCloseCondition;
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
    = _T("a tool to monitor (CRITICAL) S.M.A.R.T. parameters\n"
    "\nsee http://en.wikipedia.org/wiki/S.M.A.R.T."
    "\n(C) 2013-" __DATE__ 
    "\nby Stefan Gebauer, M.Sc. Comp. Science, Berlin, Germany");

#if defined(__WXMSW__) && wxUSE_TASKBARICON_BALLOONS
  wxGetApp().m_taskBarIcon->ShowBalloon(title, message, 15000,
    wxICON_INFORMATION);
#else // !__WXMSW__
    wxMessageBox(message, title, wxICON_INFORMATION | wxOK, this);
#endif // __WXMSW__/!__WXMSW__
}

void SMARTdialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    Show(false);
}

void SMARTdialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
  EndAllThreadsAndCloseAllOtherTopLevelWindows();
  Close(true);
}

void SMARTdialog::OnConnectToServer(wxCommandEvent& WXUNUSED(event))
{
//  if( m_p_ConnectAndDisconnectButton->GetLabelText(wxT("&Disconnect") );
  //TODO or more general: disable all service interacting buttons
//  DisableServiceInteractingButtons();
  m_p_ConnectAndDisconnectButton->Enable(false);
  //TODO Cancel connection countdown if attempt before failed.
  wxGetApp().ConnectToServer();
}

void SMARTdialog::SetState(enum SMARTmonitorClient::serverConnectionState newState)
{
  switch(newState)
  {
    case SMARTmonitorClient::connectedToService :
    {
      const wxString wxstrServiceAddress = wxWidgets::GetwxString_Inline(
        wxGetApp().m_stdstrServiceHostName);
      SetTitle(wxString::Format("wxSMARTmonitor--data from %s:%u", 
        wxstrServiceAddress,
        wxGetApp().m_socketPortNumber)
        );
    }
      break;
    case SMARTmonitorClient::unconnectedFromService :
    {
      wxString wxstrTitle = wxGetApp().GetAppName();
      const struct tm & timeOfLastSMARTvaluesUpdate = wxGetApp().
        GetLastSMARTvaluesUpdateTime();
      
      std::string timeString = UserInterface::GetTimeAsString(
        timeOfLastSMARTvaluesUpdate);
      wxstrTitle += wxString::Format( wxT("--last update:%s from %s--unconnected"),
        wxWidgets::GetwxString_Inline(timeString.c_str()), 
        wxWidgets::GetwxString_Inline(wxGetApp().m_stdstrServiceHostName.c_str() ) );
      SetTitle(wxstrTitle);
    }
      break;
  }
}

void SMARTdialog::ShowText(/*const char * const*/ wxString & text)
{
  wxString wxstrText = wxNow() + wxT(" ") + //wxWidgets::GetwxString_Inline(text);
    text;
  //TODO make the message fit (vertically) into the window
  m_p_wxMessageTextCtrl->SetValue(wxstrText);
}

void SMARTdialog::ShowMessage(
  wxString & text, 
  enum UserInterface::MessageType::messageTypes msgType)
{
  wxString wxstrText = wxNow() + wxT(" ") + //wxWidgets::GetwxString_Inline(text);
    text;
  //TODO make the message fit (vertically) into the window
  switch(msgType)
  {
    case UserInterface::MessageType::error:
      m_p_wxMessageTextCtrl->SetBackgroundColour(*wxRED);
      break;
    case UserInterface::MessageType::warning:
      m_p_wxMessageTextCtrl->SetBackgroundColour(*wxYELLOW);
      break;
    case UserInterface::MessageType::success:
      m_p_wxMessageTextCtrl->SetBackgroundColour(*wxGREEN);
      break;
    default:
      m_p_wxMessageTextCtrl->SetBackgroundColour(*wxWHITE);
      break;
  }
  m_p_wxMessageTextCtrl->SetValue(wxstrText);  
}
/** Rebuilds the lines for SMART IDs and parameter names. */
void SMARTdialog::ReBuildUserInterface()
{
  SetSMARTdriveID();
  /** TODO: avoid creating of double lines if connecting to a server more than 
   * once*/
  const bool allItemsDeleted = m_pwxlistctrl->DeleteAllItems();
  LOGN_DEBUG("all list control items deleted:" << (allItemsDeleted ? "yes" : "no" ))
  
  SMARTmonitorBase::SMARTuniqueIDandValsContType & SMARTuniqueIDsAndValues =
    wxGetApp().GetSMARTuniqueIDsAndVals();
  for(SMARTmonitorBase::SMARTuniqueIDandValsContType::const_iterator
    sMARTuniqueIDandValsIter = SMARTuniqueIDsAndValues.begin();
    sMARTuniqueIDandValsIter != SMARTuniqueIDsAndValues.end();
    sMARTuniqueIDandValsIter++)
  {
    const SMARTuniqueID & sMARTuniqueID = sMARTuniqueIDandValsIter->
      getSMARTuniqueID();
    //TODO to support multiple data carriers:
    // p_wxlistctrl = SMARTuniqueID2listCtl[&sMARTuniqueID];
    // p_wxlistctrl->CreateLines();
    m_pwxlistctrl->CreateLines(sMARTuniqueID);
    break;///Add only SMART attributes for 1 data carrier
  }
  wxGetApp().SetSMARTattribIDandNameLabel();
}

//TODO enable showing supported SMART IDs for multiple disks
void SMARTdialog::OnShowSupportedSMART_IDs(wxCommandEvent & event)
{
  ///Get SMART IDentifier from event ID because there may be > 1 data carrier.
  const int evtID = event.GetId();
  LOGN_DEBUG("begin--" << evtID)
//  wxString dvcPath = event.GetString();
  const SMARTuniqueID * p_SMARTuniqueID = wxGetApp().m_evtID2SMARTuniqueID[
    evtID];
  LOGN_DEBUG("SMART unique ID pointer " << p_SMARTuniqueID)
  if(p_SMARTuniqueID)
  {
    SupportedSMART_IDsDialog * p_SupportedSMART_IDsDialog = new
      SupportedSMART_IDsDialog(*p_SMARTuniqueID, wxGetApp() );
    wxGetApp().openTopLevelWindows.insert(p_SupportedSMART_IDsDialog);
    p_SupportedSMART_IDsDialog->Show(true);
  }
  else
    wxMessageBox(wxT("no data available (maybe not connected to server yet) "));
}

void SMARTdialog::EndAllThreadsAndCloseAllOtherTopLevelWindows()
{
  wxGetApp().EndUpdateUIthread();
  if( OperatingSystem::GetCurrentThreadNumber() == wxGetApp().s_GUIthreadID )
  {
    /** This code should only be executed in UI thread! */
    for( std::set<wxTopLevelWindow *>::iterator iter = 
      wxGetApp().openTopLevelWindows.begin(); 
      iter != wxGetApp().openTopLevelWindows.end(); iter ++ )
    {
      (*iter)->Destroy();
    }
  }
}

void SMARTdialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
  LOGN("begin")
  EndAllThreadsAndCloseAllOtherTopLevelWindows();
  Destroy();
}

void SMARTdialog::UpdateUIregarding1DataCarrierOnly()
{
  const int listItemCount = m_pwxlistctrl->GetItemCount();
//#endif
  DWORD tickCountOfCurrentTimeInMilliSeconds;
  DWORD numberOfMilliSecondsPassedSinceLastSMARTquery;
  wxString currentTime;

  for( unsigned index = 0; index < listItemCount; ++ index)
  {
    m_pwxlistctrl->SetItem(
      index,
      ColumnIndices::rawValue /** column #/ index */,
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
      ColumnIndices::lastUpdate /** column #/ index */,
      //wxString::Format(wxT("%u ms ago"), numberOfMilliSecondsPassedSinceLastSMARTquery )
      currentTime
      );
  }
}

/**@brief Usually called by posting a wxEVT_COMMAND_BUTTON_CLICKED event */
void SMARTdialog::OnUpdateSMARTparameterValuesInGUI(wxCommandEvent& event)
{
//#ifdef _DEBUG
  wxGetApp().UpdateSMARTvaluesUI();
}

void SMARTdialog::StartAsyncDrctUpd8Thread()
{
  LOGN_DEBUG("begin")
  //    m_timer.Start(1000); // 1 second interval
  if(wxGetApp().GetNumSMARTattrToObs() > 0)
  {// && wxGetApp().GetNumSMARTattrDefs() > 0
    //ReadSMARTvaluesAndUpdateUI();
#if defined(multithread) && defined(directSMARTaccess)
//    wxGetApp().s_updateSMARTparameterValuesThread.start(
//      wxUpdateSMARTparameterValuesThreadFunc, this);
    wxGetApp().StartAsyncUpdateThread(& SMARTmonitorBase::
      Upd8SMARTvalsDrctlyThreadSafe);
#endif
  }
  else
    wxMessageBox(wxT("no S.M.A.R.T. attributes to observe defined"));
}
//TODO make for more than 1 data carrier
void SMARTdialog::SetSMARTdriveID()
{
  LOGN_DEBUG("begin--evtID2SMARTuniqueID size:" << wxGetApp().
    m_evtID2SMARTuniqueID.size() )
  /** The map may contain invalid SMART unique IDs e.g. if direct SMART access
   * at first and from server later. So empty it.*/
  wxGetApp().m_evtID2SMARTuniqueID.clear();
  std::set<SMARTuniqueIDandValues> & SMARTuniqueIDsAndValues = wxGetApp().
    GetSMARTuniqueIDsAndVals();
  std::set<SMARTuniqueIDandValues>::const_iterator SMARTuniqueIDandValuesIter =
    SMARTuniqueIDsAndValues.begin();
  for( ; SMARTuniqueIDandValuesIter != SMARTuniqueIDsAndValues.end() ;
    SMARTuniqueIDandValuesIter ++)
  {
    const SMARTuniqueID & sMARTuniqueID = SMARTuniqueIDandValuesIter->
      getSMARTuniqueID();
    std::ostringstream oss;
    oss << "model:" << sMARTuniqueID.m_modelName;
    oss << " firmware:" << sMARTuniqueID.m_firmWareName;
    oss << " serial:" << sMARTuniqueID.m_serialNumber;
    std::string mediaInfo = oss.str();
    m_p_wxDataCarrierIDtextCtrl->SetValue( wxWidgets::GetwxString_Inline(
      mediaInfo) );
    
    /** Dynamically connect wxEvent to OnShowSupportedSMART_IDs because multiple
     *  data carriers possible.*/
    ///https://wiki.wxwidgets.org/Example_Of_Using_Connect_For_Events
    //TODO use Bind(...) instead as suggested here?:
    // https://wiki.wxwidgets.org/Events#Using_Connect.28.29
    Connect(/*wxID_HIGHEST+1*/showSupportedSMART_IDs, wxEVT_BUTTON,
      wxCommandEventHandler(SMARTdialog::OnShowSupportedSMART_IDs) );
    LOGN_DEBUG("adding SMART unique ID pointer " << & sMARTuniqueID << " to "
      "m_evtID2SMARTuniqueID")
    //TODO Disconnect(...) the event afterwards?
    wxGetApp().m_evtID2SMARTuniqueID.insert(std::make_pair(
      showSupportedSMART_IDs, & sMARTuniqueID) );
  }
}

#ifdef directSMARTaccess
void SMARTdialog::ReadSMARTvaluesAndUpdateUI()
{
  DWORD dwRetVal = wxGetApp().mp_SMARTaccess->ReadSMARTValuesForAllDrives(
    wxGetApp().m_SMARTattrIDsToObs, wxGetApp().s_dataCarrierID2devicePath);
  if( dwRetVal == SMARTaccessBase::accessDenied )
  {
    //SetTitle( wxT("wxS.M.A.R.T.monitor--access to SMART denied--restart as admin") );
    wxIcon icon;
    if( wxGetApp().GetSMARTwarningIcon(icon) )
    {
      SetIcon(icon);
    }
  }
  else
    SetTitle( wxT("wxSMARTmonitor") );

  m_pwxlistctrl->DeleteAllItems();

  ReBuildUserInterface();
//  UpdateSMARTvaluesUI();
}
#endif
