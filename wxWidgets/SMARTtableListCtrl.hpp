/*
 * SMARTtableListCtrl.hpp
 *
 *  Created on: 10.03.2015
 *      Author: mr.sys
 */

#ifndef WXWIDGETS_SMARTTABLELISTCTRL_HPP_
#define WXWIDGETS_SMARTTABLELISTCTRL_HPP_

#include <wx/listctrl.h> //Base class wxListCtrl

namespace wxWidgets
{

  class SMARTtableListCtrl
    : public wxListCtrl
  {
  public:
    enum columnIndices { COL_IDX_SMART_ID = 0 , COL_IDX_SMARTparameterName,
      COL_IDX_rawValue, COL_IDX_lastUpdate };
    SMARTtableListCtrl(
      wxWindow * parent,
      wxWindowID id = wxID_ANY,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize//,
//      long style = wxLC_REPORT
//      this, wxID_ANY, wxDefaultPosition,
//      wxDefaultSize, wxLC_REPORT
      );
    virtual
    ~SMARTtableListCtrl();
  };

} /* namespace wxWidgets */

#endif /* WXWIDGETS_SMARTTABLELISTCTRL_HPP_ */
