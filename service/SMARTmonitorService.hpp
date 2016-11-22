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
#include "multithread/nativeCriticalSectionType.hpp"
#include <vector> //std::vector

class SMARTmonitorService
  : public SMARTmonitorBase
{
public:
  SMARTmonitorService();
  SMARTmonitorService(const SMARTmonitorService& orig);
  virtual ~SMARTmonitorService();
  
  void SendBytes(std::string & xmlString);
  void AddClient(const int clientSocketFileDesc);
  void BeforeWait();
  void AfterGetSMARTvaluesLoop();
  void WaitForSignal();
  static DWORD ClientConnThreadFunc(void * p_v);
  
  static int s_socketFileDesc;
private:
  std::vector<int> m_clientSocketFileDescVector;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  CriticalSection_type m_clientsCriticalSection;
};

#endif /* SMARTMONITORSERVICE_HPP */

