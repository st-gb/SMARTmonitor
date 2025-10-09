///(c) Stefan Gebauer(Computer Science Master from TU Berlin)
///@author Stefan Gebauer(TU Berlin matriculation number:361095)

///MicroSoft Windows (Software Development Kit) header files:
 #include <windows.h>

///C(++) standard library header files:
 #include <iostream>///std::cout

///Stefan Gebauer's(TU matriculation number 361095) ~"cmnSrc" repository files:
 ///For TU_Bln361095OpSysUse(GetUptime)(...)
 typedef double TU_Bln361095tmCntInNsTyp;
#pragma message("set TU_Bln361095tmCntInNsTyp")
 #define TU_Bln361095TmCntDfnd
 #include <hardware/CPU/fastest_data_type.h>///TU_Bln361095::CPU::faststUint
 /**TU_Bln361095hardwareDataCarrierUse(OpenA),
    TU_Bln361095hardwareDataCarrierUse(OpnFaild) */
 #include <hardware/dataCarrier/openDataCarrier.h>
#ifdef __cplusplus
 ///TU_Bln361095hardwareDataCarrierNmSpc::cxxopts::handleCmdLineOpts(...)
 #include <hardware/dataCarrier/cxxopts/handleCmdLineOpts.hpp>
#endif
 typedef double TU_Bln361095tmCntInSecTyp;
 ///TU_Bln361095OpSysUse(GetUptimeInS)(...)
 #include <OperatingSystem/time/GetUpTime.h>
 #include <SMARTaccType.hpp>///typdef TU_Bln361095SMARTmonUse(SMARTaccessTyp)

///_This_ repository's files:
 //#include "handCmdLineOpts.cpp"
 /**TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdUse
  * TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsNmSpcBgn
  * TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsNmSpcEnd */
 #include "RdAllSuppNVMeSMARTattrIDs_ID_prefix.h"
 ///readAndPrintRawValsForAllSuppSMARTattrIDs(...)
 #include "../readAndPrintAllSuppSMARTattrIDs.h"

//TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsUse

enum TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdUse(progExitCds)
  getAndPrintSuppSMARTattrIDs(
    TU_Bln361095hardwareDataCarrierUse(IDtyp) dataCarrierID,
    const char ar_chDtCarrierPath[MAX_PATH]
  )
{
#ifdef __cplusplus
  TU_Bln361095::SMARTmon::SMARTaccessBase::SMARTuniqueIDandValsContType
    sMARTuniqueIDsAndVals;
  TU_Bln361095::SMARTmon::SMARTaccessTyp sMARTaccess(sMARTuniqueIDsAndVals);

  TU_Bln361095::SMARTmon::SMARTuniqueID sMARTuniqueID;

  sMARTaccess.fill(dataCarrierID, sMARTuniqueID);
  //TU_Bln361095SMARTmonSMARTaccess(Fill)(dataCarrierID, sMARTuniqueID);

  suppSMART_IDsType suppSMARTattrNamesAndIDs;

  TU_Bln361095tmCntInSecTyp upTimeInS_before, upTimeInS_after;
  TU_Bln361095OpSysUse(GetUptimeInS)(upTimeInS_before);
  int SuppSMARTattrIDsRtrnCd = sMARTaccess.GetSuppSMARTattrIDs(
    //dtCarrierPath.c_str()
    //ar_chDtCarrierPath
    dataCarrierID,
    suppSMARTattrNamesAndIDs);
  TU_Bln361095OpSysUse(GetUptimeInS)(upTimeInS_after);

  printf("Operating System uptime before reading all supported S.M.A.R.T. "
    "attribute IDs: %f s\n", upTimeInS_before);
  printf("Operating System uptime after reading all supported S.M.A.R.T. "
    "attribute IDs: %f s\n", upTimeInS_after);
  printf("=>Reading all supported S.M.A.R.T. attributes took ca. %f s\n",
    (upTimeInS_after - upTimeInS_before) );

  if(SuppSMARTattrIDsRtrnCd != TU_Bln361095SMARTmonNmSpc::SMARTaccessBase::
    success)
  {
    fprintf(stderr, "Failed to get supported S.M.A.R:T. IDs for \"%s\"\n",
      ar_chDtCarrierPath);
    return TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdUse(
      GetSuppSMART_IDsFaild);
  }

  std::cout << "Number of S.M.A.R.T. attributes supported by " << //dtCarrierPath
    ar_chDtCarrierPath << " (" <<
    sMARTuniqueID << "):"
    << suppSMARTattrNamesAndIDs.size()
    << std::endl;
  for(auto suppSMARTattrNameAndIDIter = suppSMARTattrNamesAndIDs.begin();
    suppSMARTattrNameAndIDIter != suppSMARTattrNamesAndIDs.end();
    suppSMARTattrNameAndIDIter++)
  {
    std::cout
      << "S.M.A.R.T. attribute ID " << suppSMARTattrNameAndIDIter->GetID()
      << ":" << suppSMARTattrNameAndIDIter->getName()
      << std::endl;
  }
#endif
  return TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdUse(
    Success);
}

int main(int procArgCnt, char * procArgArr [])
{
    //TU_Bln361095hardwareDataCarrierFirstA;
  char ar_chDtCarrierPath[MAX_PATH];

#ifdef __cplusplus
//  TU_Bln361095SMARTmonNmSpc::cxxopts::HandleCmdLineOpts(procArgCnt,
//    procArgArr, dtCarrierPath);
  TU_Bln361095::hardware::dataCarrier::cxxopts::handleCmdLneArgsRslt
    handleCmdLneArgsRslt = TU_Bln361095::hardware::dataCarrier::cxxopts::
    handleCmdLineOpts(procArgCnt, procArgArr, ar_chDtCarrierPath);
  if(handleCmdLneArgsRslt == TU_Bln361095::hardware::dataCarrier::cxxopts::
    noProcessArgs)
    TU_Bln361095hardwareDataCarrierUse(GetANSIpathFromIdx)(0,
      ar_chDtCarrierPath);
#else
  TU_Bln361095hardwareDataCarrierUse(GetANSIpathFromIdx)(0, ar_chDtCarrierPath);
#endif

  /*TU_Bln361095hardwareDataCarrierIDtyp*/
  TU_Bln361095hardwareDataCarrierUse(IDtyp) dataCarrierID =
    TU_Bln361095hardwareDataCarrierUse(OpenA)(ar_chDtCarrierPath);
  if(TU_Bln361095hardwareDataCarrierUse(OpnFaild)(dataCarrierID) )
    return TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdUse(
      OpnDtCarrierFaild);

  const enum TU_Bln361095SMARTmonTstsRdAllSuppNVMeSMARTattrIDsExitCdUse(
    progExitCds) retCode = getAndPrintSuppSMARTattrIDs(dataCarrierID,
      ar_chDtCarrierPath);

  //readAndPrintRawValsForAllSuppSMARTattrIDs(dataCarrierID);

  //TU_Bln361095hardwareDataCarrierUse(Close)(dataCarrierID);

  return retCode;
}
