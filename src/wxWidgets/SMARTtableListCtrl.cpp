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

    column.SetId(ColumnIndices::lastUpdate);
    column.SetText( wxT("last update") );
    //TODO calculate width needed for the last update time string
    column.SetWidth(300);
    InsertColumn(ColumnIndices::lastUpdate, column);
  }

  void SMARTtableListCtrl::SetSMARTattribValue(
    fastestUnsignedDataType SMARTattributeID,
    fastestUnsignedDataType columnIndex,
    const wxString & wxstrValue,
    const enum SMARTmonitorClient::SMARTvalueRating sMARTvalueRating)
  {
    fastestUnsignedDataType lineNumber = m_SMARTattribIDtoLineNumber[
      SMARTattributeID];
    SetItem(
      lineNumber, //long index
      columnIndex /** column #/ index */,
      wxstrValue);
//    if( sMARTvalueRating == SMARTmonitorClient::SMARTvalueOK )
//    {
    //TODO colour based on SMART (raw) value
//      wxListItem wxListItem;
//      wxListItem.SetId(lineNumber);
//      GetItem(wxListItem);
////    //http://docs.wxwidgets.org/3.1.0/classwx_list_item.html
//      wxListItem.SetTextColour(*wxGREEN);
//    }
  }
  
  void SMARTtableListCtrl::CreateLines()
  {
    fastestUnsignedDataType SMARTattributeID, lineNumber = 0;

    std::set<int> & IDsOfSMARTattributesToObserve = wxGetApp().
      m_IDsOfSMARTattributesToObserve;
    std::set<int>::const_iterator IDofAttributeToObserverIter = 
      IDsOfSMARTattributesToObserve.begin();

    wxListItem wxListItem;
    /** List items Need to be added via InsertItem(), else error: 
     *  "assert index>=0 && < GetItemCount()" not fulfilled */
    /** Traverse all SMART attribute IDs either got from server or read via  
     *  config file.*/
    for( ; IDofAttributeToObserverIter != IDsOfSMARTattributesToObserve.
        end() ; IDofAttributeToObserverIter ++, lineNumber ++)
    {
      SMARTattributeID = * IDofAttributeToObserverIter;

  //    wxListItem.SetText( wxString::Format(wxT("%u"), SMARTattributeID) );
      wxListItem.SetId(/*lineNumber*/SMARTattributeID);
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
