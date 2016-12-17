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
#include "Controller/time/GetTickCount.hpp" //OperatingSystem::GetTimeCountInNanoSeconds(...)
//#include <attributes/SMARTattributeNameAndID.hpp>

namespace libatasmart
{
  std::string g_stdStrAttributeName;

  void getDriveSupportedSMART_IDs(
    SkDisk * p_SkDisk,
    const SMARTentry * p_SMARTentry,
    std::vector<SMARTattributeNameAndID> * p_SMARTattributeNamesAndIDs)
  {
    p_SMARTattributeNamesAndIDs->push_back( 
      SMARTattributeNameAndID(
        p_SMARTentry->GetName(), 
        p_SMARTentry->GetAttributeID() ) 
      );
    LOGN(p_SMARTentry->GetName() << " ID:" <<
      (fastestUnsignedDataType) p_SMARTentry->GetAttributeID() )
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
    if( p_libatasmart_attribHelper->IDtoLookFor == p_SkSmartAttributeParsedData->id //||
//        strcmp(p_SkSmartAttributeParsedData->name, /*g_stdStrAttributeName.c_str()*/
//          p_libatasmart_attribHelper->attributeName.c_str() ) == 0
      )
    {
      LOGN( p_SkSmartAttributeParsedData->name << " found")
  //    std::cout << "a->name matches " << g_stdStrAttributeName.c_str() << std::endl;
      if( ! p_libatasmart_attribHelper->found //||
//        p_SkSmartAttributeParsedData->pretty_value > *p_libatasmart_attribHelper->value
        )
      {
        *p_libatasmart_attribHelper->value = p_SkSmartAttributeParsedData->pretty_value;
#ifdef DEBUG
        const uint8_t * p_rawData = p_SkSmartAttributeParsedData->raw;
        std::ostringstream stdoss;
        stdoss << "raw data for " <<  p_libatasmart_attribHelper->IDtoLookFor 
          << ":";
        for(int i = 0; i < 6; i++)
          stdoss << (unsigned) p_rawData[i] << " ";
        std::string str = stdoss.str();
        LOGN(str)
#endif
        p_libatasmart_attribHelper->found = TRUE;
      }
    }
  }

  int readAttribute(/*const char attributeName []*/
    fastestUnsignedDataType SMARTattributeID, 
    SkDisk * p_skDisk, 
    uint64_t & value)
  {
    //g_stdStrAttributeName = attributeName;
    struct attr_helper libatasmart_attribHelper;

  //   assert(d);
  //   assert(kelvin);
     libatasmart_attribHelper.found = FALSE;
     libatasmart_attribHelper.value = & value;
     libatasmart_attribHelper.IDtoLookFor = SMARTattributeID;
     //libatasmart_attribHelper.attributeName = attributeName;
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

  void SMARTaccess::copySMARTvalues(
    const SkDisk * p_skDisk, 
    const SkIdentifyParsedData * p_SkIdentifyParsedData)
  {
    uint64_t rawSMARTattrValue;
    const char * attributeName;
    constSMARTattributesContainerType::const_iterator 
      constSMARTattributesToObserveConstIter = SMARTattributesToObserve.begin();
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

    int i;
    long double timeCountInSeconds;
    std::pair<std::set<SMARTuniqueIDandValues>::iterator, bool> insert =
        m_SMARTuniqueIDandValues.insert(sMARTuniqueIDandValues);
//              if( insert.second == true )/** If actually inserted into std::set*/
    {
      for( ; constSMARTattributesToObserveConstIter !=
        SMARTattributesToObserve.end(); ++ constSMARTattributesToObserveConstIter )
      {
        SMARTattributeID = constSMARTattributesToObserveConstIter->GetAttributeID();
        attributeName = constSMARTattributesToObserveConstIter->GetName();
        i = readAttribute(/*attributeName*/SMARTattributeID, 
              (SkDisk *) p_skDisk, rawSMARTattrValue);
        if( i == 0) /** Successfully got SMART attribute value */
        {
//              std::cout << attributName << ":" << rawSMARTattrValue << std::endl;
//                  insert.first->
//                    std::set<SMARTuniqueIDandValues>::iterator it = insert.first;
          p_sMARTuniqueIDandValues = & (SMARTuniqueIDandValues &) (* insert.first);
//                    const SMARTuniqueID & currentSMARTuniqueIDandValuesSMARTuniqueID = it->getSMARTuniqueID();
          LOGN("SMART unique ID:" << p_sMARTuniqueIDandValues->getSMARTuniqueID().str())

#ifdef DEBUG
          //long val = p_sMARTuniqueIDandValues->m_SMARTrawValues[SMARTattributeID];
          //TODO 64 bit value is converted to long int value
          
#endif            /** E.g. 32 bit Linux: size of long int is 4 bytes */
          //liRawSMARTattrValue = s_smartID2LongInt
          //TODO: SIGSEGV here: p_sMARTuniqueIDandValues is NULL (0)
          p_sMARTuniqueIDandValues->m_SMARTvalues[SMARTattributeID].SetRawValue(
            rawSMARTattrValue);
#ifdef DEBUG
          
#endif
          AtomicExchange( (long int *) & p_sMARTuniqueIDandValues->
            m_SMARTvalues[SMARTattributeID].m_successfullyReadSMARTrawValue, 1);
          OperatingSystem::GetTimeCountInSeconds(timeCountInSeconds);
          AtomicExchange( (long int *) & p_sMARTuniqueIDandValues->
            m_SMARTvalues[SMARTattributeID].m_timeStampOfRetrieval, 
            (long int) (timeCountInSeconds * 1000.0) );
#ifdef __linux__
//                  timeval timevalCurrentTime;
//                  ::gettimeofday( & timevalCurrentTime, 0);
#endif
        uint64_t SMARTrawValueFromMem;
        bool bConsistent = p_sMARTuniqueIDandValues->m_SMARTvalues[
          SMARTattributeID].IsConsistent(SMARTrawValueFromMem);
        LOGN( (insert.second == true ? "inserted" : "changed")
            << " raw value for SMART attribute\"" << attributeName
            << "\" (id=" << SMARTattributeID << "):"
            << rawSMARTattrValue << " "
            << SMARTrawValueFromMem)
        }
        else
        {
          AtomicExchange( (long int *) & p_sMARTuniqueIDandValues->
            m_SMARTvalues[SMARTattributeID].m_successfullyReadSMARTrawValue, 0);
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
  }
  
  enum SMARTaccessBase::retCodes SMARTaccess::readSmartForDevice(const char device [])
  {
    LOGN("begin--device:" << device)
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
      //    const SkSmartAttributeInfo * p = lookup_attribute(& skDisk, id);
              copySMARTvalues(p_skDisk, p_SkIdentifyParsedData);
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

  int SMARTaccess::GetSupportedSMART_IDs(/*SkDisk * p_skDisk*/ 
    const char * const device,
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

  enum SMARTaccessBase::retCodes SMARTaccess::ReadSMARTValuesForAllDrives()
  {
    //TODO scan all files in /dev/ or some better idea? libblkid?
    enum SMARTaccessBase::retCodes retCode = readSmartForDevice("/dev/sda");
    return retCode;
  }
} /* namespace libatasmart */