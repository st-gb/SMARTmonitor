/** File: ConnectToServerDialog.hpp
 * Author: Stefan Gebauer, M.Sc. Comp.Sc.
 * Created on 6. Juni 2017, 18:22 */

#ifndef CONNECTTOSERVERDIALOG_HPP
#define CONNECTTOSERVERDIALOG_HPP

#include <wx/button.h>///class wxButton
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
  static wxString title;
  virtual ~ConnectToServerDialog();
  enum IDs {cnnctnTimeoutTimerID = 0, cnnctnAttmptTimerID, connect, srvAddrTxtCtl};
  void DisableConnect(){ m_p_wxCnnctBtn->Disable(); }
  void EnableCnnctBtn(){ m_p_wxCnnctBtn->Enable(); }
  void End();
  void EndCnnctnAttemptTimer();
  void EndCnnctnTimeoutTimer();
  wxTimer m_cnnctnTimeoutTimer;
  wxTimer m_cnnctnAttmptTimer;
  void ReStartTimer(){m_timeOutInSeconds = 30; m_cnnctnTimeoutTimer.Start();}
  void StartSrvCnnctnCntDown(const int timeOutInSec = -1);
  void StartSrvCnnctnAttmptCntDown(const int timeOutInSec = -1);
private:
  fastestUnsignedDataType m_timeOutInSeconds;
  int m_connectToServerSocketFileDescriptor;
  wxStaticText * m_p_wxStaticTextTimeout;
  wxStaticText * m_p_timeoutLabel;
  wxTextCtrl * m_p_srvAddrTxtCtrl;
  wxTextCtrl * m_p_portNoTxtCtrl;
  wxTextCtrl * m_p_timeoutInS_TxtCtrl;
  wxTextCtrl * m_p_cnnctnAttmptTxtCtrl;
  wxButton * m_p_wxCnnctBtn;
  void buildUI();
  void StartTimer(){m_cnnctnTimeoutTimer.Start();}
  void OnCancel(wxCommandEvent& event);
  void OnConnect(wxCommandEvent &);
  void OnCloseWindow(wxCloseEvent& event);
  void OnSrvAddrChange(wxCommandEvent &);
  void OnStartCntDown(wxCommandEvent &);
  inline void showTimeoutInTitle();
  void OnTimer(wxTimerEvent& event);
  void OnCnnctnAttmptTimer(wxTimerEvent &);
  
  DECLARE_EVENT_TABLE()
};

///For using the event outside of the implementation file.
DECLARE_LOCAL_EVENT_TYPE(StartCnnctCntDownEvtType, -1)

#endif /* CONNECTTOSERVERDIALOG_HPP */
