/** Author: sg
 * Created on 20. November 2016, 17:43 */

#include "SMARTmonitorClient.h" //class SMARTmonitorClient
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
#include <SMARTvalueFormatter.hpp> //SMARTvalueFormatter::FormatHumanReadable())
#include <compiler/GCC/enable_disable_warning.h> //GCC_DIAG_OFF(...)

/** Static/class variable defintion: */
fastestUnsignedDataType SMARTmonitorClient::s_updateUI = 1;
nativeThread_type SMARTmonitorClient::s_updateSMARTparameterValuesThread;
bool SMARTmonitorClient::s_atLeast1CriticalNonNullValue;
fastestUnsignedDataType SMARTmonitorClient::s_maxNumCharsNeededForDisplay [] =
 { 3, 30, 15, 20, 40};
fastestUnsignedDataType SMARTmonitorClient::s_charPosOAttrNameBegin [] =
 { 0, 3, 33, 48, 68};
GCC_DIAG_OFF(write-strings)
char * SMARTmonitorClient::s_columnAttriuteNames [] =
 { "ID", "name", "raw", "human readable", "last update"};
GCC_DIAG_ON(write-strings)

SMARTmonitorClient::SMARTmonitorClient() 
{
  m_getSMARTvaluesFunctionParams.p_SMARTmonitorBase = this;
  /** Setting seconds to an invalid number (100) indicates that the time
   *   hasn't been retrieved yet. */
  m_timeOfLastSMARTvaluesUpdate.tm_sec = 100;
//  for( int i = 0; i < COL_IDX_beyondLast ; i++ )
//  {
//    s_charPosOAttrNameBegin[i] += 
//  }
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

int GetConnectTimeOut(const int m_socketFileDesc)
{
//  std::string strMsg("connecting to ");
//  strMsg += hostName;
//  ShowMessage(strMsg.c_str()
  //from http://stackoverflow.com/questions/4181784/how-to-set-socket-timeout-in-c-when-making-multiple-connections
  struct timeval timevalSocketTimeout { 10, 0};
  socklen_t __optlen = sizeof(timevalSocketTimeout);
 
   int returnValue = setsockopt(m_socketFileDesc, SOL_SOCKET, SO_RCVTIMEO, 
    /** https://linux.die.net/man/7/socket
    * SO_RCVTIMEO and SO_SNDTIMEO
    * "Specify the receiving or sending timeouts until reporting an error. 
    * The argument is a struct timeval." */
    (char *)& timevalSocketTimeout,
    /** https://linux.die.net/man/2/getsockopt : 
     * "For getsockopt(), optlen is a value-result argument, initially 
     * containing the size of the buffer pointed to by optval, and modified on 
     * return to indicate the actual size of the value returned." */
    __optlen );
 
  returnValue = getsockopt(m_socketFileDesc, SOL_SOCKET, SO_RCVTIMEO, 
    /** https://linux.die.net/man/7/socket
    * SO_RCVTIMEO and SO_SNDTIMEO
    * "Specify the receiving or sending timeouts until reporting an error. 
    * The argument is a struct timeval." */
    (char *)& timevalSocketTimeout,
    /** https://linux.die.net/man/2/getsockopt : 
     * "For getsockopt(), optlen is a value-result argument, initially 
     * containing the size of the buffer pointed to by optval, and modified on 
     * return to indicate the actual size of the value returned." */
    & __optlen );
  
  returnValue = getsockopt(m_socketFileDesc, SOL_SOCKET, SO_SNDTIMEO, 
    /** https://linux.die.net/man/7/socket
    * SO_RCVTIMEO and SO_SNDTIMEO
    * "Specify the receiving or sending timeouts until reporting an error. 
    * The argument is a struct timeval." */
    (char *)& timevalSocketTimeout,
    /** https://linux.die.net/man/2/getsockopt : 
     * "For getsockopt(), optlen is a value-result argument, initially 
     * containing the size of the buffer pointed to by optval, and modified on 
     * return to indicate the actual size of the value returned." */
    & __optlen );
  if(returnValue < 0)
  {
    const int lastOSerrorCode = OperatingSystem::GetLastErrorCode();
    std::string st = OperatingSystem::EnglishMessageFromErrorCode(lastOSerrorCode);
  }
  socklen_t ntsnd = sizeof(ntsnd);
  int SndTimeO;
  //http://stackoverflow.com/questions/4181784/how-to-set-socket-timeout-in-c-when-making-multiple-connections
  returnValue = getsockopt(m_socketFileDesc, SOL_SOCKET, SO_SNDTIMEO, 
    (char *) & SndTimeO,
                & ntsnd);
  int timeout = 10000;
  //https://linux.die.net/man/7/socket
  returnValue = getsockopt(m_socketFileDesc, 6, 18, (char*) &timeout, & __optlen);
  int socketTimeoutInS = 30;
  return socketTimeoutInS;
}

inline int GetSocketFileDescriptor()
{
  /** http://pubs.opengroup.org/onlinepubs/009695399/functions/socket.html :
   * "Upon successful completion, socket() shall return a non-negative integer, 
   * the socket file descriptor. Otherwise, a value of -1 shall be returned 
   * and errno set to indicate the error." */
  const int socketFileDesc = socket(AF_INET, SOCK_STREAM,
    /** socket.h : "If PROTOCOL is zero, one is chosen automatically." */
    0 );
  if (socketFileDesc < 0)
  {
    const unsigned long int lastOSerrorCode = OperatingSystem::GetLastErrorCode();
    LOGN_ERROR("ERROR opening socket: OS error code " << lastOSerrorCode 
      << " error message:" << OperatingSystem::EnglishMessageFromErrorCode(lastOSerrorCode) )
  }
  return socketFileDesc;
}

struct hostent * GetServerHostDataBaseEntry(const char * hostName)
{
  /** http://pubs.opengroup.org/onlinepubs/009695399/functions/gethostbyaddr.html :
   * "Upon successful completion, these functions shall return a pointer to a 
   * hostent structure if the requested entry was found, and a null pointer if 
   * the end of the database was reached or the requested entry was not found.
   * 
   * Upon unsuccessful completion, gethostbyaddr() and gethostbyname() shall 
   * set h_errno to indicate the error." */
  struct hostent * p_serverHostDataBaseEntry = gethostbyname(hostName);
  if (p_serverHostDataBaseEntry == NULL) {
    //TODO check h_errno
    LOGN_ERROR("host " << hostName << " not in database;error code:" << h_errno)
  }
  return p_serverHostDataBaseEntry;
}

void ConnectToSocketNonBlocking(int socketFD)
{
  //from http://www.linuxquestions.org/questions/programming-9/why-does-connect-block-for-a-long-time-708647/
//  fcntl(sd, F_SETFL, curflags | O_NONBLOCK);
  //http://stackoverflow.com/questions/29598508/how-to-get-out-from-a-tcp-blocking-connect-call
  
//  int flags = fcntl(socketFD, F_GETFL, 0);
//  fcntl(socketFD, F_SETFL, flags | O_NONBLOCK);
//
//  result = connect(tcp_sock, ...);
//  if (result == -1)
//  {
//    if (errno == EINPROGRESS)
//    {
//      fd_set wfd;
//      FD_ZERO(&wfd);
//      FD_SET(tcp_sock, &wfd);
//
//      struct timeval timeout;
//      timeout.tv_sec = ...;
//      timeout.tv_usec = ...;
//
//      result = select(tcp_sock+1, NULL, &wrd, NULL, &timeout);
//      if (result > 0)
//      {
//          // connected
//          fcntl(tcp_sock, F_SETFL, flags);
//      }
//      else if (result == 0)
//      {
//          // time out
//      }
//      else
//      {
//          // error
//      }
//    }
//  }
}
  
fastestUnsignedDataType SMARTmonitorClient::ConnectToServer(const char * hostName) {
  //from http://www.linuxhowtos.org/data/6/client.c
  int portNumber = m_socketPortNumber, n;
  struct sockaddr_in serv_addr;
  struct hostent * p_serverHostDataBaseEntry;
  
  m_socketFileDesc = GetSocketFileDescriptor();
  if(m_socketFileDesc < 0)
    return errorOpeningSocket;  
  LOGN("successfully opened socket");
  
  p_serverHostDataBaseEntry = GetServerHostDataBaseEntry(hostName);
  if( ! p_serverHostDataBaseEntry )
    return getHostByNameFailed;  
  LOGN("got host name for \"" << hostName << "\":" << p_serverHostDataBaseEntry->h_name)

  bzero( (char *) & serv_addr, sizeof(serv_addr) );
  serv_addr.sin_family = AF_INET;
  bcopy( (char *) p_serverHostDataBaseEntry->h_addr,
    (char *) & serv_addr.sin_addr.s_addr,
    p_serverHostDataBaseEntry->h_length);
  serv_addr.sin_port = htons(portNumber);
  
  int socketTimeoutInS = GetConnectTimeOut(m_socketFileDesc);
//  MessageBox msgBox();
  
  DWORD currentThreadNumber = OperatingSystem::GetCurrentThreadNumber();
  //TODO execute socket connection in another thread than user interface thread.
  // in order to be interruptable
//  if( currentThreadNumber == s_UserInterfaceThreadID)
//  {
//  }
  ShowConnectionState(hostName, socketTimeoutInS);
//  ConnectToSocketNonBlocking();
  
  /** http://pubs.opengroup.org/onlinepubs/9699919799/functions/connect.html :
   * "Upon successful completion, connect() shall return 0; otherwise, -1 shall
   *  be returned and errno set to indicate the error." */
  const int connectResult = connect(m_socketFileDesc,(struct sockaddr *) & serv_addr,
    sizeof(serv_addr) );
//  msgBox.Close();
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

/** Called e.g. when server address was given via program options/
 *  as command line argument */
void SMARTmonitorClient::ConnectToServerAndGetSMARTvalues()
{
  const fastestUnsignedDataType res = ConnectToServer(
    m_stdstrServiceHostName.c_str() );
  if (res == connectedToService)
  {
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
      /*sMARTuniqueIDandValues*/);
    if (getSMARTvaluesResult == 0) {
      SetSMARTattributesToObserve(sMARTuniqueIDandValues);
//      m_p_ConnectAndDisconnectButton->SetLabel(wxT("disconnect"));
      ReBuildUserInterface();
      UpdateSMARTvaluesUI();
      ShowStateAccordingToSMARTvalues(s_atLeast1CriticalNonNullValue);
      m_getSMARTvaluesFunctionParams.p_getSMARTvaluesFunction =
        & SMARTmonitorBase::GetSMARTvaluesFromServer;
      StartAsyncUpdateThread(
        //UpdateSMARTvaluesThreadSafe 
        m_getSMARTvaluesFunctionParams
        );
//      ((SMARTmonitorBase *)this)->StartAsyncUpdateThread(SMARTmonitorBase::UpdateSMARTvaluesThreadSafe);
    }
  } else {
    //wxGetApp().ShowMessage("");
  }
}

void SMARTmonitorClient::ConnectToServer() {
  /** Terminate a possibly running update UI thread (e.g.. already connected
   *   to a service). */
  EndUpdateUIthread();
//  std::string stdstrServerAddress;
  GetTextFromUser("input SMART values server address", m_stdstrServiceHostName);

  SetServiceAddress(m_stdstrServerAddress);
  ConnectToServerAndGetSMARTvalues();
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
  const int lastErrorNumber = OperatingSystem::GetLastErrorCode();
  char * errorMessageForErrno = NULL;
  if( lastErrorNumber != 0)
  {
    switch(lastErrorNumber)
    {
      //TODO Unix-specific value
      case EPIPE :
        //see https://linux.die.net/man/2/write
        errorMessageForErrno = (char *) "The reading end of socket is closed.";
        break;
      default:
        errorMessageForErrno = strerror(errno);
        break;
    }
  }
  stdoss << "OS error #:" << errno ;
  switch(transmissionError)
  {
    case numBytesToReceive :
      stdoss << "ERROR reading the number of following bytes from socket";
      break;
    case SMARTdata :
      stdoss << "ERROR reading SMART data from socket";
      break;
  }
  if( errorMessageForErrno )
    stdoss << ":\n" << errorMessageForErrno;
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
//  std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValuesContainer
  )
{
  LOGN("begin")
  std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValuesContainer = 
    mp_SMARTaccess->GetSMARTuniqueIDandValues();
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

/** These values are fixed and so need to be shown only once in the user
  * interface (and not at/for every SMART values update -> saves resources) */
void SMARTmonitorClient::SetSMARTattribIDandNameLabel()
{
  LOGN("begin")
  const std::set<SMARTentry> & SMARTattributesFromConfigFile =
    mp_SMARTaccess->getSMARTattributes();
  LOGN("begin " << & SMARTattributesFromConfigFile)
  unsigned lineNumber = 0;
//  wxString wxSMARTattribName;

  std::set<SMARTentry>::const_iterator
    SMARTattributesToObserveIter = SMARTattributesFromConfigFile.begin();
  fastestUnsignedDataType SMARTattributeID;
  
  std::set<int>::const_iterator IDofAttributeToObserverIter = 
    m_IDsOfSMARTattributesToObserve.begin();
  SMARTentry sMARTentry;
  //TODO only dummy object!
  SMARTuniqueID sMARTuniqueID;
  /** Traverse all SMART attribute IDs either got from server or read via config 
   *  file.*/
  for( ; IDofAttributeToObserverIter != m_IDsOfSMARTattributesToObserve.
      end() ; IDofAttributeToObserverIter ++, lineNumber++)
  {
    SMARTattributeID = * IDofAttributeToObserverIter;
    
    std::ostringstream std_oss;
    std_oss << SMARTattributeID;
    SetAttribute(
      //TODO only dummy object!
      sMARTuniqueID,
      SMARTattributeID,
      ColumnIndices::SMART_ID,
      std_oss.str()
      );
    
    /** Now get the attribute name belonging to SMART ID */
    sMARTentry.SetAttributeID(SMARTattributeID);
    SMARTattributesToObserveIter = SMARTattributesFromConfigFile.find(sMARTentry);
    if( SMARTattributesToObserveIter != SMARTattributesFromConfigFile.end() )
    {
      const SMARTentry & SMARTattributeFromConfig =
        *SMARTattributesToObserveIter;
      //SMARTattributeToObserve.name
      SetAttribute(
        //TODO only dummy object!
        sMARTuniqueID,
        SMARTattributeID,
        ColumnIndices::SMARTparameterName,
        SMARTattributeFromConfig.GetName()
        );
//      wxSMARTattribName = wxWidgets::GetwxString_Inline(
//        SMARTattributeFromConfig.GetName() );
//      m_pwxlistctrl->SetItem(
//        lineNumber, 
//        COL_IDX_SMARTparameterName, /*wxString::Format( wxT("%s"),
//        wxSMARTattribName.c_str() )*/ wxSMARTattribName
//        );
    }
//            m_pwxlistctrl->SetItem( item );
  }
  LOGN("end")
}

void SMARTmonitorClient::UpdateTimeOfSMARTvalueRetrieval(
  const SMARTuniqueID & sMARTuniqueID,
  const fastestUnsignedDataType SMARTattributeID,
  const long int timeStampOfRetrievalIn1ks)
{  
  std::string timeFormatString;
  UserInterface::FormatTimeOfLastUpdate(
    timeStampOfRetrievalIn1ks, 
    timeFormatString);
//  wxString timeOfSMARTvalueRetrievel = wxWidgets::GetwxString_Inline(
//    timeFormatString);
  SetAttribute(
    sMARTuniqueID,
    SMARTattributeID,
    ColumnIndices::lastUpdate /** column #/ index */,
    //wxString::Format(wxT("%u ms ago"), numberOfMilliSecondsPassedSinceLastSMARTquery )
    timeFormatString
    );
}

void SMARTmonitorClient::UpdateSMARTvaluesUI()
{
  unsigned lineNumber = 0;
  bool atLeast1CriticalNonNullValue = false;

  const fastestUnsignedDataType numberOfDifferentDrives = mp_SMARTaccess->
    GetNumberOfDifferentDrives();
  SMARTaccessBase::constSMARTattributesContainerType & SMARTattributesFromConfigFile =
    mp_SMARTaccess->getSMARTattributes();

  //memory_barrier(); //TODO necessary at all??
  std::set<SMARTuniqueIDandValues> & SMARTuniqueIDsAndValues = mp_SMARTaccess->
    GetSMARTuniqueIDandValues();
  LOGN("SMART unique ID and values container:" << & SMARTuniqueIDsAndValues )
  std::set<SMARTuniqueIDandValues>::const_iterator SMARTuniqueIDandValuesIter =
    SMARTuniqueIDsAndValues.begin();
  fastestUnsignedDataType SMARTattributeID;
  uint64_t SMARTrawValue;
  const std::set<int> & IDsOfSMARTattributesToObserve =
    m_IDsOfSMARTattributesToObserve;
#ifdef DEBUG
//  int itemCount = m_pwxlistctrl->GetItemCount();
#endif
  //memory_barrier(); //TODO: not really necessary??
  
  std::string stdstrHumanReadableRawValue;
//  wxString wxstrRawValueString;
  /** Loop over data carriers. */
  for(fastestUnsignedDataType currentDriveIndex = 0;
    SMARTuniqueIDandValuesIter != SMARTuniqueIDsAndValues.end() ;
    SMARTuniqueIDandValuesIter ++)
  {
    const SMARTuniqueID sMARTuniqueID = SMARTuniqueIDandValuesIter->
      getSMARTuniqueID();
    LOGN("SMART unique ID and values object " << &(*SMARTuniqueIDandValuesIter) )
    std::set<int>::const_iterator
      IDsOfSMARTattributesToObserveIter = IDsOfSMARTattributesToObserve.begin();
    SMARTaccessBase::SMARTattributesContainerConstIterType 
      SMARTattributesFromConfigFileIter = SMARTattributesFromConfigFile.begin();
    
    /** Loop over attribute IDs to observe */
    for( ; IDsOfSMARTattributesToObserveIter != IDsOfSMARTattributesToObserve.end();
        IDsOfSMARTattributesToObserveIter ++)
    {
      SMARTattributeID = *IDsOfSMARTattributesToObserveIter;
      //TODO attribute IDs of SMART values to observe may not be a subset of
      // SMART attributes in config file!
      while( SMARTattributesFromConfigFileIter->GetAttributeID() != SMARTattributeID)
      {
        SMARTattributesFromConfigFileIter++;
        LOGN_DEBUG( "using SMART entry at address " << 
          & (* SMARTattributesFromConfigFileIter) )
      }
      
      const SMARTvalue & sMARTvalue = SMARTuniqueIDandValuesIter->m_SMARTvalues[SMARTattributeID];
      sMARTvalue.IsConsistent(SMARTrawValue);
//      memory_barrier(); //TODO: not really necessary??
      int successfullyUpdatedSMART = sMARTvalue.m_successfullyReadSMARTrawValue;
      
      //memory_barrier(); //TODO: not really necessary??
      if( successfullyUpdatedSMART )
      {
        stdstrHumanReadableRawValue = SMARTvalueFormatter::FormatHumanReadable(
          SMARTattributeID, SMARTrawValue);
//        wxstrRawValueString = wxWidgets::GetwxString_Inline(
//          stdstrHumanReadableRawValue);
        std::ostringstream std_oss;
         std_oss << SMARTrawValue;
        SetAttribute(
          sMARTuniqueID,
          SMARTattributeID,
          ColumnIndices::rawValue /** column #/ index */,
          std_oss.str() );
        SetAttribute(
          sMARTuniqueID,
          SMARTattributeID,
          ColumnIndices::humanReadableRawValue, 
          stdstrHumanReadableRawValue);
                
        /** https://cboard.cprogramming.com/c-programming/115586-64-bit-integers-printf.html
        *   : "%llu": Linux %llu, "%I64u": Windows */
          //TODO wxString::Format(...) causes "smallbin double linked list corrupted"
//          wxString::Format( , ) expands to : 
//        template < typename T1 > static wxString Format ( const wxFormatString & f1 , T1 a1 ) {
//          typedef const wxFormatString & TF1 ;
//          const wxFormatString * fmt = ( ( wxFormatStringArgumentFinder < TF1 > :: find ( f1 ) ) ) ;
//          return DoFormatWchar ( f1 , wxArgNormalizerWchar < T1 > ( a1 , fmt , 1 ) . get ( ) ) ;
//        }
//          wxString::Format( wxT("%llu"), SMARTrawValue)
        bool critical = SMARTattributesFromConfigFileIter->IsCritical();
        LOGN_DEBUG("attribute ID " << SMARTattributesFromConfigFileIter->
          GetAttributeID() << " is critical?:" << critical
          //(critical==true ? "yes" : "no") 
          )
//        if( critical && SMARTrawValue > 0)
//        {
//          atLeast1CriticalNonNullValue = true;
//          m_pwxlistctrl->SetItemBackgroundColour(lineNumber, * wxRED);
//        }
//        else
//          m_pwxlistctrl->SetItemBackgroundColour(lineNumber, * wxGREEN);
        UpdateTimeOfSMARTvalueRetrieval(
          sMARTuniqueID,
          SMARTattributeID,
          sMARTvalue.m_timeStampOfRetrieval);
      }
      else
      {
//        m_pwxlistctrl->SetItem(lineNumber,
//          SMARTtableListCtrl::COL_IDX_rawValue /** column #/ index */,
//          wxT("N/A") );
      }
//            m_pwxlistctrl->SetItem( item );
      ++ lineNumber;
    }
  }
  /** ^= state changed. */
  if( s_atLeast1CriticalNonNullValue != atLeast1CriticalNonNullValue )
  {
    ShowStateAccordingToSMARTvalues(atLeast1CriticalNonNullValue);
  }
  s_atLeast1CriticalNonNullValue = atLeast1CriticalNonNullValue;
}
