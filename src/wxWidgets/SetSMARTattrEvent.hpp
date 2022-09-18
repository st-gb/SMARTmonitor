/* File: SetSMARTattrEvent.hpp
 * Author: Stefan Gebauer, M. Sc. Comp. Sc.
 * Created on 31. Dezember 2020, 16:36 */

#ifndef SETSMARTATTREVENT_HPP
#define SETSMARTATTREVENT_HPP

#include <attributes/SMARTuniqueID.hpp>///class SMARTuniqueID
#include <UserInterface/columnIndices.hpp>///enum ColumnIndices::columnIndices
#include <SMARTvalueRater.hpp>///SMARTvalRatngTyp

class SMARTattrs{
private:
  SMARTuniqueID m_SMARTuniqueID;
  fastestUnsignedDataType m_SMARTattributeID;
  enum ColumnIndices::columnIndices m_columnIndices;
  SMARTvalRatngTyp m_SMARTvalRating;
  std::string m_str;
  void * m_data;
public:
  SMARTattrs(
    const SMARTuniqueID & _SMARTuniqueID,
    fastestUnsignedDataType SMARTattributeID, /**Usually the line (number) */
    //TODO exchange enum with fastestUnsignedDataType for performance?
    /**Usually the column (number) */
    const enum ColumnIndices::columnIndices & colIndices,
    const std::string & str,
    const SMARTvalRatngTyp SMARTvalRating,
    /**e.g. pointer to list ctrl */void * data)
   : m_SMARTuniqueID(_SMARTuniqueID),
     m_SMARTattributeID(SMARTattributeID),
     m_columnIndices(colIndices),
     m_str(str),
     m_SMARTvalRating(SMARTvalRating),
     m_data(data)
  {
    
  }
};

///see/from https://wiki.wxwidgets.org/Custom_Events#Subclassing_wxCommandEvent
class SetSMARTattrEvent;
wxDECLARE_EVENT(MY_NEW_TYPE, SetSMARTattrEvent);

class SetSMARTattrEvent
  : public wxCommandEvent,
    public SMARTattrs
{
public:
  SetSMARTattrEvent(
//    wxEventType commandType = MY_NEW_TYPE,
//    int id = 0,
     const SMARTattrs & _SMARTattrs
    )
    : SMARTattrs(_SMARTattrs)///Use SMARTattrs's copy construtor
//   :  wxCommandEvent(commandType, id)
  {}

  /** https://wiki.wxwidgets.org/Custom_Events#Subclassing_wxCommandEvent :
   *   "You *must* copy here the data to be transported" */
  //TODO check if really copied (via debugging)
  SetSMARTattrEvent(const SetSMARTattrEvent & event)
    : wxCommandEvent(event),
      SMARTattrs(event)
    {}
 
  /** https://wiki.wxwidgets.org/Custom_Events#Subclassing_wxCommandEvent :
   * "Required for sending with wxPostEvent()" */
  wxEvent* Clone() const { return new SetSMARTattrEvent(*this); }
};

#endif /* SETSMARTATTREVENT_HPP */
