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

class SMARTmonitorClient 
  : public SMARTmonitorBase
{
public:
  SMARTmonitorClient();
  SMARTmonitorClient(const SMARTmonitorClient& orig);
  virtual ~SMARTmonitorClient();
  
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
  void HandleConnectionError(const char * hostName);
  //TODO could use ByteArray datatype here
  void GetSMARTdataViaXML(uint8_t * SMARTvalues, unsigned numBytesToRead,
    /*std::set<SMARTuniqueIDandValues> & */ SMARTuniqueIDandValues &);
  virtual void ChangeState(enum state newState) { };
  fastestUnsignedDataType ConnectToServer(const char * hostName);
  /*fastestUnsignedDataType*/ void  GetSupportedSMARTattributesViaXML(
    uint8_t * xmlDataByteArray,
    fastestUnsignedDataType numBytesToRead,
    dataCarrierID2supportedSMARTattributesMap_type & supportedSMARTattributess
    );
  fastestUnsignedDataType GetSupportedSMARTidsFromServer();
  fastestSignedDataType ReadNumFollowingBytes();
  fastestUnsignedDataType GetSMARTvaluesFromServer(std::set<SMARTuniqueIDandValues> & );
  void HandleTransmissionError(enum TransmissionError transmissionError);
  
  const struct tm & GetLastSMARTvaluesUpdateTime() const { 
    return m_timeOfLastSMARTvaluesUpdate; }
  struct tm m_timeOfLastSMARTvaluesUpdate;
private:

};

#endif /* SMARTMONITORCLIENT_H */

