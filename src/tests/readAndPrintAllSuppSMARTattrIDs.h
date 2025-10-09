/**(c)from 2025 Stefan Gebauer,Computer Science Master(TU Berlin matr.no.361095)
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
 * mon=MONitor
 * attr=ATTRibute */\
 ! defined TU_Bln361095SMARTmon__tests__readAndPrintAllSuppSMARTattrIDs_h
   #define TU_Bln361095SMARTmon__tests__readAndPrintAllSuppSMARTattrIDs_h

///Stefan Gebauer's(TU Berlin matriculation number 361095) ~"cmnSrc" repository:
 /**TU_Bln361095dataCarrierNumNVMeSMARTattrIDs,
  * TU_Bln361095dataCarrierNumSATA_SMARTattrIDs */
 #include <hardware/dataCarrier/ATA3Std.h>

///this project's/repo's files:
 ///class TU_Bln361095::SMARTmon::SMARTuniqueID
 #include <attributes/SMARTUNIQUEID.HPP>

#ifdef __cplusplus
void readAndPrintRawValsForAllSuppSMARTattrIDs(
  TU_Bln361095hardwareDataCarrierUse(IDtyp) dataCarrierID,
  TU_Bln361095::SMARTmon::SMARTuniqueID & sMARTuniqueID,
  TU_Bln361095::SMARTmon::SMARTaccessTyp & sMARTaccess,
  TU_Bln361095::SMARTmon::suppSMART_IDsType & suppSMARTattrNamesAndIDs)
{
  TU_Bln361095::CPU::faststUint numDifferentSMARTattrIDs;
  const enum TU_Bln361095::hardware::bus::Type busType = sMARTuniqueID.
    getBusType();
  switch(busType)
  {
  case TU_Bln361095hardwareBusUse(NVMe):
    numDifferentSMARTattrIDs = TU_Bln361095dataCarrierNumNVMeSMARTattrIDs;
    break;
    //TU_Bln361095hardwareBusUse(SATA):
  default:///(S)ATA etc.
    numDifferentSMARTattrIDs = TU_Bln361095dataCarrierNumSATA_SMARTattrIDs;
    break;
  }
  TU_Bln361095::CPU::faststUint sMARTattrIDsToRead[
    TU_Bln361095dataCarrierNumSATA_SMARTattrIDs///Known at compile time
      //numDifferentSMARTattrIDs
    ];
  for (TU_Bln361095::CPU::faststUint arrIdx = 0; arrIdx <
    (numDifferentSMARTattrIDs - 1); arrIdx++)
    sMARTattrIDsToRead[arrIdx] = arrIdx + 1;
  sMARTattrIDsToRead[//TU_Bln361095dataCarrierNumSATA_SMARTattrIDs
    numDifferentSMARTattrIDs - 1] = 0;

  sMARTuniqueID.SetSMART_IDsToRead(suppSMARTattrNamesAndIDs, sMARTattrIDsToRead);

  double upTimeInS, upTimeInS2;
  TU_Bln361095OpSysUse(GetUptimeInS)(upTimeInS);
  TU_Bln361095SMARTmonNmSpc::SMARTuniqueIDandValues sMARTuniqueIDandVals(
    sMARTuniqueID);
  //TU_Bln361095SMARTmonSMARTaccessUse(retCodes)
  TU_Bln361095::SMARTmon::SMARTaccessBase::retCodes retCode = sMARTaccess.
    readSMARTforDevice(dataCarrierID, sMARTuniqueIDandVals, sMARTattrIDsToRead);
  TU_Bln361095OpSysUse(GetUptimeInS)(upTimeInS2);

  if (retCode == TU_Bln361095SMARTmonNmSpc::SMARTaccessBase::success)
  {
    std::cout << "Successfully read supported S.M.A.R.T. attributes' values:"
      << std::endl;
    SMARTattr::rawValTyp SMARTattrRawVal;
    sMARTuniqueIDandVals.m_SMARTattrs[0].getRawVal(SMARTattrRawVal);
    std::cout << SMARTattrRawVal;
  }

  std::cout.precision(3);///Number of digits after decimal point
  std::cout << std::fixed << "uptime before reading all supported S.M.A.R.T. "
    "attributes' values:" << upTimeInS << "s" << std::endl
    << "uptime after reading all supported S.M.A.R.T. attributes' "
    "values:" << upTimeInS2 << "s" << std::endl
    << "Reading all supported S.M.A.R.T. attributes took ca." <<
    (upTimeInS2 - upTimeInS) << "s" << std::endl;
}
#endif

#endif///include guard