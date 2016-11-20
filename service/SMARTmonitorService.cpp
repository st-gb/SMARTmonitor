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
  SMARTmonitorBase * p_SMARTmonitor = (SMARTmonitorBase * ) p_v;
  if( p_SMARTmonitor )
  {
    //Code adapted from http://www.linuxhowtos.org/data/6/server.c
    unsigned portNumber = 1000;
    struct sockaddr_in server_address,  client_address;
    memset(&server_address, 0, sizeof(server_address)); /* Clear structure */

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(portNumber);

    int socketFileDesc = socket(AF_INET, SOCK_STREAM, 0); //Erzeugt ein neues Socket bestimmten Types und alloziert hierfür Systemressourcen. Für die Identifizierung gibt die Funktion eine eindeutige Zahl vom Typ Integer zurück.
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
      if( listen(socketFileDesc,5) == 0 )
      {
      do
      {
        socklen_t sizeOfClientAddrInB = sizeof(client_address);
        LOGN("Waiting for a socket client to accept on port" << portNumber)
        int clientSocketFileDesc = accept(socketFileDesc, 
                   (struct sockaddr *) & client_address, 
                   & sizeOfClientAddrInB);
        if (clientSocketFileDesc < 0)
        {
          LOGN_ERROR("Failed to accept client")
          //return 2;
        }
        LOGN_ERROR("new client:" << client_address.sin_port)
      }while(p_SMARTmonitor->s_updateSMARTvalues);
      }
    }
  }
  LOGN("end")
  return 0;
}

void SMARTmonitorService::AfterGetSMARTvaluesLoop() {
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

  unsigned lineNumber = 0;
  for(fastestUnsignedDataType currentDriveIndex = 0;
    currentDriveIndex < numberOfDifferentDrives;
    ++ currentDriveIndex, SMARTuniqueIDandValuesIter ++)
  {
//    pDriveInfo = m_SMARTvalueProcessor.GetDriveInfo(currentDriveIndex);
    std::set<SkSmartAttributeParsedData>::const_iterator
      SMARTattributesToObserveIter = SMARTattributesToObserve.begin();
    long int currentSMARTrawValue;
    //TODO crashes here (iterator-related?!-> thread access problem??)
    for( ; SMARTattributesToObserveIter != SMARTattributesToObserve.end();
        SMARTattributesToObserveIter ++, ++lineNumber )
    {
      SMARTattributeID = SMARTattributesToObserveIter->id;
      currentSMARTrawValue = SMARTuniqueIDandValuesIter->m_SMARTrawValues[SMARTattributeID];

      m_arSMARTrawValue[lineNumber];
    }
  }
}

/*void SMARTmonitorService::AfterGetSMARTvaluesLoop()
{
  
}*/
