/**(c) from 2013 Stefan Gebauer(TU Berlin matriculation number 361095)
 * Author:Stefan Gebauer,Computer Science Master(from TU Berlin)
 * Created on:26.Nov.2013(Berlin,Germany time zone)*/

/**Prevents <winsock.h> to be included in <windows.h>. <Winsock.h> and
 * <Winsock2.h> have the same definitions.->re-definition happens if <winsock.h>
 * is included before <Winsock2.h> */
#define _WINSOCKAPI_

///This repository's files:
/** Include at 1st in Windows build to avoid:
 * "#warning Please include winsock2.h before windows.h" */
#include "PerDataCarrierPanel.hpp"///class PerDataCarrierPanel
#include "wxSMARTmonitorDialog.hpp"
#include "wxSMARTmonitorApp.hpp" //wxGetApp()
#include "wxSMARTmonitorTaskBarIcon.hpp"
///TU_Bln361095cnctBtnANSIstr,TU_Bln361095cnctBtnTltpANSIstr
#include <client/SMARTmonitorClient.h>
#include <wxWidgets/SupportedSMARTIDsDialog.hpp>///class

///wxWidgets files:
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

///Stefan Gebauer's(TU Berlin matr.#361095)"common_sourcecode" repository files:
 #ifndef TU_Bln361095TmCntDfnd
  #define TU_Bln361095TmCntDfnd
#endif
///wxWidgets::GetwxString_Inline
#include <wxWidgets/charStr/wxStringHelper.hpp>
#include <wxWidgets/UI/About.hpp>///class wxWidgets::AboutDlg
  /////for MicroSoft Windows' GetTimeCountInSeconds(...)
  //typedef double TU_Bln361095tmCntInSecTyp;
  /////for GetTimeCountInNanoSeconds(...)
  //typedef long double TU_Bln361095tmCntInNsTyp;
 //#include <Controller/time/GetTickCount.hpp>
 //#include <hardware/CPU/atomic/AtomicExchange.h>
 //#include <hardware/CPU/atomic/memory_barrier.h>
 ///TU_Bln361095::OpSys::GetCurrThreadNum()
 #include <OperatingSystem/multithread/getCurrThreadNum.h>
 ///LOGN(...),LOGN_DEBUG(...)
 #include <preprocessor_macros/logging_preprocessor_macros.h>

//extern wxSMARTmonitorApp theApp;
/** Static (class) variables. */

//from https://wiki.wxwidgets.org/Custom_Events_in_wx2.8_and_earlier#The_Normal_Case
//const wxEventType UpdateSMARTparameterValuesInGUIEventType = wxNewEventType();
DEFINE_LOCAL_EVENT_TYPE(UpdateSMARTparamValsInGUIevtType)
DEFINE_LOCAL_EVENT_TYPE(ReBuildUIeventType)

//wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
BEGIN_EVENT_TABLE(SMARTdialog, wxDialog)
//    EVT_TIMER(TIMER_ID, SMARTdialog::OnTimer)
  EVT_BUTTON(wxID_ABOUT, SMARTdialog::OnAbout)
  EVT_BUTTON(options, SMARTdialog::OnOptions)
  EVT_BUTTON(directSMARTdata, SMARTdialog::OnDrctSMARTaccss)
  EVT_BUTTON(wxID_OK, SMARTdialog::OnOK)
  EVT_BUTTON(wxID_EXIT, SMARTdialog::OnExit)
  EVT_BUTTON(showSupportedSMART_IDs, SMARTdialog::OnShowSupportedSMART_IDs)
#ifdef TU_Bln361095useClntSrv
  EVT_BUTTON(CONNECT, SMARTdialog::OnCnnctToSrvOrDiscnnct)
#endif
  EVT_CLOSE(SMARTdialog::OnCloseWindow)
  EVT_COMMAND(wxID_ANY, UpdateSMARTparamValsInGUIevtType,
    SMARTdialog::OnUpdateSMARTparameterValuesInGUI)
  EVT_COMMAND(wxID_ANY, ReBuildUIeventType, SMARTdialog::OnReBuildUI)
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
#ifdef TU_Bln361095SMARTmonDrctSMARTaccss
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

void SMARTdialog::RemovePerDataCarrierPanels()
{
  for(SMARTuniqueID2perDataCarrierPanelType::iterator iter =
    m_SMARTuniqueID2perDataCarrierPanel.begin() ; iter !=
    m_SMARTuniqueID2perDataCarrierPanel.end(); iter++)
  {
    iter->second->Destroy();//free UI control resources
  }
  m_SMARTuniqueID2perDataCarrierPanel.clear();
}

void SMARTdialog::SetStatus(const wxString & status)
{
  /** Create title as local variable for easier debugging. */
  wxString title = wxGetApp().GetAppDisplayName() + wxT("-") + status;
  SetTitle(title);
}

void SMARTdialog::ShowCurrentAction(const enum SMARTmonitorClient::CurrentAction
  currAction)
{
  switch(currAction){//TODO ability to show in different languages
   case SMARTmonitorClient::cnnctToSrv :
    {
    wxString str(wxT("blocking connecting to server") );
    ShowMessage(str, UserInterface::MessageType::info);
    }
    break;
   case SMARTmonitorClient::nonBlckCnnctToSrv :
    {
    wxString str(wxT("NON-blocking connecting to server") );
    ShowMessage(str, UserInterface::MessageType::info);
    }
    break;
   case SMARTmonitorClient::readNumBytesForSuppSMART_IDs:
    {
    wxString str(wxT("reading number of bytes for supported S.M.A.R.T. IDs") );
    ShowMessage(str, UserInterface::MessageType::info);
    }
    break;
   case SMARTmonitorClient::readNumBytesForSMARTdata :
    {
    wxString str(wxT("reading number of bytes for S.M.A.R.T. data") );
    ShowMessage(str, UserInterface::MessageType::info);
    }
    break;
   case SMARTmonitorClient::readSuppSMART_IDsXMLdata:
    {
    wxString str(wxT("reading supported S.M.A.R.T. IDs data") );
    ShowMessage(str, UserInterface::MessageType::info);
    }
    break;
   case SMARTmonitorClient::readSMARTvaluesXMLdata:
    {
    wxString str(wxT("reading S.M.A.R.T. data") );
    ShowMessage(str, UserInterface::MessageType::info);
    }
    break;
   case SMARTmonitorClient::hasReadSMARTvaluesXMLdata:
    {
    wxString str(wxT("finished reading S.M.A.R.T. data") );
    ShowMessage(str, UserInterface::MessageType::info);
    }
    break;
   case SMARTmonitorClient::WaitForSMARTupd8ThreadTerm:
    {
      wxString str(wxT("waiting for S.M.A.R.T. data update thread termination")
        );
      ShowMessage(str, UserInterface::MessageType::info);
    }
    break;
   case SMARTmonitorClient::AfterWaitForSMARTupd8ThreadTerm:
    {
      wxString str(wxT("finished waiting for S.M.A.R.T. data update thread termination")
        );
      ShowMessage(str, UserInterface::MessageType::info);
    }
    break;
  }
}

void SMARTdialog::DrctSMARTaccssUIctrls()
{
  m_p_directSMARTaccesBtn->SetLabel(wxT("End Direct S.M.A.R.T.") );
}

void SMARTdialog::NoDrctSMARTaccessUIctrls(){
  m_p_directSMARTaccesBtn->SetLabel(wxT("Direct S.M.A.R.T.") );
}

void SMARTdialog::EnableShowSupportedSMART_IDs()
{
  for(SMARTuniqueID2perDataCarrierPanelType::const_iterator iter =
    m_SMARTuniqueID2perDataCarrierPanel.begin();
    iter != m_SMARTuniqueID2perDataCarrierPanel.end(); iter++)
  {
    iter->second->EnableShowSupportedSMART_IDs();
  }
}

/**Call this function if/directly after:
 * -connection to server established */
void SMARTdialog::setUI(const enum SMARTmonitorClient::serverConnectionState
  srvCnnctnState)
{
  switch(srvCnnctnState){
#ifdef TU_Bln361095useClntSrv
   case SMARTmonitorClient::cnnctdToSrv:
    m_p_cnctAndDiscnctBtn->SetLabelText(wxT(TU_Bln361095discnctBtnANSIstr) );
    m_p_cnctAndDiscnctBtn->Enable();
    EnableShowSupportedSMART_IDs();
    break;
#endif
  }
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
void SMARTdialog::CreatePerDiskUIctrls(wxSizer * p_sizer)
{
  LOGN_DEBUG("begin--evtID2SMARTuniqueID size:" << wxGetApp().
    m_evtID2SMARTuniqueID.size() )
  std::set<TU_Bln361095::SMARTmon::SMARTuniqueIDandValues> &
    SMARTuniqueIDsAndValues = wxGetApp().GetSMARTuniqueIDsAndVals();
  std::set<TU_Bln361095::SMARTmon::SMARTuniqueIDandValues>::const_iterator
    SMARTuniqueIDandValuesIter = SMARTuniqueIDsAndValues.begin();

  /** The map may contain invalid SMART unique IDs e.g. if direct SMART access
   * at first and from server later. So empty it.*/
  wxGetApp().m_evtID2SMARTuniqueID.clear();

  fastestUnsignedDataType windowID = showSupportedSMART_IDs;
  for( ; SMARTuniqueIDandValuesIter != SMARTuniqueIDsAndValues.end() ;
    SMARTuniqueIDandValuesIter ++)
  {
    const SMARTuniqueID & sMARTuniqueID = SMARTuniqueIDandValuesIter->
      getSMARTuniqueID();

    PerDataCarrierPanel * p_perDataCarrierPanel = new PerDataCarrierPanel(this,
      windowID, sMARTuniqueID);
    p_sizer->Add(p_perDataCarrierPanel,
      1/*wxEXPAND*/,///"proportion"
      wxEXPAND, 0);
    m_SMARTuniqueID2perDataCarrierPanel.insert(std::make_pair(
      sMARTuniqueID, p_perDataCarrierPanel) );

    /** Dynamically connect wxEvent to OnShowSupportedSMART_IDs because multiple
     *  data carriers possible.*/
    ///https://wiki.wxwidgets.org/Example_Of_Using_Connect_For_Events
    //TODO use Bind(...) instead as suggested here?:
    // https://wiki.wxwidgets.org/Events#Using_Connect.28.29
    Connect(/*wxID_HIGHEST+1*/windowID, wxEVT_BUTTON,
      wxCommandEventHandler(SMARTdialog::OnShowSupportedSMART_IDs) );
    LOGN_DEBUG("adding SMART unique ID pointer " << & sMARTuniqueID << " to "
      "m_evtID2SMARTuniqueID")

    //TODO Disconnect(...) the event afterwards?
    wxGetApp().m_evtID2SMARTuniqueID.insert(std::make_pair(
        windowID, & sMARTuniqueID) );
  }
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
  #ifdef TU_Bln361095SMARTmonDrctSMARTaccss
  , m_p_directSMARTaccesBtn(NULL)
  #endif
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
  
  buildUI();
}

void SMARTdialog::buildUI()
{
  wxSizerFlags flags;
  p_sMARTinfoSizer = new wxBoxSizer(wxVERTICAL);

  CreatePerDiskUIctrls(p_sMARTinfoSizer);
//    wxSizer * const sizerListCtrl = new wxBoxSizer(wxHORIZONTAL);
//    sizerListCtrl->Add( m_pwxlistctrl, 0,
//        wxEXPAND //int flag=0
//        , /*int border*/ 0);
//    sizerTop->Add(sizerListCtrl /*, flags.Align(wxALIGN_CENTER_HORIZONTAL) */);

//    sizerTop->AddStretchSpacer()->SetMinSize(200, 50);

  wxSizer * const sizerBtns = new wxBoxSizer(wxHORIZONTAL);
  sizerBtns->Add(new wxButton(this, wxID_ABOUT, wxT("&About")), flags);
  sizerBtns->Add(new wxButton(this, options, wxT("&Options")), flags);
  #ifdef TU_Bln361095SMARTmonDrctSMARTaccss
    //TODO only add if sufficient access rights: if(directSMARTaccessAvailable)
    m_p_directSMARTaccesBtn = new wxButton(this, directSMARTdata,
      wxT("&direct S.M.A.R.T.") );
    sizerBtns->Add(m_p_directSMARTaccesBtn);//, flags);
  #endif
#ifdef TU_Bln361095useClntSrv
  m_p_cnctAndDiscnctBtn = new wxButton(this, CONNECT,
    wxSTRINGIZE_T(TU_Bln361095cnctBtnASCIstr) );
  sizerBtns->Add(m_p_cnctAndDiscnctBtn, flags);
#endif
  /** Needs to compile with wxWidgets headers > 2.8.x */
  if( wxTaskBarIcon::IsAvailable() )
  {
    /** Only show the hide button if re-showing this dialog is possible. */
    wxButton * hideButton = new wxButton(this, wxID_OK, wxT("&Hide") );
    hideButton->SetToolTip( wxT("Click on the taskbar icon to show this dialog again.") );
    sizerBtns->Add(hideButton, flags);
  }
  sizerBtns->Add(new wxButton(this, wxID_EXIT, wxT("E&xit")), flags);

  p_sMARTinfoSizer->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));
  //TODO avoid "Gtk-CRITICAL **: 19:22:10.000: gtk_box_gadget_distribute: 
  // assertion 'size >= 0' failed in GtkScrollbar"
  /** https://gitlab.gnome.org/GNOME/gtk/-/issues/210
   * "Hi all, as a workaround, and for anyone finding this bug report from a
   * search engine, I added a set_size_request(100, 200) call to my
   * Gtk.ScrolledWindow() and the warnings disappeared. I originally added the
   * set_size_request to a widget contained in the ScrolledWindow, and that
   * worked too." */
  m_p_wxMessageTextCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""), 
    wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
  m_p_wxMessageTextCtrl->SetEditable(false);
  p_sMARTinfoSizer->Add(
    m_p_wxMessageTextCtrl,
    /** "proportion": "in the main orientation of the wxBoxSizer - where 0
     * stands for not changeable" */
    0 //wxEXPAND///To not need to scroll vertically for large messages.
    , wxEXPAND //int flag=0
    , /*int border*/ 0);
  
  ///Important, else only the last added User Interface control is shown.
  SetSizerAndFit(p_sMARTinfoSizer);
  Centre();
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
  showAboutDlg(this);
}

void SMARTdialog::OnOptions(wxCommandEvent& WXUNUSED(event))
{
  static const wxChar * const title = wxT("wxS.M.A.R.T.monitor options");

  std::ostringstream stdossCmdLineUsage;
  wxGetApp().GetUsage(stdossCmdLineUsage);
  wxString optionsStr = TU_Bln361095::wxWidgets::GetwxString_inln(
    stdossCmdLineUsage.str());

  AboutDlg aboutDlg(optionsStr, title);
  aboutDlg.ShowModal();
}

void SMARTdialog::OnDrctSMARTaccss(wxCommandEvent &)
{
//  __uid_t UID = geteuid();
//  LOGN_ERROR("current UID:" << UID)
//  if(UID != 0)
//    seteuid(0);
  if(wxGetApp().upd8SMARTparamValsThrdIsRunning() ){
    wxGetApp().EndUpdateUIthread();
    NoDrctSMARTaccessUIctrls();
  }
  else{
    wxGetApp().EnsureSMARTattrToObsExist();
    ReBuildUserInterface();
    StartAsyncDrctUpd8Thread();
  }
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

/** @param serverConnectionState the current? state
 *  e.g. if transmission error then state us "unconnected" */
void SMARTdialog::SetState(enum SMARTmonitorClient::serverConnectionState
  newState)
{
  switch(newState)
  {
    case SMARTmonitorClient::valUpd8:
     {
     const wxString title = wxGetApp().GetTitleInclDataSrc() +
       wxT("--last update:") +
       //TODO: use wxGetApp().GetLastSMARTvaluesUpdateTime() instead??
       wxNow();
      SetTitle(title);
     }
     break;
    case SMARTmonitorClient::cnnctdToSrv :
      SetTitle(wxGetApp().GetTitleInclDataSrc() );
      break;
    case SMARTmonitorClient::uncnnctdToSrv :
    case SMARTmonitorClient::endedDrctSMART:
    {
      wxGetApp().GetSMARTvalsAndUpd8UIthreadID = 0;
      wxString wxstrTitle = wxGetApp().GetAppName();
      const struct tm & timeOfLastSMARTvaluesUpdate = wxGetApp().
        GetLastSMARTvaluesUpdateTime();
      
      std::string timeString = UserInterface::GetTimeAsString(
        timeOfLastSMARTvaluesUpdate);
      if(newState == SMARTmonitorClient::uncnnctdToSrv)
        wxstrTitle += wxString::Format(
          wxT("--last update at %s from %s:%u--unconnected"),
          TU_Bln361095::wxWidgets::GetwxString_inln(timeString.c_str()),
          TU_Bln361095::wxWidgets::GetwxString_inln(wxGetApp().
            m_stdstrServiceHostName.c_str() )
        /** Also show port because different hosts may be behind the same IP
         *  address (e.g. via port forwarding) */
          , wxGetApp().m_socketPortNumber);
      else
        wxstrTitle += wxString::Format(
          wxT("--last update at %s via direct S.M.A.R.T.(ended)"),
          TU_Bln361095::wxWidgets::GetwxString_inln(timeString.c_str() ) );
      /** Not needed to set the unconnected UI because this is done in
       * "StartSrvCnnctnAttmptCntDown" */
//      wxGetApp().UnCnnctdToSrvUIctrls();
      SetTitle(wxstrTitle);
    }
      /** So "s_atLeast1CriticalNonNullValue != smartValueRating" in
       * SMARTmonitorClient::UpdateSMARTvaluesUI() after next processing
       * S.M.A.R.T. values->(task bar) icon is updated to "warning" or "OK" */
      wxGetApp().s_entireSMARTstatus = SMARTvals::Rating::unknown;
      wxGetApp().ShowSMARTstatusUnknownIcon();
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
  if(TU_Bln361095::OpSys::GetCurrThreadNum() == wxGetApp().s_UIthreadID){
  //TODO remove all PerDataCarrierIDpanel instances.
//  unsigned itemCount;
//  while( (itemCount = p_sMARTinfoSizer->GetItemCount() ) > 1)
//    p_sMARTinfoSizer->Remove(0);
//  ClearUI();
//  CreatePerDiskUIctrls();
 
  LOGN_DEBUG("begin--evtID2SMARTuniqueID size:" << wxGetApp().
    m_evtID2SMARTuniqueID.size() )
  /** The map may contain invalid SMART unique IDs e.g. if direct SMART access
   * at first and from server later. So empty it.*/
  wxGetApp().m_evtID2SMARTuniqueID.clear();
  fastestUnsignedDataType windowID = showSupportedSMART_IDs;

  SMARTmonitorBase::SMARTuniqueIDandValsContType & SMARTuniqueIDsAndValues =
    wxGetApp().GetSMARTuniqueIDsAndVals();
  SMARTuniqueID2perDataCarrierPanelType::iterator iter =
    m_SMARTuniqueID2perDataCarrierPanel.begin();
  for(SMARTmonitorBase::SMARTuniqueIDandValsContType::const_iterator
    sMARTuniqueIDandValsIter = SMARTuniqueIDsAndValues.begin();
    sMARTuniqueIDandValsIter != SMARTuniqueIDsAndValues.end();
    sMARTuniqueIDandValsIter++)
  {
    const SMARTuniqueID & sMARTuniqueID = sMARTuniqueIDandValsIter->
      getSMARTuniqueID();
    PerDataCarrierPanel * p_perDataCarrierPanel;
    if(iter == m_SMARTuniqueID2perDataCarrierPanel.end() ){
      p_perDataCarrierPanel = new PerDataCarrierPanel(this, windowID,
        sMARTuniqueID);
      p_sMARTinfoSizer->Insert(0, p_perDataCarrierPanel, 1, wxEXPAND, 0);
      m_SMARTuniqueID2perDataCarrierPanel.insert(std::make_pair(
        sMARTuniqueID, p_perDataCarrierPanel) );
    }
    else{
      p_perDataCarrierPanel = (iter->second);
      iter++;
    }
    p_perDataCarrierPanel->ReCreate(sMARTuniqueID);
    p_perDataCarrierPanel->SetSMARTdriveID(sMARTuniqueID);
    p_perDataCarrierPanel->SetSMARTattribIDandNameLabel(sMARTuniqueID);
    
    //TODO Works with Bind(...) without event ID but not with Connect(...)
    /*p_perDataCarrierPanel->getSupportedSMART_IDsBtn()->*/Connect(
      /*wxID_HIGHEST+1*/windowID /** window ID */, wxEVT_BUTTON /**eventType*/,
      wxCommandEventHandler(SMARTdialog::OnShowSupportedSMART_IDs) );
    //TODO Disconnect(...) the event afterwards?
//    p_perDataCarrierPanel->getSupportedSMART_IDsBtn()->Bind(wxEVT_BUTTON,
//      &SMARTdialog::OnShowSupportedSMART_IDs, this, showSupportedSMART_IDs);
    LOGN_DEBUG("adding mapping SMART unique ID pointer " << & sMARTuniqueID <<
      "-> evtID to m_evtID2SMARTuniqueID")
    wxGetApp().m_evtID2SMARTuniqueID.insert(std::make_pair(
        windowID++, & sMARTuniqueID) );
  }
  
    //TODO to support multiple data carriers:
    // p_wxlistctrl = SMARTuniqueID2listCtl[&sMARTuniqueID];
    // p_wxlistctrl->CreateLines();
  /** Without "Fit()" the main dialog size only contains the general buttons and
   * message box. */
  Fit();
  }
  else{
    /** To execute in UI thread.
     * https://wiki.wxwidgets.org/Custom_Events_in_wx2.8_and_earlier#The_Normal_Case */
    wxCommandEvent reBuildUIevent(ReBuildUIeventType);
    wxPostEvent(this, reBuildUIevent);
  }
}

//TODO enable showing supported SMART IDs for multiple disks
void SMARTdialog::OnShowSupportedSMART_IDs(wxCommandEvent & event)
{
  ///Get SMART IDentifier from event ID because there may be > 1 data carrier.
  const int windowID = event.GetId();
  LOGN_DEBUG("begin--" << windowID)
//  wxString dvcPath = event.GetString();
  const SMARTuniqueID * p_SMARTuniqueID = wxGetApp().m_evtID2SMARTuniqueID[
    windowID];
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
  if(TU_Bln361095::OpSys::GetCurrThreadNum() == wxGetApp().s_UIthreadID)
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

void SMARTdialog::OnReBuildUI(wxCommandEvent& event)
{
  ReBuildUserInterface();
}

/**@brief Usually called by posting a wxEVT_COMMAND_BUTTON_CLICKED event */
void SMARTdialog::OnUpdateSMARTparameterValuesInGUI(wxCommandEvent& event)
{
//#ifdef _DEBUG
  wxGetApp().UpdateSMARTvaluesUI();
  LOGN_DEBUG("end")
}

//TODO move this function to class SMARTmonitorBase? (because it may be needed
// by all SMARTmonitorBase-derived classes)
void SMARTdialog::StartAsyncDrctUpd8Thread()
{
  LOGN_DEBUG("begin")
  //    m_timer.Start(1000); // 1 second interval
  if(wxGetApp().GetNumSMARTattrToObs() > 0)
  {// && wxGetApp().GetNumSMARTattrDefs() > 0
    //ReadSMARTvaluesAndUpdateUI();
#if defined(TU_Bln361095SMARTmonMultithread) && \
    defined(TU_Bln361095SMARTmonDrctSMARTaccss)
//    wxGetApp().s_updateSMARTparameterValuesThread.start(
//      wxUpdateSMARTparameterValuesThreadFunc, this);
    wxGetApp().StartAsyncUpdateThread(& SMARTmonitorBase::
      Upd8SMARTvalsDrctlyThreadSafe);
    
    wxGetApp().SetGetDirectSMARTvals();
#endif
  }
  else
    wxMessageBox(wxT("no S.M.A.R.T. attributes to observe defined"));
}

#ifdef TU_Bln361095SMARTmonDrctSMARTaccss
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

  ReBuildUserInterface();
//  UpdateSMARTvaluesUI();
}
#endif
