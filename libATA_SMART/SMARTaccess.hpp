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

namespace libatasmart
{
//  std::string g_stdStrAttributeName;

  struct attr_helper {
    uint64_t *value;
    SkBool found;
    fastestUnsignedDataType IDtoLookFor;
    std::string attributeName;
  };

  static void any_attribute_callback(
    SkDisk *d,
    const SkSmartAttributeParsedData *a,
    struct libatasmart::attr_helper *ah);

  int readAttribute(const char attributeName [], SkDisk * p_skDisk, uint64_t & value);

  class SMARTaccess
    : public SMARTaccessBase
  {
  public:
    SMARTaccess ();
    virtual
    ~SMARTaccess ();

    enum SMARTaccessBase::retCodes readSmartForDevice(const char device []);
    enum SMARTaccessBase::retCodes ReadSMARTValuesForAllDrives();
  };

} /* namespace libatasmart */

#endif /* LIBATA_SMART_SMARTACCESS_HPP_ */
