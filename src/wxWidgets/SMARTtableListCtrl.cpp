/* SMARTtableListCtrl.cpp
 * Created on: 10.03.2015
 * Author: Stefan Gebauer, M.Sc. Comp.Sc. */

/**from >>SMARTmonitor root directory<</src/wxWidgets/SMARTtableListCtrl.cpp:15:
 * >>TDM-GCC 64bit 10.3.0-2 root directory<</x86_64-w64-mingw32/include/
 * winsock2.h:15:2: warning: #warning Please include winsock2.h before
 * windows.h [-Wcpp]*/
//#include <winsock2.h>
/**Prevents <winsock.h> to be included in <windows.h>. <Winsock.h> and
 * <Winsock2.h> have the same definitions.->re-definition happens if <winsock.h>
 * is included before <Winsock2.h> */
#define _WINSOCKAPI_

///wxWidgets header files:
#include <wx/dcclient.h>///class wxClientDC
#include <wx/menu.h>///class wxMenu

//This repository's header files:
#include <wxWidgets/SMARTtableListCtrl.hpp>//This class.
 ///TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdcs enum (values)
 #include <UserInterface/columnIndices.hpp>
///enCtxMenuLwrFntSzASCIIlit,enCtxMenuHghrFntSzASCIIlit
#include <UserInterface/enLiterals.h>
//wxGetApp().m_IDsOfSMARTattributesToObserve
#include <wxWidgets/wxSMARTmonitorApp.hpp>

///Stefan Gebauer's(TU Berlin matr.#361095)"common_sourcecode" repository files:
#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType

namespace wxWidgets
{

///Don't use indentation for namespaces
BEGIN_EVENT_TABLE(SMARTtableListCtrl, wxListCtrl)
  EVT_RIGHT_DOWN(SMARTtableListCtrl::OnRightMouseButtonDown)
END_EVENT_TABLE()

  //TODO make constructor with SMARTuniqueID? because all values depend on it
  SMARTtableListCtrl::SMARTtableListCtrl(
    wxWindow * parent,
    wxWindowID id /*= wxID_ANY*/,
    const wxPoint& pos /* = wxDefaultPosition*/,
    const wxSize& size /*= wxDefaultSize*///,
    //long style *= wxLC_REPORT
    )
    : wxListCtrl(parent, id, pos, size, wxLC_REPORT)
    , m_maxSMARTattrNmStrWdthInPx(0)
    ///http://forums.wxwidgets.org/viewtopic.php?t=17143
    , clientDC(this)
    , m_r_dataCarrier(dataCarrierID)
  {
    SetToolTip(wxT(enRghtClckTooltip) );
//	colHdrStrs[enColStrsIdx][en] = enColHdrStrs;

    getSMARTAttrNmWithMaxPx();///For width of parameter name column
    createCols(dataCarrierID);
  }

  inline void SMARTtableListCtrl::createATAcols()
  {
//    this->GetDC
//    unsigned numpixels = textControlFont.GetWidth("255");
    /** Adapted from
     * http://wiki.wxwidgets.org/WxListCtrl#Minimal_example_to_get_started */
    wxListItem column;

    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::
      ID);

    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::
      NrmlzdCurrVal);
    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::
      NrmlzdThresh);

    column.SetId(TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::Name);
    column.SetText(wxT("parameter name"));
    column.SetWidth(200);
    InsertColumn(TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::Name, column);

    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::
      RawVal);
    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::
      HumanReadableRawVal);
    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::
      Unit);
    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::
      UnitRange);
    //TODO calculate width needed for the last update time string
    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::ATA_SMARTattrTblColIdx::
      LastUpdate);
  }

  inline void SMARTtableListCtrl::createNVMeCols()
  {
    /** Adapted from
     * http://wiki.wxwidgets.org/WxListCtrl#Minimal_example_to_get_started */
    wxListItem column;

    /**Calculate width needed for 3 digits ("255" is the highest SMART ID)
     * is 0 under Windows.*/
//    const int letterWidth = textControlFont.GetPixelSize().x;
//    column.SetWidth(50/*letterWidth * 4*/);

    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::NVMeSMARTattrColIdx::ID);

    column.SetId(TU_Bln361095::SMARTmon::NVMeSMARTattrColIdx::Name);
    column.SetText(wxT("parameter name"));
    column.SetWidth(200);
    InsertColumn(TU_Bln361095::SMARTmon::NVMeSMARTattrColIdx::Name, column);

    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::NVMeSMARTattrColIdx::
      RawVal);
    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::NVMeSMARTattrColIdx::
      HumanReadableRawVal);
    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::NVMeSMARTattrColIdx::
      Unit);
    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::NVMeSMARTattrColIdx::
      UnitRange);
    //TODO calculate width needed for the last update time string
    setColHdrAndInsCol(column, TU_Bln361095::SMARTmon::NVMeSMARTattrColIdx::
      LastUpdate);
  }

  void SMARTtableListCtrl::createCols(const SMARTuniqueID & dataCarrierID)
  {
//    this->GetDC
//    unsigned numpixels = textControlFont.GetWidth("255");

    if(m_r_dataCarrier.getBusType() == TU_Bln361095::hardware::bus::NVMe)
      createNVMeCols();
    else
    {
      createATAcols();
    }
  }

inline void GetClrAccordng1toMns1Rng(const SMARTvalRatngTyp SMARTvalRatng,
  wxColour & color)
{
  if(SMARTvalRatng < 0.0f){///current value < threshold->make red
    /**Should be at least medium red to signalize/visualize error so it can be
     *  seen easer by human eyes.*/
    ///Worst rating value: -1.0 => with worst rating it gets dark red.
    const wxColourBase::ChannelType blueAndGreen = 127 - SMARTvalRatng*-127.0f;
  /** http://docs.wxwidgets.org/3.0/classwx_colour.html : red, green, blue,
   * transparent */
    color.Set(255, blueAndGreen, blueAndGreen);
  }
  /**Between fully OK and threshold=>color between green and yellow meaning "OK"
    * or "warning"*/
  else if(SMARTvalRatng <= 1.0f){
/**http://en.wikipedia.org/wiki/Yellow : "sRGB^B (r, g, b)	(255, 255, 0)"
 * "B: Normalized to [0–255] (byte)" */
/**http://en.wikipedia.org/wiki/Orange_(colour):"sRGB^B (r, g, b)	(255, 165, 0)"
 * "B: Normalized to [0–255] (byte)" */
    wxColourBase::ChannelType red = 255-(int)(255.0f * SMARTvalRatng /*/FLT_MAX*/);
  /** http://docs.wxwidgets.org/3.0/classwx_colour.html : red, green, blue,
   * transparent */
    color.Set(red,/**green*/255, 0);
  }
  else///Rating is "unknown", for example because of missing metric.
    color.Set(0,255,255);///cyan
}

inline void GetClrAccordng1toEnum(const enum SMARTvalueRating SMARTvalRatng,
  wxColour & color)
{
  switch(SMARTvalRatng){
   case SMARTvals::Rating::OK:
    color = *wxGREEN;
    break;
   case SMARTvals::Rating::atLeast1Warn:
    color = *wxYELLOW;
    break;
//    default:
  }
}

void SMARTtableListCtrl::SetSMARTattribValue(
  fastestUnsignedDataType SMARTattributeID,
  fastestUnsignedDataType columnIndex,
  const wxString & wxstrValue,
  const SMARTvalRatngTyp SMARTvalRatng)
{
  fastestUnsignedDataType lineNumber = m_SMARTattribIDtoLineNumber[
    SMARTattributeID];
  SetItem(
    lineNumber, //long index
    columnIndex /** column #/ index */,
    wxstrValue);
  wxColour color;
  GetClrAccordng1toMns1Rng(SMARTvalRatng, color);
  ///Only do it once/for 1 attribute/column
  if(columnIndex == colIndices::rawValue)
  {
    //http://docs.wxwidgets.org/3.1.0/classwx_list_item.html
/*  wxListItem wxListItem;
    wxListItem.SetId(lineNumber);
    GetItem(wxListItem);
    wxListItem.SetTextColour(color);*/
    ///https://forums.wxwidgets.org/viewtopic.php?t=26576
    SetItemBackgroundColour(lineNumber, color);
  }
}

void SMARTtableListCtrl::CreateLines(const SMARTuniqueID & sMARTuniqueID)
{
  fastestUnsignedDataType SMARTattributeID, lineNumber = 0;

  wxListItem wxListItem;
  /** List items Need to be added via InsertItem(), else error: 
   *  "assert index>=0 && < GetItemCount()" not fulfilled */
  /** Traverse all SMART attribute IDs either got from server or read via  
   *  config file.*/
  for(;sMARTuniqueID.SMART_IDsToReadNotEnd(lineNumber); lineNumber++)
  {
    SMARTattributeID = sMARTuniqueID.m_SMART_IDsToRd[lineNumber];

  //    wxListItem.SetText( wxString::Format(wxT("%u"), SMARTattributeID) );
    wxListItem.SetId(SMARTattributeID);
    m_SMARTattribIDtoLineNumber[SMARTattributeID] = lineNumber;
    InsertItem(wxListItem);
  }
#ifdef DEBUG
  int itemCount = GetItemCount();
  itemCount = itemCount;
#endif
}

void SMARTtableListCtrl::OnRightMouseButtonDown(wxMouseEvent & evt)
{
  wxMenu menu(/*wxT("")*//**Set menu title because menu may be shown outside of
    * user interface contol */wxGetApp().GetAppName()
    /*+ wxT("SMART parameters table")*/);

  wxFont font = GetFont();
  const int fontPointSize = font.GetPointSize();
  if(fontPointSize > wxGetApp().GetMinFntSizInPt() )
    menu.Append(decreaseFontSize, wxT(enCtxMenuLwrFntSzASCIIlit) );
  menu.Append(increaseFontSize, wxT(enCtxMenuHghrFntSzASCIIlit) );
  menu.Connect(wxEVT_COMMAND_MENU_SELECTED,
    /**func*/(wxObjectEventFunction) & SMARTtableListCtrl::OnPopupClick,
    /**userData*/NULL, /**eventSink*/this);
  PopupMenu(&menu);
}

void SMARTtableListCtrl::OnPopupClick(wxCommandEvent & evt)
{
  switch(evt.GetId() )
  {
    case increaseFontSize :
     {
      wxFont font = GetFont();
      int fontPointSize = font.GetPointSize();
      fontPointSize++;
      font.SetPointSize(fontPointSize);
      SetFont(font);
     }
     break;
    case decreaseFontSize :
     {
      wxFont font = GetFont();
      int fontPointSize = font.GetPointSize();
      if(fontPointSize > wxGetApp().GetMinFntSizInPt() ){
        fontPointSize--;
        font.SetPointSize(fontPointSize);
        SetFont(font);
      }
     }
     break;
  }
}

  SMARTtableListCtrl::~SMARTtableListCtrl()
  {
    // TODO Auto-generated destructor stub
  }

} /* namespace wxWidgets */
