/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SMARTmonitorClient.h
 * Author: root
 *
 * Created on 20. November 2016, 17:43
 */

#ifndef SMARTMONITORCLIENT_H
#define SMARTMONITORCLIENT_H

#include "../SMARTmonitorBase.hpp"
#include <OperatingSystem/multithread/nativeThreadType.hpp>
#include <UserInterface/columnIndices.hpp> 

class SMARTmonitorClient 
  : public SMARTmonitorBase
{
public:
  SMARTmonitorClient();
  SMARTmonitorClient(const SMARTmonitorClient& orig);
  virtual ~SMARTmonitorClient();
#ifdef _DEBUG
  DWORD GetSMARTvalsAndUpd8UIthreadID = 0;
#endif
  static fastestUnsignedDataType s_maxNumCharsNeededForDisplay[];
  static fastestUnsignedDataType s_charPosOAttrNameBegin[ColumnIndices::beyondLast];
  static char * s_columnAttriuteNames [];
      
  static bool s_atLeast1CriticalNonNullValue;
  static nativeThread_type s_updateSMARTparameterValuesThread;
  static fastestUnsignedDataType s_updateUI;
  dataCarrierID2supportedSMARTattrMap_type
    dataCarrierIDandSMARTidsContainer;
  
  enum serverConnectionState {connectedToService, unconnectedFromService};
  enum transmission { successfull = 0, readLessBytesThanIntended, unsetTransmResult };
  enum TransmissionError { numBytesToReceive, SMARTdata, SMARTparameterValues};
  enum SMARTvalueRating{noCriticalValue,SMARTvalueOK,SMARTvalueWarning,unknown};
  
  int m_socketFileDesc;
  fastestUnsignedDataType m_serviceConnectionCountDownInSeconds;
  fastestUnsignedDataType m_serverConnectionState;
  
  void AfterGetSMARTvaluesLoop(int getSMARTvaluesResult);
  /** E.g. show a dialog that enables cancelling of connection in implementation. */
  virtual void BeforeConnectToServer()/**Implement for not to override by a 
    small test program*/{};
  virtual void GetTextFromUser(const char * label, std::string & ) { };
  ///Called after connection attempt to the server.(either from the thread that
  ///prepared the server connection or from another). So UI operations should 
  /// be ensured to run in UI thread via events etc..)
  ///Can be used to hide a "connect to server" window.
  virtual void AfterConnectToServer(int connectResult) { };
  void HandleConnectionError(const char * hostName);
  //TODO could use ByteArray datatype here
  void GetSMARTdataViaXML(uint8_t * SMARTvalues, unsigned numBytesToRead,
    /*std::set<SMARTuniqueIDandValues> & */ SMARTuniqueIDandValues &);
  virtual void ChangeState(enum serverConnectionState newState) { };
  void ConnectToServerAndGetSMARTvalues();
  void ConnectToServer();
  fastestUnsignedDataType ConnectToServer(const char * hostName, bool asyncConnect);
  /*fastestUnsignedDataType*/ void  GetSupportedSMARTattributesViaXML(
    uint8_t * xmlDataByteArray,
    fastestUnsignedDataType numBytesToRead,
    dataCarrierID2supportedSMARTattrMap_type & supportedSMARTattributess
    );
  void EndUpdateUIthread();
  fastestUnsignedDataType GetSupportedSMARTidsFromServer();
  fastestSignedDataType ReadNumFollowingBytes();
  fastestUnsignedDataType GetSMARTattrValsFromSrv(//std::set<SMARTuniqueIDandValues> & 
    );
  ///Should be called in user interface thread? (because it calls functions to 
  /// create the user interface)
  void GetSMARTvaluesAndUpdateUI();
  void HandleTransmissionError(enum TransmissionError transmissionError);
  
  const struct tm & GetLastSMARTvaluesUpdateTime() const {
    //TODO because "tm" is a struct with multiple fields/members: 
    // changes non-atomically in get SMART values thread.
    // ->possible inconsistency 
    return m_timeOfLastSMARTvaluesUpdate; }
  struct tm m_timeOfLastSMARTvaluesUpdate;
  
  void SetServiceAddress(const std::string & str) {
    m_stdstrServerAddress = str;
  }
  void SetSMARTattribIDandNameLabel();
  virtual void StartServiceConnectionCountDown(
    const fastestUnsignedDataType countDownInSeconds) {}
  /** Operations that only need to be done once after connection to the service
      is established. 
      "virtual" is needed in order to generate a table of virtual functions. */
  virtual void ReBuildUserInterface() { 
    //SetSMARTdriveID();
    SetSMARTattribIDandNameLabel();
  }
  inline void UpdateTimeOfSMARTvalueRetrieval(
    const SMARTuniqueID & sMARTuniqueID,
    const fastestUnsignedDataType SMARTattributeID,
    const long int timeStampOfRetrieval);
  /*virtual*/ void UpdateSMARTvaluesUI();
  virtual void SetAttribute(
    const SMARTuniqueID &, 
    fastestUnsignedDataType SMARTattributeID, /**Usually the line (number) */
    //TODO exchange enum with fastestUnsignedDataType for performance?
    const enum ColumnIndices::columnIndices &,/**Usually the column (number) */
    const std::string &,
    const enum SMARTvalueRating) {}
  virtual void ShowStateAccordingToSMARTvalues(bool ) { }
  std::string m_stdstrServerAddress;
protected:
private:
};

#endif /* SMARTMONITORCLIENT_H */

