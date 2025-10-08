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
 ///TU_Bln361095SMARTmonUse(EnATA_SMARTattrTblColHdrWstrs)
 #include <UserInterface/SMARTparamTblHdrWStr.h>

///Stefan Gebauer's(TU Berlin matricul. num. 361095) ~"cmnSrc" repository files:
 ///TU_Bln361095disableUseSecC_RunTimeStrFnWarn
 #include <compiler/C,C++/useSecureC_RunTimeStrFuncsWarn.h>

///wxWidgets library header files:
/** (needs package "libwxgtk<<version>>-dev", "libwxbase<<version>>-dbg" under
 * Linux?) */
 /**Avoid Microsoft Visual Studio compiler warnings like :
  * "include\wx / wxcrt.h(213, 14) warning C4996 : 'strcpy' : This function
  * or variable may be unsafe.Consider using strcpy_s instead.To disable
  * deprecation, use _CRT_SECURE_NO_WARNINGS.See online help for details." */
#include <wx/dcclient.h>///class wxClientDC
#include <wx/listctrl.h>///Base class wxListCtrl
#include <wx/settings.h>///wxSystemSettings
 TU_Bln361095disableUseSecC_RunTimeStrFnWarn
 TU_Bln361095enableUseSecC_RunTimeStrFnWarn

///Stefan Gebauer's(TU Berlin matr.#361095)"common_sourcecode" repository files:
 #include <hardware/CPU/fastest_data_type.h>///TU_Bln361095::CPU::faststUint
 ///TU_Bln361095dataCarrierNumSATA_SMARTattrIDs
 #include <hardware/dataCarrier/ATA3Std.h>
//enum SMARTmonitorClient::SMARTvalueRating;

namespace wxWidgets
{

class SMARTtableListCtrl
  : public wxListCtrl
{
private:
  enum evtIDs{decreaseFontSize,increaseFontSize};
  const SMARTuniqueID & m_r_dataCarrier;
public:
  wxClientDC clientDC;
  /*static*/ bool setColWdthAccHdrStrWdth = /*true*/false;
  TU_Bln361095::CPU::faststUint m_maxSMARTattrNmStrWdthInPx;
  TU_Bln361095::CPU::faststUint m_SMARTattribIDtoLineNumber[
    TU_Bln361095dataCarrierNumSATA_SMARTattrIDs+1];

  SMARTtableListCtrl(
    wxWindow * parent,
    const SMARTuniqueID & dataCarrierID,
    wxWindowID id = wxID_ANY,
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize//,
//      long style = wxLC_REPORT
//      this, wxID_ANY, wxDefaultPosition,
//      wxDefaultSize, wxLC_REPORT
    );
  void createCols(const SMARTuniqueID & dataCarrierID);
  /** @brief Before a list item can be changed via "SetItem" it needs to be 
   * inserted.*/
  inline void createATAcols();
  inline void createNVMeCols();
  void CreateLines(const SMARTuniqueID &);
  void SetSMARTattribValue(
    TU_Bln361095::CPU::faststUint SMARTattributeID,
    TU_Bln361095::CPU::faststUint columnIndex,
    const wxString & wxstrValue,
    const SMARTvalRatngTyp);
  virtual
    ~SMARTtableListCtrl();
  const wchar_t * getLabel(/*const int lang,*/ const unsigned colID)
  {
    const wchar_t * lbl;
//    const unsigned numArrEles = sizeof(enColHdrWstrs)/sizeof(enColHdrWstrs[0]);
	/*numArrEles*/)
    if(m_r_dataCarrier.getBusType() == TU_Bln361095::hardware::bus::NVMe)
    {
      if(//lang < /*byndLastLang*//*numLangs &&*/
        colID < TU_Bln361095::SMARTmon::NVMeSMARTattrColIdx::ByndLast)
        lbl = //TU_Bln361095SMARTmonEnNVMeSMARTattrTblColHdrWchrStrs
          TU_Bln361095SMARTmonUse(EnNVMeSMARTattrTblColHdrWchrStrs)/*[lang]*/
            [colID];
      else
        lbl = NULL;
    }
    else
    {
      if(//lang < /*byndLastLang*//*numLangs &&*/
        colID < TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::ByndLast)
        lbl = //TU_Bln361095SMARTmonEnATA_SMARTattrTblColHdrWstrs/*[lang]*/
          TU_Bln361095::SMARTmon::EnATA_SMARTattrTblColHdrWstrs)[colID];
      else
        lbl = NULL;
    }
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
  void getSMARTAttrNmWithMaxPx()
  {
    TU_Bln361095::SMARTmon::MaxSMARTattrTblColValStrWdthInPx[
      TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::ID] =
      getTxtWdthInPx(TU_Bln361095::SMARTmon::WidestATA_SMARTattrTblColWchrStrs[
        TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::ID]);
    TU_Bln361095::SMARTmon::MaxSMARTattrTblColValStrWdthInPx[
      TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::Name] =
      m_maxSMARTattrNmStrWdthInPx;
    for(TU_Bln361095::hardware::CPU::FaststUint colID =
      TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::NrmlzdCurrVal;
      colID < TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::ByndLast; colID++)
    {
      TU_Bln361095::SMARTmon::MaxSMARTattrTblColValStrWdthInPx[colID] =
/**Alternative:get max. value string width via SMARTmonClient::
 * upd8rawAndH_andTime(...) (simulate real output)*/
        getTxtWdthInPx(TU_Bln361095::SMARTmon::WidestATA_SMARTattrTblColWchrStrs
          [colID]);
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
        colWdthInPx += /*50*/TU_Bln361095::SMARTmon::
          MaxSMARTattrTblColValStrWdthInPx[colID];
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
