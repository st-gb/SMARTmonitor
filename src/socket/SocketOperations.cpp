#ifdef __linux__
  #include <sys/ioctl.h>///FIONREAD
#endif
#include <OperatingSystem/BSD/socket/socket.h>///socket(...), InitSocket(...)
#include <OperatingSystem/BSD/socket/sockaddr_in.h>///struct sockaddr_in
#include <fcntl.h> //fcntl(...)

//#include "SocketOperations.h"
#include <client/SMARTmonitorClient.h>

///Stefan Gebauer's common_sourcecode repository files:
#include <preprocessor_macros/logging_preprocessor_macros.h>
#include <OperatingSystem/time/GetCurrentTime.hpp>
#include "OperatingSystem/GetLastErrorCode.hpp" //OperatingSystem::GetLastErrorCode()
#ifdef __linux__
#include <OperatingSystem/Linux/EnglishMessageFromErrorCode/EnglishMessageFromErrorCode.h>
#endif
#include <OperatingSystem/BSD/socket/getSocketError.h>///getSocketError(...)
#include <OperatingSystem/BSD/socket/socketTimeout.h>///getSocketTimeout(...)
///setNonBlockingSocket(...), setBlockingSocket(...)
#include <OperatingSystem/BSD/socket/setBlockingMode.h>
///isNonBlckngCnnctRtrnCode()
#include <OperatingSystem/BSD/socket/isNonBlckngCnnctRtrnCode.h>
///OperatingSystem::BSD::sockets::NonBlckingIsCnnctd(...)
#include <OperatingSystem/BSD/socket/NonBlockingConnect.h>

///https://docs.microsoft.com/de-de/windows/win32/winsock/porting-socket-applications-to-winsock
using namespace OperatingSystem::BSD::sockets;

fastestSignedDataType SMARTmonitorClient::ReadNumFollowingBytes()
{
  LOGN("reading 2 bytes from socket #" << m_socketFileDesc)
  uint16_t numDataBytesToRead;
  const size_t numBytesToRead = 2;
  //https://stackoverflow.com/questions/3053757/read-from-socket:
#ifdef __linux__
  int numBinRead = 0;
  ioctl(m_socketFileDesc, FIONREAD, &numBinRead);
  LOGN_DEBUG("# bytes in read buffer for socket:" << numBinRead)
#endif
  unsigned numBytesRead;
  //TODO connection to service error here when expecting data for the 2nd time 
  // running the wx GUI. errno: 11 from GetSMARTattrValsFromSrv
  // Maybe because the 2nd time is from another thread.
  //TODO Did not continue:message stays: "reading number of bytes for S.M.A.R.T.
  // data" after PC with S.M.A.R.T. server shut down/service process killed with
  // "-SIGKILL". Disconnecting via
  // "Disconnect" button was impossible. Create at least 1 unit test for this
  // case. Maybe a signal handler with EPIPE for client helps?
  // Maybe "netstat -tn src :>>S.M.A.R.T. server port<<" does not show it after
  // loss of connection resp. it shows state "CLOSE_WAIT".
  
  ///https://stackoverflow.com/questions/283375/detecting-tcp-client-disconnect
  int rdErrno;
  int rdFrmScktRslt = OperatingSystem::BSD::sockets::readFromSocket2(
    m_socketFileDesc,& numDataBytesToRead, numBytesToRead, & numBytesRead,
    & rdErrno);
  if(numBytesRead < (int) numBytesToRead){
    HandleTransmissionError(numBytesToReceive,
      /*numBytesRead < 0 ? 0 : numBytesRead*/numBytesRead,
      numBytesToRead,
      rdErrno);
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
  int rdFrmScktRslt;
  unsigned numBytesRead;
  SetCurrentAction(readNumBytesForSMARTdata);
  ///Value is 0 if connect to server for the 2nd call of this function.
  ///Value is garbage if connect to server in another thread for the 2nd call 
  /// of this function.
  /**Sometimes used the 1st 2 bytes from XML data ("<d" from "<data carrier ..."
   * as num bytes (15460dec=3C64hex. due to multiple threads read from socket.*/
  fastestSignedDataType numBytesToRead = ReadNumFollowingBytes();
  if(numBytesToRead < 1){
    LOGN_ERROR("read 0 B->return")
    return readLessBytesThanIntended;
  }
  const fastestUnsignedDataType numBytesToAllocate = numBytesToRead + 1;
  uint8_t SMARTdataXML[numBytesToAllocate];
  if(SMARTdataXML)
  {
    LOGN_DEBUG("read " << numBytesToRead << "B from socket file desc " <<
      m_socketFileDesc)
#ifdef __linux__
    //https://stackoverflow.com/questions/3053757/read-from-socket:
    int numBinRead = 0;
    ioctl(m_socketFileDesc, FIONREAD, &numBinRead);
#endif
    SetCurrentAction(readSMARTvaluesXMLdata);
    int rdError;
    /** http://man7.org/linux/man-pages/man2/read.2.html :
     *  "On error, -1 is returned, and errno is set appropriately." */
    rdFrmScktRslt = readFromSocket2(m_socketFileDesc,
      SMARTdataXML, numBytesToRead, & numBytesRead, & rdError);
    SetCurrentAction(hasReadSMARTvaluesXMLdata);
    //TODO often numBytesRead < numBytesToRead if this function is called from 
    //  "UpdateSMARTparameterValuesThreadFunc"
    if(numBytesRead < numBytesToRead) {
      HandleTransmissionError(SMARTparameterValues,
        //numBytesRead < 0 ? 0: numBytesRead,
        numBytesRead, numBytesToRead, rdError);
      LOGN_ERROR("read less bytes (" << numBytesRead << ") than expected (" 
        << numBytesToRead << ")");
      std::string stdstrXML((char*)SMARTdataXML, numBytesRead);
      LOGN_ERROR("content:" << stdstrXML)
      return readLessBytesThanIntended; //TODO provide error handling (show message to user etc.)
    }
    SMARTdataXML[numBytesToRead] = '\0';
    SMARTuniqueIDandValues sMARTuniqueIDandValues;
    
    /**To let the values be updated in UI (same pointer to SMARTuniqueID object)
    *:Create SMARTuniqueIDandValues object only via unique ID (no SMART values).
    * Then search in the container via "sMARTuniqueIDandValsCont.find(
    * sMARTuniqueIDandValues);" and  use the iterator.
    * Alternative:(slower?) also fill the SMART attribute values from the server
    * data and copy to the SMARTuniqueIDandValues object if already contained in
    * sMARTuniqueIDandValsCont after "sMARTuniqueIDandValsCont.insert(...)".*/
    const bool succBeganSrvDataProc = srvDataProcessor.Begin(SMARTdataXML,
      numBytesToRead);
    if(succBeganSrvDataProc)
      srvDataProcessor.GetSMARTuniqueID(sMARTuniqueIDandValues);
    
    LOGN("SMART unique ID and values object " << & sMARTuniqueIDandValues )
    //sMARTuniqueIDandValuesContainer.f
    std::pair<std::set<SMARTuniqueIDandValues>::iterator, bool> insert = 
      sMARTuniqueIDandValsCont.insert/*emplace*/(sMARTuniqueIDandValues);
    LOGN("insered object into container?:" << insert.second);
    if(insert.second)///<=>not already contained/inserted
    {
      LOGN("SMART unique ID and values object in container:" << &(*insert.first) )
    }
    if(succBeganSrvDataProc)
      srvDataProcessor.GetSMARTrawValues(
        /**Use the version from the container so it is ensured to use the same
         * pointer to the SMARTuniqueID object.*/
        (SMARTuniqueIDandValues &) *insert.first);
    //RebuildGUI();
    OperatingSystem::GetCurrentTime(m_timeOfLastSMARTvaluesUpdate);
//    delete [] SMARTvalues;
  }
  else
  {
    LOGN_ERROR("Failed to allocate " << numBytesToAllocate << " bytes")
  }
  LOGN("return " << successfull)
  return successfull;
}

struct SocketConnectThreadFuncParams
{
  int socketFileDesc;
  struct sockaddr_in srvAddr;
  SMARTmonitorClient * p_SMARTmonitorClient;
  fastestUnsignedDataType connectTimeoutInSeconds;
  int errNo;
  void AfterConnectToServer()
  {
//    p_SMARTmonitorClient->CloseServerConnectStatusUI();
  }
};

//#ifdef __linux__
///\return -1 if connection failed.
///Non-Blocking is an alternative option to set a self-defined connect timeout.
int ConnectToSocketNonBlocking(
//  SocketConnectThreadFuncParams & socketConnectThreadFuncParams
  int socketFileDescriptor, 
  struct sockaddr_in & serv_addr, 
  long int connectTimeoutInSeconds,
  SMARTmonitorClient * p_smartMonClient,
  int & errNo
  )
{
  errNo = -1;
    
  /** Make non-blocking because see
   * http://www.linuxquestions.org/questions/programming-9/why-does-connect-block-for-a-long-time-708647/ */
  //http://stackoverflow.com/questions/29598508/how-to-get-out-from-a-tcp-blocking-connect-call
  //https://stackoverflow.com/questions/17769964/linux-sockets-non-blocking-connect
  setNonBlockingSocket(socketFileDescriptor);

  p_smartMonClient->SetCurrentAction(SMARTmonitorClient::nonBlckCnnctToSrv);
  /** http://man7.org/linux/man-pages/man2/connect.2.html :
   *  "If the connection or binding succeeds, zero is returned. On error,
   *  -1 is returned, and errno is set appropriately." */
  int result = connect(socketFileDescriptor, (struct sockaddr *) & serv_addr,
    sizeof(serv_addr) );
  if(result == -1)///connect(...) returns -1 for non-blocking sockets
  {
    if(OperatingSystem::BSD::sockets::isNonBlckngCnnctRtrnCode() )
    {
//      fd_set wfd;
      fd_set readFileDescriptorSet;
      FD_ZERO(&readFileDescriptorSet);
     ///https://docs.microsoft.com/de-de/windows/win32/winsock/select-and-fd---2
      FD_SET(socketFileDescriptor, &readFileDescriptorSet);

      /** Values for members tv_sec and tv_usec change after/while executing
       *  "select(...)" */
      struct timeval socketConnectTimeout;
      socketConnectTimeout.tv_sec = connectTimeoutInSeconds;
      socketConnectTimeout.tv_usec = 0;

#ifdef __linux__
      ///from https://linux.die.net/man/2/pselect , section "Description"
      sigset_t origmask, sigmask;
      /** https://www.gnu.org/software/libc/manual/html_node/Signal-Sets.html
       * for being able to stop in select().from the User Interface thread via
       * a signal. */
      int i = sigfillset(&sigmask);
      pthread_sigmask(SIG_SETMASK, &sigmask, &origmask);
#endif

      /** Because returning from select(...) may take some (see its last
       * parameter) seconds->show timeout in UI.*/
      p_smartMonClient->startSrvCnnctCntDown();
      /** https://linux.die.net/man/2/select :
       * "On success, select() and pselect() return the number of file
       * descriptors contained in the three returned descriptor sets (that is,
       * the total number of bits that are set in readfds, writefds, exceptfds)
       * which may be zero if the timeout expires before anything interesting
       * happens. On error, -1 is returned, and errno is set appropriately; the
       * sets and timeout become undefined, so do not rely on their contents
       * after an error." */
      const int selectRslt = 
        /** https://linux.die.net/man/2/select :
          * "waiting until one or more of the file descriptors become
          * "ready" for some class of I/O operation (e.g., input possible)."*/
///https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-select
        select(/** "This argument should be set to the highest-numbered
        * file descriptor in any of the three sets, plus 1.  The indicated
        * file descriptors in each set are checked, up to this limit
        * (but see BUGS)."
        * https://www.gnu.org/software/libc/manual/html_node/Waiting-for-I_002fO.html
        * : "The select function checks only the first nfds file descriptors."
        * -> checks >nfds< lowmost bits of all max. 3 fds bitmasks ? */
        socketFileDescriptor + 1, & readFileDescriptorSet,
        /**write file descriptors*/NULL,
        /**except file descriptors*/NULL,
        &socketConnectTimeout);
      //TODO: evaluate errno/call WSAGetLastError() after calling select(...)?
      /** File Descriptor. set included 1st param val of select() after select(...)
       * when the connection was successful. */
      pthread_sigmask(SIG_SETMASK, &origmask, NULL);
//      int errorNumber = errno;
      if(selectRslt > 0) /** > 0 ready file descriptors */
      {
//        result = 0;
//        FD_ISSET()

        int iSO_ERROR = getSocketError(socketFileDescriptor);

        /** Change back to blocking mode. */
//        setBlockingSocket(socketFileDescriptor);

        int getpeernameErrNo;
        const bool isConnected = OperatingSystem::BSD::sockets::
          NonBlckingIsCnnctd(
            socketFileDescriptor,
            *( (struct sockaddr *) & serv_addr),
            sizeof(serv_addr),
            getpeernameErrNo);
        iSO_ERROR = getSocketError(socketFileDescriptor);
        const int isSet =FD_ISSET(socketFileDescriptor,& readFileDescriptorSet);
//        if(/*isSet*/ iSO_ERROR > 0){
          /*close(socketFileDescriptor);
          result = -1;*/
//        }
        /** Result is sometimes 0 even if not connected(e.g.if 2nd time
         *  connection attempt with that socket FD?)? */
        /** In Lubuntu 16.04 Ubuntu 16.04.4 LTS:
         *  select(...) returned 1 but "errno" was 115 after select(...) */
        /*else*/ if(isConnected /*&& getsockoptRslt == 0*/ && selectRslt > 0
#ifdef __linux__
       /** iSO_ERROR is always -1 under non-Linux/MicroSoft Windows, so ignore it
        * in these cases. */
          && iSO_ERROR == 0
#endif
          )
        {
          LOGN_INFO("successfully connected to " << serv_addr.sin_addr.s_addr)
//          p_smartMonClient->SetCurrentAction(SMARTmonitorClient::cnnctdToSrv);
          //TODO result was success even if the server/service is not running
          /** Change back to blocking mode. */
          setBlockingSocket(socketFileDescriptor);
          errNo = 0;
        }
        else{
          close(socketFileDescriptor);
          /** Error on Lubuntu 16.04 Ubuntu 16.04.4 LTS:
           *  select(...) returned 1 but "errno" was 115 after select(...) */
          if(iSO_ERROR != 0)
            errNo = errno;
          else
            errNo = getpeernameErrNo;
       }
//          result = iSO_ERROR;
      }
      /** https://man7.org/linux/man-pages/man2/select.2.html : "The return
       * value may be zero if the timeout expired before any file descriptors
       * became ready." */
      else if(selectRslt == 0) // time out occurred
      {
        close(socketFileDescriptor);
        errNo = ETIMEDOUT;
        result = -1;
      }
      else///return value of "select" = -1
      {
        /** https://linux.die.net/man/2/select :
         *[...]On error, -1 is returned, and errno is set appropriately [...]"*/
        result = errno;
//        enum connStep = select;
        errNo = errno;
      }
    }
/**Preparing to connect/calling connect(...) failed<=>"errno"<>EINPROGRESS in
 * Linux/WSAGetLastError()<>WSAEWOULDBLOCK in Microsoft Windows.*/
    else{
/**Do not rely on "errno" because in >common_sourcecode<\OperatingSystem\Windows
 * \BSD_sockets\isNonBlckngCnnctRtrnCode.h for OperatingSystem::BSD::sockets::
 * isNonBlckngCnnctRtrnCode() errno was 0 although "WSAEFAULT" means an error.*/
//      errNo = errno;
//     connStep = connect;
      result = -1;
    }
  }
  return result;
}
//#endif///#ifdef __linux__

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
      p_socketConnectThreadFuncParams->srvAddr,
      p_socketConnectThreadFuncParams->connectTimeoutInSeconds,
      p_socketConnectThreadFuncParams->p_SMARTmonitorClient,
      p_socketConnectThreadFuncParams->errNo
      );
  
    if(/*connectResult < 0*/ p_socketConnectThreadFuncParams->errNo != 0)
    {
//      HandleConnectionError(hostName);
//      return errorConnectingToService;
      /**Close the socket here because so it is done for all user interfaces.*/
      close(p_socketConnectThreadFuncParams->socketFileDesc);
    }
    else/*if(connectResult == 0)*/
    {
      LOGN("successfully connected")
      p_socketConnectThreadFuncParams->p_SMARTmonitorClient->ShowMessage(
        "successfully connected to service", UserInterface::MessageType::success);
    }
    p_socketConnectThreadFuncParams->p_SMARTmonitorClient->AfterConnectToServer(
      /*connectResult*/p_socketConnectThreadFuncParams->errNo);
    delete p_socketConnectThreadFuncParams;
    return connectResult;
  }
  return -1;
}

DWORD InterruptableBlckngCnnctToSrvThrdFn(void * p_v)
{
  int cnnctRslt = -1;
  SocketConnectThreadFuncParams * p_socketConnectThreadFuncParams = 
    (SocketConnectThreadFuncParams *) p_v;
  if( p_socketConnectThreadFuncParams)
  {
#ifdef __linux__ ///Works under Linux
    ///from https://linux.die.net/man/2/pselect , section "Description"
    sigset_t origmask, sigmask;
    /** https://www.gnu.org/software/libc/manual/html_node/Signal-Sets.html
     * for being able to stop in select().from the User Interface thread via
     * a signal. */
    int i = sigfillset(&sigmask);
    pthread_sigmask(SIG_SETMASK, &sigmask, &origmask);
#endif
    SMARTmonitorClient * p_SMARTmonitorClient = p_socketConnectThreadFuncParams
      ->p_SMARTmonitorClient;
    /** Because returning from connect(...) may take some (see its last
     * parameter) seconds->show timeout in UI.*/
    //TODO does not stop connecting after timeout elapses
    //TODO does not change state from unknown after connection was lost and
    // successful reconnect
    p_SMARTmonitorClient->startSrvCnnctCntDown();
    p_SMARTmonitorClient->SetCurrentAction(SMARTmonitorClient::cnnctToSrv);
    /** https://linux.die.net/man/3/connect :"Upon successful completion,
     * connect() shall return 0; otherwise, -1 shall be returned and errno set
     * to indicate the error." */
    cnnctRslt = connect(p_socketConnectThreadFuncParams->socketFileDesc,
      (struct sockaddr *) & p_socketConnectThreadFuncParams->srvAddr,
      sizeof(p_socketConnectThreadFuncParams->srvAddr) );
//    if(cnnctRslt == -1){
      int errNo = errno;
//    }
#ifdef __linux__ ///Works under Linux
    pthread_sigmask(SIG_SETMASK, &sigmask, &origmask);
#endif
    p_SMARTmonitorClient->AfterConnectToServer(/*cnnctRslt*/errNo);
    delete p_socketConnectThreadFuncParams;
  }
  return cnnctRslt;
}

fastestUnsignedDataType SMARTmonitorClient::ConnectToServer(
  const char * hostName, bool asyncConnect)
{
  regCancelSelectSigHandler();
  OperatingSystem::BSD::sockets::InitSocket();//TODO close the socket at end
  struct sockaddr_in srvAddr;
  fastestUnsignedDataType prepCnnctToSrvRslt = prepCnnctToSrv(hostName,
    m_socketPortNumber, & srvAddr, AF_INET, & m_socketFileDesc);
  if(prepCnnctToSrvRslt != prepCnnctToSrvSucceeded)
    return prepCnnctToSrvRslt;
  ///TCP: client sends "SYN"; server sends "ACK", client "SYNACK"
  double cnnctTimeoutInS;
  GetSocketTimeoutInS(m_socketFileDesc, & cnnctTimeoutInS);
  if(cnnctTimeoutInS == 0)///0 means ca. 90s under Linux.
    cnnctTimeoutInS = 30;
//  MessageBox msgBox();
  
  DWORD currentThreadNumber = OperatingSystem::GetCurrentThreadNumber();
  //TODO execute socket connection in another thread than user interface thread.
  // in order to be interruptable
//  if( currentThreadNumber == s_UserInterfaceThreadID)
//  {
//  }
  
  //TODO prevent multiple connection(s) (attempts) at the same time?:
  // CompareAndSwap(m_currCnnctnAttmpOrCnnctn, 1);
  // currCnnctnAttmpOrCnnctnCritsec.Enter();
#ifdef multithread
//#ifdef __linux__
  SocketConnectThreadFuncParams * p_socketCnnctThrdFnParams = new 
    SocketConnectThreadFuncParams {m_socketFileDesc, srvAddr, this,
      (fastestUnsignedDataType)cnnctTimeoutInS };
  if( asyncConnect )
  {
    ShowConnectionState(hostName, cnnctTimeoutInS);
    /** call in another thread in order to enable breaking connection */
    connectThread.start(SocketConnectThreadFunc, p_socketCnnctThrdFnParams);
    BeforeConnectToServer();
  }
  else///sychronous connect to server
//#endif
#endif
  /** http://man7.org/linux/man-pages/man2/connect.2.html :
      "If the connection or binding succeeds, zero is returned." */
//    return ConnectToSocketNonBlocking(
//      m_socketFileDesc, 
//      serv_addr, 
//      connectTimeoutInSeconds);
    connectThread.start(InterruptableBlckngCnnctToSrvThrdFn,
      p_socketCnnctThrdFnParams);
//  ShowConnectToServerStatus("cancel connection");
//  close(m_socketFileDesc);
//  connectThread.WaitForTermination();
  //bzero(buffer,256);
  return /*unconnectedFromService*/ prepCnnctToSrvRslt;
}

void SMARTmonitorClient::AfterGetSMARTvaluesLoop(int getSMARTvaluesResult)
{
  close(m_socketFileDesc);
  if(getSMARTvaluesResult != 0)
  {
    StartSrvCnnctnAttmptCntDown(60);
  }
}

/**@param connectResult the errno from calling "connect" (blocking connect)/
 * "select" (non-blocking connect) */
void SMARTmonitorClient::HandleConnectionError(const char * hostName,
  const int connectResult)
{
  //TODO: show error via user interface
  //Message("")
  std::ostringstream oss;
  oss << "error connecting to S.M.A.R.T. values server at \"" << 
    m_stdstrServiceHostName << "\", port " << m_socketPortNumber << ":";
  //TODO the following is Linux-specific and should be OS-independent
  //see http://man7.org/linux/man-pages/man2/connect.2.html
  //TODO errno must come from connect(...), no other function call inbetween
  //TODO use error description from common_sourcecode/.../BSD/socket
  //BlockingCnnct::getPossibleCause
  switch(/*errno*/ connectResult)
  {
    //see http://man7.org/linux/man-pages/man2/connect.2.html
    case ECONNREFUSED :
      oss << "No process listening on the remote address";
      break;
    case ETIMEDOUT :
      oss << "connection timed out";
     break;
#ifdef __linux__ ///EBADFD does not exist in MinGW
    case EBADFD :///https://man7.org/linux/man-pages/man2/connect.2.html
      oss << "The socket file descriptor #" << m_socketFileDesc <<
        " is not a valid open file descriptor.";
      break;
#endif
    default :
    {
//      const int errorCode = OperatingSystem::GetLastErrorCode();
#ifdef __linux__
      oss << OperatingSystem::EnglishMessageFromErrorCode(connectResult) <<
        " (OS error code: " << /*errorCode*/connectResult << ")";
#endif
    }
    break;
  }
  ShowMessage(oss.str().c_str(), MessageType::error );
  LOGN_ERROR(oss.str() )
}
