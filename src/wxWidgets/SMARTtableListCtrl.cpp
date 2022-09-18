/* SMARTtableListCtrl.cpp
 * Created on: 10.03.2015
 * Author: Stefan Gebauer, M.Sc. Comp.Sc. */

//This repository's header files:
#include <wxWidgets/SMARTtableListCtrl.hpp>//This class.
#include <UserInterface/columnIndices.hpp> //enum columnIndices
//wxGetApp().m_IDsOfSMARTattributesToObserve
#include <wxWidgets/wxSMARTmonitorApp.hpp> 

///"common_sourcecode" repository:
#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType

namespace wxWidgets
{
  //TODO make constructor with SMARTuniqueID? because all values depend on it
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

    /** Adapted from 
     * http://wiki.wxwidgets.org/WxListCtrl#Minimal_example_to_get_started */
    wxListItem column;
    
    //TODO is "SetId" needed at all?
    column.SetId(ColumnIndices::SMART_ID);
    column.SetText(wxT("ID") );
    //TODO calculate width needed for 3 digits ("255" is the highest SMART ID)
    //TODO is 0 under Windows.
    const int letterWidth = textControlFont.GetPixelSize().x;
    column.SetWidth(letterWidth * 4);
    InsertColumn(ColumnIndices::SMART_ID, column);

    column.SetId(ColumnIndices::nrmlzdCurrVal);
    column.SetText(wxT("current value(normalized)") );
    column.SetWidth(50);
    InsertColumn(ColumnIndices::nrmlzdCurrVal, column);

    column.SetId(ColumnIndices::nrmlzdThresh);
    column.SetText(wxT("threshold value(normalized)") );
    column.SetWidth(50);
    InsertColumn(ColumnIndices::nrmlzdThresh, column);

    column.SetId(ColumnIndices::SMARTparameterName);
    column.SetText(wxT("parameter name") );
    column.SetWidth(200);
    InsertColumn(ColumnIndices::SMARTparameterName, column);

    column.SetId(ColumnIndices::rawValue);
    column.SetText( wxT("raw value") );
    column.SetWidth(70);
    InsertColumn(ColumnIndices::rawValue, column);

    column.SetId(ColumnIndices::humanReadableRawValue);
    column.SetText( wxT("human readable") );
    column.SetWidth(50);
    InsertColumn(ColumnIndices::humanReadableRawValue, column);

    column.SetId(ColumnIndices::unit);
    column.SetText( wxT("unit") );
    column.SetWidth(50);
    InsertColumn(ColumnIndices::unit, column);
    
    column.SetId(ColumnIndices::unitRange);
    column.SetText( wxT("~unit range") );
    column.SetWidth(50);
    InsertColumn(ColumnIndices::unitRange, column);
    
    column.SetId(ColumnIndices::lastUpdate);
    column.SetText( wxT("last update [uptime]") );
    //TODO calculate width needed for the last update time string
    column.SetWidth(150);
    InsertColumn(ColumnIndices::lastUpdate, column);
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
  else{/**Between fully OK and threshold=>color between green and yellow meaning
    * OK or warning*/
/**http://en.wikipedia.org/wiki/Yellow : "sRGB^B (r, g, b)	(255, 255, 0)"
 * "B: Normalized to [0–255] (byte)" */
/**http://en.wikipedia.org/wiki/Orange_(colour):"sRGB^B (r, g, b)	(255, 165, 0)"
 * "B: Normalized to [0–255] (byte)" */
    wxColourBase::ChannelType red = 255-(int)(255.0f * SMARTvalRatng /*/FLT_MAX*/);
  /** http://docs.wxwidgets.org/3.0/classwx_colour.html : red, green, blue,
   * transparent */
    color.Set(red,/**green*/255, 0);
  }
}

inline void GetClrAccordng1toEnum(const enum SMARTvalueRating SMARTvalRatng,
  wxColour & color)
{
  switch(SMARTvalRatng){
   case SMARTvalueOK:
    color = *wxGREEN;
    break;
   case SMARTvalueWarning:
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
  if(columnIndex == ColumnIndices::rawValue)
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

  SMARTtableListCtrl::~SMARTtableListCtrl()
  {
    // TODO Auto-generated destructor stub
  }

} /* namespace wxWidgets */
