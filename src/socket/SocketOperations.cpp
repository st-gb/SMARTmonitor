#include <sys/socket.h> //socket(...))
#include <netinet/in.h> //sockaddr_in
#include <fcntl.h> //fcntl(...)

//#include "SocketOperations.h"
#include <client/SMARTmonitorClient.h>
#include <preprocessor_macros/logging_preprocessor_macros.h>
#include <OperatingSystem/time/GetCurrentTime.hpp>
#include "OperatingSystem/GetLastErrorCode.hpp" //OperatingSystem::GetLastErrorCode()
#include <OperatingSystem/Linux/EnglishMessageFromErrorCode/EnglishMessageFromErrorCode.h>
#include <OperatingSystem/BSD/socket/prepCnnctToSrv.h>///prepCnnctToSrv(...)
#include <OperatingSystem/BSD/socket/socketTimeout.h>///getSocketTimeout(...)

using namespace OperatingSystem::BSD::sockets;

fastestSignedDataType SMARTmonitorClient::ReadNumFollowingBytes()
{
  LOGN("reading 2 bytes from socket #" << m_socketFileDesc)
  uint16_t numDataBytesToRead;
  const size_t numBytesToRead = 2;
  //https://stackoverflow.com/questions/3053757/read-from-socket:
  int numBinRead = 0;
  ioctl(m_socketFileDesc, FIONREAD, &numBinRead);
  LOGN_DEBUG("# bytes in read buffer for socket:" << numBinRead)
  //TODO connection to service error here when expecting data for the 2nd time 
  // running the wx GUI. errno: 11 from GetSMARTattrValsFromSrv
  // Maybe because the 2nd time is from another thread.
  int numBytesRead = read(m_socketFileDesc, & numDataBytesToRead, numBytesToRead);
  if( numBytesRead < numBytesToRead ) {
    HandleTransmissionError(numBytesToReceive);
    return -1;
  }
  numDataBytesToRead = ntohs(numDataBytesToRead);
  LOGN_DEBUG("# bytes to read:" << numDataBytesToRead)
  return numDataBytesToRead;
}

fastestUnsignedDataType SMARTmonitorClient::GetSMARTattrValsFromSrv(
//  std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValuesContainer
  )
{
  LOGN_DEBUG("begin")
  std::set<SMARTuniqueIDandValues> & sMARTuniqueIDandValsCont = 
    SMARTuniqueIDsAndValues;
  enum transmission transmissionResult = unsetTransmResult;
  //TODO this destroys the SMART unique ID object and so the pointer of the to 
  // the event becomes invaled.
//  sMARTuniqueIDandValsCont.clear();
  //TODO if a data carrier is detached it does not disappear in 
  // SMARTuniqueIDsAndValues?
  int numBytesRead;
  ///Value is 0 if connect to server for the 2nd call of this function.
  ///Value is garbage if connect to server in another thread for the 2nd call 
  /// of this function.
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
      sMARTuniqueIDandValsCont.insert/*emplace*/(sMARTuniqueIDandValues);
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

///Non-Blocking is an alternative option to set a self-defined connect timeout.
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

fastestUnsignedDataType SMARTmonitorClient::ConnectToServer(
  const char * hostName, bool asyncConnect)
{
  struct sockaddr_in srvAddr;
  fastestUnsignedDataType prepCnnctToSrvRslt = prepCnnctToSrv(hostName,
    m_socketPortNumber, & srvAddr, AF_INET, & m_socketFileDesc);
  if(prepCnnctToSrvRslt != prepCnnctToSrvSucceeded)
    return prepCnnctToSrvRslt;
  ///TCP: client sends "SYN"; server sends "ACK", client "SYNACK"
  double cnnctTimeoutInS;
  GetSocketTimeoutInS(m_socketFileDesc, & cnnctTimeoutInS);
  if(cnnctTimeoutInS == 0)
    cnnctTimeoutInS = 30;
//  MessageBox msgBox();
  
  DWORD currentThreadNumber = OperatingSystem::GetCurrentThreadNumber();
  //TODO execute socket connection in another thread than user interface thread.
  // in order to be interruptable
//  if( currentThreadNumber == s_UserInterfaceThreadID)
//  {
//  }
#ifdef multithread
  if( asyncConnect )
  {
    ShowConnectionState(hostName, cnnctTimeoutInS);
    //TODO make as member variable (else is deleted if this block ends).
    nativeThread_type connectThread;
    SocketConnectThreadFuncParams * p_socketConnectThreadFuncParams = new 
      SocketConnectThreadFuncParams {m_socketFileDesc, srvAddr, this,
        (fastestUnsignedDataType)cnnctTimeoutInS };
    /** call in another thread in order to enable breaking connection */
    connectThread.start(SocketConnectThreadFunc, p_socketConnectThreadFuncParams);
    BeforeConnectToServer();
  }
  else
#endif
  /** http://man7.org/linux/man-pages/man2/connect.2.html :
      "If the connection or binding succeeds, zero is returned." */
//    return ConnectToSocketNonBlocking(
//      m_socketFileDesc, 
//      serv_addr, 
//      connectTimeoutInSeconds);
    return connect(m_socketFileDesc, (struct sockaddr *) & srvAddr, sizeof(srvAddr) );
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
  //TODO errno must come from connect(...), no other function call inbetween
  //TODO use error description from common_sourcecode/.../BSD/socket
  //BlockingCnnct::getoPossibleCause
  switch(errno)
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
