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

class SMARTmonitorClient 
  : public SMARTmonitorBase
{
public:
  SMARTmonitorClient();
  SMARTmonitorClient(const SMARTmonitorClient& orig);
  virtual ~SMARTmonitorClient();
  
  enum columnIndices { COL_IDX_SMART_ID = 0 , COL_IDX_SMARTparameterName,
    COL_IDX_rawValue, COL_IDX_humanReadableRawValue, COL_IDX_lastUpdate, COL_IDX_beyondLast };
  static fastestUnsignedDataType s_maxNumCharsNeededForDisplay[];
  static fastestUnsignedDataType s_charPosOAttrNameBegin[COL_IDX_beyondLast];
  static char * s_columnAttriuteNames [];
      
  static bool s_atLeast1CriticalNonNullValue;
  static nativeThread_type s_updateSMARTparameterValuesThread;
  static fastestUnsignedDataType s_updateUI;
  typedef std::set<fastestSignedDataType> supportedSMARTattributeIDs_type;
  typedef std::map<SMARTuniqueID,supportedSMARTattributeIDs_type >
    dataCarrierID2supportedSMARTattributesMap_type;
  dataCarrierID2supportedSMARTattributesMap_type
    dataCarrierIDandSMARTidsContainer;
  
  enum state {connectedToService, unconnectedFromService};
  enum transmission { successfull = 0, readLessBytesThanIntended, unsetTransmResult };
  enum TransmissionError { numBytesToReceive, SMARTdata};
  enum ConnectToServerResult{ connectionToServiceSucceeded = 0, 
    errorOpeningSocket, getHostByNameFailed, errorConnectingToService };
  
  int m_socketFileDesc;
  
  virtual void GetTextFromUser(const char * label, std::string & ) { };
  void HandleConnectionError(const char * hostName);
  //TODO could use ByteArray datatype here
  void GetSMARTdataViaXML(uint8_t * SMARTvalues, unsigned numBytesToRead,
    /*std::set<SMARTuniqueIDandValues> & */ SMARTuniqueIDandValues &);
  virtual void ChangeState(enum state newState) { };
  void ConnectToServer();
  fastestUnsignedDataType ConnectToServer(const char * hostName);
  /*fastestUnsignedDataType*/ void  GetSupportedSMARTattributesViaXML(
    uint8_t * xmlDataByteArray,
    fastestUnsignedDataType numBytesToRead,
    dataCarrierID2supportedSMARTattributesMap_type & supportedSMARTattributess
    );
  void EndUpdateUIthread();
  fastestUnsignedDataType GetSupportedSMARTidsFromServer();
  fastestSignedDataType ReadNumFollowingBytes();
  fastestUnsignedDataType GetSMARTvaluesFromServer(//std::set<SMARTuniqueIDandValues> & 
    );
  void HandleTransmissionError(enum TransmissionError transmissionError);
  
  const struct tm & GetLastSMARTvaluesUpdateTime() const { 
    return m_timeOfLastSMARTvaluesUpdate; }
  struct tm m_timeOfLastSMARTvaluesUpdate;
  
  void SetServiceAddress(const std::string & str) {
    m_stdstrServerAddress = str;
  }
  void SetSMARTattribIDandNameLabel();
  /** Operations that only need to be done once after connection to the service
      is established. 
      "virtual" is needed in order to generate a table of virtual functions. */
  virtual void ReBuildUserInterface() { }
  inline void UpdateTimeOfSMARTvalueRetrieval(
    const SMARTuniqueID & sMARTuniqueID,
    const fastestUnsignedDataType SMARTattributeID,
    const long int timeStampOfRetrieval);
  /*virtual*/ void UpdateSMARTvaluesUI();
  virtual void SetAttribute(
    const SMARTuniqueID &, 
    fastestUnsignedDataType SMARTattributeID,
    const enum columnIndices &,
    const std::string &) {}
  virtual void ShowStateAccordingToSMARTvalues(bool ) { }
  std::string m_stdstrServerAddress;
protected:
private:
};

#endif /* SMARTMONITORCLIENT_H */

