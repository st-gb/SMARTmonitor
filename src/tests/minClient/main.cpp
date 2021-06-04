///Standard C(++) header files:
#include <fcntl.h>///fcntl(...)
#include <iostream>///std::cout, std::cerr
#include <string.h>///strerror(...)

/** Stefan Gebauer's common_sourcecode repository header files: */
///OperatingSystem::GetLastErrorCode(...)
#include <OperatingSystem/GetLastErrorCode.hpp>
///OperatingSystem::BSD::sockets::BlockingCnnctError::GetPossibleCause_inl(...)
#include <OperatingSystem/BSD/socket/blckngCnnctErrMsg.hpp>
#include <OperatingSystem/BSD/socket/prepCnnctToSrv.h>///prepCnnctToSrv(...)
#include <OperatingSystem/BSD/socket/socketTimeout.h>///getSocketTimeout(...)
///struct sockaddr_in, ntohs(...)
#include <OperatingSystem/BSD/socket/sockaddr_in.h>
#include <OperatingSystem/BSD/socket/socket.h>///readFromSocket(...)
typedef double TimeCountInSecType;///for GetTimeCountInSeconds(...)
///for GetTimeCountInNanoSeconds(...)
typedef long double TimeCountInNanosec_type;
#include <OperatingSystem/time/GetUpTime.h>///OperatingSystem::GetUptimeInS(...)

#include "cxxopts/handleCmdLineOpts.hpp"///cxxopts::HandleCmdLineOpts(...)
//#include "handleCmdLineOpts.hpp"///handleCmdLineArgs(...))
#include "ProgRetCodes.h"///enum ProgRetCodes

using namespace OperatingSystem::BSD::sockets;

//TODO could add a signal handler for "Ctrl+C"
int main(int argCount, char * argVec [])
{
  int port = 2000;/// > ca. 1024 is above well-known ports->needs less rights
  std::string srvHost = "localhost";
  /**Use same data type as member in "struct timeval" to avoid g++ compiler
   *  warning "warning: narrowing conversion [...] to '__time_t' [...]" */
  time_t timeoutInUs =
    ///12000 micros is a sensible minimal timeout to test for send timeout
    5000000;///5 s for via Internet
  cxxopts::HandleCmdLineOpts(argCount,/** Avoid GCC compiler "error: invalid
    *conversion from 'char**' to 'const char**' [-fpermissive]"*/(const char **)
    argVec, srvHost, port, timeoutInUs);
  struct sockaddr_in srvAddr;
  int socketFileDesc;
  OperatingSystem::BSD::sockets::InitSocket();
  enum PrepCnnctToSrvRslt cnnctToSrvRslt = prepCnnctToSrv(srvHost.c_str (),
    port, & srvAddr, AF_INET, & socketFileDesc);
  if(cnnctToSrvRslt != prepCnnctToSrvSucceeded){
    std::cerr << "Preparing connect to \"" << srvHost << "\",port " << port <<
      " failed:" << enErrorMsgs[cnnctToSrvRslt] << "->exiting." << std::endl;
    return cnnctToSrvRslt;
  }
  /**Use same data type as member in "struct timeval" to avoid g++ compiler
   *  warning "warning: narrowing conversion [...] to '__time_t' [...]" */
  time_t timeoutInS = 0;
  if(timeoutInUs > 1000000){
    timeoutInS = timeoutInUs / 1000000;
    timeoutInUs %= 1000000;
    }
  struct timeval tvSndTimeout{timeoutInS,timeoutInUs}, tvRcvTimeout;
  if( !(tvSndTimeout.tv_sec == 0 && tvSndTimeout.tv_usec == 0) )
    setSocketTimeout(socketFileDesc, & tvSndTimeout, SO_SNDTIMEO);
  getSocketTimeout(socketFileDesc, & tvSndTimeout, SO_SNDTIMEO);
  getSocketTimeout(socketFileDesc, & tvRcvTimeout, SO_RCVTIMEO);
  TimeCountInSecType upTimeInS;
  OperatingSystem::GetUptimeInS(upTimeInS);
  std::cout.precision(3);///# digits after decimal point
  std::cout << std::fixed << "Current uptime:" << upTimeInS << "s connecting to"
    " \"" << srvHost << "\"" << " port " << port << " wait max." << tvSndTimeout
    .tv_sec << "s" << tvSndTimeout.tv_usec << "micros" << std::endl;
  const int cnnctRslt = connect(socketFileDesc, (struct sockaddr *) & srvAddr,
    sizeof(srvAddr) );
  if(cnnctRslt == -1){///connect(...) failed
    unsigned long lastOSerrCode = OperatingSystem::GetLastErrorCode();
#ifdef __linux__
    int flags = fcntl(socketFileDesc, F_GETFL, 0);
#endif
    OperatingSystem::GetUptimeInS(upTimeInS);
    std::cout << "Current uptime:" << upTimeInS << "s connecting failed. OS "
      "error code:" << lastOSerrCode << " " << strerror(lastOSerrCode) <<
      std::endl;
    const enum errorCodes errorCode = OperatingSystem::BSD::sockets::
      GetLastError();
    const std::string possibleCause = BlockingCnnctError::GetPossibleCause_inl(
      errorCode, port);
    std::cout << possibleCause << std::endl;
    return cnnctToSrvFailed;
  }
  OperatingSystem::GetUptimeInS(upTimeInS);
  std::cout << "Current uptime:" << upTimeInS << "s connected. port:" <<
    srvAddr.sin_port << std::endl;
  uint16_t numDataBytesToRead;
  int numSocketBytesRead;
  size_t numSocketBytesToRead = 2;
  while(1){///Loop for supported S.M.A.R.T. IDs and S.M.A.R.T. data values.
    numSocketBytesRead = OperatingSystem::BSD::sockets::readFromSocket(
      socketFileDesc, & numDataBytesToRead, numSocketBytesToRead);
    if( numSocketBytesRead < numSocketBytesToRead){
      std::cerr << "Read " << numSocketBytesRead << " B instead of "<<
        numSocketBytesToRead <<" B->exiting."<< std::endl;
      return readLessThanIntended;
    }
    numDataBytesToRead = ntohs(numDataBytesToRead);
    std::cout << "# XML bytes to read:" << numDataBytesToRead << std::endl;

    const unsigned numBytesToAlloc = numDataBytesToRead + 1;
    uint8_t SMARTvalues[numBytesToAlloc];//TODO change to heap allocation?
    //TODO Can variable-length arrays be NULL? Depends on compiler?
    if(! SMARTvalues){
      std::cerr << "Failed to allocate " << numBytesToAlloc << " bytes array" <<
        std::endl;
      return allocMemFailed;}
    /** http://man7.org/linux/man-pages/man2/read.2.html :
     *  "On error, -1 is returned, and errno is set appropriately." */
    numSocketBytesRead = OperatingSystem::BSD::sockets::readFromSocket(
      socketFileDesc, SMARTvalues, numDataBytesToRead);
    SMARTvalues[numDataBytesToRead] = '\0';
    //printf("%s\n", (char*)SMARTvalues);
    std::cout << std::string((char*)SMARTvalues, numDataBytesToRead) << std::
      endl;
  }
  return 0;
}
