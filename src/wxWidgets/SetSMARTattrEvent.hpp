/* File: SetSMARTattrEvent.hpp
 * Author: Stefan Gebauer, M. Sc. Comp. Sc.
 * Created on 31. Dezember 2020, 16:36 */

#ifndef SETSMARTATTREVENT_HPP
#define SETSMARTATTREVENT_HPP

///Stefan Gebauer's(TU Berlin matric. number 361095) SMARTmonitor header files:
 #include <attributes/SMARTuniqueID.hpp>///class SMARTuniqueID
 ///enum TU_Bln361095::SMARTmon::ATA_SMARTattrColIdx::ATA_SMARTattrTblColIdcs
 #include <UserInterface/columnIndices.hpp>
 #include <SMARTvalueRater.hpp>///SMARTvalRatngTyp

class SMARTattrs{
private:
  SMARTuniqueID m_SMARTuniqueID;
  fastestUnsignedDataType m_SMARTattributeID;
  TU_Bln361095::CPU::faststUint m_SMARTattrTblColIdx;
  SMARTvalRatngTyp m_SMARTvalRating;
  std::string m_str;
  void * m_data;
public:
  SMARTattrs(
    const SMARTuniqueID & _SMARTuniqueID,
    fastestUnsignedDataType SMARTattributeID, /**Usually the line (number) */
    //TODO exchange enum with fastestUnsignedDataType for performance?
    /**Usually the column (number) */
    TU_Bln361095::CPU::faststUint SMARTattrTblColIdx,
    const std::string & str,
    const SMARTvalRatngTyp SMARTvalRating,
    /**e.g. pointer to list ctrl */void * data)
   : m_SMARTuniqueID(_SMARTuniqueID),
     m_SMARTattributeID(SMARTattributeID),
     m_SMARTattrTblColIdx(SMARTattrTblColIdx),
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
