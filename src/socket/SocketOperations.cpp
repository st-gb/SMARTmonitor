#include <sys/socket.h> //socket(...))
#include <netinet/in.h> //sockaddr_in
#include <netdb.h> //gethostbyname(...)
#include <fcntl.h> //fcntl(...)

//#include "SocketOperations.h"
#include <client/SMARTmonitorClient.h>
#include <preprocessor_macros/logging_preprocessor_macros.h>
#include <OperatingSystem/time/GetCurrentTime.hpp>
#include "OperatingSystem/GetLastErrorCode.hpp" //OperatingSystem::GetLastErrorCode()
#include <OperatingSystem/Linux/EnglishMessageFromErrorCode/EnglishMessageFromErrorCode.h>

fastestSignedDataType SMARTmonitorClient::ReadNumFollowingBytes()
{
  LOGN("reading 2 bytes from socket #" << m_socketFileDesc)
  uint16_t numDataBytesToRead;
  const size_t numBytesToRead = 2;
  int numBytesRead = read(m_socketFileDesc, & numDataBytesToRead, numBytesToRead);
  if( numBytesRead < numBytesToRead ) {
    HandleTransmissionError(numBytesToReceive);
    return -1;
  }
  numDataBytesToRead = ntohs(numDataBytesToRead);
  LOGN_DEBUG("# bytes to read:" << numDataBytesToRead)
  return numDataBytesToRead;
}

fastestUnsignedDataType SMARTmonitorClient::GetSMARTattributeValuesFromServer(
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
  if( numBytesToRead < 1 )
    return readLessBytesThanIntended;
  const fastestUnsignedDataType numBytesToAllocate = numBytesToRead + 1;
  uint8_t SMARTvalues[numBytesToAllocate];
  if( SMARTvalues)
  {
    /** http://man7.org/linux/man-pages/man2/read.2.html :
     *  "On error, -1 is returned, and errno is set appropriately." */
    numBytesRead = read(m_socketFileDesc, SMARTvalues, numBytesToRead);
    //TODO often numBytesRead < numBytesToRead if this function is called from 
    //  "UpdateSMARTparameterValuesThreadFunc"
    if (numBytesRead < numBytesToRead) {
      HandleTransmissionError(SMARTparameterValues);
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

struct SocketConnectThreadFuncParams
{
  int socketFileDesc;
  struct sockaddr_in serv_addr;
  SMARTmonitorClient * p_SMARTmonitorClient;
  fastestUnsignedDataType connectTimeoutInSeconds;
  void AfterConnectToServer()
  {
//    p_SMARTmonitorClient->CloseServerConnectStatusUI();
  }
};

int GetConnectTimeOut(const int m_socketFileDesc)
{
//  std::string strMsg("connecting to ");
//  strMsg += hostName;
//  ShowMessage(strMsg.c_str()
  //from http://stackoverflow.com/questions/4181784/how-to-set-socket-timeout-in-c-when-making-multiple-connections
  struct timeval timevalSocketTimeout = { 10, 0};
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

int ConnectToSocketNonBlocking(
  int socketFileDescriptor, 
  struct sockaddr_in & serv_addr, 
  long int connectTimeoutInSeconds)
{
  /** from http://www.linuxquestions.org/questions/programming-9/why-does-connect-block-for-a-long-time-708647/ */
//  fcntl(socketFD, F_SETFL, curflags | O_NONBLOCK);
  //http://stackoverflow.com/questions/29598508/how-to-get-out-from-a-tcp-blocking-connect-call
  //https://stackoverflow.com/questions/17769964/linux-sockets-non-blocking-connect
  
  int flags = fcntl(socketFileDescriptor, F_GETFL, 0);
  fcntl(socketFileDescriptor, F_SETFL, flags | O_NONBLOCK);

  /** http://man7.org/linux/man-pages/man2/connect.2.html :
      "If the connection or binding succeeds, zero is returned." */
  int result = connect(socketFileDescriptor, (struct sockaddr *) & serv_addr, sizeof(serv_addr) );
  if (result == -1)
  {
    if (errno == EINPROGRESS) /** For non-blocking sockets */
    {
//      fd_set wfd;
      fd_set readFileDescriptorSet;
      FD_ZERO(&readFileDescriptorSet);
      FD_SET(socketFileDescriptor, &readFileDescriptorSet);

      struct timeval socketConnectTimeout;
      socketConnectTimeout.tv_sec = connectTimeoutInSeconds;
      socketConnectTimeout.tv_usec = 0;

      /** https://linux.die.net/man/2/select : 
        * "waiting until one or more of the file descriptors become
          "ready" for some class of I/O operation (e.g., input possible)."  */
      result = select(socketFileDescriptor + 1, & readFileDescriptorSet, NULL, NULL, &socketConnectTimeout);
//      int errorNumber = errno;
      if (result > 0) /** connected */
      {
//        FD_ISSET()
        //http://pubs.opengroup.org/onlinepubs/7908799/xns/getsockopt.html
        // "This option stores an int value."
        int iSO_ERROR;
        socklen_t optlen;
        /** http://man7.org/linux/man-pages/man2/connect.2.html :
          "After select(2) indicates writability, use getsockopt(2) to read the
           SO_ERROR option at level SOL_SOCKET to determine whether
           connect() completed successfully (SO_ERROR is zero)" */
        
        /** http://man7.org/linux/man-pages/man2/setsockopt.2.html
         * " On success, zero is returned for the standard options." */
        result = getsockopt(
          socketFileDescriptor, 
          SOL_SOCKET /*int level*/, 
          SO_ERROR, //int optname
          & iSO_ERROR //void *optval
          , & optlen //socklen_t *optlen
          );
        if( result == 0 && iSO_ERROR == 0)
        {
          
          LOGN_INFO("successfully connected to " << serv_addr.sin_addr.s_addr )
          //TODO result was success even if the server/service is not running
          /** Change back to blocking mode. */
          result = fcntl(socketFileDescriptor, F_SETFL, flags);
        }
        else
          result = iSO_ERROR;
      }
      else if (result == 0) // time out occurred
      {
        result = errno;
      }
      else
      {
          // error
      }
    }
  }
  return result;
}

DWORD SocketConnectThreadFunc(void * p_v)
{
  SocketConnectThreadFuncParams * p_socketConnectThreadFuncParams = 
    (SocketConnectThreadFuncParams *) p_v;
  if( p_socketConnectThreadFuncParams)
  {
  /** http://pubs.opengroup.org/onlinepubs/9699919799/functions/connect.html :
   * "Upon successful completion, connect() shall return 0; otherwise, -1 shall
   *  be returned and errno set to indicate the error." */
//    const int connectResult = connect(
//      p_socketConnectThreadFuncParams->socketFileDesc,
//        (struct sockaddr *) & p_socketConnectThreadFuncParams->serv_addr,
//      sizeof(p_socketConnectThreadFuncParams->serv_addr) );
//    const int connectResult = connect(m_socketFileDesc,(struct sockaddr *) & serv_addr,
//      sizeof(serv_addr) );
    int connectResult = ConnectToSocketNonBlocking(
      p_socketConnectThreadFuncParams->socketFileDesc,
      p_socketConnectThreadFuncParams->serv_addr,
      p_socketConnectThreadFuncParams->connectTimeoutInSeconds
      );
  
    if( connectResult < 0)
    {
//      HandleConnectionError(hostName);
//      return errorConnectingToService;
      /**Close the socket here because so it is done for all user interfaces.*/
      close(p_socketConnectThreadFuncParams->socketFileDesc);
    }
    else if( connectResult == 0)
    {
      LOGN("successfully connected")
      p_socketConnectThreadFuncParams->p_SMARTmonitorClient->ShowMessage(
        "successfully connected to service", UserInterface::MessageType::success);
    }
    p_socketConnectThreadFuncParams->p_SMARTmonitorClient->
      AfterConnectToServer(connectResult);
    delete p_socketConnectThreadFuncParams;
    return connectResult;
  }
  return -1;
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

fastestUnsignedDataType SMARTmonitorClient::ConnectToServer(
  const char * hostName, bool asyncConnect)
{
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
  
  int connectTimeoutInSeconds = GetConnectTimeOut(m_socketFileDesc);
//  MessageBox msgBox();
  
  DWORD currentThreadNumber = OperatingSystem::GetCurrentThreadNumber();
  //TODO execute socket connection in another thread than user interface thread.
  // in order to be interruptable
//  if( currentThreadNumber == s_UserInterfaceThreadID)
//  {
//  }
  if( asyncConnect )
  {
    ShowConnectionState(hostName, connectTimeoutInSeconds);
    //TODO make as member variable (else is deleted if this block ends).
    nativeThread_type connectThread;
    SocketConnectThreadFuncParams * p_socketConnectThreadFuncParams = new 
      SocketConnectThreadFuncParams {m_socketFileDesc, serv_addr, this, connectTimeoutInSeconds };
    /** call in another thread in order to enable breaking connection */
    connectThread.start(SocketConnectThreadFunc, p_socketConnectThreadFuncParams);
    BeforeConnectToServer();
  }
  else
    return ConnectToSocketNonBlocking(
      m_socketFileDesc, 
      serv_addr, 
      connectTimeoutInSeconds);
//  ShowConnectToServerStatus("cancel connection");
//  close(m_socketFileDesc);
//  connectThread.WaitForTermination();
  //bzero(buffer,256);
  return unconnectedFromService;
}

void SMARTmonitorClient::AfterGetSMARTvaluesLoop(int getSMARTvaluesResult)
{
  close(m_socketFileDesc);
  if(getSMARTvaluesResult != 0)
  {
    StartServiceConnectionCountDown(60);
  }
}

void SMARTmonitorClient::HandleConnectionError(const char * hostName)
{
  //TODO: show error via user interface
  //Message("")
  std::ostringstream oss;
  oss << "error connecting to S.M.A.R.T. values service " << 
    m_stdstrServiceHostName << ",port:" << m_socketPortNumber << "\n";
  //TODO the following is Linux-specific and should be OS-independent
  //see http://man7.org/linux/man-pages/man2/connect.2.html
  switch(errno )
  {
    //see http://man7.org/linux/man-pages/man2/connect.2.html
    case ECONNREFUSED :
      oss << "No process listening on the remote address \"" << //hostName 
        m_stdstrServiceHostName
        << "\", port:" << m_socketPortNumber;
      break;
    default :
    {
      int errorCode = OperatingSystem::GetLastErrorCode();
      oss << OperatingSystem::EnglishMessageFromErrorCode( errorCode );
    }
    break;
  }
  ShowMessage(oss.str().c_str(), MessageType::error );
  LOGN_ERROR(oss.str() )
}
