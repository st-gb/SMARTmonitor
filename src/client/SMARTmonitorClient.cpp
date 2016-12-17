/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SMARTmonitorClient.cpp
 * Author: root
 * 
 * Created on 20. November 2016, 17:43
 */

#include "SMARTmonitorClient.h"
#include <sys/socket.h> //socket(...))
#include <netinet/in.h> //sockaddr_in
#include <netdb.h> //gethostbyname(...)
#include <stdint.h> //uint8_t
#include "tinyxml2.h" //
#include "hardware/CPU/atomic/AtomicExchange.h"
#include <OperatingSystem/time/GetCurrentTime.hpp>

SMARTmonitorClient::SMARTmonitorClient() {
}

SMARTmonitorClient::SMARTmonitorClient(const SMARTmonitorClient& orig) {
}

SMARTmonitorClient::~SMARTmonitorClient() {
}

fastestUnsignedDataType SMARTmonitorClient::ConnectToServer(const char * hostName) {
  //from http://www.linuxhowtos.org/data/6/client.c
  int portNumber = m_socketPortNumber, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  
  m_socketFileDesc = socket(AF_INET, SOCK_STREAM,
    /** socket.h : "If PROTOCOL is zero, one is chosen automatically." */
    0 );
  if (m_socketFileDesc < 0)
  {
    LOGN_ERROR("ERROR opening socket");
    return 1;
  }
  LOGN("successfully opened socket");
  server = gethostbyname(hostName);
  if (server == NULL) {
    LOGN_ERROR("host " << hostName << " not in database");
    return 2;
  }
  LOGN("got host name for \"" << hostName << "\":" << server->h_name)
  bzero( (char *) & serv_addr, sizeof(serv_addr) );
  serv_addr.sin_family = AF_INET;
  bcopy( (char *) server->h_addr,
    (char *) & serv_addr.sin_addr.s_addr,
    server->h_length);
  serv_addr.sin_port = htons(portNumber);
  const int connectResult = connect(m_socketFileDesc,(struct sockaddr *) & serv_addr,
    sizeof(serv_addr) );
  if( connectResult < 0)
  {
    //TODO: show error via user interface
    //Message("")
    LOGN_ERROR("error connecting to SMART values service:" << strerror( errno ));
    std::ostringstream oss;
    oss << "error connecting to server:\n";
    //see http://man7.org/linux/man-pages/man2/connect.2.html
    switch(errno )
    {
      //see http://man7.org/linux/man-pages/man2/connect.2.html
      case ECONNREFUSED :
        oss << "No process listening on the remote address \"" << hostName 
          << "\", port:" << portNumber;
        ShowMessage(oss.str().c_str() );
        break;
    }
    return 3;
  }
  else
    LOGN("successfully connected")
  //bzero(buffer,256);
  return 0;
}

void GetSMARTuniqueID(
  tinyxml2::XMLElement * p_tinyxml2XMLelement, 
  SMARTuniqueID & sMARTuniqueID)
{
  //tinyxml2::XMLAttribute * p_tinyxml2XMLattribute = 
  const char * modelString = p_tinyxml2XMLelement->Attribute("model", NULL);
  const char * firmwareString = p_tinyxml2XMLelement->Attribute("firmware", NULL);
  const char * serialNumberString = p_tinyxml2XMLelement->Attribute("serial_number", NULL);
//      if( ! p_tinyxml2XMLattribute)
//      {
//        LOGN_ERROR("Failed to get XML model attribute")
//        return;
//      }
  if( ! modelString ) {
    LOGN_ERROR("failed to get model atribute value")
    return;
  }
  if( ! firmwareString ) {
    LOGN_ERROR("failed to get firmware atribute value")
    return;
  }
  if( ! serialNumberString ) {
    LOGN_ERROR("failed to get serial number atribute value")
    return;
  }
  sMARTuniqueID.SetModelName(modelString);
  sMARTuniqueID.SetFirmwareName(firmwareString);
  sMARTuniqueID.SetSerialNumber(serialNumberString);
  LOGN("data carrier ID:" << sMARTuniqueID)
}

void HandleSingleSMARTentry(
  tinyxml2::XMLElement * p_SMARTelement,
  SMARTuniqueIDandValues & sMARTuniqueIDandValues)
{
  const int SMARTattributeID = p_SMARTelement->IntAttribute("ID", 0);
  if( SMARTattributeID >= NUM_DIFFERENT_SMART_ENTRIES)
  {
    LOGN_ERROR("SMART ID too high:" << SMARTattributeID)
    return;
  }
  if( SMARTattributeID < 0)
  {
    LOGN_ERROR("SMART ID is negative:" << SMARTattributeID)
    return;
  }
  const int64_t SMART_raw_value = p_SMARTelement->Int64Attribute("raw_value", 0);
  SMARTvalue & sMARTvalue = sMARTuniqueIDandValues.m_SMARTvalues[SMARTattributeID];
  if( SMART_raw_value < 0)
  {
    LOGN_ERROR("SMART raw value for attrib ID " << SMARTattributeID 
      << " is negative:" << SMART_raw_value)
    //sMARTuniqueIDandValues.m_successfullyReadSMARTrawValue[SMARTattributeID] = 0;
    AtomicExchange(& sMARTvalue.m_successfullyReadSMARTrawValue, 0);
    return;
  }
  AtomicExchange(& sMARTvalue.m_successfullyReadSMARTrawValue, 1);
  const float timeInS = p_SMARTelement->FloatAttribute("time_in_s", 0.0f);
  
  AtomicExchange(& sMARTvalue.m_timeStampOfRetrieval, timeInS * 1000.0f );
  
  LOGN("adding SMART raw value " << SMART_raw_value << " (time:" << timeInS 
    << ") to SMART ID " << SMARTattributeID)
  sMARTvalue.SetRawValue(SMART_raw_value);
}

void GetSMARTrawValues(
  tinyxml2::XMLElement * p_tinyxml2XMLelement, 
  SMARTuniqueIDandValues & sMARTuniqueIDandValues)
{
  tinyxml2::XMLElement * p_SMARTelement = p_tinyxml2XMLelement->FirstChildElement("SMART");
  if( ! p_SMARTelement)
  {
    LOGN_ERROR("no SMART XML element name below the root element")
    return;
  }
  /** from http://stackoverflow.com/questions/13919817/sscanf-and-locales-how-does-one-really-parse-things-like-3-14 :
   *  For "FloatAttribute" which uses "sscanf" to use "." as a decimal point. */
  setlocale(LC_NUMERIC, "C");
  HandleSingleSMARTentry(p_SMARTelement, sMARTuniqueIDandValues);
  
  p_SMARTelement = p_SMARTelement->NextSiblingElement("SMART");
  while(p_SMARTelement)
  {
    HandleSingleSMARTentry(p_SMARTelement, sMARTuniqueIDandValues);
    p_SMARTelement = p_SMARTelement->NextSiblingElement("SMART");
  }
  LOGN("end")
}

void SMARTmonitorClient::GetSMARTdataViaXML(
  uint8_t * SMARTvalues, unsigned numBytesToRead,
  //std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValuesContainter
  SMARTuniqueIDandValues & sMARTuniqueIDandValues)
{
  std::string xmlData( (const char *) SMARTvalues);
  LOGN("XML data:" << xmlData)
  tinyxml2::XMLDocument tinyXML2Doc;
  const tinyxml2::XMLError xmlErr = tinyXML2Doc.Parse(xmlData.c_str(), numBytesToRead);
  LOGN("result of parsing XML data:" << xmlErr)
  if( xmlErr != tinyxml2::XML_SUCCESS)
  {
    switch(xmlErr)
    {
      case tinyxml2::XML_ERROR_PARSING_ELEMENT :
        LOGN_ERROR("XML parsing error")
        break;
    }
    return;
  }
  tinyxml2::XMLElement * p_tinyxml2XMLelement = tinyXML2Doc.RootElement();
  if( ! p_tinyxml2XMLelement )
  {
    LOGN_ERROR("Failed to get XML root element")
    return;
  }
  LOGN("got XML root element")
  SMARTuniqueID sMARTuniqueID;
  GetSMARTuniqueID(p_tinyxml2XMLelement,sMARTuniqueID);
  //SMARTuniqueIDandValues sMARTuniqueIDandValues(sMARTuniqueID);
  sMARTuniqueIDandValues.SetDataCarrierID(sMARTuniqueID);
  //TODO
  GetSMARTrawValues(p_tinyxml2XMLelement, sMARTuniqueIDandValues);
//  SMARTuniqueIDandValues.m_SMARTrawValues;
  //sMARTuniqueIDandValuesContainter.insert(sMARTuniqueIDandValues);
}

void SMARTmonitorClient::HandleTransmissionError( 
  enum SMARTmonitorClient::TransmissionError transmissionError)
{
  std::ostringstream stdoss;
  switch(transmissionError)
  {
    case numBytesToReceive :
      stdoss << "ERROR reading the number of following bytes from socket";
      break;
    case SMARTdata :
      stdoss << "ERROR reading SMART data from socket";
      break;
  }
  if( errno != 0)
    stdoss << strerror(errno);
  switch(errno)
  {
    case EPIPE :
      //see https://linux.die.net/man/2/write
      stdoss << "The reading end of socket is closed.";
      break;
  }
  LOGN_ERROR(stdoss.str() );
  ShowMessage(stdoss.str().c_str());
  //TODO set connection status of the user interface to "network errors"/"unconnected"
  ChangeState(unconnectedFromService);
  //TODO close socket, set status (also in UI) to unconnected
}

fastestUnsignedDataType SMARTmonitorClient::GetSMARTvaluesFromServer(
  std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValuesContainer)
{
  LOGN("begin")
  sMARTuniqueIDandValuesContainer.clear();
  //int lineNumber = 0;
  //uint8_t array[5];
  uint16_t numBytesToRead;
  
  //int numberOfDataSets;
  //do
  //{
  LOGN("reading 2 bytes from socket #" << m_socketFileDesc)
    int numBytesRead = read(m_socketFileDesc, & numBytesToRead, 2);
    if( numBytesRead < 2 ) {
      HandleTransmissionError(numBytesToReceive);
      return 1;
    }
    numBytesToRead = ntohs(numBytesToRead);
    LOGN("# bytes to read:" << numBytesToRead)
            
    const fastestUnsignedDataType numBytesToAllocate = numBytesToRead + 1;
    uint8_t * SMARTvalues = new uint8_t[numBytesToAllocate];
    if( SMARTvalues)
    {
      numBytesRead = read(m_socketFileDesc, SMARTvalues, numBytesToRead);
      if (numBytesRead < numBytesToRead) {
        HandleTransmissionError(SMARTdata);
        LOGN_ERROR("read less bytes (" << numBytesRead << ") than expected (" 
          << numBytesToRead << ")");
        return 2; //TODO provide error handling (show message to user etc.)
      }
      SMARTvalues[numBytesToRead] = '\0';
      SMARTuniqueIDandValues sMARTuniqueIDandValues;
      GetSMARTdataViaXML(SMARTvalues, numBytesToRead, sMARTuniqueIDandValues);
      LOGN("SMART unique ID and values object " << & sMARTuniqueIDandValues )
      //sMARTuniqueIDandValuesContainer.f
      std::pair<std::set<SMARTuniqueIDandValues>::iterator, bool> insert = 
        sMARTuniqueIDandValuesContainer.insert(sMARTuniqueIDandValues);
      LOGN("insered object into container?:" << insert.second);
      if(insert.second)
      {
        LOGN("SMART unique ID and values object in container:" << &(*insert.first) )
      }
      //RebuildGUI();
      OperatingSystem::GetCurrentTime(m_timeOfLastSMARTvaluesUpdate);
      //LOGN("got model attribute")
      //const char * modelString = p_tinyxml2XMLattribute->Value();
      //LOGN("model: " << modelString)
      delete [] SMARTvalues;
    }
    else
    {
      LOGN_ERROR("Failed to allocate " << numBytesToAllocate << " bytes")
    }
    
//    char * str = (char *) "<data_carrier></data_carrier";
//    numberOfDataSets = numBytesToRead / 5;
//    
//    if (numBytesRead < 5) {
//      LOGN_ERROR("ERROR reading from socket");
//      return;
//    }

    //long currentSMARTrawValue = ntohl( * (long*) & array[1]);
    /*AtomicExchange( (long int *) & m_arSMARTrawValue[lineNumber] //long * Target */
      //,//* (long int *) //SMARTattributesToObserveIter->pretty_value /*raw*/ /*long val*/
      /*currentSMARTrawValue);
  //          AtomicExchange( (long int *) & m_arSMART_ID[lineNumber]
    LOGN(m_arSMARTrawValue[lineNumber] << " " << currentSMARTrawValue)
    AtomicExchange(
      & m_arTickCountOfLastQueryInMilliSeconds[lineNumber]
      , GetTickCount() /*long val*/ //);
  //}while(1);
  LOGN("end")
  return 0;
}
