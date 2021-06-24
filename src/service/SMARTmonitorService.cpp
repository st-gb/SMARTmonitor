/** Author: Stefan Gebauer, M.Sc. Comp.Sc.
 * Created on 19. November 2016, 22:13 */

///Standard C(++) header files:
#include <sys/socket.h> //socket(...), bind(...), ...)
#include <netinet/in.h> //struct sockaddr_in
#include <vector>///class std::vector

///Files from Stefan Gebauer's "common_sourcecode" repository:
#include "Controller/time/GetTickCount.hpp" //OperatingSystem::GetTimeCountInNanoSeconds(...)
#include "hardware/CPU/atomic/AtomicExchange.h" // AtomicExchange(...)
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
///OperatingSystem::BSD::sockets::initSrv(...)
#include <OperatingSystem/BSD/socket/initSrv.h>

#include "SMARTmonitorService.hpp"
#include <attributes/SMARTattributeNameAndID.hpp> //SMARTattributeNameAndID

/** Definitions of class (static) variables. */
int SMARTmonitorService::s_socketFileDesc = 0;
SMARTmonitorService * SMARTmonitorService::p_SMARTmonSvc;

SMARTmonitorService::SMARTmonitorService() 
  //: 
  /** https://computing.llnl.gov/tutorials/pthreads/#ConditionVariables :
       [...] must be initialized before they can be used */
  //cond(PTHREAD_COND_INITIALIZER)
{
  /** from https://computing.llnl.gov/tutorials/pthreads/#ConditionVariables */
   pthread_mutex_init(& mutex, NULL);
   pthread_cond_init (&cond, NULL);
  p_SMARTmonSvc = this;
}

SMARTmonitorService::SMARTmonitorService(const SMARTmonitorService& orig) {
}

SMARTmonitorService::~SMARTmonitorService() {
  //WaitForSignal();
  /**  https://computing.llnl.gov/tutorials/pthreads/#ConditionVariables :
   *  "free a condition variable that is no longer needed." */
  pthread_cond_destroy(& cond);
  pthread_mutex_destroy(& mutex);
}

void SMARTmonitorService::EndUpdateSMARTvaluesThread()
{
  LOGN("ending get SMART values loop");
  /** Inform the SMART values update thread about we're going to exit,*/
  AtomicExchange( (long *) & s_updateSMARTvalues, 0);
}

void SMARTmonitorService::WaitForSignal()
{
  LOGN("locking signal mutex")
  pthread_mutex_lock(& mutex);
  LOGN("Waiting for signal")
  int i = pthread_cond_wait(& cond, & mutex);
  if( i != 0)
    LOGN_ERROR("pthread wait failed")
  else
    LOGN("pthread wait succeeded")
  LOGN("Unlocking signal mutex")
  pthread_mutex_unlock(&mutex);
  
  //http://stackoverflow.com/questions/10619952/how-to-completely-destroy-a-socket-connection-in-c
  //TODO close all sockets etc. to prevent bind(...) (to socket) here?
  LOGN("end")
}

//<data_carrier model="HGST HTS725050A7E630" firmware="GH2OA560" serial_number="TF755AWHKU3RVM"><SMART ID="5" raw_value="0" time_in_s="122806.328"/><SMART ID="9" raw_value="44658000000" time_in_s="122806.344"/><SMART ID="196" raw_value="0" time_in_s="122806.383"/><SMART ID="197" raw_value="0" time_in_s="122806.398"/><SMART ID="198" raw_value="0" time_in_s="122806.414"/><SMART ID="199" raw_value="0" time_in_s="122806.438"/></data_carrier>
void SMARTmonitorService::SendSupportedSMART_IDsToClient(const int clientSocketFileDesc)
{
  SMARTaccess_type & SMARTaccess =  * mp_SMARTaccess;
  /** Send to all clients non-blocking */
  const fastestUnsignedDataType numberOfDifferentDrives = SMARTaccess.
    GetNumberOfDifferentDrives();
  std::set<SMARTuniqueIDandValues> & SMARTuniqueIDsAndValues = SMARTaccess.
    GetSMARTuniqueIDandValues();
  LOGN("address:" << & SMARTuniqueIDsAndValues )
  std::set<SMARTuniqueIDandValues>::const_iterator SMARTuniqueIDandValuesIter =
    SMARTuniqueIDsAndValues.begin();

  std::ostringstream std_ostringstream;
  fastestUnsignedDataType SMARTattrID;
  for(fastestUnsignedDataType currentDriveIndex = 0;
    currentDriveIndex < numberOfDifferentDrives;
    ++ currentDriveIndex, SMARTuniqueIDandValuesIter ++)
  {
    const SMARTuniqueID & SMARTuniqueID = SMARTuniqueIDandValuesIter->
      getSMARTuniqueID();
    
    //TODO mapping from data carrier ID to device name
    dataCarrierID2devicePath_type::const_iterator dataCarrierID2dvcPathIter =
      s_dataCarrierID2devicePath.find(SMARTuniqueID);
    if( dataCarrierID2dvcPathIter != s_dataCarrierID2devicePath.end() )
    {
      const char * device = dataCarrierID2dvcPathIter->second.c_str();
      LOGN_DEBUG("device file path belonging to (SMART) data carrier ID:"
         << device )
      suppSMART_IDsType SMARTattrNamesAndIDs;
      //TODO Getting supported SMART attrs only needs to be done once for a drive
      // should be the same for all same model or same model, firmware
      // Can store this in a dataCarrierID2supportedSMARTattributesMap_type
      SMARTaccess.GetSupportedSMART_IDs(device, SMARTattrNamesAndIDs);
      suppSMART_IDsType::const_iterator supportedSMARTattrIDsIter =
        SMARTattrNamesAndIDs.begin();
      if( supportedSMARTattrIDsIter != SMARTattrNamesAndIDs.end() )
      {
        SMARTattrID = supportedSMARTattrIDsIter->GetID();
        std_ostringstream << "<data_carrier model=\"" << SMARTuniqueID.m_modelName << "\""
          << " firmware=\"" << SMARTuniqueID.m_firmWareName << "\""
          << " serial_number=\"" << SMARTuniqueID.m_serialNumber << "\">"
          << "<supportedSMART_IDs>" << SMARTattrID;
        for(supportedSMARTattrIDsIter ++;//TODO why incr. here?
            supportedSMARTattrIDsIter != SMARTattrNamesAndIDs.end();
            supportedSMARTattrIDsIter ++ )
        {
          SMARTattrID = supportedSMARTattrIDsIter->GetID();
          std_ostringstream << "," << SMARTattrID;
        }
        std_ostringstream << "</supportedSMART_IDs>";
        std_ostringstream << "</data_carrier>";
        std::string xmlString = std_ostringstream.str();
        LOGN_DEBUG("XML data to send:" << xmlString)
        SendBytesTo1Client(clientSocketFileDesc, xmlString);
      }
    }
  }
  LOGN_DEBUG("end")
}

fastestUnsignedDataType SMARTmonitorService::BindAndListenToSocket()
{
  fastestUnsignedDataType retCode = SMARTmonitorService::unset;
  //Code adapted from http://www.linuxhowtos.org/data/6/server.c
  struct sockaddr_in server_address;
  const enum OperatingSystem::BSD::sockets::InitSrvRslt initSrvRslt =
    OperatingSystem::BSD::sockets::initSrv(
      server_address,
      m_socketPortNumber,
      AF_INET,
      SOCK_STREAM,
      s_socketFileDesc
      );

  const int socketFileDesc = s_socketFileDesc;

  if(initSrvRslt != OperatingSystem::BSD::sockets::boundToSocket)
  {
    char * errorMessageForErrno = NULL;
    //https://linux.die.net/man/2/bind
    if( errno != 0 )
      errorMessageForErrno = strerror(errno);
//    switch(errno)
//    {
//      case EADDRINUSE :  
//    }
    std::string std_strErrorMessage = "Failed to bind server";
    if( errorMessageForErrno)
      std_strErrorMessage += errorMessageForErrno;
    LOGN_ERROR(std_strErrorMessage)
    std_strErrorMessage += "->exiting";
    ShowMessage(std_strErrorMessage.c_str() );
    retCode = failedToBindToSocket;
  }
  else
  {
    LOGN_INFO("bound server to socket " << socketFileDesc)
    //Versetzt einen gebundenen STREAM (TCP/IP) Socket in einen Lauschen-Modus. Wird auf Server-Seite benutzt.
    if( listen(socketFileDesc, 5) == 0 )
      retCode = listeningToSocket;
    else 
      retCode = failedToListenToSocket;
  }
  return retCode;
}

DWORD SMARTmonitorService::ClientConnThreadFunc( void * p_v)
{
  LOGN_DEBUG("begin")
  SMARTmonitorService * p_SMARTmonitor = (SMARTmonitorService * ) p_v;
  if( p_SMARTmonitor )
  {
    struct sockaddr_in client_address;
    do
    {
      socklen_t sizeOfClientAddrInB = sizeof(client_address);
      LOGN("Waiting for a socket client to accept on file descr." << 
        s_socketFileDesc << ",port " << p_SMARTmonitor->m_socketPortNumber)
      /** accept(...) is blocking, can be cancelled with "shutdown(...)" (from
       * another thread)*/
      const int clientSocketFileDesc = accept(
        s_socketFileDesc, 
        (struct sockaddr *) & client_address, 
        & sizeOfClientAddrInB);
      if (clientSocketFileDesc < 0)
      {
        ///was EINVAL when called by "shutdown".
        if(errno != EINVAL)///Error INVALid
          LOGN_ERROR("Failed to accept client:" << errno)
        //return 2;
      }
      else
      {
        LOGN("new client at port " << client_address.sin_port 
          << " file desc.:" << clientSocketFileDesc)
        //TODO does it make problems when sending from 2 different threads?
          // (here and in SMART values thread)
        //https://stackoverflow.com/questions/7418093/use-one-socket-in-two-threads
        p_SMARTmonitor->SendSupportedSMART_IDsToClient(clientSocketFileDesc);
        p_SMARTmonitor->AddClient(clientSocketFileDesc);
      }
    }while(p_SMARTmonitor->s_updateSMARTvalues);
    LOGN("after \"wait for socket connection\" loop")
    /**Prevent the bind(...) failure after restarting this program (not tested)*/
    p_SMARTmonitor->CloseAllClientSockets();
  }
  LOGN_DEBUG("end")
  return 0;
}

void SMARTmonitorService::AddClient(const int clientSocketFileDesc)
{
  m_clientsCriticalSection.Enter();
  //m_clientSocketFileDescVector.push_back(clientSocketFileDesc);
  LOGN("adding client with file descr. " << clientSocketFileDesc)
  m_clientSocketFileDescSet.insert(clientSocketFileDesc);
  m_clientsCriticalSection.Leave();  
}

void SMARTmonitorService::AfterGetSMARTvaluesLoop(int getSMARTvaluesResult) {
  LOGN("closing server socket file descr." << s_socketFileDesc)
  //from http://www.mombu.com/microsoft/t-how-to-cancel-a-blocking-socket-call-12612626.html
  //"Is the server multithreaded? If so, then closing the socket on another 
  //thread will unblock the accept() call."
  //see http://stackoverflow.com/questions/2486335/wake-up-thread-blocked-on-accept-call
  const int shutdownResult = shutdown(s_socketFileDesc, SHUT_RDWR);
  if( shutdownResult == 0)
    LOGN("successfully shut down server socket")
  else
    LOGN_ERROR("Error shuting down server socket")
  close(s_socketFileDesc);
//  LOGN("closing all client socket file descr." << s_socketFileDesc)
//  m_clientsCriticalSection.Enter();
//  m_clientSocketFileDescVector.push_back(clientSocketFileDesc);
//  m_clientsCriticalSection.Leave();

  LOGN("locking mutex for signalling")
  pthread_mutex_lock(&mutex);
  LOGN("signalling the main thread to end")
  int i = pthread_cond_signal(& cond);
  if( i != 0)
    LOGN_ERROR("pthread signal failed")
  else
    LOGN("pthread signal succeeded")
  LOGN("Unlocking mutex for signalling")
  pthread_mutex_unlock(& mutex);
  LOGN("end")
}

void SendSMART_IDandRawValue()
{
//  uint8_t array[5];
//  array[0] = SMARTattributeID;
//  memcpy( & array[1], & SMARTrawDataInHostFormat, 4);
//  uint32_t SMARTrawDataInHostFormat;
//  SMARTrawDataInHostFormat = htonl(currentSMARTrawValue);
}

void SMARTmonitorService::RemoveFileDescsOfBrokenSocketConns(
  std::vector<int> & fileDescsToDelete)
{
  //TODO remove broken connections from clients list and stop get SMART data loop
  // if no more clients
  int clientSocketFileDesc;
  for( std::vector<int>::const_iterator iter = fileDescsToDelete.begin();
    iter != fileDescsToDelete.end(); iter ++)
  {
    clientSocketFileDesc = * iter;
    LOGN("Removing client with file descr. " << clientSocketFileDesc
      << " from list because of broken socket connection")
    m_clientSocketFileDescSet.erase(clientSocketFileDesc);
  }
}

uint8_t * ByteArrayFromString(
  const std::string & xmlString,
  uint16_t & numOverallBytes)
{
  numOverallBytes = xmlString.length() + 2;
  LOGN("# overall bytes to send:" << numOverallBytes << " # XML data bytes:" 
    << numOverallBytes - 2)
  uint8_t * array = new uint8_t[numOverallBytes];
  
  memcpy(array + 2, xmlString.c_str(), numOverallBytes - 2);
  const uint16_t byteSizeInNWbyteOrder = htons(xmlString.length());
  memcpy(array, & byteSizeInNWbyteOrder, 2);
  return array;
}

int SMARTmonitorService::SendBytesTo1Client(
  const int clientSocketFileDesc, 
  const std::string & xmlString)
{
  fastestUnsignedDataType returnValue = successfullyWroteAllBytes;
  uint16_t numOverallBytes;
  uint8_t * array = ByteArrayFromString(xmlString, numOverallBytes);
  LOGN_DEBUG("Sending to client: " <<std::string((char*) array, numOverallBytes)
    )
  int n = write(clientSocketFileDesc, array, numOverallBytes);
  delete [] array;
  if (n < 0)
  {
    std::ostringstream stdoss;
    stdoss << "error writing to socket with file desc " 
      << clientSocketFileDesc << " OS error number:" << errno << ".";
    switch(errno)
    {
      case EPIPE :
        //see https://linux.die.net/man/2/write
        stdoss << "The reading end of socket is closed.";
        returnValue = readingEndOfSocketClosed;
        break;
      default:
        returnValue = wroteLessBytesThanIntended;
    }
    LOGN_ERROR(stdoss.str())
  }
  else
    LOGN("successfully wrote " << numOverallBytes << " bytes to file descr." 
      << clientSocketFileDesc)
  return returnValue;
}

void SMARTmonitorService::SendBytesToAllClients(std::string & xmlString)
{
  LOGN_DEBUG("begin")
  uint16_t numOverallBytes;
  uint8_t * array = ByteArrayFromString(xmlString, numOverallBytes);
  
  int clientSocketFileDesc;
  /** Client socket connection thread runs in parallel--so mutually exclude 
    * access to m_clientSocketFileDescSet */
  m_clientsCriticalSection.Enter();
  //std::vector<int>::const_iterator socketFileDescIter = m_clientSocketFileDescVector.begin();
  std::set<int>::const_iterator socketFileDescIter = m_clientSocketFileDescSet.begin();
  std::vector<int> fileDescsToDelete;
  int sendResult;
  for( ; socketFileDescIter != m_clientSocketFileDescSet.end() ; socketFileDescIter ++)
  {
    clientSocketFileDesc = *socketFileDescIter;
    sendResult = SendBytesTo1Client(clientSocketFileDesc, xmlString);
    if( sendResult != successfullyWroteAllBytes )
      fileDescsToDelete.push_back(clientSocketFileDesc);
  }
  RemoveFileDescsOfBrokenSocketConns(fileDescsToDelete);
  m_clientsCriticalSection.Leave();
  delete [] array;
}

void SMARTmonitorService::BeforeWait()
{
  int i = 0;
  SMARTaccess_type & SMARTaccess =  * mp_SMARTaccess;
  /** Send to all clients non-blocking */
  const fastestUnsignedDataType numberOfDifferentDrives = SMARTaccess.
    GetNumberOfDifferentDrives();
  SMARTaccessBase::SMARTuniqueIDandValsContType & SMARTuniqueIDsAndValues =
    SMARTaccess.GetSMARTuniqueIDandValues();
  LOGN("address:" << & SMARTuniqueIDsAndValues )
  std::set<SMARTuniqueIDandValues>::const_iterator SMARTuniqueIDandValuesIter =
    SMARTuniqueIDsAndValues.begin();
  fastestUnsignedDataType SMARTattrID;
  uint64_t SMARTrawValue;
  LOGN( "# SMART attributes to observe:" << m_IDsOfSMARTattrsToObserve.size() )

  std::ostringstream oss;
  oss.precision(3); //Allow 3 digits right of decimal point
  for(fastestUnsignedDataType currentDriveIndex = 0;
    currentDriveIndex < numberOfDifferentDrives;
    ++ currentDriveIndex, SMARTuniqueIDandValuesIter ++)
  {
    const SMARTuniqueIDandValues & sMARTuniqueIDandVals =
      *SMARTuniqueIDandValuesIter;
    const SMARTuniqueID & SMARTuniqueID = SMARTuniqueIDandValuesIter->
      getSMARTuniqueID();
    oss << "<data_carrier model=\"" << SMARTuniqueID.m_modelName << "\""
      << " firmware=\"" << SMARTuniqueID.m_firmWareName << "\""
      << " serial_number=\"" << SMARTuniqueID.m_serialNumber << "\">";
    
//    pDriveInfo = m_SMARTvalueProcessor.GetDriveInfo(currentDriveIndex);
//    long int currentSMARTrawValue;
    //TimeCountInNanosec_type timeCountInNanoSeconds;
//    long double timeCountInSeconds;
    //TODO crashes here (iterator-related?!-> thread access problem??)
    for(fastestUnsignedDataType arrIdx = 0;
      SMARTuniqueID.SMART_IDsToReadNotEnd(arrIdx); arrIdx++)
    {
      SMARTattrID = SMARTuniqueID.m_SMART_IDsToRd[arrIdx];
      //OperatingSystem::GetTimeCountInNanoSeconds(timeCountInNanoSeconds);
      //OperatingSystem::GetTimeCountInSeconds(timeCountInSeconds);
      const SMARTvalue & sMARTvalue = sMARTuniqueIDandVals.m_SMARTvalues[
        SMARTattrID];
      
      uint64_t timeInMs;
      float fTimeInS;
      if(sMARTvalue.GetRetrievalTime(timeInMs) )
        fTimeInS = (float) timeInMs / 1000.0f;
      else{
        fTimeInS = 0.0f;
        LOGN_WARNING("uptime of S.M.A.R.T. value retrieval is 0")
      }
      LOGN("time in [s] as int * 1000: " << timeInMs
          << " as float:" << fTimeInS)
      if( sMARTvalue.m_successfullyReadSMARTrawValue &&
          //m_timeStampOfRetrieval > 0 && 
          sMARTvalue.IsConsistent(SMARTrawValue) )
      {
        oss << "<SMART ID=\"" << SMARTattrID 
          << "\" raw_value=\"" << SMARTrawValue 
          << "\" time_in_s=\"" <</**Ensure not using scientific output (exponent
           * representation/notation) */ std::fixed 
          << fTimeInS;
        SMARTuniqueID::unitDataType unit = SMARTuniqueID.units[SMARTattrID];
        if(unit != 0){
          SMARTuniqueID::unitDataType MSB = GetBitMaskForMostSignificantBit(
            unit);
          if(unit & MSB){
            unit = removeBits(unit, MSB);
            oss << "\" unit=\">" << unit;
          }
          else
            oss << "\" unit=\"" << unit;
        }
        if(SMARTuniqueID.lowerUnitBound[SMARTattrID] != 0)
          oss << "\" lower_unit_bound=\"" << SMARTuniqueID.lowerUnitBound[
            SMARTattrID];
        if(SMARTuniqueID.upperUnitBound[SMARTattrID] != 0)
          oss << "\" upper_unit_bound=\"" << SMARTuniqueID.upperUnitBound[
            SMARTattrID];
        oss << "\"/>";
      }
      //m_arSMARTrawValue[lineNumber];
    }///Ends loop over all S.M.A.R.T. attributes.
    /** For data carriers that don't have S.M.A.R.T. ID 241 (Total Data
     *  written) like HDD model "SAMSUNG HA200JC", firmware:"WS100-33" to
     *  provide this info. It is useful if e.g. the computer doesn't react and
     *  the LED shows access to the data carrier to know if it may be a read/
     *  write error by knowing how much is read/written. */
    /*oss << "<numBwrittenSinceOSstart=\"" <<
      SMARTuniqueID.numBwrittenSinceOSstart << "\"/>";
    oss << "<numB_readSinceOSstart=\"" <<
      SMARTuniqueID.numB_readSinceOSstart << "\"/>";*/
    oss << "</data_carrier>";
    std::string xmlString = oss.str();
    SendBytesToAllClients(xmlString);
  }
}

/** In order to avoid the "Failed to bind server" error when calling "bind(...)"
* after restarting the server.
* see http://stackoverflow.com/questions/10619952/how-to-completely-destroy-a-socket-connection-in-c
* Not tested yet if this works. */
void SMARTmonitorService::CloseAllClientSockets()
{
  int clientSocketFileDesc;
  m_clientsCriticalSection.Enter();
  std::set<int>::const_iterator socketFileDescIter = m_clientSocketFileDescSet.
    begin();
  for( ; socketFileDescIter != m_clientSocketFileDescSet.end() ; 
    socketFileDescIter ++)
  {
    clientSocketFileDesc = *socketFileDescIter;
    close(clientSocketFileDesc);
  }
  m_clientsCriticalSection.Leave();
}

/*void SMARTmonitorService::AfterGetSMARTvaluesLoop()
{
  
}*/
