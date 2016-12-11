/*
 * SMARTaccess.hpp
 *
 *  Created on: 31.07.2016
 *      Author: sg
 */

#ifndef LIBATA_SMART_SMARTACCESS_HPP_
#define LIBATA_SMART_SMARTACCESS_HPP_

#include <SMARTaccessBase.hpp> //base class SMARTreaderBase
#include <string> //class std::string
#include <vector> //class std::vector

namespace libatasmart
{
//  std::string g_stdStrAttributeName;

  struct attr_helper {
    uint64_t *value;
    SkBool found;
    fastestUnsignedDataType IDtoLookFor;
    std::string attributeName;
  };

  int getSupportedSMART_IDs(const char * const,
                          std::vector<SMARTattributeNameAndID> & );

  static void getSMARTrawValueCallback(
    SkDisk * d,
    const SkSmartAttributeParsedData *a,
    struct libatasmart::attr_helper *ah);

  int readAttribute(/*const char attributeName []*/
    fastestUnsignedDataType SMARTattributeID,
    SkDisk * p_skDisk, 
    uint64_t & value);

  class SMARTaccess
    : public SMARTaccessBase
  {
  public:
    SMARTaccess ();
    virtual
    ~SMARTaccess ();

    enum SMARTaccessBase::retCodes readSmartForDevice(const char device []);
    int GetSupportedSMART_IDs(
      const char * const device,
      std::vector<SMARTattributeNameAndID> & SMARTattributeNamesAndIDs);
    enum SMARTaccessBase::retCodes ReadSMARTValuesForAllDrives();
    void copySMARTvalues(const  SkDisk *, const SkIdentifyParsedData *);
  };

} /* namespace libatasmart */

#endif /* LIBATA_SMART_SMARTACCESS_HPP_ */
