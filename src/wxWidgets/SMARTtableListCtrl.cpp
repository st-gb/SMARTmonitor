/*
 * SMARTtableListCtrl.cpp
 *
 *  Created on: 10.03.2015
 *      Author: mr.sys
 */

#include <wxWidgets/SMARTtableListCtrl.hpp>

namespace wxWidgets
{

  SMARTtableListCtrl::SMARTtableListCtrl(
    wxWindow * parent,
    wxWindowID id /*= wxID_ANY*/,
    const wxPoint& pos /* = wxDefaultPosition*/,
    const wxSize& size /*= wxDefaultSize*///,
    //long style *= wxLC_REPORT
    )
    : wxListCtrl(parent, id, pos, size, wxLC_REPORT)
  {
    const wxFont & textControlFont = GetFont();
//    this->GetDC
//    unsigned numpixels = textControlFont.GetWidth("255");

    /** Adapted from http://wiki.wxwidgets.org/WxListCtrl#Minimal_example_to_get_started*/
    // Add first column
    wxListItem col0;
    col0.SetId(COL_IDX_SMART_ID);
    col0.SetText( _("ID") );
    //TODO calculate width needed for 3 digits
    col0.SetWidth(textControlFont.GetPixelSize().x * 4);
    InsertColumn(COL_IDX_SMART_ID, col0);

    // Add second column
    wxListItem col1;
    col1.SetId(COL_IDX_SMARTparameterName);
    col1.SetText( _("parameter name") );
    col1.SetWidth(200);
    InsertColumn(COL_IDX_SMARTparameterName, col1);

    // Add third column
    col1.SetId(COL_IDX_rawValue);
    col1.SetText( wxT("raw value") );
    col1.SetWidth(70);
    InsertColumn(COL_IDX_rawValue, col1);

    col1.SetId(COL_IDX_humanReadableRawValue);
    col1.SetText( wxT("human readable") );
    col1.SetWidth(50);
    InsertColumn(COL_IDX_humanReadableRawValue, col1);

    col1.SetId(COL_IDX_lastUpdate);
    col1.SetText( wxT("last update") );
    //TODO calculate width needed for the last update time string
    col1.SetWidth(300);
    InsertColumn(COL_IDX_lastUpdate, col1);
  }

  SMARTtableListCtrl::~SMARTtableListCtrl()
  {
    // TODO Auto-generated destructor stub
  }

} /* namespace wxWidgets */
