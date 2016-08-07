/*
 * SMARTaccess.cpp
 *
 *  Created on: 31.07.2016
 *      Author: sg
 */

#include <libATA_SMART/SMARTaccess.hpp>
#include <atasmart.h> //sk_disk_smart_is_available
#include <errno.h> //errno, EACCES
#include <string> //class std::string
#include <string.h> //strcmp(...)

namespace libatasmart
{
  std::string g_stdStrAttributeName;

  //TODO sk_disk_smart_parse_attributes calls this callback for all 255 SMART
  //IDs. So the comparison may be
  static void any_attribute_callback(
    SkDisk *d,
    const SkSmartAttributeParsedData *a,
    struct libatasmart::attr_helper *ah)
  {
  //  std::cout << "a->name: " << a->name << " " << g_stdStrAttributeName.c_str() << std::endl;
  //  if (a->pretty_unit != SK_SMART_ATTRIBUTE_UNIT_NONE)
  //  {
  //    std::cout << "a->pretty_unit != SK_SMART_ATTRIBUTE_UNIT_NONE" << std::endl;
  //    return;
  //  }

    /** If strings are identical. */
    if ( strcmp(a->name, g_stdStrAttributeName.c_str() ) == 0 ) {
  //    std::cout << "a->name matches " << g_stdStrAttributeName.c_str() << std::endl;
      if (!ah->found || a->pretty_value > *ah->value)
        *ah->value = a->pretty_value;

      ah->found = TRUE;
    }
  }

  int readAttribute(const char attributeName [], SkDisk * p_skDisk, uint64_t & value)
  {
    g_stdStrAttributeName = attributeName;
    struct attr_helper ah;

  //   assert(d);
  //   assert(kelvin);
     ah.found = FALSE;
     ah.value = & value;
     //TODO "sk_disk_smart_parse_attributes" traverses all attributes and calls
     // the callback function
    if( sk_disk_smart_parse_attributes(
          p_skDisk,
          (SkSmartAttributeParseCallback) any_attribute_callback,
          &ah)
        < 0
       )
     return -1;
    if (!ah.found) {
      errno = ENOENT;
      return -2;
    }
    value = * ah.value;
    return 0;
  }

  SMARTaccess::SMARTaccess ()
  {
    // TODO Auto-generated constructor stub

  }

  SMARTaccess::~SMARTaccess ()
  {
    // TODO Auto-generated destructor stub
  }

  enum SMARTaccessBase::retCodes SMARTaccess::readSmartForDevice(const char device [])
  {
    enum SMARTaccessBase::retCodes retVal = SMARTaccessBase::success;
//    sk_disk_smart_is_available(& skDisk, & SMARTavailable);
//    if( SMARTavailable )
    {
//      SkDisk skDisk;
      SkDisk * p_skDisk;
  //  sk_disk_smart_is_enabled(& skDisk);

      /** "sk_disk_open" allocates an p_skDisk and assigns pointer to it.
       *  (must be freed by caller later)*/
      int i = sk_disk_open(device, /* SkDisk **_d*/ & p_skDisk);
      if( i != -1)
      {
    //    i = sk_init_smart( & skDisk);
//        sk_disk_check_power_mode(p_skDisk);
        i = sk_disk_smart_read_data(p_skDisk);
        if( i == 0)
        {
          SkBool diskIdentityAvailable;
          SkBool SMARTavailable;
          i = sk_disk_smart_is_available(p_skDisk, & SMARTavailable);
          i = sk_disk_identify_is_available(p_skDisk, & diskIdentityAvailable);
          if(i == 0 && diskIdentityAvailable)
          {
            const SkIdentifyParsedData * p_SkIdentifyParsedData;
            i = sk_disk_identify_parse(p_skDisk, & p_SkIdentifyParsedData);
            /** i == 0 -> successfuly got disk info */
            if( i == 0)
            {
  //          uint64_t milliSeconds;
  //          i = sk_disk_smart_get_power_on(p_skDisk, & milliSeconds);
  //          if( i == 0)
  //              outputPowerOnHours(milliSeconds);
  //
  //          uint64_t sectors;
  //          i = sk_disk_smart_get_bad(p_skDisk, & sectors);
  //          if( i == 0)
  //            std::cout << "successfully got # bad sectors:" << sectors << std::endl;
  //      //      i = sk_disk_smart_status(SkDisk *d, SkBool *good);
  //          uint64_t mkelvin;
  //          i = sk_disk_smart_get_temperature(p_skDisk,  & mkelvin);
  //          if( i == 0)
  //            std::cout << "successfully got temperature of drive:"
  //              << mkelvin << "mK=" << mkelvin/1000 << "K="
  //              << (float)mkelvin/1000.0f - 273.1f << "°C" << std::endl;
  //          else
  //            std::cout << "failed to get temperature of drive:" << i << " errno:" << errno << std::endl;
      //    const SkSmartAttributeInfo * p = lookup_attribute(& skDisk, id);

              uint64_t rawSMARTattrValue;
              const char * attributName;
              constSMARTattributesType::const_iterator citer =
                SMARTattributesToObserve.begin();

  //            std::pair<std::set<SkIdentifyParsedData>::iterator, bool> insert =
  //              m_SMARTuniqueIDs.insert(*p_SkIdentifyParsedData);

              //TODO sMARTuniqueIDandValues.SMARTuniqueID is not a 100% copy of
              // "* p_SkIdentifyParsedData" (seems that parts of the "firmware"
              //  string is copied into "serial"
              SMARTuniqueIDandValues sMARTuniqueIDandValues(
                *(SMARTuniqueIDandValues*) p_SkIdentifyParsedData);
              SMARTuniqueIDandValues * p_sMARTuniqueIDandValues;
              std::pair<std::set<SMARTuniqueIDandValues>::iterator, bool> insert =
                  m_SMARTuniqueIDandValues.insert(sMARTuniqueIDandValues);
              if( insert.second == true )/** If actually inserted into std::set*/
              {
                for( ; citer != SMARTattributesToObserve.end(); ++ citer )
                {
                  attributName = citer->name;
                  i = readAttribute(attributName, p_skDisk, rawSMARTattrValue);
                  if( i == 0)
      //              std::cout << attributName << ":" << rawSMARTattrValue << std::endl;
  //                  insert.first->
                    p_sMARTuniqueIDandValues = & (SMARTuniqueIDandValues &) insert.first;
                  p_sMARTuniqueIDandValues->m_SMARTrawValues[citer->id] = rawSMARTattrValue;
      //            else
      //              std::cerr << "Failed to get attribute value for \"" << attributName << "\":"
      //                << strerror(errno) << std::endl;
                    //return retVa;
                }
              }
//              sk_disk_identity_free( p_SkIdentifyParsedData);
            }
          }
        }
//        else
//          std::cout << "failed to read SMART data:" << i << " errno:" << errno
//            << " " << strerror(errno) << std::endl;
        sk_disk_free( p_skDisk);
      }
//      else
//        std::cout << "failed to open disk for SMART:" << i << " error:" <<
//        /*errno*/ strerror(errno) << std::endl;
      if( errno == EACCES ) /** If access denied */
        retVal = SMARTaccessBase::accessDenied;
    }
    return retVal;
  }

  enum SMARTaccessBase::retCodes SMARTaccess::ReadSMARTValuesForAllDrives()
  {
    //TODO scan all files in /dev/ or some better idea? libblkid?
    enum SMARTaccessBase::retCodes retCode = readSmartForDevice("/dev/sda");
    return retCode;
  }
} /* namespace libatasmart */
