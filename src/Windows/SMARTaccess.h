/**(c)from 2023 Stefan Gebauer,Computer Science Master(TU Berlin matr.no.361095)
 * @author Stefan Gebauer(TU Berlin matriculation number 361095)*/

///Include guard,see http://en.wikipedia.org/wiki/Include_guard :

/**Bln=BerLiN:http://www.acronymfinder.com/Berlin-(Bln).html
 * Prgm=PRaGMa
 * Incl=INCLude:http://www.abbreviations.com/abbreviation/include
 * Grd=GuaRD:http://www.abbreviations.com/abbreviation/guard */
#ifdef TU_Bln361095usePrgmInclGrd
/**Non-standard include guard:supported by many, but not all industry compilers:
 * see http://en.wikipedia.org/wiki/Pragma_once#Portability */
  #pragma once
#endif
#if defined TU_Bln361095usePrgmInclGrd ||\
/**Include guard supported by (nearly) all industry compilers:*/\
/**Bln=BerLiN: http://www.acronymfinder.com/Berlin-(Bln).html
 * OpSys=OPerating SYStem: http://www.abbreviations.com/OpSys 
 * MS=MicroSoft: http://www.abbreviations.com/abbreviation/MicroSoft
 * Win=WINdows: http://www.abbreviations.com/abbreviation/Windows */\
 ! defined TU_Bln361095SMARTmon_OpSys_MS_Win_hardware_dataCarrier_SMARTaccess_h
   #define TU_Bln361095SMARTmon_OpSys_MS_Win_hardware_dataCarrier_SMARTaccess_h

///_This_ repository's files:
#include <SMARTaccessBase.hpp>///base class SMARTaccessBase
 /**TU_Bln361095SMARTmon_OpSysWindowsNmSpcBgn,
  * TU_Bln361095SMARTmon_OpSysWindowsNmSpcEnd */
#include "SMARTmonOpsSysWindows_ID_prefix.h"

///Stefan Gebauer's(TU Berlin matr.#361095)~"common_sourcecode"repository files:
#include <hardware/bus/busType.h>///enum TU_Bln361095::hardware::bus::Type
#include <hardware/dataCarrier/NVMe/NVMe_SMART_attr.h>
 ///TU_Bln361095::hardware::bus::UnifyType()
#include <OperatingSystem/Windows/hardware/busType.h>
 ///TU_Bln361095hardwareDataCarrierUse(GetPath)
#include <OperatingSystem/Windows/hardware/dataCarrier/getDataCarrierPath.h>
 ///Get the interface type (NVMe/SATA/IDE etc.)
#include <OperatingSystem/Windows/hardware/dataCarrier/getIdentity.h>
 ///TU_Bln361095hardwareDataCarrierUse(Open)
#include <OperatingSystem/Windows/hardware/dataCarrier/openDataCarrier.h>
 ///TU_Bln361095::hardware::dataCarrier::GetStorageDvcInfo(...)
#include <OperatingSystem/Windows/hardware/dataCarrier/getStorageDvcInfo.h>
#include <OperatingSystem/Windows/hardware/NVMe/getSMARTvals.h>

#ifndef TU_Bln361095TmCntDfnd
  #define TU_Bln361095TmCntDfnd
  typedef uint64_t TimeCountInNanosec_type;
  typedef double TimeCountInSecType;
#endif
#include <OperatingSystem/time/GetUpTime.h>///OperatingSystem::GetUptimeInS(...)

TU_Bln361095SMARTmon_OpSysWindowsNmSpcBgn

class SMARTaccess
  : public SMARTaccessBase
{
public:
  SMARTaccess(SMARTuniqueIDandValsContType & sMARTuniqueIDsandVals)
    : SMARTaccessBase(sMARTuniqueIDsandVals)
  {}
  void fill(HANDLE deviceHandle, SMARTuniqueID & sMARTuniqueID)
  {
  	TU_Bln361095::hardware::dataCarrier::getIdentity::Rslt
      getDataCarrierIdentityRslt = TU_Bln361095::hardware::dataCarrier::
        GetIdentity(
          deviceHandle,
          (uint8_t *) sMARTuniqueID.getModelNameAddr(),///manufacturer,
          (uint8_t *) sMARTuniqueID.getSerialNumberAddr(),///serNo,
          (uint8_t *) sMARTuniqueID.getFirmWareNameAddr()///firmWare
		  );
  }

  ///@param dataCarrierHandle must be open
  inline enum SMARTaccessBase::retCodes readSMARTviaNVMe(
    HANDLE dataCarrierHandle,
    SMARTuniqueIDandValues & sMARTuniqueIDandVals,
    ///For reading different IDs (either all or a subset of supported IDs) 
    const TU_Bln361095::CPU::FaststUint IDsOfSMARTattrsToRd[]
    )
  {
    uint8_t * pDvcIOctlBuf;
    NVME_HEALTH_INFO_LOG * pNMVeHealthInfoLog;
    enum TU_Bln361095::hardware::dataCarrier::NVMe::getSMARTvals::Rslt
      getSMARTvalsRslt = TU_Bln361095::hardware::dataCarrier::NVMe::
        GetSMARTvals(
        dataCarrierHandle,
        & pDvcIOctlBuf,
        & pNMVeHealthInfoLog);

    if(getSMARTvalsRslt != TU_Bln361095::hardware::dataCarrier::NVMe::
      getSMARTvals::Sccss)
    {
      if(getSMARTvalsRslt != TU_Bln361095::hardware::dataCarrier::NVMe::
        getSMARTvals::AllocBufFaild)
        free(pDvcIOctlBuf);
      return SMARTaccessBase::otherError;
    }

    long double uptimeInSeconds;
    OperatingSystem::GetUptimeInS(uptimeInSeconds);

    SMARTuniqueID & sMARTuniqueID = sMARTuniqueIDandVals.getSMARTuniqueID();

    if(SMARTuniqueID::isEmpty(sMARTuniqueID.getSupportedSMART_IDs() ) )
    {
      suppSMART_IDsType suppSMARTattrNamesAndIDs;
      for(TU_Bln361095::CPU::FaststUint SMARTattrID = TU_Bln361095::
        dataCarrier::NVMe::SMART::Attr::TotalDataRead;
        SMARTattrID <= TU_Bln361095::dataCarrier::NVMe::SMART::Attr::
        ErrorInfoLogEntryCount; SMARTattrID++)
      {
        /**Only needs to be done for the 1st time to create the intersection
         * of supported and SMART IDs to read from the configuration file.*/
        suppSMARTattrNamesAndIDs.insert(SMARTattributeNameAndID("", SMARTattrID)
          );
      }
      sMARTuniqueID.setSupportedSMART_IDs(suppSMARTattrNamesAndIDs);
      sMARTuniqueID.SetSMART_IDsToRead(suppSMARTattrNamesAndIDs, 
        IDsOfSMARTattrsToRd);
    }
    for(TU_Bln361095::CPU::FaststUint SMART_IDsToReadArrIdx = 0;
      sMARTuniqueID.SMART_IDsToReadNotEnd(SMART_IDsToReadArrIdx);
      SMART_IDsToReadArrIdx++)
    {
      const TU_Bln361095::CPU::FaststUint SMART_ID = sMARTuniqueID.
        m_SMART_IDsToRd[SMART_IDsToReadArrIdx];
  /**See
http://media.kingston.com/support/downloads/MKP_521.6_SMART-DCP1000_attribute.pdf
   * : "ByteIndex": "47:32" -> 16 byte S.M.A.R.T. attribute raw values beginning
   * with NVMe S.M.A.R.T. attribute ID "Data Units Read".*/
      if(SMART_ID >= TU_Bln361095::dataCarrier::NVMe::SMART::Attr::
        TotalDataRead)
      {
        SMARTvalue & sMARTval = sMARTuniqueIDandVals.m_SMARTvalues[SMART_ID];
        const uint8_t * const SMARTrawValAddr =( (uint8_t*) pNMVeHealthInfoLog->
          DataUnitRead) + TU_Bln361095dataCarrierNVMeSMARTattrNumRawValB * 
          (SMART_ID - TU_Bln361095::dataCarrier::NVMe::SMART::Attr::
            TotalDataRead);
        sMARTval.SetRawVal(SMARTrawValAddr,
          TU_Bln361095dataCarrierNVMeSMARTattrNumRawValB, uptimeInSeconds);
      }
    }
    free(pDvcIOctlBuf);
    return SMARTaccessBase::success;
  }

  ///@param dataCarrierHandle must be open
  enum SMARTaccessBase::retCodes readSMARTforDevice(
    HANDLE dataCarrierHandle,
    SMARTuniqueIDandValues & sMARTuniqueIDandVals,
    ///For reading different IDs (either all or a subset of supported IDs) 
    const TU_Bln361095::CPU::FaststUint IDsOfSMARTattrsToRd[]
    )
  {
    enum TU_Bln361095::hardware::bus::Type busType = sMARTuniqueIDandVals.
      getSMARTuniqueID().getBusType();
    switch(busType)
    {
    case TU_Bln361095::hardware::bus::NVMe:
      return readSMARTviaNVMe(
        dataCarrierHandle,
        sMARTuniqueIDandVals,
        IDsOfSMARTattrsToRd);
    case TU_Bln361095::hardware::bus::SATA:
    case TU_Bln361095::hardware::bus::USB:
      break;
    }
    return SMARTaccessBase::success;
  }

  enum SMARTaccessBase::retCodes ReadSMARTValuesForAllDrives(
    ///May be a subset of supported SMART IDs.
    const TU_Bln361095::CPU::FaststUint SMARTattrIDsToRead[],
    dataCarrierID2devicePath_type & dataCarrierID2dvcPath)
  {
	  TU_Bln361095::CPU::FaststUint idx = 0;
	  do
    {
      HANDLE dataCarrierHandle;
      char dataCarrierPath[MAX_PATH];
      std::ostringstream oss;
      oss << idx;

      TU_Bln361095hardwareDataCarrierUse(GetPathA)(
        oss.str().c_str(),///const TCHAR dataCarrierID[],
        dataCarrierPath);
      dataCarrierHandle = TU_Bln361095hardwareDataCarrierUse(OpenA)(
        dataCarrierPath///const TCHAR dataCarrierPath[]
        );
	    if(dataCarrierHandle == INVALID_HANDLE_VALUE){
        const DWORD lastOSerror = GetLastError();
        ///No more data carrier available.
	      if(lastOSerror == ERROR_FILE_NOT_FOUND)
	        break;///End loop because no more data carriers.
      }

      STORAGE_DEVICE_DESCRIPTOR storageDvcDesc;
      //TU_Bln361095hardwareDataCarrierUse(Info) dataCarrierInfoRslt =
      BOOL bDvcIOctrlRslt =
        TU_Bln361095::hardware::dataCarrier::GetStorageDvcInfo(
          dataCarrierHandle,
          &storageDvcDesc);
      if(bDvcIOctrlRslt != TRUE)
        return SMARTaccessBase::getBusTypFaild;

      SMARTuniqueID sMARTuniqueID;
      fill(dataCarrierHandle, sMARTuniqueID);

      /**Store the bus type so it does not need to be determined later.
       * But the bus type for the same data carrier may change?:
       * hotplug-able PCI express or connected via USB? So (un)plug events must
       * be tracked. */
      const enum TU_Bln361095::hardware::bus::Type busType = TU_Bln361095::
        hardware::bus::UnifyType(storageDvcDesc.BusType);
      sMARTuniqueID.setBusType(busType);

      std::pair<std::set<SMARTuniqueIDandValues>::iterator, bool> insertRslt =
#if __cplusplus >= 201103///C++ 11
        m_SMARTuniqueIDsandVals.emplace(SMARTuniqueIDandValues(sMARTuniqueID) );
#else
        m_SMARTuniqueIDsandVals.insert(SMARTuniqueIDandValues(sMARTuniqueID) );
#endif
      LOGN_DEBUG((insertRslt.second == true ? "inserted" : "changed")
        << " SMARTuniqueIDandValues object")
      SMARTuniqueIDandValues & sMARTuniqueIDsandVals =
        (SMARTuniqueIDandValues&)(*insertRslt.first);

      enum SMARTaccessBase::retCodes retCode = readSMARTforDevice(
        dataCarrierHandle,
        sMARTuniqueIDsandVals,
        ///Intersection of SMART IDs to read and supported SMART IDs.
        sMARTuniqueID.m_SMART_IDsToRd);

#if __cplusplus >= 201103///C++ 11
      ///http://en.cppreference.com/w/cpp/container/map/emplace : "since C++11"
	    dataCarrierID2dvcPath.emplace(sMARTuniqueID);
#else
      dataCarrierID2dvcPath.insert(std::make_pair(
        sMARTuniqueID, dataCarrierPath) );
#endif
      idx++;
    }while(true);
    return SMARTaccessBase::success;
  }
};

TU_Bln361095SMARTmon_OpSysWindowsNmSpcEnd

#endif///include guard
