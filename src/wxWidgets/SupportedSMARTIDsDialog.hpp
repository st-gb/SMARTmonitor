/** SupportedSMARTIDs.hpp
 *  Created on: 29.10.2016
 *  Author:Stefan Gebauer, M.Sc.Comp.Sc.*/

#ifndef WXWIDGETS_SUPPORTEDSMARTIDSDIALOG_HPP_
#define WXWIDGETS_SUPPORTEDSMARTIDSDIALOG_HPP_

#include <wx/dialog.h> //base class wxDialog
///Standard C/C++ includes
#include <vector>///class std::vector

///This repo
#include <attributes/SMARTattributeNameAndID.hpp>
#include <client/SMARTmonitorClient.h> //class SMARTmonitorClient
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
  void cr8ListCtrl();
  wxSizer * CreateGUI(const SMARTuniqueID & dataCarrierID);
  void FillGUI(SMARTuniqueID &, SMARTmonitorClient &, const bool create);
  void SetTitleFromDataCarrierID(const SMARTuniqueID & dataCarrierID);
  void OnCloseWindow(wxCloseEvent& event);
  void OnUpd8SupportedSMART_IDs(wxCommandEvent &);
  
  /** Necessary for receiving events. */
  DECLARE_EVENT_TABLE()
};

#endif /* WXWIDGETS_SUPPORTEDSMARTIDSDIALOG_HPP_ */
