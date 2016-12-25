/*
 * SupportedSMARTIDsDialog.cpp
 *
 *  Created on: 29.10.2016
 *      Author: root
 */

#include <wx/listctrl.h> //class wxListCtrl
#include <wx/sizer.h> // class wxBoxSizer
#include <wx/defs.h> //wxID_ANY
#include <wxWidgets/SupportedSMARTIDsDialog.hpp>
#include <fastest_data_type.h> //fastestUnsignedDataType
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
//#include <attributes/SMARTuniqueID.hpp>

SupportedSMART_IDsDialog::SupportedSMART_IDsDialog(
  //std::vector<SMARTattributeNameAndID> & SMARTattributeNamesAndIDs
  SMARTmonitorClient::dataCarrierID2supportedSMARTattributesMap_type::const_iterator iter
  )
  : wxDialog(NULL, wxID_ANY, wxT("supported SMART IDs"), wxDefaultPosition, //wxDefaultSize,
     wxSize(400,400),
     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  std::string std_strDataCarrierID = iter->first.str();
  SMARTmonitorClient::supportedSMARTattributeIDs_type supportedSMART_IDs = iter->second;
  wxString wstrDataCarrierID = wxWidgets::GetwxString_Inline(std_strDataCarrierID);
  /** Show data carrier model (, firmware, serial #) in title bar because 
   *  the supported attribute IDs are specific to the HDD model e.g.. */
  SetTitle("SMART IDs supported by " + wstrDataCarrierID );
  wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

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

  wxString wxSMARTattribName;
  fastestUnsignedDataType SMART_ID;
//  m_pwxlistctrl->SetItemCount(SMARTattributeNamesAndIDs.size() );
   SMARTmonitorClient::supportedSMARTattributeIDs_type::const_iterator 
    supportedSMART_IDsIter = supportedSMART_IDs.begin();
  for( fastestUnsignedDataType lineNumber = 0; lineNumber < supportedSMART_IDs.
    size() ; ++ lineNumber, supportedSMART_IDsIter++)
  {
    wxListItem item;
    SMART_ID = *supportedSMART_IDsIter;
    item.SetId(lineNumber); //item line number
    item.SetText( wxString::Format(wxT("%u"), SMART_ID ) );
    m_pwxlistctrl->InsertItem( item );

//    wxSMARTattribName = wxWidgets::GetwxString_Inline(
//      supportedSMART_IDs[lineNumber].getName().c_str());
//    m_pwxlistctrl->SetItem(lineNumber, 1, wxSMARTattribName);
  }
  SetSizerAndFit(sizerTop);
  Centre();
}

SupportedSMART_IDsDialog::~SupportedSMART_IDsDialog ()
{
  // TODO Auto-generated destructor stub
}

