/* File:   main.cpp
 * Author: Stefan Gebauer, M.Sc.Comp. Sc.
 * Created on 28. März 2020, 11:07 */
#include <client/SMARTmonitorClient.h>///base class SMARTmonitorClient
#include <iostream>///std::cout

class MinSMARTmonClientDerived: public SMARTmonitorClient
{
  void AfterConnectToServer(int connectResult){
    if(connectResult == -1 /*cnnctFailed*/)
      std::cout << "failed to connect to server." << std::endl;
  }
  void BeforeWait(){UpdateSMARTvaluesUI(); }
  void SetAttribute(
    const SMARTuniqueID &, 
    fastestUnsignedDataType SMARTattributeID, /**Usually the line (number) */
    //TODO exchange enum with fastestUnsignedDataType for performance?
    const enum ColumnIndices::columnIndices & colIdx,/**Usually the column (#)*/
    const std::string & str,
    const enum SMARTvalueRating){
   switch(colIdx){
//     case ColumnIndices::SMART_ID:
      case ColumnIndices::humanReadableRawValue:
      std::cout << "SMART attrib ID:" << SMARTattributeID;//break;
      std::cout << " value:" << str /*<< std::endl*/;break;
     case ColumnIndices::lastUpdate:
      std::cout << " last upd8:" << str << std::endl;break;
    }
  }
};

int main (int argc, char** argv)
{
  MinSMARTmonClientDerived SMARTmonClient;
  SMARTmonClient.SetCommandLineArgs(argc, argv);
  SMARTmonClient.OutputUsage();
  SMARTmonClient.m_stdstrServiceHostName = "localhost";
  SMARTmonClient.InitializeSMART();
  SMARTmonClient.CnnctToSrvAndGetSMARTvals();
  return 0;
}
