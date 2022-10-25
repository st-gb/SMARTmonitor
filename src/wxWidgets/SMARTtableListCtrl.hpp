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
#include <UserInterface/SMARTparamTblHdrWStr.h>///enColHdrWstrs

///wxWidgets library header files:
/** (needs package "libwxgtk<<version>>-dev", "libwxbase<<version>>-dbg" under
 * Linux?) */
#include <wx/dcclient.h>///class wxClientDC
#include <wx/listctrl.h>///Base class wxListCtrl
#include <wx/settings.h>///wxSystemSettings

///Stefan Gebauer's(TU Berlin matr.#361095)"common_sourcecode" repository files:
#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType

//enum SMARTmonitorClient::SMARTvalueRating;

namespace wxWidgets
{

class SMARTtableListCtrl
  : public wxListCtrl
{
private:
  enum evtIDs{decreaseFontSize,increaseFontSize};
public:
  wxClientDC clientDC;
  /*static*/ bool setColWdthAccHdrStrWdth = /*true*/false;
  fastestUnsignedDataType m_maxSMARTattrNmStrWdthInPx;
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
  void createCols();
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
  const wchar_t * getLabel(/*const int lang,*/ const unsigned colID)
  {
    const wchar_t * lbl;
//    const unsigned numArrEles = sizeof(enColHdrWstrs)/sizeof(enColHdrWstrs[0]);
    if(//lang < /*byndLastLang*//*numLangs &&*/
      colID < /*byndLastCol*/colIndices::byndLast
	/*numArrEles*/)
      lbl = enColHdrWstrs/*[lang]*/[colID];
    else
      lbl = NULL;
    return lbl;
  }
  /*static*/inline int getTxtWdthInPx(const wchar_t * str) const{
    const wxSize & textSize = clientDC.GetTextExtent(str);
    const int textWidth = textSize.GetWidth();
    return textWidth;
  }
  ///Only needs to be done once for all wxListCtrl objects? On the other hand
  /// different objects may have different font sizes.
  /**@brief gets maximum "parameter name" column width in pixel*/
  void getSMARTAttrNmWithMaxPx(){
    maxColValStrWdthInPx[colIndices::SMART_ID] =
      getTxtWdthInPx(widestColValStr[colIndices::SMART_ID]);
    maxColValStrWdthInPx[colIndices::SMARTparameterName] =
      m_maxSMARTattrNmStrWdthInPx;
    for(unsigned colID = 2; colID < colIndices::byndLast; colID++){
      maxColValStrWdthInPx[colID] =
/**Alternative:get max. value string width via SMARTmonClient::
 * upd8rawAndH_andTime(...) (simulate real output)*/
        getTxtWdthInPx(widestColValStr[colID]);
    }
  }
  inline void setColHdrAndInsCol(wxListItem & column, const long colID)
  {
    column.SetId(colID);//TODO is "SetId" needed at all?
    const wchar_t * lbl = getLabel(/*lang,*/colID);
    if(lbl != NULL)
    {
      column.SetText(lbl);
    /**Add pixels for the vertical separator between 2 columns, else the right
     * part of the value may not be shown.*/
      int colWdthInPx = wxSystemSettings::GetMetric(wxSYS_DRAG_X
/**http://docs.wxwidgets.org/3.0/settings_8h.html#a0f2b19d7a3717cdbef5a04cb05ab8f26
 * "Width of single border."*/ /*wxSYS_BORDER_X*/);
/**http://docs.wxwidgets.org/3.0/classwx_system_settings.html#aa18e3b5794dc4193c4b0668d28d4933a
 * "Returns the value of a system metric, or -1 if the metric is not supported
 * on the current system."*/
      if(colWdthInPx ==  -1)
        colWdthInPx = 10;
      if(setColWdthAccHdrStrWdth)
      {
        const int textWidth = getTxtWdthInPx(lbl);
        colWdthInPx += /*50*/textWidth;
      }
      else/* if(setColWdthAccMaxValStrWdth)*/
        colWdthInPx += /*50*/maxColValStrWdthInPx[colID];
      column.SetWidth(colWdthInPx);
      InsertColumn(colID, column);
    }
  }
  void setMaxSMARTattrNmStrWdthInPx(const wxString & str)
  {
    const wxSize & textSize = clientDC.GetTextExtent(str);
    const int strWdth = textSize.GetWidth();
    if(strWdth > m_maxSMARTattrNmStrWdthInPx)
      m_maxSMARTattrNmStrWdthInPx = strWdth;
  }
  void OnRightMouseButtonDown(wxMouseEvent &);
  void OnPopupClick(wxCommandEvent &);
  DECLARE_EVENT_TABLE()
};
} /* namespace wxWidgets */

#endif /* WXWIDGETS_SMARTTABLELISTCTRL_HPP_ */
