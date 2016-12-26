/*
 * SupportedSMARTIDs.hpp
 *
 *  Created on: 29.10.2016
 *      Author: root
 */

#ifndef WXWIDGETS_SUPPORTEDSMARTIDSDIALOG_HPP_
#define WXWIDGETS_SUPPORTEDSMARTIDSDIALOG_HPP_

#include <wx/dialog.h> //base class wxDialog
#include <attributes/SMARTattributeNameAndID.hpp>
#include <vector> //class std::vector
#include <client/SMARTmonitorClient.h> //class SMARTmonitorClient

/** Forward declarations: */
class wxListCtrl;
class wxSizer;
//class SMARTmonitorClient;
//SMARTmonitorClient::dataCarrierID2supportedSMARTattributesMap_type;

class SupportedSMART_IDsDialog
  : public wxDialog
{
  wxListCtrl * m_pwxlistctrl;
public:
  SupportedSMART_IDsDialog (//std::vector<SMARTattributeNameAndID> &
    SMARTmonitorClient::dataCarrierID2supportedSMARTattributesMap_type::const_iterator);
  virtual
  ~SupportedSMART_IDsDialog ();
  enum columnIndices { COL_IDX_SMART_ID = 0 , COL_IDX_SMARTparameterName};
  wxSizer * CreateGUI(const SMARTuniqueID & dataCarrierID);
  void FillGUI(const SMARTmonitorClient::supportedSMARTattributeIDs_type & 
    supportedSMART_IDs);
  void SetTitleFromDataCarrierID(const SMARTuniqueID & dataCarrierID);
  void OnCloseWindow(wxCloseEvent& event);
  
  /** Necessary for receiving events. */
  DECLARE_EVENT_TABLE()
};

#endif /* WXWIDGETS_SUPPORTEDSMARTIDSDIALOG_HPP_ */
