/** SupportedSMARTIDsDialog.cpp
 *  Created on: 29.10.2016
 *  Author:Stefan Gebauer, M.Sc.Comp.Sc.*/

/**Prevents <winsock.h> to be included in <windows.h>. <Winsock.h> and
 * <Winsock2.h> have the same definitions.->re-definition happens if <winsock.h>
 * is included before <Winsock2.h> */
#define _WINSOCKAPI_

/** Include at 1st in Windows build to avoid:
 * "#warning Please include winsock2.h before windows.h" */
///_This_ (project's) repository header files:
 #include <wxWidgets/SupportedSMARTIDsDialog.hpp>
 #include "wxSMARTmonitorApp.hpp"///wxGetApp()

///C(++) standard library files:
 #include <sstream>///class std::ostringstream

///wxWidgets library header files:
#include <wx/button.h>///class wxButton
#include <wx/listctrl.h> //class wxListCtrl
#include <wx/sizer.h> // class wxBoxSizer
#include <wx/defs.h> //wxID_ANY

#include "wxSMARTmonitorApp.hpp" //wxGetApp()
//#include <attributes/SMARTuniqueID.hpp>
///Stefan Gebauer's(TU Berlin matr. # 361095) ~"cmnSrc" repository header files:
 #include <hardware/CPU/fastest_data_type.h>///TU_Bln361095::CPU::faststUint
 ///TU_Bln361095dataCarrierNumSATA_SMARTattrIDs
 #include <hardware/dataCarrier/ATA3Std.h>
 ///TU_Bln361095::wxWidgets::GetwxString_inln(...)
 #include <wxWidgets/charStr/wxStringHelper.hpp>

BEGIN_EVENT_TABLE(SupportedSMART_IDsDialog, wxDialog)
  EVT_CLOSE(SupportedSMART_IDsDialog::OnCloseWindow)
  EVT_BUTTON(upd8SupportedSMART_IDs, SupportedSMART_IDsDialog::
    OnUpd8SupportedSMART_IDs)
END_EVENT_TABLE()

SupportedSMART_IDsDialog::SupportedSMART_IDsDialog(
  const SMARTuniqueID & sMARTuniqueID,
  SMARTmonitorClient & sMARTmonClient//TODO param not needed. use wxGetApp()
  )
  : wxDialog(NULL, wxID_ANY, wxT("supported SMART IDs"), wxDefaultPosition,
     //wxDefaultSize,
     wxSize(400,700),
     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    ,m_sMARTuniqueID( (SMARTuniqueID &) sMARTuniqueID)
    ,m_sMARTmonClient(sMARTmonClient)
{
  SetTitleFromDataCarrierID(sMARTuniqueID);
  wxSizer * sizerTop = CreateGUI(sMARTuniqueID);

  FillGUI((SMARTuniqueID &) sMARTuniqueID, sMARTmonClient, true);
  SetSizerAndFit(sizerTop);
  Centre();
}

SupportedSMART_IDsDialog::~SupportedSMART_IDsDialog ()
{
  // TODO Auto-generated destructor stub
}

void SupportedSMART_IDsDialog::create2ColListCtrl()
{
//  m_pwxlistctrl = new wxListCtrl( (wxWindow *) this, wxID_ANY, 
//    wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
  /** Adapted from http://wiki.wxwidgets.org/WxListCtrl#Minimal_example_to_get_started*/
  // Add first column
  wxListItem col0;
  col0.SetId(COL_IDX_SMART_ID);
  col0.SetText( _("ID") );
  col0.SetWidth(40);
  m_pwxlistctrl->InsertColumn(COL_IDX_SMART_ID, col0);

  // Add second column
  wxListItem col1;
  col1.SetId(COL_IDX_SMARTparameterName);
  col1.SetText( _("parameter name") );
  col1.SetWidth(200);
  m_pwxlistctrl->InsertColumn(COL_IDX_SMARTparameterName, col1);
}

void SupportedSMART_IDsDialog::cr8ListCtrl()
{
//  create2ColListCtrl();
  m_pwxlistctrl = new wxWidgets::SMARTtableListCtrl( (wxWindow *) this, 
    wxID_ANY, wxDefaultPosition, wxDefaultSize);
}

wxSizer * SupportedSMART_IDsDialog::CreateGUI(
  const SMARTuniqueID & dataCarrierID)
{
  wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);
  wxSizer * const sizerTopLine = new wxBoxSizer(wxHORIZONTAL);
  std::string std_strDataCarrierID = dataCarrierID.str();
  wxTextCtrl * p_wxTextCtrl = new wxTextCtrl(this, wxID_ANY, 
    wxT("ALL S.M.A.R.T. attribute IDs supported by\n") + 
      TU_Bln361095::wxWidgets::GetwxString_inln(std_strDataCarrierID),
    wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
  p_wxTextCtrl->SetEditable(false);

  sizerTopLine->Add(p_wxTextCtrl,
    /** "in the main orientation of the wxBoxSizer - where 0 stands for not
     * changeable" */
    1,
    wxEXPAND, 0);

  m_p_upd8SupportedSMART_IDs = new wxButton(this, upd8SupportedSMART_IDs, 
    wxT("update"));
  sizerTopLine->Add(m_p_upd8SupportedSMART_IDs, 0, wxSTRETCH_NOT, 0);
  
  sizerTop->Add(sizerTopLine,
    /** "in the main orientation of the wxBoxSizer - where 0 stands for not
     * changeable" */
    0,
    wxEXPAND, 0);

  cr8ListCtrl();

  sizerTop->Add( m_pwxlistctrl, 1, wxEXPAND, 0);
  return sizerTop;
}

void SupportedSMART_IDsDialog::FillGUI(
  SMARTuniqueID & sMARTuniqueID
  ,SMARTmonitorClient & sMARTmonClient
  ,const bool create
  )
{
  TU_Bln361095::CPU::faststUint SMART_ID;
//  m_pwxlistctrl->SetItemCount(SMARTattributeNamesAndIDs.size() );
  
  const SMARTuniqueIDandValues * p_sMARTuniqueIDandValues = NULL;

#ifdef TU_Bln361095SMARTmonDrctSMARTaccss_
  //TODO # supported SMART IDs is 30? -> less space
  TU_Bln361095::CPU::faststUint sMARTattrIDsToRead[
    TU_Bln361095dataCarrierNumSATA_SMARTattrIDs];
  TU_Bln361095::CPU::faststUint arrIdx = 0;
  for(; arrIdx < TU_Bln361095dataCarrierNumSATA_SMARTattrIDs; arrIdx++)
    sMARTattrIDsToRead[arrIdx] = arrIdx + 1;
  if(arrIdx < TU_Bln361095dataCarrierNumSATA_SMARTattrIDs)
    sMARTattrIDsToRead[arrIdx] = 0;
  
  //Get supported SMART IDs for data carrier identifier
  std::string dvc = wxGetApp().s_dataCarrierID2devicePath[sMARTuniqueID];
  if(dvc != ""){///Empty string if S.M.A.R.T. data indirectly/from server.
    const enum SMARTaccessBase::retCodes SMARTaccRetCode = wxGetApp().
      m_SMARTaccess.readSMARTforDevice(
      dvc.c_str(),
      sMARTuniqueID,
      wxGetApp().s_dataCarrierID2devicePath,
      sMARTattrIDsToRead);
    if(SMARTaccRetCode == SMARTaccessBase::success)
      p_sMARTuniqueIDandValues = &*wxGetApp().m_SMARTaccess.
        GetSMARTuniqueIDandValues().find(SMARTuniqueIDandValues(sMARTuniqueID));
  }
  else{
#endif
  SMARTmonitorBase::SMARTuniqueIDandValsContType::const_iterator
    sMARTuniqueIDandValsIter = wxGetApp().GetSMARTuniqueIDsAndVals().find(
    TU_Bln361095::SMARTmon::SMARTuniqueIDandValues(sMARTuniqueID) );
  if(sMARTuniqueIDandValsIter != wxGetApp().GetSMARTuniqueIDsAndVals().end() )
    p_sMARTuniqueIDandValues = &*sMARTuniqueIDandValsIter;
#ifdef TU_Bln361095SMARTmonDrctSMARTaccss_
  }
#endif
  
  for(TU_Bln361095::CPU::faststUint lineNumber = 0; lineNumber <
    TU_Bln361095dataCarrierNumSATA_SMARTattrIDs
    && sMARTuniqueID.supportedSMART_IDs[lineNumber] != 0; ++ lineNumber)
  {
    SMART_ID = sMARTuniqueID.supportedSMART_IDs[lineNumber];
    if(create){
      wxListItem item;
      item.SetId(lineNumber); //item line number
      m_pwxlistctrl->InsertItem( item );
     ///Necessary for indirectly called SMARTtableListCtrl::SetSMARTattribValue(
      m_pwxlistctrl->m_SMARTattribIDtoLineNumber[SMART_ID] = lineNumber;
      wxGetApp().setIDandLabel(sMARTuniqueID, SMART_ID, m_pwxlistctrl);
    }
    if(p_sMARTuniqueIDandValues){
      const ModelAndFirmware * p_currModelAndFirmware = wxGetApp().
        getDataCarrierAttrDefs(sMARTuniqueID);
      sMARTmonClient.upd8rawAndH_andTime(SMART_ID, *p_sMARTuniqueIDandValues,
        m_pwxlistctrl, p_currModelAndFirmware);
    }
  }
}

void SupportedSMART_IDsDialog::OnCloseWindow(wxCloseEvent& event)
{
  wxGetApp().openTopLevelWindows.erase(this);
  Destroy();
}

void SupportedSMART_IDsDialog::OnUpd8SupportedSMART_IDs(wxCommandEvent & event){
  FillGUI(m_sMARTuniqueID, m_sMARTmonClient, false);
}

void SupportedSMART_IDsDialog::SetTitleFromDataCarrierID(
  const SMARTuniqueID & dataCarrierID)
{
  std::ostringstream std_oss;
  std_oss << dataCarrierID/*.str()*/.m_modelName << " " <<
    dataCarrierID.m_firmWareName;
  wxString wstrDataCarrierID = TU_Bln361095::wxWidgets::GetwxString_inln(
    std_oss.str() );
  /** Show data carrier model (, firmware, serial #) in title bar because 
   *  the supported attribute IDs are specific to the HDD model e.g.. */
  SetTitle(wstrDataCarrierID + wxT("--all supported S.M.A.R.T. IDs") );
}