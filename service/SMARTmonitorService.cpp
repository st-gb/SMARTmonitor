/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SMARTmonitorService.cpp
 * Author: root
 * 
 * Created on 19. November 2016, 22:13
 */

#include "SMARTmonitorService.hpp"
#include <sys/socket.h> //socket(...), bind(...), ...)
#include <netinet/in.h> //struct sockaddr_in
#include "Controller/time/GetTickCount.hpp" //OperatingSystem::GetTimeCountInNanoSeconds(...)

int SMARTmonitorService::s_socketFileDesc = 0;

SMARTmonitorService::SMARTmonitorService() 
  //: 
  /** https://computing.llnl.gov/tutorials/pthreads/#ConditionVariables :
       [...] must be initialized before they can be used */
  //cond(PTHREAD_COND_INITIALIZER)
{
  //from https://computing.llnl.gov/tutorials/pthreads/#ConditionVariables
   pthread_mutex_init(& mutex, NULL);
   pthread_cond_init (&cond, NULL);
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
  LOGN("end")
}

DWORD SMARTmonitorService::ClientConnThreadFunc( void * p_v)
{
  LOGN("begin")
  SMARTmonitorService * p_SMARTmonitor = (SMARTmonitorService * ) p_v;
  if( p_SMARTmonitor )
  {
    //Code adapted from http://www.linuxhowtos.org/data/6/server.c
    unsigned portNumber = 1000;
    struct sockaddr_in server_address,  client_address;
    memset(&server_address, 0, sizeof(server_address)); /* Clear structure */

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(portNumber);

    //Erzeugt ein neues Socket bestimmten Types und alloziert hierfür Systemressourcen. Für die Identifizierung gibt die Funktion eine eindeutige Zahl vom Typ Integer zurück.
    p_SMARTmonitor->s_socketFileDesc = socket(AF_INET, SOCK_STREAM, 0);
    const int socketFileDesc = p_SMARTmonitor->s_socketFileDesc;
    //Bindet den Socket an eine Socket Adressinformation, in der Regel an eine IP-Adresse und Port. Wird typischerweise auf Server-Seite benutzt.
    if( bind(socketFileDesc, (struct sockaddr *) &server_address, 
        sizeof(server_address)) < 0)
    {
      LOGN_ERROR("Failed to bind server")
      return 1;
    }
    else
    {
      LOGN_INFO("bound server")
      //Versetzt einen gebundenen STREAM (TCP/IP) Socket in einen Lauschen-Modus. Wird auf Server-Seite benutzt.
      if( listen(socketFileDesc, 5) == 0 )
      {
      do
      {
        socklen_t sizeOfClientAddrInB = sizeof(client_address);
        LOGN("Waiting for a socket client to accept on file descr." << 
          socketFileDesc << ",port " << portNumber)
        const int clientSocketFileDesc = accept(
          socketFileDesc, 
          (struct sockaddr *) & client_address, 
          & sizeOfClientAddrInB);
        if (clientSocketFileDesc < 0)
        {
          LOGN_ERROR("Failed to accept client")
          //return 2;
        }
        else
        {
          LOGN("new client at port " << client_address.sin_port 
            << " file desc.:" << clientSocketFileDesc)
          p_SMARTmonitor->AddClient(clientSocketFileDesc);
        }
      }while(p_SMARTmonitor->s_updateSMARTvalues);
      LOGN("after \"wait for socket connection\" loop")
      }
    }
    
  }
  LOGN("end")
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

void SMARTmonitorService::AfterGetSMARTvaluesLoop() {
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
  for( std::vector<int>::const_iterator iter = fileDescsToDelete.begin();
    iter != fileDescsToDelete.end(); iter ++)
  {
//    int n = * iter;
//    std::vector<int>::iterator iterToDelete = std::find(
//        m_clientSocketFileDescVector.begin(), 
//        m_clientSocketFileDescVector.end(),
//        n);
    //if( m_clientSocketFileDescVector.
//    if( iterToDelete != m_clientSocketFileDescVector.end() )
//    {
//      m_clientSocketFileDescVector.erase(iterToDelete);
//    }
    LOGN("Removing client with file descr. " << *iter << " from list "
      "because of broken socket connection")
    m_clientSocketFileDescSet.erase(*iter);
  }
}

void SMARTmonitorService::SendBytes(std::string & xmlString)
{
  const uint16_t numOverallBytes = xmlString.length() + 2;
  LOGN("# bytes to send:" << numOverallBytes << " XML data:" << numOverallBytes - 2)
  int clientSocketFileDesc;
  uint8_t * array = new uint8_t[numOverallBytes];
  
  memcpy(array + 2, xmlString.c_str(), numOverallBytes - 2);
  const uint16_t byteSizeInNWbyteOrder = htons(xmlString.length());
  memcpy(array, & byteSizeInNWbyteOrder, 2);
  
  m_clientsCriticalSection.Enter();
  //std::vector<int>::const_iterator socketFileDescIter = m_clientSocketFileDescVector.begin();
  std::set<int>::const_iterator socketFileDescIter = m_clientSocketFileDescSet.begin();
  std::vector<int> fileDescsToDelete;
  for( ; socketFileDescIter != m_clientSocketFileDescSet.end() ; socketFileDescIter ++)
  {
    clientSocketFileDesc = *socketFileDescIter;
    int n = write(clientSocketFileDesc, array, numOverallBytes);
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
          fileDescsToDelete.push_back(clientSocketFileDesc);
          break;
      }
      LOGN_ERROR(stdoss.str())
    }
    else
      LOGN("successfully wrote " << numOverallBytes << " bytes to file descr." 
        << clientSocketFileDesc)
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
  std::set<SMARTuniqueIDandValues> & SMARTuniqueIDsAndValues = SMARTaccess.
    GetSMARTuniqueIDandValues();
  LOGN("address:" << & SMARTuniqueIDsAndValues )
  std::set<SMARTuniqueIDandValues>::const_iterator SMARTuniqueIDandValuesIter =
    SMARTuniqueIDsAndValues.begin();
  fastestUnsignedDataType SMARTattributeID;
  uint64_t SMARTrawValue;
  const std::set<SkSmartAttributeParsedData> & SMARTattributesToObserve =
    SMARTaccess.getSMARTattributesToObserve();
  LOGN( "# SMART attributes to observe:" << SMARTattributesToObserve.size() )

  std::ostringstream oss;
  oss.precision(3); //Allow 3 digits right of decimal point
  unsigned lineNumber = 0;
  for(fastestUnsignedDataType currentDriveIndex = 0;
    currentDriveIndex < numberOfDifferentDrives;
    ++ currentDriveIndex, SMARTuniqueIDandValuesIter ++)
  {
    const SMARTuniqueID & SMARTuniqueID = SMARTuniqueIDandValuesIter->
      getSMARTuniqueID();
    oss << "<data_carrier model=\"" << SMARTuniqueID.m_modelName << "\""
      << " firmware=\"" << SMARTuniqueID.m_firmWareName << "\""
      << " serial_number=\"" << SMARTuniqueID.m_serialNumber << "\">";
    
//    pDriveInfo = m_SMARTvalueProcessor.GetDriveInfo(currentDriveIndex);
    std::set<SkSmartAttributeParsedData>::const_iterator
      SMARTattributesToObserveIter = SMARTattributesToObserve.begin();
//    long int currentSMARTrawValue;
    //TimeCountInNanosec_type timeCountInNanoSeconds;
//    long double timeCountInSeconds;
    SMARTvalue sMARTvalue;
    //TODO crashes here (iterator-related?!-> thread access problem??)
    for( ; SMARTattributesToObserveIter != SMARTattributesToObserve.end();
        SMARTattributesToObserveIter ++, ++lineNumber )
    {
      SMARTattributeID = SMARTattributesToObserveIter->id;
      //OperatingSystem::GetTimeCountInNanoSeconds(timeCountInNanoSeconds);
      //OperatingSystem::GetTimeCountInSeconds(timeCountInSeconds);
      sMARTvalue = SMARTuniqueIDandValuesIter->m_SMARTvalues[SMARTattributeID];
      
      float fTimeInS = sMARTvalue.m_timeStampOfRetrieval;
      fTimeInS /= 1000.0f;
      LOGN("time in [s] as int * 1000: " << sMARTvalue.m_timeStampOfRetrieval 
          << "as float:" << fTimeInS)
      if( sMARTvalue.IsConsistent(SMARTrawValue) )
      {
        oss << "<SMART ID=\"" << SMARTattributeID 
          << "\" raw_value=\"" << SMARTrawValue 
          << "\" time_in_s=\"" << 
          /** Ensure not using scientific output (exponent representation/notation) */
          std::fixed 
          << fTimeInS << "\"/>";
      }
      //m_arSMARTrawValue[lineNumber];  
    }
    oss << "</data_carrier>";
    std::string xmlString = oss.str();
    SendBytes(xmlString);  
  }
}

/*void SMARTmonitorService::AfterGetSMARTvaluesLoop()
{
  
}*/
