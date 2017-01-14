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
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...))
#include <OperatingSystem/Linux/EnglishMessageFromErrorCode/EnglishMessageFromErrorCode.h>
#include "OperatingSystem/GetLastErrorCode.hpp" //OperatingSystem::GetLastErrorCode()
#include "hardware/CPU/atomic/AtomicExchange.h"
#include "Controller/character_string/ConvertStdStringToTypename.hpp"
#include <OperatingSystem/time/GetCurrentTime.hpp>

/** Static/class variable defintion: */
fastestUnsignedDataType SMARTmonitorClient::s_updateUI = 1;
nativeThread_type SMARTmonitorClient::s_updateSMARTparameterValuesThread;
bool SMARTmonitorClient::s_atLeast1CriticalNonNullValue;

SMARTmonitorClient::SMARTmonitorClient() {
}

SMARTmonitorClient::SMARTmonitorClient(const SMARTmonitorClient& orig) {
}

SMARTmonitorClient::~SMARTmonitorClient() {
}

void SMARTmonitorClient::HandleConnectionError(const char * hostName)
{
  //TODO: show error via user interface
  //Message("")
  std::ostringstream oss;
  oss << "error connecting to S.M.A.R.T. values service:\n";
  //TODO the following is Linux-specific and should be OS-independent
  //see http://man7.org/linux/man-pages/man2/connect.2.html
  switch(errno )
  {
    //see http://man7.org/linux/man-pages/man2/connect.2.html
    case ECONNREFUSED :
      oss << "No process listening on the remote address \"" << hostName 
        << "\", port:" << m_socketPortNumber;
      break;
    default :
    {
      int errorCode = OperatingSystem::GetLastErrorCode();
      oss << OperatingSystem::EnglishMessageFromErrorCode( errorCode );
    }
    break;
  }
  ShowMessage(oss.str().c_str() );
  LOGN_ERROR(oss.str() )
}

fastestUnsignedDataType SMARTmonitorClient::ConnectToServer(const char * hostName) {
  //from http://www.linuxhowtos.org/data/6/client.c
  int portNumber = m_socketPortNumber, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  
  /** http://pubs.opengroup.org/onlinepubs/009695399/functions/socket.html :
   * "Upon successful completion, socket() shall return a non-negative integer, 
   * the socket file descriptor. Otherwise, a value of -1 shall be returned 
   * and errno set to indicate the error." */
  m_socketFileDesc = socket(AF_INET, SOCK_STREAM,
    /** socket.h : "If PROTOCOL is zero, one is chosen automatically." */
    0 );
  if (m_socketFileDesc < 0)
  {
    OperatingSystem::GetLastErrorCode();
    LOGN_ERROR("ERROR opening socket: error code " << errno 
      << " error message:" << OperatingSystem::EnglishMessageFromErrorCode(errno) )
    return errorOpeningSocket;
  }
  LOGN("successfully opened socket");
  /** http://pubs.opengroup.org/onlinepubs/009695399/functions/gethostbyaddr.html :
   * "Upon successful completion, these functions shall return a pointer to a 
   * hostent structure if the requested entry was found, and a null pointer if 
   * the end of the database was reached or the requested entry was not found.
   * 
   * Upon unsuccessful completion, gethostbyaddr() and gethostbyname() shall 
   * set h_errno to indicate the error." */
  server = gethostbyname(hostName);
  if (server == NULL) {
    LOGN_ERROR("host " << hostName << " not in database;error code:" << h_errno)
    return getHostByNameFailed;
  }
  LOGN("got host name for \"" << hostName << "\":" << server->h_name)
  bzero( (char *) & serv_addr, sizeof(serv_addr) );
  serv_addr.sin_family = AF_INET;
  bcopy( (char *) server->h_addr,
    (char *) & serv_addr.sin_addr.s_addr,
    server->h_length);
  serv_addr.sin_port = htons(portNumber);
  /** http://pubs.opengroup.org/onlinepubs/9699919799/functions/connect.html :
   * "Upon successful completion, connect() shall return 0; otherwise, -1 shall
   *  be returned and errno set to indicate the error." */
  const int connectResult = connect(m_socketFileDesc,(struct sockaddr *) & serv_addr,
    sizeof(serv_addr) );
  if( connectResult < 0)
  {
    HandleConnectionError(hostName);
    return errorConnectingToService;
  }
  else
    LOGN("successfully connected")
  //bzero(buffer,256);
  return connectedToService;
}

void SMARTmonitorClient::EndUpdateUIthread()
{
  if( s_updateSMARTparameterValuesThread.IsRunning() )
  {
    /** Inform the SMART values update thread about we're going to exit,*/
    //m_wxCloseMutex.TryLock();
    LOGN("disabling update UI (ends update UI thread)");
    AtomicExchange( (long *) & s_updateUI, 0);
    
    LOGN("waiting for close event");
    /** http://docs.wxwidgets.org/trunk/classwx_condition.html : 
      "Wait() atomically unlocks the mutex which allows the thread to continue "
      "and starts waiting */
    //m_p_wxCloseCondition->Wait();
    
    s_updateSMARTparameterValuesThread.WaitForTermination();
  }
}

void SMARTmonitorClient::ConnectToServer() {
  /** Terminate a possibly running update UI thread (e.g.. already connected
   *   to a service). */
  EndUpdateUIthread();
  //  wxString wxstrServerAddress;
  std::string stdstrServerAddress;
  GetTextFromUser("input SMART values server address", stdstrServerAddress);
//  const wxString wxstrServerAddress = wxWidgets::GetwxString_Inline(stdstrServerAddress);

  SetServiceAddress(stdstrServerAddress);
  const fastestUnsignedDataType res = ConnectToServer(stdstrServerAddress.c_str());
  if (res == 0) {
    ChangeState(connectedToService);
    //SMARTaccess_type & sMARTaccess = m_SMARTaccess.;
    std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValues = mp_SMARTaccess->
      GetSMARTuniqueIDandValues();
    /*const int res =*/ GetSupportedSMARTidsFromServer();
    LOGN("SMART unique ID and values container:" << &sMARTuniqueIDandValues)
    /** Get # of attributes to in order build the user interface (write 
     *  attribute ID an name into the table--creating the UI needs to be done 
     *  only once because the attribute IDs received usually do not change).*/
    const int getSMARTvaluesResult = GetSMARTvaluesFromServer(
    sMARTuniqueIDandValues);
    if (getSMARTvaluesResult == 0) {
      SetSMARTattributesToObserve(sMARTuniqueIDandValues);
//      m_p_ConnectAndDisconnectButton->SetLabel(wxT("disconnect"));
      ReBuildUserInterface();
      UpdateSMARTvaluesUI();
      ShowStateAccordingToSMARTvalues(s_atLeast1CriticalNonNullValue);
      StartAsyncUpdateThread();
    }
  } else {
    //wxGetApp().ShowMessage("");
  }
}

bool GetSMARTuniqueID(
  tinyxml2::XMLElement * p_tinyxml2XMLelement, 
  SMARTuniqueID & sMARTuniqueID)
{
  //tinyxml2::XMLAttribute * p_tinyxml2XMLattribute = 
  const char * modelString = p_tinyxml2XMLelement->Attribute("model", 
    /** Value: specify '0' in order to retrieve the value */ NULL);
  const char * firmwareString = p_tinyxml2XMLelement->Attribute("firmware", 
    /** Value: specify '0' in order to retrieve the value */ NULL);
  const char * serialNumberString = p_tinyxml2XMLelement->Attribute(
    "serial_number", 
    /** Value: specify '0' in order to retrieve the value */ NULL);
//      if( ! p_tinyxml2XMLattribute)
//      {
//        LOGN_ERROR("Failed to get XML model attribute")
//        return;
//      }
  if( ! modelString ) {
    LOGN_ERROR("failed to get model atribute value")
    return false;
  }
  if( ! firmwareString ) {
    LOGN_ERROR("failed to get firmware atribute value")
    return false;
  }
  if( ! serialNumberString ) {
    LOGN_ERROR("failed to get serial number atribute value")
    return false;
  }
  sMARTuniqueID.SetModelName(modelString);
  sMARTuniqueID.SetFirmwareName(firmwareString);
  sMARTuniqueID.SetSerialNumber(serialNumberString);
  LOGN("data carrier ID:" << sMARTuniqueID)
  return true;
}

int CheckSMARTidRange(const int SMARTattributeID)
{
  if( SMARTattributeID >= NUM_DIFFERENT_SMART_ENTRIES)
  {
    LOGN_ERROR("SMART ID too high:" << SMARTattributeID)
    return 1;
  }
  if( SMARTattributeID < 0)
  {
    LOGN_ERROR("SMART ID is negative:" << SMARTattributeID)
    return -1;
  }
  return 0;
}

void HandleSingleSMARTentry(
  tinyxml2::XMLElement * p_SMARTelement,
  SMARTuniqueIDandValues & sMARTuniqueIDandValues)
{
  const int SMARTattributeID = p_SMARTelement->IntAttribute("ID", 0);
  if( CheckSMARTidRange(SMARTattributeID) != 0)
  {
    LOGN_ERROR("SMART ID is not in range 0..255->not processing this SMART entry")
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
  tinyxml2::XMLElement * p_SMARTelement = p_tinyxml2XMLelement->
    FirstChildElement("SMART");
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

void HandleXMLresult(const tinyxml2::XMLError XMLparsingResult)
{
  LOGN("result of parsing XML data:" << XMLparsingResult)
  if( XMLparsingResult != tinyxml2::XML_SUCCESS)
  {
    LOGN_ERROR("XML error occured")
    switch(XMLparsingResult)
    {
      case tinyxml2::XML_ERROR_PARSING_ELEMENT :
        LOGN_ERROR("XML parsing error")
        break;
    }
    return;
  }
}

void ConvertStringToInt(
  const char * const p_BeginOfNumber, 
  SMARTmonitorClient::supportedSMARTattributeIDs_type & supportedSMARTattributeIDs )
{
  fastestUnsignedDataType number;
  //std::string std_strConvertToNumber(p_BeginOfNumber);
  
  if( ConvertCharStringToTypename(number, p_BeginOfNumber) )
  {
    if( CheckSMARTidRange(number) == 0 )
    {
      LOGN_DEBUG( "string:" << p_BeginOfNumber << " as int:" << number )
      supportedSMARTattributeIDs.insert( number );
    }
    else
      LOGN_ERROR("SMART ID is not in range 0..255->not processing this SMART entry")
  }
  else
    LOGN_ERROR("error converting " << p_BeginOfNumber << " to a number")
}

void SMARTmonitorClient::GetSupportedSMARTattributesViaXML(
  uint8_t * xmlDataByteArray,
  fastestUnsignedDataType numBytesToRead,
  //std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValuesContainter
  dataCarrierID2supportedSMARTattributesMap_type & 
    dataCarrierID2supportedSMARTattributess)
{
  std::string std_strXMLdata( (const char *) xmlDataByteArray);
  LOGN("XML data:" << std_strXMLdata)
  //TODO source out into tinyxml2.cpp because this code is specific to the XML lib
  tinyxml2::XMLDocument tinyXML2Doc;
  const tinyxml2::XMLError XMLparsingResult = tinyXML2Doc.Parse(
    std_strXMLdata.c_str(), 
    numBytesToRead);
  HandleXMLresult(XMLparsingResult);
  if( XMLparsingResult != tinyxml2::XML_SUCCESS )
  {
    ShowMessage("error parsing the XML data->no further processing->exit");
    LOGN_ERROR("error parsing the XML data->no further processing->exit")
    return;
  }
  tinyxml2::XMLElement * p_tinyxml2XMLelement = tinyXML2Doc.RootElement();
  if( ! p_tinyxml2XMLelement )
  {
    LOGN_ERROR("Failed to get XML root element")
    return;
  }
  LOGN_DEBUG("got XML root element")
  SMARTuniqueID sMARTuniqueID;
  if( GetSMARTuniqueID(p_tinyxml2XMLelement, sMARTuniqueID) )
  {
    p_tinyxml2XMLelement = p_tinyxml2XMLelement->FirstChildElement("supportedSMART_IDs");
    if( ! p_tinyxml2XMLelement )
    {
      LOGN_ERROR("no \"supportedSMART_IDs\" child element in XML data??")
      return;
    }
    const char * supportedSMARTattributeIDsString = p_tinyxml2XMLelement->GetText();
//      "supportedSMART_IDs", NULL);
    if( supportedSMARTattributeIDsString == NULL )
    {
      LOGN_ERROR("no \"supportedSMART_IDs\" attribute in XML data??")
      return;
    }
    LOGN_DEBUG( "text inside \"supportedSMART_IDs\":" << 
      supportedSMARTattributeIDsString )
    supportedSMARTattributeIDs_type supportedSMARTattributeIDs;
    
    const char * p_lastComma = (char * ) supportedSMARTattributeIDsString;
    char * p_currentChar=(char * )supportedSMARTattributeIDsString;
    for( ; *p_currentChar != '\0' ; ++ p_currentChar)
    {
      if( * p_currentChar == ',')
      {
        * p_currentChar = '\0';
        ConvertStringToInt(p_lastComma, supportedSMARTattributeIDs);
        p_lastComma = p_currentChar + 1;
      }
      //TODO
//      GetSupportedSMARTattributes(p_tinyxml2XMLelement, supportedSMARTattributes);
    }
    if( p_lastComma < p_currentChar ) /** Process last attribute ID */
      ConvertStringToInt(p_lastComma, supportedSMARTattributeIDs);
      
    dataCarrierID2supportedSMARTattributess.insert( std::make_pair(sMARTuniqueID, 
      supportedSMARTattributeIDs) );
  }
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

fastestSignedDataType SMARTmonitorClient::ReadNumFollowingBytes()
{
  LOGN("reading 2 bytes from socket #" << m_socketFileDesc)
  uint16_t numBytesToRead;
  int numBytesRead = read(m_socketFileDesc, & numBytesToRead, 2);
  if( numBytesRead < 2 ) {
    HandleTransmissionError(numBytesToReceive);
    return 1;
  }
  numBytesToRead = ntohs(numBytesToRead);
  LOGN_DEBUG("# bytes to read:" << numBytesToRead)
  return numBytesToRead;
}

fastestUnsignedDataType SMARTmonitorClient::GetSupportedSMARTidsFromServer()
{
  dataCarrierIDandSMARTidsContainer.clear();
  fastestSignedDataType numBytesToRead = ReadNumFollowingBytes();
  LOGN_DEBUG("# of following bytes: " << numBytesToRead );
  
  const fastestUnsignedDataType numBytesToAllocate = numBytesToRead + 1;
  uint8_t * XMLdata = new uint8_t[numBytesToAllocate];
  if( XMLdata)
  {
    int numBytesRead = read(m_socketFileDesc, XMLdata, numBytesToRead);
    if (numBytesRead < numBytesToRead) {
      HandleTransmissionError(SMARTdata);
      LOGN_ERROR("read less bytes (" << numBytesRead << ") than expected (" 
        << numBytesToRead << ")");
      return 2; //TODO provide error handling (show message to user etc.)
    }
    LOGN_DEBUG("successfully read " << numBytesRead << "bytes")
    XMLdata[numBytesToRead] = '\0';

    GetSupportedSMARTattributesViaXML(XMLdata, numBytesToRead, 
      dataCarrierIDandSMARTidsContainer);
  }
}

fastestUnsignedDataType SMARTmonitorClient::GetSMARTvaluesFromServer(
  std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValuesContainer)
{
  LOGN("begin")
  enum transmission transmissionResult = unsetTransmResult;
  sMARTuniqueIDandValuesContainer.clear();
  int numBytesRead;
  fastestSignedDataType numBytesToRead = ReadNumFollowingBytes();
  
  const fastestUnsignedDataType numBytesToAllocate = numBytesToRead + 1;
  uint8_t SMARTvalues[numBytesToAllocate];
  if( SMARTvalues)
  {
    numBytesRead = read(m_socketFileDesc, SMARTvalues, numBytesToRead);
    if (numBytesRead < numBytesToRead) {
      HandleTransmissionError(SMARTdata);
      LOGN_ERROR("read less bytes (" << numBytesRead << ") than expected (" 
        << numBytesToRead << ")");
      return readLessBytesThanIntended; //TODO provide error handling (show message to user etc.)
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
//    delete [] SMARTvalues;
  }
  else
  {
    LOGN_ERROR("Failed to allocate " << numBytesToAllocate << " bytes")
  }
  LOGN("end")
  return successfull;
}
