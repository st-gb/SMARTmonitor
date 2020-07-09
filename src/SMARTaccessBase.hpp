/** SMARTreaderBase.hpp
 *  Created on: 31.07.2016
 *  Author:Stefan Gebauer,M.Sc. Comp.Sc./Informatik (TU Berlin)*/
#ifndef SMARTACCESSBASE_HPP_
#define SMARTACCESSBASE_HPP_

#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType

#include <attributes/SMARTuniqueIDandValues.hpp> //class SMARTuniqueIDandValues
#include <attributes/SMARTattributeNameAndID.hpp> //class SMARTattributeNameAndID
#include <attributes/SMARTattrDefAccss.hpp>///base class SMARTattrDefAccss

//http://www.samsung.com/global/business/semiconductor/products/SSD/downloads/ds_SS805_NSSD_100_SLC_SATA_II_rev11.pdf

//TODO Is the base class of direct SMART access? Then rename to 
// "DrctSMARTaccBase?
class SMARTaccessBase///Base class for (direct) access to S.M.A.R.T.
  : public SMARTattrDefAccss
{
private:
//  std::set<SkIdentifyParsedData> m_SMARTuniqueIDs;
public:
  static fastestUnsignedDataType s_sizeOfLongIntInBytes;
  typedef std::set<SMARTuniqueIDandValues> SMARTuniqueIDandValsContType;
protected:
  SMARTuniqueIDandValsContType & m_SMARTuniqueIDsandVals;
public:
  enum retCodes { success = 0, accessDenied, noSingleSMARTdevice, 
    gotSMARTattributeValue, unset};
  SMARTaccessBase(SMARTuniqueIDandValsContType &);
  virtual
  ~SMARTaccessBase ();

  ///\brief fill @sMARTuniqueID with firmware, model and serial number
  virtual void fill(const char device[], SMARTuniqueID & sMARTuniqueID) = 0;
  /**It makes sense to do the unit detection/calculation in this class because
  * it belongs to the server and the logic only needs to be done once at 
  * server side. Could also do it to in the client/dyn lib but then it has to be
  * delivered some more info to the client/dyn lib (e.g. # B written since OS 
  * start) */
  /** For drive quirks and real units see also SMARTctrl database:
  * https://www.thomas-krenn.com/de/wiki/Smartmontools_Drive_Database_aktualisieren
  * value meanings specific to drive: /var/lib/smartmontools/drivedb/drivedb.h
  *  https://www.smartmontools.org/browser/trunk/smartmontools/drivedb.h */
  void possiblyAutoDetectUnit(
    const fastestUnsignedDataType SMARTattrID,
    const uint64_t & SMARTrawVal,
    SMARTuniqueID & sMARTuniqueID,
    const std::string & dataCarrierDevPath);
  /** Implement functionality in subclasses because the SMART access part is
   *  specific to Linux, Windows or used SMART library and so on.*/
  virtual enum retCodes ReadSMARTValuesForAllDrives(
    ///May be a subset of supported SMART IDs.
    const fastestUnsignedDataType SMARTattrIDsToRead[],
    dataCarrierID2devicePath_type &) = 0;

  fastestUnsignedDataType GetNumberOfDifferentDrives() const {
    return m_SMARTuniqueIDsandVals.size();
  }
  SMARTuniqueIDandValsContType & GetSMARTuniqueIDandValues() {
    return m_SMARTuniqueIDsandVals;
  }
  
  virtual int GetSupportedSMART_IDs(
    const char * const device,
    suppSMART_IDsType & SMARTattributeNamesAndIDs) {};
};

#endif /* SMARTACCESSBASE_HPP_ */
