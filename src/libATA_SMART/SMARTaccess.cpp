/** SMARTaccess.cpp
 *  Created on: 31.07.2016
 *  Author: Stefan Gebauer, M.Sc. Comp. Sc.(TU Berlin)*/

#include <libATA_SMART/SMARTaccess.hpp>

///Standard C/C++ libs
#include <atasmart.h> //sk_disk_smart_is_available
#include <errno.h> //errno, EACCES
#include <string> //class std::string
#include <string.h> //strcmp(...)
#include <iostream> //std::cout
#include <sstream>///class std::ostringstream

///Stefan Gebauer's common_sourcecode repo
///OperatingSystem::GetTimeCountInNanoSeconds(...)
#include "Controller/time/GetTickCount.hpp"
#include <hardware/CPU/atomic/AtomicExchange.h>///AtomicExchange(...)
#include <hardware/dataCarrier/ATA3Std.h>///numSMARTrawValB
#include <OperatingSystem/POSIX/GetBlockDeviceFiles.h>//GetBlockDeviceFiles(...)
#include <preprocessor_macros/logging_preprocessor_macros.h>///LOGN(...)
//#include <attributes/SMARTattributeNameAndID.hpp>

#define numCharsForDvcAndTerm0 6/** 5 characters for "/dev/" + '\0' */
///Only 3 character device names and not "sda1" (partitions).
#define numCharsForDataCarrierFiles 3

namespace libatasmart
{
  std::string g_stdStrAttributeName;

  /** Attention : function must have the same signature as 
   * SkSmartAttributeParseCallback */
  void getDriveSupportedSMART_IDs(
    SkDisk * p_SkDisk,
    const SkSmartAttributeParsedData * p_SkSmartAttributeParsedData,
    suppSMART_IDsType * p_SMARTattrNamesAndIDs)
  {
    p_SMARTattrNamesAndIDs->insert(
      SMARTattributeNameAndID(
        p_SkSmartAttributeParsedData->name, 
        p_SkSmartAttributeParsedData->id )
      );
    LOGN(p_SkSmartAttributeParsedData->name << " ID:" <<
      (fastestUnsignedDataType) p_SkSmartAttributeParsedData->id )
  }

  //TODO sk_disk_smart_parse_attributes calls this callback for all 255 SMART
  //IDs. So the comparison may be
  /** Attention : function must have the same signature as 
   * SkSmartAttributeParseCallback */
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
      *p_libatasmart_attribHelper->value =
      /** For SSD model HFS256G39TND-N210A firmware 30001P10 serial
       * EJ7CN55981080CH09:for S.M.A.R.T. ID 241 (total LBAs/data written)
       * SkSmartAttributeParsedData.pretty_value = 181395
       * SkSmartAttributeParsedData.pretty_unit = SK_SMART_ATTRIBUTE_UNIT_MB =>
       *  181395 * 1024*1024 B=190206443520 B=190GB-> wrong value
       * SkSmartAttributeParsedData.raw value is: 21 30 => 21*256+30=5406
       * -if unit=LBA & logical sector size: 5406 * 512 B =  27678is72 B
       * -if unit=LBA & physical sector size: 5406 4096 B = 22142976 B 
       * In reality SkSmartAttributeParsedData.raw for this SSD: unit ~= GiB */
        //p_SkSmartAttributeParsedData->pretty_value;
        ///Lowmost byte of array "raw" at array index 0
        *(uint64_t*) p_SkSmartAttributeParsedData->raw;
#ifdef DEBUG
        const uint8_t * p_rawData = p_SkSmartAttributeParsedData->raw;
        std::ostringstream stdoss;
        stdoss << "raw data for " <<  p_libatasmart_attribHelper->IDtoLookFor 
          << ":";
        for(fastestUnsignedDataType idx = 0; idx < numSMARTrawValB; idx++){
          stdoss << (unsigned) p_rawData[idx] << " ";
          ///Lowmost byte at array index 0
//          *( (uint8_t *) p_libatasmart_attribHelper->value + i) = p_rawData[i];
        }
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
     //TODO sk_disk_smart_parse_attributes reads all supported attributes?->
     // is done for all SMART attributes to read.->takes much longer
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

  //\param sMARTuniqueIDsandVals : shoud be usable by a minimal program etc..->
  // without many dependencies like large classes
  SMARTaccess::SMARTaccess(SMARTuniqueIDandValsContType & sMARTuniqueIDsandVals)
    : SMARTaccessBase(sMARTuniqueIDsandVals)
  {
    // TODO Auto-generated constructor stub

  }

  SMARTaccess::~SMARTaccess ()
  {
    // TODO Auto-generated destructor stub
  }


/** \brief Uses lock-free atomic exchange operations (faster than critical 
 * sections) because the read of S.M.A.R.T. values in the server may be in 
 * another thread than the write to the clients. */
//TODO test if AtomExchange is necessary/effective at all. Maybe better
// exchange a pointer to the raw value instead of exchanging itself
void SMARTaccess::copySMARTvalues(
    const SkDisk * p_skDisk, 
    //const SkIdentifyParsedData * p_SkIdentifyParsedData
    SMARTuniqueID & sMARTuniqueID,
    const char device [])
  {
    uint64_t rawSMARTattrValue;
    const char * attributeName;
//    SMARTmonitorBase::SMARTattrToObsType::const_iterator 
//      constSMARTattrsToObsConstIter = sMARTmonBase.m_IDsOfSMARTattrsToObserve.
//      begin();
//            std::pair<std::set<SkIdentifyParsedData>::iterator, bool> insert =
//              m_SMARTuniqueIDs.insert(*p_SkIdentifyParsedData);

    //TODO sMARTuniqueIDandValues.SMARTuniqueID is not a 100% copy of
    // "* p_SkIdentifyParsedData" (seems that parts of the "firmware"
    //  string is copied into "serial"

    
    LOGN("SMART unique ID:" << sMARTuniqueID.str() )
    SMARTuniqueIDandValues sMARTuniqueIDandValues(sMARTuniqueID);
    SMARTuniqueIDandValues * p_sMARTuniqueIDandValues;
    fastestUnsignedDataType SMARTattrID /*= citer->id*/=0;

    int i;
    long double timeCountInSeconds;
    std::pair<std::set<SMARTuniqueIDandValues>::iterator, bool> insert =
        m_SMARTuniqueIDsandVals.insert(sMARTuniqueIDandValues);
//              if( insert.second == true )/** If actually inserted into std::set*/
//    {
  for( //; constSMARTattrsToObsConstIter !=
      //sMARTattrDefs.end(); ++ constSMARTattrsToObsConstIter
//    ;SMARTattributeID < NUM_DIFFERENT_SMART_ENTRIES; SMARTattributeID ++
    fastestUnsignedDataType sMART_IDsToReadIdx = 0; sMARTuniqueID.
    m_SMART_IDsToRd[sMART_IDsToReadIdx] != 0 &&
    ///Prevent stack overflow
    sMART_IDsToReadIdx <= numDifferentSMART_IDs; sMART_IDsToReadIdx++)
  {
    SMARTattrID = sMARTuniqueID.m_SMART_IDsToRd[sMART_IDsToReadIdx];
      //TODO only retrieve the data of the SMART IDs to observe list (and where
      // a definition is available?) Only get SMART info if it is supported??
//      SMARTattrDef * p_SMARTattrDef = SMARTaccessBase::getSMARTattrDef(
//        SMARTattrID);
//    if(p_SMARTattrDef){
//        SMARTattributeID = constSMARTattributesToObserveConstIter->GetAttributeID();
//        attributeName = p_SMARTattrDef->GetName();
        i = readAttribute(/*attributeName*/SMARTattrID, 
              (SkDisk *) p_skDisk, rawSMARTattrValue);
        LOGN_INFO( "SMART raw value for SMART ID " << SMARTattrID << ":"
          << rawSMARTattrValue)
      /** Assign the pointer before the branch instruction because it is also
       * used in the "else" part.*/
      p_sMARTuniqueIDandValues = & (SMARTuniqueIDandValues &) (* insert.first);
      if( i == 0) /** Successfully got SMART attribute value */
      {
//        std::cout << attributName << ":" << rawSMARTattrValue << std::endl;
//                  insert.first->
//        std::set<SMARTuniqueIDandValues>::iterator it = insert.first;
//       const SMARTuniqueID & currentSMARTuniqueIDandValuesSMARTuniqueID = it->getSMARTuniqueID();
//          LOGN_DEBUG("SMART unique ID:" << p_sMARTuniqueIDandValues->getSMARTuniqueID().str())
        /**Unit auto-detection has to be done here because the data carrier
        * to retrieve additional data for is here (and not at client side).*/
        possiblyAutoDetectUnit(SMARTattrID, rawSMARTattrValue, 
          sMARTuniqueID, device);
#ifdef DEBUG
          //long val = p_sMARTuniqueIDandValues->m_SMARTrawValues[SMARTattributeID];
          //TODO 64 bit value is converted to long int value
          
#endif            /** E.g. 32 bit Linux: size of long int is 4 bytes */
          //liRawSMARTattrValue = s_smartID2LongInt
          //TODO: SIGSEGV here: p_sMARTuniqueIDandValues is NULL (0)
          p_sMARTuniqueIDandValues->m_SMARTvalues[SMARTattrID].SetRawValue(
            rawSMARTattrValue);
#ifdef DEBUG
          
#endif
          AtomicExchange( (long int *) & p_sMARTuniqueIDandValues->
            m_SMARTvalues[SMARTattrID].m_successfullyReadSMARTrawValue, 1);
          OperatingSystem::GetTimeCountInSeconds(timeCountInSeconds);
          AtomicExchange( (long int *) & p_sMARTuniqueIDandValues->
            m_SMARTvalues[SMARTattrID].m_timeStampOfRetrieval, 
            (long int) (timeCountInSeconds * 1000.0) );
#ifdef __linux__
//                  timeval timevalCurrentTime;
//                  ::gettimeofday( & timevalCurrentTime, 0);
#endif
        uint64_t SMARTrawValueFromMem;
        bool bConsistent = p_sMARTuniqueIDandValues->m_SMARTvalues[
          SMARTattrID].IsConsistent(SMARTrawValueFromMem);
        LOGN_DEBUG( (insert.second == true ? "inserted" : "changed")
            << " raw value for SMART attribute\"" << attributeName
            << "\" (id=" << SMARTattrID << "):"
            << rawSMARTattrValue << " "
            << SMARTrawValueFromMem)
        }
        else
        {
          AtomicExchange( (long int *) & p_sMARTuniqueIDandValues->
            m_SMARTvalues[SMARTattrID].m_successfullyReadSMARTrawValue, 0);
        LOGN( "reading SMART value for SMART attribute "
            //\"" << attributeName << "\""
          " (id=" << SMARTattrID << ") failed:"
            << (fastestSignedDataType) i )
        }
//            else
//              std::cerr << "Failed to get attribute value for \"" << attributName << "\":"
//                << strerror(errno) << std::endl;
          //return retVa;
      }
//    }
}

enum SMARTaccessBase::retCodes SMARTaccess::readSMARTforDevice(
  const char device [],
  SMARTuniqueID & sMARTuniqueID)
{
    LOGN("begin--device:" << device)
    enum SMARTaccessBase::retCodes retVal = SMARTaccessBase::unset;
    errno = 0;
//    sk_disk_smart_is_available(& skDisk, & SMARTavailable);
//    if( SMARTavailable )
    {
//      SkDisk skDisk;
      SkDisk * p_skDisk;
  //  sk_disk_smart_is_enabled(& skDisk);

      /** "sk_disk_open" allocates a "SkDisk" struct and assigns pointer to it.
       *  (must be freed by caller later)*/
      int i = sk_disk_open(device, /* SkDisk **_d*/ & p_skDisk);
      if( i != -1)
      {
        LOGN_DEBUG("Disk \"" << device << "\" successfully opened.")
    //    i = sk_init_smart( & skDisk);
//        sk_disk_check_power_mode(p_skDisk);
        i = sk_disk_smart_read_data(p_skDisk);
        if( i == 0)
        {
          LOGN_DEBUG("sk_disk_smart_read_data for \"" << device << "\" succeeded.")
          SkBool diskIdentityAvailable;
          SkBool SMARTavailable;
          i = sk_disk_smart_is_available(p_skDisk, & SMARTavailable);
          i = sk_disk_identify_is_available(p_skDisk, & diskIdentityAvailable);
          if(i == 0 && diskIdentityAvailable)
          {
            LOGN_DEBUG("sk_disk_identify_is_available for \"" << device << 
              "\" succeeded.")
            {
              LOGN_DEBUG("sk_disk_identify_parse for \"" << device << 
                "\" succeeded.")
      //    const SkSmartAttributeInfo * p = lookup_attribute(& skDisk, id);
              copySMARTvalues(p_skDisk, sMARTuniqueID, device);
              retVal = SMARTaccessBase::success;
            }
          }
        }
        else
          retVal = SMARTaccessBase::noSingleSMARTdevice;
//          std::cout << "failed to read SMART data:" << i << " errno:" << errno
//            << " " << strerror(errno) << std::endl;
        sk_disk_free( p_skDisk);
      }
//      else
//        retVal = SMARTaccessBase::noSingleSMARTdevice;
//        std::cout << "failed to open disk for SMART:" << i << " error:" <<
//        /*errno*/ strerror(errno) << std::endl;
      switch( errno)
      {
        case EACCES : /** If access denied */
          LOGN_ERROR("access denied to " << device)
          retVal = SMARTaccessBase::accessDenied;
          break;
//        case ENOENT :
//          LOGN_ERROR("no entry for " << device)
//          retVal = SMARTaccessBase::noSingleSMARTdevice;
//          break;
      }
    }
    LOGN("end for \"" << device << "\":returning " << retVal)
    //TODO program crashed when /dev/sr0 was opened
    return retVal;
  }

  int SMARTaccess::GetSupportedSMART_IDs(/*SkDisk * p_skDisk*/ 
    const char * const device,
    suppSMART_IDsType & SMARTattrNamesAndIDs)
  {
    SkDisk * p_skDisk;
  //  sk_disk_smart_is_enabled(& skDisk);

    /** "sk_disk_open" allocates an p_skDisk and assigns pointer to it.
     *  (must be freed by caller later)*/
    int i = sk_disk_open(device, /* SkDisk **_d*/ & p_skDisk);
    if( i == -1)
      LOGN_ERROR("error opening device \"" << device << "\"")
    else
    {
      LOGN_DEBUG("successfully opened device " << device)
  //    i = sk_init_smart( & skDisk);
  //        sk_disk_check_power_mode(p_skDisk);
      i = sk_disk_smart_read_data(p_skDisk);
      if( i == 0)
      {
        LOGN_DEBUG("successfully called sk_disk_smart_read_data for " << device)
        int retVal = ///Reads all supported attributes?
          sk_disk_smart_parse_attributes(
           p_skDisk,
           (SkSmartAttributeParseCallback) getDriveSupportedSMART_IDs,
           & SMARTattrNamesAndIDs);
             //TODO "sk_disk_smart_parse_attributes" traverses all attributes and calls
             // the callback function
        if( retVal < 0 )
        {
          LOGN_ERROR("sk_disk_smart_parse_attributes retVal:" << retVal)
          return -1;
        }
      }
      sk_disk_free( p_skDisk);
    }
  return 0;
}

  //TODO store paths where access was denied in order to show it later
enum SMARTaccessBase::retCodes SMARTaccess::ReadSMARTValuesForAllDrives(
  const fastestUnsignedDataType sMARTattrIDsToRead[])
{
    enum SMARTaccessBase::retCodes overallRetCode = SMARTaccessBase::
      noSingleSMARTdevice;
    //TODO scan all files in /dev/ or some better idea? libblkid?
    fastestUnsignedDataType numCharsNeeded = ::GetBlockDeviceFiles(NULL);
    char arch[numCharsNeeded];
    numCharsNeeded = ::GetBlockDeviceFiles(arch);
  char const * dvcFileNameBgn = arch;
  for( int index = 0; index < numCharsNeeded ; index ++ )
  {
    if( arch[index] == '\t' )
    {
        arch[index] = '\0';
        LOGN_DEBUG("current block device:" << dvcFileNameBgn )
      if( (arch + index - dvcFileNameBgn) == numCharsForDataCarrierFiles )
      {
        char absDvcFilePath[strlen(dvcFileNameBgn) + numCharsForDvcAndTerm0];
        ///Alternative:strcat(...)
        sprintf(absDvcFilePath, "/dev/%s", dvcFileNameBgn);
          
        suppSMART_IDsType suppSMARTattrNamesAndIDs;
          
        SMARTuniqueID sMARTuniqueID;
        fill(absDvcFilePath, sMARTuniqueID);

        //Only needs to be done for the 1st time to create the intersection
        // of supported and.
        if(GetSupportedSMART_IDs(absDvcFilePath, suppSMARTattrNamesAndIDs) == 0)
          sMARTuniqueID.SetSMART_IDsToRead(suppSMARTattrNamesAndIDs, 
            sMARTattrIDsToRead);

        enum SMARTaccessBase::retCodes retCode = readSMARTforDevice(//"/dev/sda"
          absDvcFilePath, sMARTuniqueID);
        switch( retCode)
        {
            case SMARTaccessBase::success :
              overallRetCode = SMARTaccessBase::success;
              break;
            case SMARTaccessBase::accessDenied :
              overallRetCode = SMARTaccessBase::accessDenied;
              break;
          }
        }
      dvcFileNameBgn = arch + index + /** "+1" to be beyond 'tab' */ 1 ;
    }
  }
  return overallRetCode;
}
} /* namespace libatasmart */
