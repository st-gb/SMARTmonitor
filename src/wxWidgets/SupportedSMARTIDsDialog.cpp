/** SupportedSMARTIDsDialog.cpp
 *  Created on: 29.10.2016
 *      Author: SG  */
#include <wx/listctrl.h> //class wxListCtrl
#include <wx/sizer.h> // class wxBoxSizer
#include <wx/defs.h> //wxID_ANY
#include <wxWidgets/SupportedSMARTIDsDialog.hpp>
#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
#include "wxSMARTmonitorApp.hpp" //wxGetApp()
//#include <attributes/SMARTuniqueID.hpp>

BEGIN_EVENT_TABLE(SupportedSMART_IDsDialog, wxDialog)
  EVT_CLOSE(SupportedSMART_IDsDialog::OnCloseWindow)
END_EVENT_TABLE()

SupportedSMART_IDsDialog::SupportedSMART_IDsDialog(
  //std::vector<SMARTattributeNameAndID> & SMARTattributeNamesAndIDs
  dataCarrierID2supportedSMARTattributesMap_type::const_iterator iter
  )
  : wxDialog(NULL, wxID_ANY, wxT("supported SMART IDs"), wxDefaultPosition, //wxDefaultSize,
     wxSize(400,400),
     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  SetTitleFromDataCarrierID(iter->first);  
  wxSizer * sizerTop = CreateGUI(iter->first);

  FillGUI(iter->second);
  SetSizerAndFit(sizerTop);
  Centre();
}

SupportedSMART_IDsDialog::~SupportedSMART_IDsDialog ()
{
  // TODO Auto-generated destructor stub
}

wxSizer * SupportedSMART_IDsDialog::CreateGUI(
  const SMARTuniqueID & dataCarrierID)
{
  wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);
  std::string std_strDataCarrierID = dataCarrierID.str();
  wxTextCtrl * p_wxTextCtrl = new wxTextCtrl(this, wxID_ANY, 
    wxT("S.M.A.R.T. attribute IDs supported by\n") + 
      wxWidgets::GetwxString_Inline(std_strDataCarrierID),
    wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
  p_wxTextCtrl->SetEditable(false);

  sizerTop->Add( p_wxTextCtrl,
    /** "in the main orientation of the wxBoxSizer - where 0 stands for not
     * changeable" */
    0,
    wxEXPAND, 1);

  m_pwxlistctrl = new wxListCtrl( (wxWindow *) this, wxID_ANY, wxDefaultPosition,
    wxDefaultSize, wxLC_REPORT);

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

  sizerTop->Add( m_pwxlistctrl, 1, wxEXPAND, 0);
  return sizerTop;
}

void SupportedSMART_IDsDialog::FillGUI(
  const supportedSMARTattributeIDs_type & supportedSMART_IDs)
{
  fastestUnsignedDataType SMART_ID;
//  m_pwxlistctrl->SetItemCount(SMARTattributeNamesAndIDs.size() );
   supportedSMARTattributeIDs_type::const_iterator 
    supportedSMART_IDsIter = supportedSMART_IDs.begin();
  
  wxString wxSMARTattribName;
  for( fastestUnsignedDataType lineNumber = 0; lineNumber < supportedSMART_IDs.
    size() ; ++ lineNumber, supportedSMART_IDsIter++)
  {
    wxListItem item;
    SMART_ID = *supportedSMART_IDsIter;
    item.SetId(lineNumber); //item line number
    item.SetText( wxString::Format(wxT("%u"), SMART_ID ) );
    m_pwxlistctrl->InsertItem( item );
    
    SMARTattrDef * p_sMARTattrDef;
    if( (p_sMARTattrDef = wxGetApp().getSMARTattrDef(SMART_ID)) != NULL ){
      wxSMARTattribName = wxWidgets::GetwxString_Inline(
        p_sMARTattrDef->GetName() );
      m_pwxlistctrl->SetItem(lineNumber, 1, wxSMARTattribName);
    }
  }
}

void SupportedSMART_IDsDialog::OnCloseWindow(wxCloseEvent& event)
{
  wxGetApp().openTopLevelWindows.erase(this);
  Destroy();
}

void SupportedSMART_IDsDialog::SetTitleFromDataCarrierID(
  const SMARTuniqueID & dataCarrierID)
{
  std::ostringstream std_oss;
  std_oss << dataCarrierID/*.str()*/.m_modelName << " " <<
    dataCarrierID.m_firmWareName;
  wxString wstrDataCarrierID = wxWidgets::GetwxString_Inline(std_oss.str() );
  /** Show data carrier model (, firmware, serial #) in title bar because 
   *  the supported attribute IDs are specific to the HDD model e.g.. */
  SetTitle( /*"SMART IDs supported by " +*/ wstrDataCarrierID );  
}