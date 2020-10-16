/** File: ConnectToServerDialog.cpp
 * Author: Stefan Gebauer, M.Sc.Comp.Sc.
 * Created on 6. Juni 2017, 18:22 */

#include "ConnectToServerDialog.hpp"
#include "wxSMARTmonitorApp.hpp"///wxGetApp()

///wxWidgets include files:
#include <wx/button.h>///class wxButton
#include <wx/msgdlg.h>///wxMessageBox(...)
#include <wx/sizer.h>///class wxBoxSizer
#include <wx/stattext.h>///class wxStaticText

#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>

#include "wxSMARTmonitorDialog.hpp"
extern SMARTdialog * gs_dialog;

BEGIN_EVENT_TABLE(ConnectToServerDialog, wxDialog)
  EVT_TIMER(TIMER_ID, ConnectToServerDialog::OnTimer)
  EVT_CLOSE(ConnectToServerDialog::OnCloseWindow)
  EVT_BUTTON(wxID_CANCEL, ConnectToServerDialog::OnCancel)
  EVT_BUTTON(connect, ConnectToServerDialog::OnConnect)
END_EVENT_TABLE()

inline wxString GetTimeOutLabelText(const fastestUnsignedDataType timeOutInSeconds)
{
  return //wxString::Format(/*%u*//*,timeOutInSeconds*/);
    wxT("seconds until connection timeout:");
}

inline void addToHorizSizer(
  wxSizer * const p_sizer,
  wxWindow * p_leftWnd,
  wxWindow * p_rightWnd)
{
  wxSizer * const srvAddrSizer = new wxBoxSizer(wxHORIZONTAL);
  srvAddrSizer->Add(p_leftWnd, 0, wxALIGN_CENTER_VERTICAL, 0);
  srvAddrSizer->Add(p_rightWnd, 0, 0, 0);
  p_sizer->Add(srvAddrSizer);
}

ConnectToServerDialog::ConnectToServerDialog(
  const char * const pchServerAddress,
  const fastestUnsignedDataType servicePortNumber,
  const fastestUnsignedDataType timeOutInSeconds,
  const int connectToServerSocketFileDescriptor)
  : wxDialog(/*NULL*/ gs_dialog, 
        wxID_ANY, wxT("") //, 
//      wxDefaultPosition, //wxDefaultSize,
//      wxSize(400,400),
//      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
    )
  , m_timer(this, TIMER_ID)
  , m_timeOutInSeconds(timeOutInSeconds)
  , m_connectToServerSocketFileDescriptor(connectToServerSocketFileDescriptor)
{
  buildUI();
}

void ConnectToServerDialog::buildUI(){
//  const wxString wxstrServerAddress = wxWidgets::GetwxString_Inline(
//    pchServerAddress);
  SetTitle(wxT("connect to S.M.A.R.T. values server/service (via TCP)"));
  // \"%s\" port:%u"),
//    wxstrServerAddress, servicePortNumber));
  wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

  m_p_srvAddrTxtCtrl = new wxTextCtrl(this, wxID_ANY, wxGetApp().
    m_stdstrServiceHostName);
  wxStaticText * p_srvAddrLabel = new wxStaticText(this, wxID_ANY,
    wxT("server IPv4 address/host name:") );
  addToHorizSizer(sizerTop, p_srvAddrLabel, m_p_srvAddrTxtCtrl);

  m_p_portNoTxtCtrl = new wxTextCtrl(this, wxID_ANY, wxString::
    Format(wxT("%u"), wxGetApp().m_socketPortNumber) );
  wxStaticText * p_portNoLabel = new wxStaticText(this, wxID_ANY,
    /** http://en.wikipedia.org/wiki/Port_(computer_networking)#Port_number :
     *  "0 to 65535" */
    wxT("socket port number (0-65535):") );
  addToHorizSizer(sizerTop, p_portNoLabel, m_p_portNoTxtCtrl);

  m_p_timeoutInS_TxtCtrl = new wxTextCtrl(this, wxID_ANY,
    wxString::Format(wxT("%u"), m_timeOutInSeconds) );
  m_p_wxStaticTextTimeout = new wxStaticText(this, wxID_ANY,
    GetTimeOutLabelText(m_timeOutInSeconds) );
  addToHorizSizer(sizerTop, m_p_wxStaticTextTimeout, m_p_timeoutInS_TxtCtrl);

  wxButton * p_wxCnnctBtn = new wxButton(this, connect, wxT("connect"));
  wxButton * p_wxCancelButton = new wxButton(this, wxID_CANCEL, wxT("cancel"));
  wxSizer * const actionSizer = new wxBoxSizer(wxHORIZONTAL);
  actionSizer->Add(p_wxCnnctBtn, 0, 0, 0);
  actionSizer->Add(p_wxCancelButton, 0, 0, 0);
  sizerTop->Add(actionSizer);

//  SetSizerAndFit(sizerTop);
  SetSizer(sizerTop);
  Centre();
}

void ConnectToServerDialog::End(){
//  delete this;
  /** http://docs.wxwidgets.org/3.0/classwx_window.html#a6bf0c5be864544d9ce0560087667b7fc
   *  wxWindow::Close : "To guarantee that the window will be destroyed, call
   *  wxWindow::Destroy instead" */
  m_timer.Stop();
  if( IsModal() )
    EndModal(0);
  else
    const bool successfullyDestroyed = Destroy();
  wxGetApp().EnableSrvUIctrls();
}

void ConnectToServerDialog::OnConnect(wxCommandEvent & event){
  m_timer.Start(1000);///1 second interval
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
    m_timeOutInSeconds;
  if(! m_p_timeoutInS_TxtCtrl->GetValue().ToULong(p_timeoutInS) ){
    wxMessageBox(wxT("error converting timeout character string to integer") );
    convFailed = true;
  }
  if(convFailed)
    wxMessageBox(wxT("not connecting to server because character string "
      "conversion failed.") );
  else
    wxGetApp().ConnectToServerAndGetSMARTvalues(true);
  /** Stop timer and close this dialog in "SMARTmonitorClient::
   * AfterCcnnectToServer" (in a derived class) */
}

void ConnectToServerDialog::OnCancel(wxCommandEvent& event)
{
//  m_timer.Stop();
  /** Closing the socket causes the server connect thread to break/finish */
  close(m_connectToServerSocketFileDescriptor);
//  Close(true);
}

void ConnectToServerDialog::OnCloseWindow(wxCloseEvent& event)
{
  End();
}

void ConnectToServerDialog::OnTimer(wxTimerEvent& event)
{
  if( m_timeOutInSeconds > 0)
  {
    m_timeOutInSeconds --;
    m_p_wxStaticTextTimeout->SetLabel( GetTimeOutLabelText(m_timeOutInSeconds) );
  }
  else
    m_timer.Stop();    
}

ConnectToServerDialog::~ConnectToServerDialog() {
  m_timer.Stop();
}
