/** SMARTaccess.cpp
 *  Created on: 31.07.2016
 *  Author: Stefan Gebauer, M.Sc. Comp. Sc.(TU Berlin)*/

///_This_ repository's files:
#include <libATA_SMART/SMARTaccess.hpp>
#include <SMARTmonitorBase.hpp>///gp_SMARTmon

///Standard C/C++ libs
#include <atasmart.h> //sk_disk_smart_is_available
#include <errno.h> //errno, EACCES
#include <string> //class std::string
#include <string.h> //strcmp(...)
#include <iostream> //std::cout
#include <sstream>///class std::ostringstream

///Stefan Gebauer's(TU Berlin matr.#361095)"common_sourcecode" repository files:
///OperatingSystem::GetTimeCountInNanoSeconds(...)
#include "Controller/time/GetTickCount.hpp"
#include <hardware/CPU/atomic/AtomicExchange.h>///AtomicExchange(...)
#include <hardware/dataCarrier/ATA3Std.h>///numSMARTrawValB
#include <OperatingSystem/POSIX/GetBlockDeviceFiles.h>//GetBlockDeviceFiles(...)
#include <OperatingSystem/time/GetUpTime.h>///OperatingSystem::GetUptimeInS(...)
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

inline void cpySMARTattrThreadSafe(SMARTvalue & sMARTvalue, const uint64_t & 
  rawSMARTattrVal)
{
  sMARTvalue.SetRawValue(rawSMARTattrVal);
  AtomicExchange( (long int *) & sMARTvalue.m_successfullyReadSMARTrawValue, 1);
  long double uptimeInSeconds;
  OperatingSystem::GetUptimeInS(uptimeInSeconds);
#ifdef DEBUG_
  std::string str;
  UserInterface::FormatTime(uptimeInSeconds,str);
#endif
  sMARTvalue.SetRetrievalTime(uptimeInSeconds);
#ifdef _DEBUG
  uint64_t SMARTrawValueFromMem;
  bool bConsistent = sMARTvalue.IsConsistent(SMARTrawValueFromMem);
#endif
}

/** Is called in sk_disk_smart_parse_attributes(...) for every supported SMART
 *  attribute of the disk. Attention : function must have the same signature as 
 * SkSmartAttributeParseCallback */
void getSMARTrawValueCallback(
  SkDisk * p_SkDisk,
  const SkSmartAttributeParsedData * p_SkSmartAttributeParsedData,
  struct libatasmart::attr_helper * p_libatasmart_attrHelper)
{
  //  std::cout << "a->name: " << a->name << " " << g_stdStrAttributeName.c_str() << std::endl;
  //  if (a->pretty_unit != SK_SMART_ATTRIBUTE_UNIT_NONE)
  //  {
  //    std::cout << "a->pretty_unit != SK_SMART_ATTRIBUTE_UNIT_NONE" << std::endl;
  //    return;
  //  }
//    LOGN( "current attrib:" << p_SkSmartAttributeParsedData->name )
  const SMARTuniqueIDandValues * p_sMARTuniqueIDandValues =
    p_libatasmart_attrHelper->p_sMARTuniqueIDandValues;
  const SMARTuniqueID & sMARTuniqueID = p_sMARTuniqueIDandValues->
    getSMARTuniqueID();
  const fastestUnsignedDataType * IDsOfSMARTattrsToRd =
    p_libatasmart_attrHelper->IDsOfSMARTattrsToRd;
  
  const fastestUnsignedDataType SMARTattrID = p_SkSmartAttributeParsedData->id;
  SMARTaccessBase * pSMARTacc = p_libatasmart_attrHelper->pSMARTacc;
  
  fastestUnsignedDataType & currSMART_IDtoReadIdx = p_libatasmart_attrHelper->
    currSMART_IDtoReadIdx;
  while(IDsOfSMARTattrsToRd[currSMART_IDtoReadIdx] < SMARTattrID){
    if(IDsOfSMARTattrsToRd[currSMART_IDtoReadIdx] == 0)
      break;
    currSMART_IDtoReadIdx++;
  }
  
  /** If strings are identical. */
  if(IDsOfSMARTattrsToRd[p_libatasmart_attrHelper->currSMART_IDtoReadIdx] ==
     SMARTattrID)
  {
    SMARTvalue & sMARTvalue = (SMARTvalue &) p_sMARTuniqueIDandValues->
      m_SMARTvalues[SMARTattrID];
    uint64_t rawSMARTattrVal =///Lowmost byte at array index 0
      *(uint64_t*) p_SkSmartAttributeParsedData->raw;
    LOGN( p_SkSmartAttributeParsedData->name << " found")
    /** For SSD model HFS256G39TND-N210A firmware 30001P10 serial
     * EJ7CN55981080CH09:for S.M.A.R.T. ID 241 (total LBAs/data written)
     * SkSmartAttributeParsedData.pretty_value = 181395
     * SkSmartAttributeParsedData.pretty_unit = SK_SMART_ATTRIBUTE_UNIT_MB =>
     *  181395 * 1024*1024 B=190206443520 B=190GB-> wrong value
     * SkSmartAttributeParsedData.raw value is: 21 30 => 21*256+30=5406
     * -if unit=LBA & logical sector size: 5406 * 512 B =  27678is72 B
     * -if unit=LBA & physical sector size: 5406 4096 B = 22142976 B 
     * In reality SkSmartAttributeParsedData.raw for this SSD: unit ~= GiB */
#ifdef DEBUG
      ///Lowmost byte of array "raw" at array index 0
      const uint8_t * p_rawData = p_SkSmartAttributeParsedData->raw;
      std::ostringstream stdoss;
      stdoss << "raw data for " << SMARTattrID << ":";
      for(fastestUnsignedDataType idx = 0; idx < numSMARTrawValB; idx++){
        stdoss << (unsigned) p_rawData[idx] << " ";
      }
      std::string str = stdoss.str();
      LOGN(str)
#endif
    /**Unit auto-detection has to be done here because the data carrier to
    * retrieve additional data for is here (and not at client side).*/
    pSMARTacc->possiblyAutoDetectUnit(SMARTattrID, rawSMARTattrVal, 
      (SMARTuniqueID &) sMARTuniqueID, p_libatasmart_attrHelper->device);
    cpySMARTattrThreadSafe(sMARTvalue, rawSMARTattrVal);
  /**The following values are set by manufacturer (in drive controller?). They
   * are important for detecting if the number of reallocated sectors is
   * exceeded for example. Especially if this information is not known (from
   * datasheets) then we must rely on how far the "current_value" is away from
   * the "threshold".*/
    AtomicExchange(sMARTvalue.GetNrmlzdThresh(),
      p_SkSmartAttributeParsedData->threshold);
    //TODO When debugging "current" value gotten via direct S.M.A.R.T. access in
    // frontend code via was once 200 for reallocated sectors although it is
    // only 100
    AtomicExchange(sMARTvalue.GetNrmlzdCurrVal(),
      p_SkSmartAttributeParsedData->current_value);
  }
  else
    AtomicExchange( (long int *) & p_sMARTuniqueIDandValues->
      m_SMARTvalues[SMARTattrID].m_successfullyReadSMARTrawValue, 0);
}

int readSMARTAttrs(
  SkDisk * p_skDisk, 
  struct attr_helper & libatasmart_attribHelper
  )
{
//   assert(d);
//   assert(kelvin);
  /** sk_disk_smart_parse_attributes reads all supported attributes?*/
  int retVal = sk_disk_smart_parse_attributes(
    p_skDisk,
    (SkSmartAttributeParseCallback) getSMARTrawValueCallback,
    & libatasmart_attribHelper);
  if( retVal < 0 )
  {
    LOGN_ERROR("sk_disk_smart_parse_attributes failed.retVal:" << retVal)
    return -1;
  }
/*  if( ! libatasmart_attribHelper.found) {
    errno = ENOENT;
    return -2;
  }*/
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
  struct attr_helper & libatasmartAttrHelper
  )
{
//    SMARTmonitorBase::SMARTattrToObsType::const_iterator 
//      constSMARTattrsToObsConstIter = sMARTmonBase.m_IDsOfSMARTattrsToObserve.
//      begin();
//            std::pair<std::set<SkIdentifyParsedData>::iterator, bool> insert =
//              m_SMARTuniqueIDs.insert(*p_SkIdentifyParsedData);

    //TODO sMARTuniqueIDandValues.SMARTuniqueID is not a 100% copy of
    // "* p_SkIdentifyParsedData" (seems that parts of the "firmware"
    //  string is copied into "serial"

    int i;
//              if( insert.second == true )/** If actually inserted into std::set*/
//    {
      //TODO only retrieve the data of the SMART IDs to observe list (and where
      // a definition is available?) Only get SMART info if it is supported??
//      SMARTattrDef * p_SMARTattrDef = SMARTaccessBase::getSMARTattrDef(
//        SMARTattrID);
//    if(p_SMARTattrDef){
//        SMARTattributeID = constSMARTattributesToObserveConstIter->GetAttributeID();
//        attributeName = p_SMARTattrDef->GetName();

  i = readSMARTAttrs((SkDisk *) p_skDisk, libatasmartAttrHelper);
      /** Assign the pointer before the branch instruction because it is also
       * used in the "else" part.*/
      if( i == 0) /** Successfully got SMART attribute value */
      {
//        std::cout << attributName << ":" << rawSMARTattrValue << std::endl;
//                  insert.first->
//        std::set<SMARTuniqueIDandValues>::iterator it = insert.first;
//       const SMARTuniqueID & currentSMARTuniqueIDandValuesSMARTuniqueID = it->getSMARTuniqueID();
//          LOGN_DEBUG("SMART unique ID:" << p_sMARTuniqueIDandValues->getSMARTuniqueID().str())
        }
        else
        {
    LOGN( "reading SMART values failed:" << i << " error code:" << errno)
        }
//            else
//              std::cerr << "Failed to get attribute value for \"" << attributName << "\":"
//                << strerror(errno) << std::endl;
          //return retVa;
//    }
}

enum SMARTaccessBase::retCodes SMARTaccess::readSMARTforDevice(
  const char device [],
  SMARTuniqueID & sMARTuniqueID,
  dataCarrierID2devicePath_type & dataCarrierID2devicePath
  ///For reading different IDs (either all or a subset of supported IDs) 
  , const fastestUnsignedDataType IDsOfSMARTattrsToRd []
  )
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
          std::pair<std::set<SMARTuniqueIDandValues>::iterator, bool> insert =
            //TODO enable emplace(...) if C++ 11
            m_SMARTuniqueIDsandVals.insert/*emplace*/(SMARTuniqueIDandValues(
              sMARTuniqueID));
          LOGN_DEBUG( (insert.second == true ? "inserted" : "changed")
            << " SMARTuniqueIDandValues object")
          SMARTuniqueIDandValues & sMARTuniqueIDsandVals = 
            (SMARTuniqueIDandValues &) (* insert.first);
#ifdef _DEBUG
          std::pair<dataCarrierID2devicePath_type::iterator, bool> 
            dataCarrierID2devicePathInsert =
#endif
          dataCarrierID2devicePath.insert(std::make_pair(
            sMARTuniqueIDsandVals.getSMARTuniqueID(), device));

          struct attr_helper libatasmartAttrHelper;
          libatasmartAttrHelper.pSMARTacc = this;
          libatasmartAttrHelper.p_sMARTuniqueIDandValues =
            & sMARTuniqueIDsandVals;
          libatasmartAttrHelper.device = device;
          libatasmartAttrHelper.IDsOfSMARTattrsToRd = IDsOfSMARTattrsToRd;

              copySMARTvalues(p_skDisk, libatasmartAttrHelper);
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

/**@param dvcFlPth file system path of device file
 * @return 0 success, else error*/
int SMARTaccess::GetSupportedSMART_IDs(/*SkDisk * p_skDisk*/
  const char dvcFlPth[],
  suppSMART_IDsType & SMARTattrNamesAndIDs)
{
  SkDisk * p_skDisk;
  //  sk_disk_smart_is_enabled(& skDisk);

  /** "sk_disk_open" allocates an p_skDisk and assigns pointer to it.
   *  (must be freed by caller later)*/
  errno = 0;///Set to 0 to detect errors in sk_disk_open(...)
/**Iterating over block devices took ca. 24,5 s because also floppy drives/disks
 * are examined. See log file entries:
 * "2022.04.16 15:55:42s817ms INFO [UI-3365] :: readSMARTforDevice end for
 * "/dev/sda":returning 0
 * 2022.04.16 15:56:07s345ms ERROR [UI-3365] :: GetSupportedSMART_IDs error
 * opening device "/dev/fd0" OS error code:6 error:No such device or address"
 * Should display a status message ~"opening device" in this case.*/
  gp_SMARTmon->ShowMessage("Opening device"/*UI::OpnDvc, dvcFlPth*/);

  ///https://github.com/Rupan/libatasmart/blob/master/atasmart.c
  /** "sk_disk_open" allocates an p_skDisk and assigns pointer to it.
   *  (must be freed by caller via "sk_disk_free(...)" later)*/
  int rtrnVal = sk_disk_open(dvcFlPth, /* SkDisk **_d*/ & p_skDisk);
/**"sk_disk_open" failed :
 * http://github.com/Rupan/libatasmart/blob/master/atasmart.c#L2759 */
  if(rtrnVal == -1)
  {
      //TODO fails often-> too many error messages
      //Solution?: only check if device plugged in:
      // https://unix.stackexchange.com/questions/315216/ubuntu-16-04-how-can-i-detect-a-device-independent-usb-insert-event
      // Get event with "udevadm monitor" on console
      // https://askubuntu.com/questions/1175856/how-can-i-run-a-command-when-a-usb-device-is-dis-connected
      // programmatically: via "libudev-dev" ? udev_monitor[...] (livbudev.h)
      //  udev_queue
      // https://github.com/gentoo/eudev/blob/master/src/udev/udevadm-monitor.c
      // static int adm_monitor(struct udev *udev, int argc, char *argv[])
      if(errno > 0)///sk_disk_open(...) sets "errno" on error
        LOGN_ERROR("error opening device \"" << dvcFlPth << "\" OS error code:"
          << errno << " error:" << strerror(errno) )
    }
    else
    {
      LOGN_DEBUG("successfully opened device " << dvcFlPth)
  //    i = sk_init_smart( & skDisk);
  //        sk_disk_check_power_mode(p_skDisk);
      rtrnVal = sk_disk_smart_read_data(p_skDisk);///sk_disk_smart_is_available(...)
      if(rtrnVal == 0)
      {
        LOGN_DEBUG("successfully called sk_disk_smart_read_data for " << dvcFlPth)
        rtrnVal = ///Reads all supported attributes?
          sk_disk_smart_parse_attributes(
           p_skDisk,
           (SkSmartAttributeParseCallback) getDriveSupportedSMART_IDs,
           & SMARTattrNamesAndIDs);
             //TODO "sk_disk_smart_parse_attributes" traverses all attributes and calls
             // the callback function
        if( rtrnVal < 0 )
        {
          LOGN_ERROR("sk_disk_smart_parse_attributes return value:" << rtrnVal)
        }
      }
      sk_disk_free( p_skDisk);
    }
  return rtrnVal;
}

  //TODO store paths where access was denied in order to show it later
enum SMARTaccessBase::retCodes SMARTaccess::ReadSMARTValuesForAllDrives(
  const fastestUnsignedDataType sMARTattrIDsToRead[],
  dataCarrierID2devicePath_type & dataCarrierID2devicePath)
{
  enum SMARTaccessBase::retCodes overallRetCode = SMARTaccessBase::
    noSingleSMARTdevice;
  //TODO scan all files in /dev/ or some better idea? libblkid?
  fastestUnsignedDataType numCharsNeeded = ::GetBlockDeviceFiles(NULL);
  char arch[numCharsNeeded];
/**Iterating over block devices took ca. 24,5 s because also floppy drives/disks
 * are examined. See log file entries:
 * "2022.04.16 15:55:42s817ms INFO [UI-3365] :: readSMARTforDevice end for
 * "/dev/sda":returning 0
 * 2022.04.16 15:56:07s345ms ERROR [UI-3365] :: GetSupportedSMART_IDs error
 * opening device "/dev/fd0" OS error code:6 error:No such device or address"
 * Should display a status message ~"opening device" in this case.
 * After ca. 8 h (ca.15:55-01:00):
 * Unit range for model:WDC WD400EB-00CPF0 firmware:06.04G06 serial number:
 * WD-WCAAT4328734 was then ca.1h1s-1h30s. Without this lag/ unit range was
 * 59min47s541ms-59min55s88ms.
 * Unit range for model :QUANTUM FIREBALLP LM10 firmware:A35.0500 serial number:
 * 882008566041 was then ca.1h1s-1h30s */
  //numCharsNeeded = TU_Bln361095OpSys(GetAllBlkDvcFiles)(archDirPath, sepStr);
  numCharsNeeded = ::GetBlockDeviceFiles(arch);

  char const * dvcFileNameBgn = arch;
  ///Loop to filter for data carriers (file names without decimial digits(s) ).
  for(TU_Bln361095::hardware::CPU::FaststUint index = 0; index < numCharsNeeded;
   index++)
  {
    if(arch[index] >= '0' && arch[index] <=9)
      ;
    if(arch[index] == '\t')
    {
      ///Mark character string end for Standard C's sprintf(...)
      arch[index] = '\0';
      LOGN_DEBUG("current block device:" << dvcFileNameBgn )
      if( (arch + index - dvcFileNameBgn) == numCharsForDataCarrierFiles )
      {
        char absDvcFilePath[strlen(dvcFileNameBgn) + numCharsForDvcAndTerm0];
        ///Alternative:strcat(...)
        sprintf(absDvcFilePath, "/dev/%s", dvcFileNameBgn);
        //strcat()
        suppSMART_IDsType suppSMARTattrNamesAndIDs;

        SMARTuniqueID sMARTuniqueID;
        fill(absDvcFilePath, sMARTuniqueID);

        if(sMARTuniqueID.getSupportedSMART_IDs()[0] == 0)
          //Only needs to be done for the 1st time to create the intersection
          // of supported and.
          if(GetSupportedSMART_IDs(absDvcFilePath,suppSMARTattrNamesAndIDs) ==0)
          {
            sMARTuniqueID.setSupportedSMART_IDs(suppSMARTattrNamesAndIDs);
            sMARTuniqueID.SetSMART_IDsToRead(suppSMARTattrNamesAndIDs, 
              sMARTattrIDsToRead);
          }
        enum SMARTaccessBase::retCodes retCode = readSMARTforDevice(
          absDvcFilePath, sMARTuniqueID, dataCarrierID2devicePath,
          ///Intersection of SMART IDs to read and supported SMART IDs.
          sMARTuniqueID.m_SMART_IDsToRd);
        ///TODO may already have been retrieved by SMARTuniqueID::guessUnit(...)
        // if done there then do not do it here.
        // For example use "udev" monitoring if a data carrier was added or not
        // therefore.
//        sMARTuniqueID.numBwrittenSinceOSstart = /*OperatingSystem::*/dataCarrier::
//          getNumBwrittenSinceOSstart(stdstrDataCarrierPath);
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
}/**namespace libatasmart*/
