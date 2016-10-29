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

SupportedSMART_IDsDialog::SupportedSMART_IDsDialog(
  std::vector<SMARTattributeNameAndID> & SMARTattributeNamesAndIDs)
  : wxDialog(NULL, wxID_ANY, wxT("supported SMART IDs"), wxDefaultPosition, //wxDefaultSize,
     wxSize(400,400),
     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
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
//  m_pwxlistctrl->SetItemCount(SMARTattributeNamesAndIDs.size() );
  for( fastestUnsignedDataType lineNumber = 0; lineNumber < SMARTattributeNamesAndIDs.
    size() ; ++ lineNumber)
  {
    wxListItem item;
    item.SetId(lineNumber); //item line number
    item.SetText( wxString::Format(wxT("%u"),
      SMARTattributeNamesAndIDs[lineNumber].getID())
      );
    m_pwxlistctrl->InsertItem( item );

    wxSMARTattribName = wxWidgets::GetwxString_Inline(
      SMARTattributeNamesAndIDs[lineNumber].getName().c_str());
    m_pwxlistctrl->SetItem(lineNumber, 1, wxSMARTattribName);
  }
  SetSizerAndFit(sizerTop);
  Centre();
}

SupportedSMART_IDsDialog::~SupportedSMART_IDsDialog ()
{
  // TODO Auto-generated destructor stub
}

