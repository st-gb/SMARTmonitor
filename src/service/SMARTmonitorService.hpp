/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SMARTmonitorService.hpp
 * Author: root
 *
 * Created on 19. November 2016, 22:13
 */

#ifndef SMARTMONITORSERVICE_HPP
#define SMARTMONITORSERVICE_HPP

#include "../SMARTmonitorBase.hpp"
#include "OperatingSystem/multithread/nativeCriticalSectionType.hpp"
#include <vector> //std::vector
#include <set> //class std::set

class SMARTmonitorService
  : public SMARTmonitorBase
{
public:
  SMARTmonitorService();
  SMARTmonitorService(const SMARTmonitorService& orig);
  virtual ~SMARTmonitorService();
  
  enum BindAndListenRetCode { listeningToSocket, unset, 
    failedToBindToSocket, failedToListenToSocket };
  enum SocketSendReturnCodes { successfullyWroteAllBytes, readingEndOfSoecketClosed, 
    wroteLessBytesThanIntended } ;
  fastestUnsignedDataType BindAndListenToSocket();
  void EndUpdateSMARTvaluesThread();
  int SendBytesTo1Client(const int clientSocketFileDesc, 
    const std::string & xmlString);
  void SendBytesToAllClients(std::string & xmlString);
  void AddClient(const int clientSocketFileDesc);
  void BeforeWait();
  void AfterGetSMARTvaluesLoop();
  void WaitForSignal();
  static DWORD ClientConnThreadFunc(void * p_v);
  void RemoveFileDescsOfBrokenSocketConns(
    std::vector<int> & fileDescsToDelete);
  
  static int s_socketFileDesc;
private:
  // std::vector<int> m_clientSocketFileDescVector;
  /** Preferred STL container because it's able to insert and delete easily,
      also no double values are possible */
  std::set<int> m_clientSocketFileDescSet;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  nativeCriticalSection_type m_clientsCriticalSection;
  void SendSupportedSMART_IDsToClient(const int clientSocketFileDesc);
};

#endif /* SMARTMONITORSERVICE_HPP */

