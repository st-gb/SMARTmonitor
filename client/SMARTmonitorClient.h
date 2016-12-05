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
  
  enum state {connectedToService, unconnectedFromService};

  enum TransmissionError { numBytesToReceive, SMARTdata};
  int m_socketFileDesc;
  //TODO could use ByteArray datatype here
  void GetSMARTdataViaXML(uint8_t * SMARTvalues, unsigned numBytesToRead,
    /*std::set<SMARTuniqueIDandValues> & */ SMARTuniqueIDandValues &);
  virtual void ChangeState(enum state newState) { };
  fastestUnsignedDataType ConnectToServer(const char * hostName);
  fastestUnsignedDataType  GetSMARTvaluesFromServer(std::set<SMARTuniqueIDandValues> & );
  void HandleTransmissionError(enum TransmissionError transmissionError);
private:

};

#endif /* SMARTMONITORCLIENT_H */

