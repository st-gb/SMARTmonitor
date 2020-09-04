/* SMARTtableListCtrl.cpp
 *  Created on: 10.03.2015
 *      Author: mr.sys */

#include <wxWidgets/SMARTtableListCtrl.hpp> //This class.
#include <UserInterface/columnIndices.hpp> //enum columnIndices
#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType
//wxGetApp().m_IDsOfSMARTattributesToObserve
#include <wxWidgets/wxSMARTmonitorApp.hpp> 

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
    column.SetText( _("ID") );
    //TODO calculate width needed for 3 digits ("255" is the highest SMART ID)
    column.SetWidth(textControlFont.GetPixelSize().x * 4);
    InsertColumn(ColumnIndices::SMART_ID, column);

    column.SetId(ColumnIndices::SMARTparameterName);
    column.SetText( _("parameter name") );
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

void SMARTtableListCtrl::SetSMARTattribValue(
  fastestUnsignedDataType SMARTattributeID,
  fastestUnsignedDataType columnIndex,
  const wxString & wxstrValue,
  const enum SMARTvalueRating sMARTvalueRating)
{
  fastestUnsignedDataType lineNumber = m_SMARTattribIDtoLineNumber[
    SMARTattributeID];
  SetItem(
    lineNumber, //long index
    columnIndex /** column #/ index */,
    wxstrValue);
  ///Only do it once/for 1 attribute/column
  if(columnIndex == ColumnIndices::rawValue)
    switch(sMARTvalueRating)
      case SMARTvalueOK:
      case SMARTvalueWarning:
    {
      wxListItem wxListItem;
      wxListItem.SetId(lineNumber);
      GetItem(wxListItem);
      switch(sMARTvalueRating){
       case SMARTvalueOK:
        //http://docs.wxwidgets.org/3.1.0/classwx_list_item.html
        //wxListItem.SetTextColour(*wxGREEN);
        ///https://forums.wxwidgets.org/viewtopic.php?t=26576
        SetItemBackgroundColour(lineNumber, *wxGREEN);
        break;
       case SMARTvalueWarning:
        SetItemBackgroundColour(lineNumber, *wxYELLOW);
        break;
      }
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
