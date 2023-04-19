/** SupportedSMARTIDs.hpp
 *  Created on: 29.10.2016
 *  Author:Stefan Gebauer, M.Sc.Comp.Sc.*/

#ifndef WXWIDGETS_SUPPORTEDSMARTIDSDIALOG_HPP_
#define WXWIDGETS_SUPPORTEDSMARTIDSDIALOG_HPP_

/** Include at 1st in Windows build to avoid:
 * "#warning Please include winsock2.h before windows.h" */
#include <client/SMARTmonitorClient.h> //class SMARTmonitorClient

///wxWidgets library header files:
 /**Avoid Microsoft Visual Studio compiler warnings like :
  * "include\wx / wxcrt.h(213, 14) warning C4996 : 'strcpy' : This function
  * or variable may be unsafe.Consider using strcpy_s instead.To disable
  * deprecation, use _CRT_SECURE_NO_WARNINGS.See online help for details." */
 #pragma warning(disable:4996)
#include <wx/dialog.h> //base class wxDialog
 #pragma warning(enable:4996)

///Standard C/C++ includes
#include <vector>///class std::vector

///This repository's files:
#include <attributes/SMARTattributeNameAndID.hpp>
#include "SMARTtableListCtrl.hpp"///class wxWidgets::SMARTtableListCtrl

///Forward declarations (faster than #include):
class wxButton;
class wxListCtrl;
class wxSizer;
//class SMARTmonitorClient;
//SMARTmonitorClient::dataCarrierID2supportedSMARTattributesMap_type;

class SupportedSMART_IDsDialog
  : public wxDialog
{
  /*wxListCtrl*/wxWidgets::SMARTtableListCtrl * m_pwxlistctrl;
  wxButton * m_p_upd8SupportedSMART_IDs;
  enum UIctrlIDs {upd8SupportedSMART_IDs};
  SMARTmonitorClient & m_sMARTmonClient;
  SMARTuniqueID & m_sMARTuniqueID;
public:
  SupportedSMART_IDsDialog(const SMARTuniqueID &, SMARTmonitorClient &);
  virtual
  ~SupportedSMART_IDsDialog ();
  enum columnIndices { COL_IDX_SMART_ID = 0 , COL_IDX_SMARTparameterName};
  
  void create2ColListCtrl();
  inline void cr8ListCtrl();
  wxSizer * CreateGUI(const SMARTuniqueID & dataCarrierID);
  void FillGUI(SMARTuniqueID &, SMARTmonitorClient &, const bool create);
  void SetTitleFromDataCarrierID(const SMARTuniqueID & dataCarrierID);
  void OnCloseWindow(wxCloseEvent& event);
  void OnUpd8SupportedSMART_IDs(wxCommandEvent &);
  
  /** Necessary for receiving events. */
  DECLARE_EVENT_TABLE()
};

#endif /* WXWIDGETS_SUPPORTEDSMARTIDSDIALOG_HPP_ */
