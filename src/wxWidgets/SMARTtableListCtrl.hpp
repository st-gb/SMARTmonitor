/* SMARTtableListCtrl.hpp
 *  Created on: 10.03.2015
 *      Author: mr.sys  */
#ifndef WXWIDGETS_SMARTTABLELISTCTRL_HPP_
#define WXWIDGETS_SMARTTABLELISTCTRL_HPP_

#include <wx/listctrl.h> //Base class wxListCtrl
#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType
#include <client/SMARTmonitorClient.h>//enum SMARTmonitorClient::SMARTvalueRating

//enum SMARTmonitorClient::SMARTvalueRating;

namespace wxWidgets
{

class SMARTtableListCtrl
  : public wxListCtrl
{
private:
public:
  fastestUnsignedDataType m_SMARTattribIDtoLineNumber[numDifferentSMART_IDs+1];
    SMARTtableListCtrl(
      wxWindow * parent,
      wxWindowID id = wxID_ANY,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize//,
//      long style = wxLC_REPORT
//      this, wxID_ANY, wxDefaultPosition,
//      wxDefaultSize, wxLC_REPORT
      );
  /** @brief Before a list item can be changed via "SetItem" it needs to be 
   * inserted.*/
  void CreateLines(const SMARTmonitorBase::SMARTattrToObsType &IDsOfSMARTattrs);
    void SetSMARTattribValue(
      fastestUnsignedDataType SMARTattributeID,
      fastestUnsignedDataType columnIndex,
      const wxString & wxstrValue,
      const enum SMARTmonitorClient::SMARTvalueRating);
    virtual
    ~SMARTtableListCtrl();
  };

} /* namespace wxWidgets */

#endif /* WXWIDGETS_SMARTTABLELISTCTRL_HPP_ */
