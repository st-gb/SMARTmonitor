/** File: ConnectToServerDialog.cpp
 * Author: Stefan Gebauer, M.Sc.Comp.Sc.
 * Created on 6. Juni 2017, 18:22 */

///This repository's files:
/** Include at 1st in Windows build to avoid:
 * "#warning Please include winsock2.h before windows.h" */
#include "wxSMARTmonitorApp.hpp"///wxGetApp()
#include "ConnectToServerDialog.hpp"

///wxWidgets include files:
#include <wx/button.h>///class wxButton
#include <wx/msgdlg.h>///wxMessageBox(...)
#include <wx/sizer.h>///class wxBoxSizer
#include <wx/stattext.h>///class wxStaticText

///S. Gebauer's common_sourcecode files:
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
///OperatingSystem::BSD::socket::interruptSelect(...)
#include <OperatingSystem/BSD/socket/interruptSelect.h>

#include <signal.h>///SIGUSR1

#include "wxSMARTmonitorDialog.hpp"
extern SMARTdialog * gs_dialog;

//DEFINE_LOCAL_EVENT_TYPE(StartCnnctCntDownEvtType)

BEGIN_EVENT_TABLE(ConnectToServerDialog, wxDialog)
  EVT_TEXT(srvAddrTxtCtl, ConnectToServerDialog::OnSrvAddrChange)
  EVT_TIMER(cnnctnTimeoutTimerID, ConnectToServerDialog::OnTimer)
  EVT_TIMER(cnnctnAttmptTimerID, ConnectToServerDialog::OnCnnctnAttmptTimer)
  EVT_CLOSE(ConnectToServerDialog::OnCloseWindow)
  EVT_BUTTON(wxID_CANCEL, ConnectToServerDialog::OnCancel)
  EVT_BUTTON(connect, ConnectToServerDialog::OnConnect)
  EVT_COMMAND(wxID_ANY, StartCnnctCntDownEvtType, ConnectToServerDialog::
    OnStartCntDown)
END_EVENT_TABLE()

wxString ConnectToServerDialog::title =
  wxT("connect to S.M.A.R.T. values server/service");

inline wxString GetTimeOutLabelText(
  const fastestUnsignedDataType timeOutInSeconds,
  const wxChar str [])
{
  return wxString::Format(wxT("seconds until %s:")
    /*%u*//*,timeOutInSeconds);*/
    /*+*/, str /* + " timeout:"*/);
}

inline void addToHorizSizer(
  wxSizer * const p_sizer,
  wxWindow * p_leftWnd,
  wxWindow * p_rightWnd)
{
  wxSizer * const srvAddrSizer = new wxBoxSizer(wxHORIZONTAL);
  srvAddrSizer->Add(p_leftWnd, 0, wxALIGN_CENTER_VERTICAL, 0);
  srvAddrSizer->Add(p_rightWnd,
    /** https://docs.wxwidgets.org/3.0/classwx_sizer.html#a4e2122f2749261473c21cb192d00709f :
     * "it is used in wxBoxSizer to indicate if a child of a sizer can change 
     *  its size in the main orientation of the wxBoxSizer - where 0 stands for
     *  not changeable and a value of more than zero is interpreted relative to
     * the value of other children of the same wxBoxSizer. "*/
    1,
    ///"The item will be expanded to fill the space assigned to the item."
    /*0*/wxEXPAND, 0);
  p_sizer->Add(srvAddrSizer);
}

ConnectToServerDialog::ConnectToServerDialog(
  const char * const pchServerAddress,
  const fastestUnsignedDataType servicePortNumber,
  const fastestUnsignedDataType timeOutInSeconds,
  const int connectToServerSocketFileDescriptor)
  : wxDialog(/*NULL*/ gs_dialog, 
      wxID_ANY, wxT(""),
      wxDefaultPosition,
      /*wxDefaultSize*/wxSize(550,200),
      /**For "close dialog" button.*/wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
    )
  , m_cnnctnTimeoutTimer(this, cnnctnTimeoutTimerID)
  , m_cnnctnAttmptTimer(this, cnnctnAttmptTimerID)
  , m_timeOutInSeconds(timeOutInSeconds)
  , m_connectToServerSocketFileDescriptor(connectToServerSocketFileDescriptor)
{
  /** Already register the signal handler for asynchronous connect via select()
   * here because one may press "cancel"->OnCancel() -> "raise(SIGUSR1)" without
   * calling SMARTmonitorClient::ConnectToServer -> registerSignalHandler()
   * before. */
  wxGetApp().registerSignalHandler();
  buildUI();
}

void ConnectToServerDialog::buildUI(){
//  const wxString wxstrServerAddress = wxWidgets::GetwxString_Inline(
//    pchServerAddress);
  SetTitle(title);
  // \"%s\" port:%u"),
//    wxstrServerAddress, servicePortNumber));
  wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

  /** wxTextCtrl c+'tor with text caused (indirect) call of "OnSrvAddrChange"
   *  because of event -> program crash because wxTextCtrl object is NULL */
  m_p_srvAddrTxtCtrl = new wxTextCtrl(this, srvAddrTxtCtl);
  m_p_srvAddrTxtCtrl->SetValue(wxWidgets::GetwxString_Inline(wxGetApp().
    m_stdstrServiceHostName.c_str() ) );
#ifdef __linux__
  m_p_srvAddrTxtCtrl->SetToolTip(wxT("for valid host names see file /etc/hosts")
    );
#endif
  wxStaticText * p_srvAddrLabel = new wxStaticText(this, wxID_ANY,
    wxT("server IPv4 address/host name:") );
  addToHorizSizer(sizerTop, p_srvAddrLabel, m_p_srvAddrTxtCtrl);

  m_p_portNoTxtCtrl = new wxTextCtrl(this, wxID_ANY, wxString::
    Format(wxT("%u"), wxGetApp().m_socketPortNumber) );
  wxStaticText * p_portNoLabel = new wxStaticText(this, wxID_ANY,
    /** http://en.wikipedia.org/wiki/Port_(computer_networking)#Port_number :
     *  "0 to 65535" */
    wxT("TCP socket port number (0-65535):") );
  addToHorizSizer(sizerTop, p_portNoLabel, m_p_portNoTxtCtrl);

  m_p_timeoutInS_TxtCtrl = new wxTextCtrl(this, wxID_ANY,
    wxString::Format(wxT("%u"), m_timeOutInSeconds) );
  m_p_timeoutInS_TxtCtrl->SetToolTip(wxT("try TCP handshake with server until "
    "this timeout until elapses") );
  m_p_wxStaticTextTimeout = new wxStaticText(this, wxID_ANY,
    GetTimeOutLabelText(m_timeOutInSeconds, wxT("connection timeout")) );
  addToHorizSizer(sizerTop, m_p_wxStaticTextTimeout, m_p_timeoutInS_TxtCtrl);

  m_p_cnnctnAttmptTxtCtrl = new wxTextCtrl(this, wxID_ANY,
    wxString::Format(wxT("%u"), wxGetApp().
    m_srvCnnctnCntDownInSec) );
  wxStaticText * p_cnnctnAttmptTxt = new wxStaticText(this, wxID_ANY,
    GetTimeOutLabelText(m_timeOutInSeconds, wxT("connection attempt") ) );
  addToHorizSizer(sizerTop, p_cnnctnAttmptTxt, m_p_cnnctnAttmptTxtCtrl);

  m_p_timeoutLabel = new wxStaticText(this, wxID_ANY, wxT("current state:"));
  sizerTop->Add(m_p_timeoutLabel);

  m_p_wxCnnctBtn = new wxButton(this, connect, wxT("connect"));
  wxButton * p_wxCancelButton = new wxButton(this, wxID_CANCEL, wxT("cancel"));
  wxSizer * const actionSizer = new wxBoxSizer(wxHORIZONTAL);
  actionSizer->Add(m_p_wxCnnctBtn, 0, 0, 0);
  actionSizer->Add(p_wxCancelButton, 0, 0, 0);
  sizerTop->Add(actionSizer);

  /** https://wiki.wxwidgets.org/Beech:Using_custom_dialogs_with_sizers :
   * "SetSizeHints() directs the sizer to set the minimal size of the window to
   * match the sizer's minimal size." */
  sizerTop->SetSizeHints(this);
  //TODO set with to fit title bar width
//  SetSize(GetSize().y, GetSize().x)
//  SetSizerAndFit(sizerTop);
  SetSizer(sizerTop);
  Centre();
}

/** Called:
 *  -after successfully connecting to server
 *  -when closing this dialog by hand. */
void ConnectToServerDialog::End(){
//  delete this;
  /** http://docs.wxwidgets.org/3.0/classwx_window.html#a6bf0c5be864544d9ce0560087667b7fc
   *  wxWindow::Close : "To guarantee that the window will be destroyed, call
   *  wxWindow::Destroy instead" */
  m_cnnctnTimeoutTimer.Stop();
  m_cnnctnAttmptTimer.Stop();
  if( IsModal() )
    EndModal(0);
  else
    const bool successfullyDestroyed = Destroy();
  wxGetApp().m_p_cnnctToSrvDlg = NULL;
  ///To enable "Connect" button in main dialog.
  wxGetApp().setUI(SMARTmonitorClient::uncnnctdToSrv);
}

void ConnectToServerDialog::EndCnnctnAttemptTimer()
{
  m_cnnctnAttmptTimer.Stop();
  m_p_timeoutLabel->SetLabel(wxT(""));
  SetTitle(title);
}

void ConnectToServerDialog::EndCnnctnTimeoutTimer()
{
  m_cnnctnTimeoutTimer.Stop();
  m_p_timeoutLabel->SetLabel(wxT(""));
  SetTitle(title);
}

void ConnectToServerDialog::OnConnect(wxCommandEvent & event){
//  m_timer.Start(1000);///1 second interval
  wxGetApp().m_stdstrServiceHostName = m_p_srvAddrTxtCtrl->GetValue();
  unsigned long * p_socketPortNo = (unsigned long *) & wxGetApp().
    m_socketPortNumber;
  bool convFailed = false;
  if(! m_p_portNoTxtCtrl->GetValue().ToULong(p_socketPortNo) ){
    wxMessageBox(wxT("error converting port number character string to "
      "integer"));
    convFailed = true;
  }
  unsigned long * p_timeoutInS = (unsigned long *) & wxGetApp().
    m_cnnctTimeOutInSec;
  if(! m_p_timeoutInS_TxtCtrl->GetValue().ToULong(p_timeoutInS) ){
    wxMessageBox(wxT("error converting timeout character string to integer") );
    convFailed = true;
  }
  p_timeoutInS = (unsigned long *) & wxGetApp().
    m_srvCnnctnCntDownInSec;
  if(! m_p_cnnctnAttmptTxtCtrl->GetValue().ToULong(p_timeoutInS) ){
    wxMessageBox(wxT("error converting timeout attempt character string to "
      "integer") );
    convFailed = true;
  }
  if(convFailed)
    wxMessageBox(wxT("not connecting to server because character string "
      "conversion failed.") );
  else{
    wxGetApp().m_p_cnnctToSrvDlg = this;
    
#ifdef __linux__ //SIGUSR1 not available in MinGW
    ///https://en.wikipedia.org/wiki/C_signal_handling
    ///Needed, else program exits when calling raise(SIGUSR1).
    signal(SIGUSR1, SMARTmonitorBase::sigHandler);
#endif
    //TODO wait until connection (attempt) ends first.
    cancelCnnctnOrCnnctnAttmpt();
    
    const fastestUnsignedDataType cnnctToSrvrRslt = wxGetApp().
      CnnctToSrvAndGetSMARTvals(wxGetApp().isAsyncCnnct() );
    if(cnnctToSrvrRslt == OperatingSystem::BSD::sockets::getHostByNameFailed
     + /*OperatingSystem::BSD::sockets::*/gethostbynameUnknownHost)
    {
      m_p_srvAddrTxtCtrl->SetBackgroundColour(*wxRED);
      m_p_srvAddrTxtCtrl->SetToolTip(wxT("unknown host name") );
      std::ostringstream oss;
      oss << "\"" << wxGetApp().m_stdstrServiceHostName.c_str() << 
        "\" not found in host DataBase";
      wxGetApp().ShowMessage(oss.str().c_str() );
    }
  }
  /** Stop timer and close this dialog in "SMARTmonitorClient::
   * AfterCcnnectToServer" (in a derived class) */
}

inline void interruptSocketSelect()
{
  //TODO crashes here (Linux)
#ifdef __linux__
  int i = SIGUSR1;
  ///This cancels the waiting in "select(...)".
  OperatingSystem::BSD::sockets::interruptSelect(
    /** Avoid g++ "error: invalid conversion from ´int´ to ´void*´
     * [-fpermissive]" by explicit casting */
    (void *) i);
#endif
#ifdef _WIN32
  void * p = wxGetApp().connectThread.GetThreadHandle();
  ///This cancels the waiting in "select(...)".
  OperatingSystem::BSD::sockets::interruptSelect(p);
#endif
}

void ConnectToServerDialog::cancelCnnctnOrCnnctnAttmpt()
{
//  m_timer.Stop();
  /** Closing the socket causes the server connect thread to break/finish */
  close(m_connectToServerSocketFileDescriptor);
//  Close(true);
  ///https://www.thegeekstuff.com/2011/02/send-signal-to-process/
  /*kill(getpid(), SIGUSR1);*/
  
  ///May be currently waiting with a timeout or indefinitely in select(...).
  interruptSocketSelect();
  ///Each one of both timers may be currently running.
  EndCnnctnTimeoutTimer();
  EndCnnctnAttemptTimer();
//  wxGetApp().EndWaitTillCnnctTimer();
}

void ConnectToServerDialog::OnCancel(wxCommandEvent& event)
{
  cancelCnnctnOrCnnctnAttmpt();
}

void ConnectToServerDialog::OnCloseWindow(wxCloseEvent& event)
{
  End();
}

void ConnectToServerDialog::OnSrvAddrChange(wxCommandEvent & evt){
  m_p_srvAddrTxtCtrl->SetBackgroundColour(*wxWHITE);
  m_p_srvAddrTxtCtrl->SetToolTip(wxT("") );
}

void ConnectToServerDialog::OnStartCntDown(wxCommandEvent & event){
  m_cnnctnTimeoutTimer.Start(1000);
}

inline void ConnectToServerDialog::showTimeoutInTitle(){
  SetTitle(wxString::Format(wxT("%s--timeout in max. ca. %us"), title.c_str(),
    //TODO In _Linux_: could also use timeout argument from "select(...)"
    m_timeOutInSeconds) );
  //TODO set the dialog width to include the title
  //wxSystemSettings::GetMetric(wxSYS_CAPTION_Y);
  //MS Windows: SystemParametersInfo SPI_GETNONCLIENTMETRICS NONCLIENTMETRICS
  // lfCaptionFont
}

void ConnectToServerDialog::OnTimer(wxTimerEvent& event)
{
  if( m_timeOutInSeconds > 0)
  {
    m_timeOutInSeconds --;
//    showTimoutInTitle();
    m_p_timeoutLabel->SetLabel(wxString::Format(
      wxT("connection TIMEOUT in ca. %us"), m_timeOutInSeconds
      //TODO maybe test if reading the timeval struct passed to "select(...)"
      // can be displayed here:
      /*wxGetApp().currCnnctTimeout.tv_sec*/ ) );
    //TODO break connect() by interrupting?
  }
  else{
    m_cnnctnTimeoutTimer.Stop();
//    raise(SIGUSR1);
//    StartSrvCnnctnAttmptCntDown();

    /** Default title does not need to be set if immediately after a stop a next
     *  timer starts.*/
//    SetTitle(title);
  }
}

void ConnectToServerDialog::OnCnnctnAttmptTimer(wxTimerEvent& event)
{
  if( m_timeOutInSeconds > 0)
  {
    wxString label = wxString::Format(
      wxT("connection ATTEMPT in ca. %us"), m_timeOutInSeconds);
    m_p_timeoutLabel->SetLabel(label);
    m_timeOutInSeconds --;
  }
  else{
    m_cnnctnAttmptTimer.Stop();
    wxGetApp().CnnctToSrvAndGetSMARTvals(wxGetApp().isAsyncCnnct() );
    StartSrvCnnctnCntDown();
  }
}

ConnectToServerDialog::~ConnectToServerDialog() {
  m_cnnctnTimeoutTimer.Stop();
}

/**Timeout for "connect(...)" (blocking) or select(...) (non-blocking) function/
 * for TCP handshake */
void ConnectToServerDialog::StartSrvCnnctnCntDown(const int timeOutInSec)
{
  if(timeOutInSec == -1)///Use default timeOut
  {
    unsigned long * p_timeoutInS = (unsigned long *) & wxGetApp().
      m_cnnctTimeOutInSec;
    if(! m_p_timeoutInS_TxtCtrl->GetValue().ToULong(p_timeoutInS) ){
      wxMessageBox(wxT("error converting timeout character string to integer") );
      m_timeOutInSeconds = wxGetApp().m_cnnctTimeOutInSec;
    }
    else
      m_timeOutInSeconds = * p_timeoutInS;
  }
  else
    m_timeOutInSeconds = timeOutInSec;
  
  m_p_timeoutLabel->SetLabel(wxString::Format(
    wxT("connection TIMEOUT in ca. %u s"), m_timeOutInSeconds) );
  m_cnnctnTimeoutTimer.Start(1000);
}

void ConnectToServerDialog::StartSrvCnnctnAttmptCntDown(const int timeOutInSec)
{
  if(timeOutInSec == -1)///Use default timeOut
  {
    unsigned long * p_timeoutInS = (unsigned long *) & wxGetApp().
      m_srvCnnctnCntDownInSec;
    if(! m_p_cnnctnAttmptTxtCtrl->GetValue().ToULong(p_timeoutInS) ){
      wxMessageBox(wxT("error converting timeout attempt character string to "
        "integer") );
      m_timeOutInSeconds = wxGetApp().m_srvCnnctnCntDownInSec;
    }
    else
      m_timeOutInSeconds = * p_timeoutInS;
  }
  else
    m_timeOutInSeconds = timeOutInSec;
  m_p_timeoutLabel->SetLabel(wxString::Format(
    wxT("connection ATTEMPT in ca. %u s"), m_timeOutInSeconds) );
  m_cnnctnAttmptTimer.Start(1000);
}
