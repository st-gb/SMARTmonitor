/* SMARTtableListCtrl.hpp
 * Created on: 10.03.2015
 * Author: Stefan Gebauer, M.Sc. Comp.Sc. */
#ifndef WXWIDGETS_SMARTTABLELISTCTRL_HPP_
#define WXWIDGETS_SMARTTABLELISTCTRL_HPP_

///Files from _this_ repository:
/** Include at 1st in Windows build to avoid:
 * "#warning Please include winsock2.h before windows.h" */
///enum SMARTmonitorClient::SMARTvalueRating
#include <client/SMARTmonitorClient.h>

///wxWidgets library header files:
/** (needs package "libwxgtk<<version>>-dev", "libwxbase<<version>>-dbg" under
 * Linux?) */
#include <wx/listctrl.h>///Base class wxListCtrl

///Stefan Gebauer's "common_sourcecode" repository:
#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType

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
  void CreateLines(const SMARTuniqueID &);
    void SetSMARTattribValue(
      fastestUnsignedDataType SMARTattributeID,
      fastestUnsignedDataType columnIndex,
      const wxString & wxstrValue,
      const SMARTvalRatngTyp);
    virtual
    ~SMARTtableListCtrl();
  };

} /* namespace wxWidgets */

#endif /* WXWIDGETS_SMARTTABLELISTCTRL_HPP_ */
