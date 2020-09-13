/** File:   ConnectToServerDialog.hpp
 * Author: sg
 * Created on 6. Juni 2017, 18:22 */

#ifndef CONNECTTOSERVERDIALOG_HPP
#define CONNECTTOSERVERDIALOG_HPP

#include <wx/dialog.h> /** base class wxDialog */
#include <wx/timer.h> //class wxTimer
#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType

/** Forward declarations: */
class wxStaticText;
class wxTextCtrl;

class ConnectToServerDialog
  : public wxDialog
{
public:
  ConnectToServerDialog(
    const char * const pchServerAddress,
    const fastestUnsignedDataType servicePortNumber,
    const fastestUnsignedDataType timeOutInSeconds,
    const int cnnctToSrvSocketFileDesc);
  virtual ~ConnectToServerDialog();
  enum IDs {TIMER_ID = 0, connect};
  void End();
private:
  wxTimer m_timer;
  fastestUnsignedDataType m_timeOutInSeconds;
  int m_connectToServerSocketFileDescriptor;
  wxStaticText * m_p_wxStaticTextTimeout;
  wxTextCtrl * m_p_srvAddrTxtCtrl;
  wxTextCtrl * m_p_portNoTxtCtrl;
  wxTextCtrl * m_p_timeoutInS_TxtCtrl;
  void buildUI();
  void OnCancel(wxCommandEvent& event);
  void OnConnect(wxCommandEvent &);
  void OnCloseWindow(wxCloseEvent& event);
  void OnTimer(wxTimerEvent& event);
  
  DECLARE_EVENT_TABLE()
};

#endif /* CONNECTTOSERVERDIALOG_HPP */

