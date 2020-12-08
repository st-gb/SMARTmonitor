#ifdef __linux__
  #include <sys/ioctl.h>///FIONREAD
#endif
#include <OperatingSystem/BSD/socket/socket.h>///socket(...), InitSocket(...)
#include <OperatingSystem/BSD/socket/sockaddr_in.h>///struct sockaddr_in
#include <fcntl.h> //fcntl(...)

//#include "SocketOperations.h"
#include <client/SMARTmonitorClient.h>
#include <preprocessor_macros/logging_preprocessor_macros.h>
#include <OperatingSystem/time/GetCurrentTime.hpp>
#include "OperatingSystem/GetLastErrorCode.hpp" //OperatingSystem::GetLastErrorCode()
#ifdef __linux__
#include <OperatingSystem/Linux/EnglishMessageFromErrorCode/EnglishMessageFromErrorCode.h>
#endif
#include <OperatingSystem/BSD/socket/prepCnnctToSrv.h>///prepCnnctToSrv(...)
#include <OperatingSystem/BSD/socket/socketTimeout.h>///getSocketTimeout(...)

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
  int rdFrmScktRslt = OperatingSystem::BSD::sockets::readFromSocket2(
    m_socketFileDesc,& numDataBytesToRead, numBytesToRead, & numBytesRead);
  if(numBytesRead < (int) numBytesToRead){
    HandleTransmissionError(numBytesToReceive,
      /*numBytesRead < 0 ? 0 : numBytesRead*/numBytesRead,
      numBytesToRead);
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
    /** http://man7.org/linux/man-pages/man2/read.2.html :
     *  "On error, -1 is returned, and errno is set appropriately." */
    rdFrmScktRslt = readFromSocket2(m_socketFileDesc,
      SMARTdataXML, numBytesToRead, & numBytesRead);
    //TODO often numBytesRead < numBytesToRead if this function is called from 
    //  "UpdateSMARTparameterValuesThreadFunc"
    if(numBytesRead < numBytesToRead) {
      HandleTransmissionError(SMARTparameterValues,
        //numBytesRead < 0 ? 0: numBytesRead,
        numBytesRead, numBytesToRead);
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
  LOGN("end")
  return successfull;
}

struct SocketConnectThreadFuncParams
{
  int socketFileDesc;
  struct sockaddr_in srvAddr;
  SMARTmonitorClient * p_SMARTmonitorClient;
  fastestUnsignedDataType connectTimeoutInSeconds;
  void AfterConnectToServer()
  {
//    p_SMARTmonitorClient->CloseServerConnectStatusUI();
  }
};

#ifdef __linux__
///\return -1 if connection failed.
///Non-Blocking is an alternative option to set a self-defined connect timeout.
int ConnectToSocketNonBlocking(
  int socketFileDescriptor, 
  struct sockaddr_in & serv_addr, 
  long int connectTimeoutInSeconds,
  SMARTmonitorClient * p_smartMonClient)
{
  /** from http://www.linuxquestions.org/questions/programming-9/why-does-connect-block-for-a-long-time-708647/ */
//  fcntl(socketFD, F_SETFL, curflags | O_NONBLOCK);
  //http://stackoverflow.com/questions/29598508/how-to-get-out-from-a-tcp-blocking-connect-call
  //https://stackoverflow.com/questions/17769964/linux-sockets-non-blocking-connect
  ///see https://www.cs.odu.edu/~cs779/spring10/lectures/nonblockingIO.html
  int flags = fcntl(socketFileDescriptor, F_GETFL, 0);
  fcntl(socketFileDescriptor, F_SETFL, flags | O_NONBLOCK);

  /** http://man7.org/linux/man-pages/man2/connect.2.html :
   *  "If the connection or binding succeeds, zero is returned. On error,
   *  -1 is returned, and errno is set appropriately." */
  int result = connect(socketFileDescriptor, (struct sockaddr *) & serv_addr,
    sizeof(serv_addr) );
  if(result == -1)/// -1 for non-blocking sockets
  {
    if(errno == EINPROGRESS) /** For non-blocking _BSD_ sockets */
    {
//      fd_set wfd;
      fd_set readFileDescriptorSet;
      FD_ZERO(&readFileDescriptorSet);
      FD_SET(socketFileDescriptor, &readFileDescriptorSet);

      struct timeval socketConnectTimeout;
      socketConnectTimeout.tv_sec = connectTimeoutInSeconds;
      socketConnectTimeout.tv_usec = 0;

      ///from https://linux.die.net/man/2/pselect , section "Description"
      sigset_t origmask, sigmask;
      /** https://www.gnu.org/software/libc/manual/html_node/Signal-Sets.html
       * for being able to stop in select().from the User Interface thread via
       * a signal. */
      int i = sigfillset(&sigmask);
      pthread_sigmask(SIG_SETMASK, &sigmask, &origmask);

      /** Because returning from select(...) may take some (see its last
       * parameter) seconds->show timeout in UI.*/
      p_smartMonClient->startCnnctCountDown();
      /** https://linux.die.net/man/2/select :
       * "On success, select() and pselect() return the number of file
       * descriptors contained in the three returned descriptor sets (that is,
       * the total number of bits that are set in readfds, writefds, exceptfds)
       * which may be zero if the timeout expires before anything interesting
       * happens. On error, -1 is returned, and errno is set appropriately; the
       * sets and timeout become undefined, so do not rely on their contents
       * after an error." */
      result = 
        /** https://linux.die.net/man/2/select :
          * "waiting until one or more of the file descriptors become
          * "ready" for some class of I/O operation (e.g., input possible)."*/
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
      /** File Descriptor. set included 1st param val of select() after select(...)
       * when the connection was successful. */
      pthread_sigmask(SIG_SETMASK, &origmask, NULL);
//      int errorNumber = errno;
      if(result > 0) /** > 0 ready file descriptors */
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
        const int isSet =FD_ISSET(socketFileDescriptor,& readFileDescriptorSet);
        if(/*isSet*/ iSO_ERROR > 0){
          /*close(socketFileDescriptor);
          result = -1;*/
        }
        else if(result == 0 && iSO_ERROR == 0)//TODO result is 0 even if not connected
        {// (if 2nd time connection attempt with that socket FD?)
          
          LOGN_INFO("successfully connected to " << serv_addr.sin_addr.s_addr )
          //TODO result was success even if the server/service is not running
          /** Change back to blocking mode. */
          result = fcntl(socketFileDescriptor, F_SETFL, flags);
        }
        else
          close(socketFileDescriptor);
//          result = iSO_ERROR;
      }
      else if(result == 0) // time out occurred
      {
//        result = errno;
        close(socketFileDescriptor);
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
      p_socketConnectThreadFuncParams->srvAddr,
      p_socketConnectThreadFuncParams->connectTimeoutInSeconds,
      p_socketConnectThreadFuncParams->p_SMARTmonitorClient
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
#endif///#ifdef __linux__

DWORD InterruptableBlckngCnnctToSrvThrdFn(void * p_v)
{
  int cnnctRslt = -1;
  SocketConnectThreadFuncParams * p_socketConnectThreadFuncParams = 
    (SocketConnectThreadFuncParams *) p_v;
  if( p_socketConnectThreadFuncParams)
  {
    ///from https://linux.die.net/man/2/pselect , section "Description"
    sigset_t origmask, sigmask;
    /** https://www.gnu.org/software/libc/manual/html_node/Signal-Sets.html
     * for being able to stop in select().from the User Interface thread via
     * a signal. */
    int i = sigfillset(&sigmask);
    pthread_sigmask(SIG_SETMASK, &sigmask, &origmask);

    /** Because returning from connect(...) may take some (see its last
     * parameter) seconds->show timeout in UI.*/
    p_socketConnectThreadFuncParams->p_SMARTmonitorClient->startCnnctCountDown();
    cnnctRslt = connect(p_socketConnectThreadFuncParams->socketFileDesc,
      (struct sockaddr *) & p_socketConnectThreadFuncParams->srvAddr,
      sizeof(p_socketConnectThreadFuncParams->srvAddr) );
    pthread_sigmask(SIG_SETMASK, &sigmask, &origmask);
    p_socketConnectThreadFuncParams->p_SMARTmonitorClient->
      AfterConnectToServer(cnnctRslt);
    delete p_socketConnectThreadFuncParams;
  }
  return cnnctRslt;
}

fastestUnsignedDataType SMARTmonitorClient::ConnectToServer(
  const char * hostName, bool asyncConnect)
{
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
#ifdef multithread
#ifdef __linux__
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
  else
#endif
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
    case EBADFD :///https://man7.org/linux/man-pages/man2/connect.2.html
      oss << "The socket file descriptor #" << m_socketPortNumber <<
        " is not a valid open file descriptor.";
      break;
    default :
    {
      int errorCode = OperatingSystem::GetLastErrorCode();
#ifdef __linux__
      oss << OperatingSystem::EnglishMessageFromErrorCode(errorCode);
#endif
    }
    break;
  }
  ShowMessage(oss.str().c_str(), MessageType::error );
  LOGN_ERROR(oss.str() )
}
