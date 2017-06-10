/* * File:   ConnectToServerDialog.cpp
 * Author: sg
 * Created on 6. Juni 2017, 18:22 */

#include "ConnectToServerDialog.hpp"
#include <wx/sizer.h> //class wxBoxSizer
#include <wx/stattext.h> //class wxStaticText
#include <wx/button.h> //class wxButton
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>

BEGIN_EVENT_TABLE(ConnectToServerDialog, wxDialog)
  EVT_TIMER(TIMER_ID, ConnectToServerDialog::OnTimer)
  EVT_CLOSE(ConnectToServerDialog::OnCloseWindow)
  EVT_BUTTON(wxID_CANCEL, ConnectToServerDialog::OnCancel)
END_EVENT_TABLE()

inline wxString GetTimeOutLabelText(const fastestUnsignedDataType timeOutInSeconds)
{
 return wxString::Format( wxT("%useconds until connection timeout"), timeOutInSeconds);
}

ConnectToServerDialog::ConnectToServerDialog(
  const char * const pchServerAddress,
  const fastestUnsignedDataType servicePortNumber,
  const fastestUnsignedDataType timeOutInSeconds,
  const int connectToServerSocketFileDescriptor)
  : wxDialog(NULL, wxID_ANY, wxT("") //, 
//      wxDefaultPosition, //wxDefaultSize,
//      wxSize(400,400),
//      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
    )
  , m_timer(this, TIMER_ID)
  , m_timeOutInSeconds (timeOutInSeconds)
  , m_connectToServerSocketFileDescriptor(connectToServerSocketFileDescriptor)
{
  const wxString wxstrServerAddress = wxWidgets::GetwxString_Inline(pchServerAddress);
  SetTitle( wxString::Format( wxT("connect to server %s port:%u"), 
    wxstrServerAddress, servicePortNumber ));
  wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);
  m_p_wxStaticTextTimeout = new wxStaticText(this, wxID_ANY, GetTimeOutLabelText(timeOutInSeconds) );
  wxButton * p_wxCancelButton = new wxButton(this, wxID_CANCEL, wxT("cancel"));
  sizerTop->Add( m_p_wxStaticTextTimeout, 0, 0, 0);
  sizerTop->Add( p_wxCancelButton, 0, 0, 0);
//  SetSizerAndFit(sizerTop);
  SetSizer(sizerTop);
  Centre();
  m_timer.Start(1000); /** 1 second interval */
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
  m_timer.Stop();
  if( IsModal() )
    EndModal(0);
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

