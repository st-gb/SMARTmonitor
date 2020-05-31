#include <iostream>///std::cout

#include <OperatingSystem/time/GetUpTime.h>///OperatingSystem::GetUptimeInS(...)
#include <SMARTaccType.hpp>///typdef SMARTaccess_type
#include "cxxopts/handCmdLineOpts.hpp"///cxxopts::HandleCmdLineOpts(...)
//#include "handCmdLineOpts.cpp"

int main(int argCount, char * argVec [])
{
  std::string dvcPath = "/dev/sda";
  cxxopts::HandleCmdLineOpts(argCount, argVec, dvcPath);

  std::set<SMARTuniqueIDandValues> sMARTuniqueIDsAndVals;
  SMARTaccess_type sMARTaccess(sMARTuniqueIDsAndVals);
  
  SMARTuniqueID sMARTuniqueID;
  sMARTaccess.fill(dvcPath.c_str(), sMARTuniqueID);
  std::set<SMARTattributeNameAndID> suppSMARTattrNamesAndIDs;
  if(sMARTaccess.GetSupportedSMART_IDs(dvcPath.c_str(),suppSMARTattrNamesAndIDs)
    != 0){
    std::cerr << "Failed to get supported S.M.A.R:T. IDs for " << dvcPath <<
      std::endl;
    return 1;
  }
  std::cout << "# S.M.A.R.T. attributes supported by " << dvcPath << " (" <<
    sMARTuniqueID << "):" << suppSMARTattrNamesAndIDs.size() << std::endl;
  
  fastestUnsignedDataType sMARTattrIDsToRead[NUM_DIFFERENT_SMART_ENTRIES];
  for(fastestUnsignedDataType arrIdx = 0; arrIdx < 
    (NUM_DIFFERENT_SMART_ENTRIES - 1); arrIdx++)
    sMARTattrIDsToRead[arrIdx] = arrIdx + 1;
  sMARTattrIDsToRead[NUM_DIFFERENT_SMART_ENTRIES - 1] = 0;
  
  sMARTuniqueID.SetSMART_IDsToRead(suppSMARTattrNamesAndIDs, sMARTattrIDsToRead);

  double upTimeInS, upTimeInS2;
  OperatingSystem::GetUptimeInS(upTimeInS);
  SMARTaccessBase::retCodes retCode = sMARTaccess.readSMARTforDevice(
    dvcPath.c_str(), sMARTuniqueID);
  OperatingSystem::GetUptimeInS(upTimeInS2);
  
  std::cout.precision(3);///# digits after decimal point
  std::cout << std::fixed << "uptime before reading all supported S.M.A.R.T. "
    "attributes' values:" << upTimeInS << "s" << std::endl
    << "uptime after reading all supported S.M.A.R.T. attributes' "
    "values:" << upTimeInS2 << "s" << std::endl
    << "Reading all supported S.M.A.R.T. attributes took ca." <<
    (upTimeInS2 - upTimeInS) << "s" << std::endl;
  return 0;
}
