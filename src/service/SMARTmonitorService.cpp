/** Author: sg
 * Created on 19. November 2016, 22:13 */
#include "SMARTmonitorService.hpp"
#include <sys/socket.h> //socket(...), bind(...), ...)
#include <netinet/in.h> //struct sockaddr_in
#include "Controller/time/GetTickCount.hpp" //OperatingSystem::GetTimeCountInNanoSeconds(...)
#include "hardware/CPU/atomic/AtomicExchange.h" // AtomicExchange(...))
#include <attributes/SMARTattributeNameAndID.hpp> //SMARTattributeNameAndID
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
#include <vector> //class std::vector

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
  unsigned SMARTattributeID;
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
      std::vector<SMARTattributeNameAndID> SMARTattributeNamesAndIDs;
      //TODO Getting supported SMART attrs only needs to be done once for a drive
      // should be the same for all same model or same model, firmware
      // Can store this in a dataCarrierID2supportedSMARTattributesMap_type
      SMARTaccess.GetSupportedSMART_IDs(device, SMARTattributeNamesAndIDs);
      std::vector<SMARTattributeNameAndID>::const_iterator
        supportedSMARTattributeIDsIter = SMARTattributeNamesAndIDs.begin();
      if( supportedSMARTattributeIDsIter != SMARTattributeNamesAndIDs.end() )
      {
        SMARTattributeID = supportedSMARTattributeIDsIter->GetID();
        std_ostringstream << "<data_carrier model=\"" << SMARTuniqueID.m_modelName << "\""
          << " firmware=\"" << SMARTuniqueID.m_firmWareName << "\""
          << " serial_number=\"" << SMARTuniqueID.m_serialNumber << "\">"
          << "<supportedSMART_IDs>" << SMARTattributeID;
        for( supportedSMARTattributeIDsIter ++; 
            supportedSMARTattributeIDsIter != SMARTattributeNamesAndIDs.end();
            supportedSMARTattributeIDsIter ++ )
        {
          SMARTattributeID = supportedSMARTattributeIDsIter->GetID();
          std_ostringstream << "," << SMARTattributeID;
        }
        std_ostringstream << "</supportedSMART_IDs>";
        std_ostringstream << "</data_carrier>";
        std::string xmlString = std_ostringstream.str();
        LOGN_DEBUG("XML data to send:" << xmlString)
        SendBytesTo1Client(clientSocketFileDesc, xmlString);
      }
    }
  }
  LOGN("end" )
}

fastestUnsignedDataType SMARTmonitorService::BindAndListenToSocket()
{
  fastestUnsignedDataType retCode = SMARTmonitorService::unset;
  //TODO replace the following by "common_sourcecode/socket/bindAndListen.cpp"
    //Code adapted from http://www.linuxhowtos.org/data/6/server.c
  unsigned portNumber = m_socketPortNumber;
  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address)); /* Clear structure */

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(portNumber);

  //Erzeugt ein neues Socket bestimmten Types und alloziert hierfür Systemressourcen. Für die Identifizierung gibt die Funktion eine eindeutige Zahl vom Typ Integer zurück.
  s_socketFileDesc = socket(AF_INET, SOCK_STREAM, 0);
  const int socketFileDesc = s_socketFileDesc;
  
  /** see/from 
   * http://stackoverflow.com/questions/10619952/how-to-completely-destroy-a-socket-connection-in-c
  *  : Avoid bind problems (errno = EADDRINUSE) */
  int true_ = 1;
  setsockopt(socketFileDesc, SOL_SOCKET, SO_REUSEADDR, & true_, sizeof(true_) );
          
  //Bindet den Socket an eine Socket Adressinformation, in der Regel an eine IP-Adresse und Port. Wird typischerweise auf Server-Seite benutzt.
  if( bind(socketFileDesc, (struct sockaddr *) & server_address, 
      sizeof(server_address) ) < 0)
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
  unsigned lineNumber = 0;
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
    SMARTattrToObsType::const_iterator
      SMARTattrToObsIter = m_IDsOfSMARTattrsToObserve.begin();
//    long int currentSMARTrawValue;
    //TimeCountInNanosec_type timeCountInNanoSeconds;
//    long double timeCountInSeconds;
    SMARTvalue sMARTvalue;
    //TODO crashes here (iterator-related?!-> thread access problem??)
    for( ; SMARTattrToObsIter != m_IDsOfSMARTattrsToObserve.end();
        SMARTattrToObsIter ++, ++lineNumber )
    {
      SMARTattrID = *SMARTattrToObsIter;
      //OperatingSystem::GetTimeCountInNanoSeconds(timeCountInNanoSeconds);
      //OperatingSystem::GetTimeCountInSeconds(timeCountInSeconds);
      sMARTvalue = sMARTuniqueIDandVals.m_SMARTvalues[SMARTattrID];
      
      float fTimeInS = sMARTvalue.m_timeStampOfRetrieval;
      fTimeInS /= 1000.0f;
      LOGN("time in [s] as int * 1000: " << sMARTvalue.m_timeStampOfRetrieval 
          << "as float:" << fTimeInS)
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
          SMARTuniqueID::unitDataType MSB = mostSignificantBit(unit);
          if(unit & MSB){
            unit = removeBits(unit, MSB);
            oss << "\" unit=\">" << unit;
          }
          else
            oss << "\" unit=\"" << unit;
        }
        oss << "\"/>";
      }
      //m_arSMARTrawValue[lineNumber];  
    }
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
