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
#include <hardware/CPU/atomic/AtomicExchange.h>
#include <iostream> //std::cout
#include <preprocessor_macros/logging_preprocessor_macros.h>
//#include <attributes/SMARTattributeNameAndID.hpp>

namespace libatasmart
{
  std::string g_stdStrAttributeName;

  void getDriveSupportedSMART_IDs(
    SkDisk * p_SkDisk,
    const SkSmartAttributeParsedData * p_SkSmartAttributeParsedData,
    std::vector<SMARTattributeNameAndID> * p_SMARTattributeNamesAndIDs)
  {
    p_SMARTattributeNamesAndIDs->push_back( SMARTattributeNameAndID(
      p_SkSmartAttributeParsedData->name, p_SkSmartAttributeParsedData->id) );
    LOGN(p_SkSmartAttributeParsedData->name << " ID:" <<
      (fastestUnsignedDataType) p_SkSmartAttributeParsedData->id)
  }

  int getSupportedSMART_IDs(/*SkDisk * p_skDisk*/ const char * const device,
     std::vector<SMARTattributeNameAndID> & SMARTattributeNamesAndIDs)
  {
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
        int retVal = sk_disk_smart_parse_attributes(
           p_skDisk,
           (SkSmartAttributeParseCallback) getDriveSupportedSMART_IDs,
           & SMARTattributeNamesAndIDs);
             //TODO "sk_disk_smart_parse_attributes" traverses all attributes and calls
             // the callback function
        if( retVal < 0 )
        {
          LOGN("sk_disk_smart_parse_attributes retVal:" << retVal)
          return -1;
        }
      }
    }
  }

  //TODO sk_disk_smart_parse_attributes calls this callback for all 255 SMART
  //IDs. So the comparison may be
  void getSMARTrawValueCallback(
    SkDisk * p_SkDisk,
    const SkSmartAttributeParsedData * p_SkSmartAttributeParsedData,
    struct libatasmart::attr_helper * p_libatasmart_attribHelper)
  {
  //  std::cout << "a->name: " << a->name << " " << g_stdStrAttributeName.c_str() << std::endl;
  //  if (a->pretty_unit != SK_SMART_ATTRIBUTE_UNIT_NONE)
  //  {
  //    std::cout << "a->pretty_unit != SK_SMART_ATTRIBUTE_UNIT_NONE" << std::endl;
  //    return;
  //  }
//    LOGN( "current attrib:" << p_SkSmartAttributeParsedData->name )

    /** If strings are identical. */
    if( p_libatasmart_attribHelper->IDtoLookFor == p_SkSmartAttributeParsedData->id ||
        strcmp(p_SkSmartAttributeParsedData->name, /*g_stdStrAttributeName.c_str()*/
          p_libatasmart_attribHelper->attributeName.c_str() ) == 0
      )
    {
      LOGN( p_SkSmartAttributeParsedData->name << " found")
  //    std::cout << "a->name matches " << g_stdStrAttributeName.c_str() << std::endl;
      if( ! p_libatasmart_attribHelper->found //||
//        p_SkSmartAttributeParsedData->pretty_value > *p_libatasmart_attribHelper->value
        )
      {
        *p_libatasmart_attribHelper->value = p_SkSmartAttributeParsedData->pretty_value;
      p_libatasmart_attribHelper->found = TRUE;
      }
    }
  }

  int readAttribute(const char attributeName [], SkDisk * p_skDisk, uint64_t & value)
  {
    g_stdStrAttributeName = attributeName;
    struct attr_helper libatasmart_attribHelper;

  //   assert(d);
  //   assert(kelvin);
     libatasmart_attribHelper.found = FALSE;
     libatasmart_attribHelper.value = & value;
     libatasmart_attribHelper.attributeName = attributeName;
     int retVal = sk_disk_smart_parse_attributes(
       p_skDisk,
       (SkSmartAttributeParseCallback) getSMARTrawValueCallback,
       & libatasmart_attribHelper);
     //TODO "sk_disk_smart_parse_attributes" traverses all attributes and calls
     // the callback function
    if( retVal < 0 )
    {
      LOGN("sk_disk_smart_parse_attributes retVal:" << retVal)
      return -1;
    }
    if( ! libatasmart_attribHelper.found) {
      errno = ENOENT;
      return -2;
    }
    value = * libatasmart_attribHelper.value;
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
    LOGN("begin")
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
              const char * attributeName;
              constSMARTattributesType::const_iterator SMARTattributesToObserveIter =
                SMARTattributesToObserve.begin();

  //            std::pair<std::set<SkIdentifyParsedData>::iterator, bool> insert =
  //              m_SMARTuniqueIDs.insert(*p_SkIdentifyParsedData);

              //TODO sMARTuniqueIDandValues.SMARTuniqueID is not a 100% copy of
              // "* p_SkIdentifyParsedData" (seems that parts of the "firmware"
              //  string is copied into "serial"
              SMARTuniqueID sMARTuniqueID(*p_SkIdentifyParsedData);
              LOGN("SMART unique ID:" << sMARTuniqueID.str() )
              SMARTuniqueIDandValues sMARTuniqueIDandValues(sMARTuniqueID);
              SMARTuniqueIDandValues * p_sMARTuniqueIDandValues;
              fastestUnsignedDataType SMARTattributeID /*= citer->id*/;

              std::pair<std::set<SMARTuniqueIDandValues>::iterator, bool> insert =
                  m_SMARTuniqueIDandValues.insert(sMARTuniqueIDandValues);
//              if( insert.second == true )/** If actually inserted into std::set*/
              {
                for( ; SMARTattributesToObserveIter !=
                  SMARTattributesToObserve.end(); ++ SMARTattributesToObserveIter )
                {
                  SMARTattributeID = SMARTattributesToObserveIter->id;
                  attributeName = SMARTattributesToObserveIter->name;
                  i = readAttribute(attributeName, p_skDisk, rawSMARTattrValue);
                  if( i == 0) /** Successfully got SMART attribute value */
                  {
      //              std::cout << attributName << ":" << rawSMARTattrValue << std::endl;
  //                  insert.first->
//                    std::set<SMARTuniqueIDandValues>::iterator it = insert.first;
                    p_sMARTuniqueIDandValues = & (SMARTuniqueIDandValues &) (* insert.first);
//                    const SMARTuniqueID & currentSMARTuniqueIDandValuesSMARTuniqueID = it->getSMARTuniqueID();
                  LOGN("SMART unique ID:" << p_sMARTuniqueIDandValues->getSMARTuniqueID().str())

                  //TODO: SIGSEGV here: p_sMARTuniqueIDandValues is NULL (0)
//                  p_sMARTuniqueIDandValues->m_SMARTrawValues[citer->id] = rawSMARTattrValue;
                  AtomicExchange( (long int *) & p_sMARTuniqueIDandValues->m_SMARTrawValues[SMARTattributeID] //long * Target
                    , /*pSmartInfo->m_dwAttribValue*/
                    //* (long int *) //SMARTattributesToObserveIter->pretty_value /*raw*/ /*long val*/
                    rawSMARTattrValue);
                  AtomicExchange( (long int *) & p_sMARTuniqueIDandValues->
                    m_successfullyReadSMARTrawValue[SMARTattributeID], 1 );

#ifdef __linux__
//                  timeval timevalCurrentTime;
//                  ::gettimeofday( & timevalCurrentTime, 0);
#endif
                  LOGN( (insert.second == true ? "inserted" : "changed")
                      << " raw value for SMART attribute\"" << attributeName
                      << "\" (id=" << SMARTattributeID << "):"
                      << rawSMARTattrValue << " "
                      << p_sMARTuniqueIDandValues->m_SMARTrawValues[SMARTattributeID])
                  }
                  else
                  {
                    AtomicExchange( (long int *) & p_sMARTuniqueIDandValues->
                      m_successfullyReadSMARTrawValue[SMARTattributeID], 0 );
                    LOGN( "reading SMART value for SMART attribute\"" << attributeName
                      << "\" (id=" << SMARTattributeID << ") failed:"
                      << (fastestSignedDataType) i )
                  }
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
    LOGN("end")
    return retVal;
  }

  enum SMARTaccessBase::retCodes SMARTaccess::ReadSMARTValuesForAllDrives()
  {
    //TODO scan all files in /dev/ or some better idea? libblkid?
    enum SMARTaccessBase::retCodes retCode = readSmartForDevice("/dev/sda");
    return retCode;
  }
} /* namespace libatasmart */
