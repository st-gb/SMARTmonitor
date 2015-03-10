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
    col0.SetWidth(30);
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

    col1.SetId(COL_IDX_lastUpdate);
    col1.SetText( wxT("last update") );
    col1.SetWidth(100);
    InsertColumn(COL_IDX_lastUpdate, col1);
  }

  SMARTtableListCtrl::~SMARTtableListCtrl()
  {
    // TODO Auto-generated destructor stub
  }

} /* namespace wxWidgets */
