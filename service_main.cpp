/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   service_main.cpp
 * Author: sg
 *
 * Created on 17. November 2016, 12:50
 */

#include <cstdlib>

#ifdef __linux__
  #include <OperatingSystem/Linux/daemon/daemon.h>
  
  #include "service/SMARTmonitorService.hpp"
#endif
#include "hardware/CPU/atomic/AtomicExchange.h"

using namespace std;

//static SMARTmonitorService/*<char>*/ SMARTmonitor;//(argc, argv);
static SMARTmonitorService/*<char>*/ * gp_SMARTmonitor;//(argc, argv);

static void child_handler(int signum)
{
  LOGN("received signal")
  switch(signum)
  {
  case SIGALRM: exit(EXIT_FAILURE); break;
    case SIGPIPE:
      LOGN_ERROR("pipe/socket is broken")
      break;
  //case SIGUSR1: exit(EXIT_SUCCESS); break;
  case SIGCHLD: exit(EXIT_FAILURE); break;
    case SIGTERM:
    case SIGKILL:
    case SIGINT:
      LOGN("received term or kill or interrupt signal")
      
      LOGN("ending get SMART values loop");
      /** Inform the SMART values update thread about we're going to exit,*/
      AtomicExchange( (long *) & gp_SMARTmonitor->s_updateSMARTvalues, 0);

      break;
  }
}

void TrapSignals2()
{
  /* Trap signals that we expect to receive */
  signal(SIGCHLD, child_handler);
  signal(SIGTERM, child_handler);
  signal(SIGKILL, child_handler);
  signal(SIGUSR1, child_handler);
  signal(SIGALRM, child_handler);
  signal(SIGPIPE, child_handler);
  //SIGHUP = Reload config signal.
  signal(SIGHUP, child_handler);
  signal(SIGINT, child_handler);

}

/*
 * 
 */
int main(int argc, char** argv) {
  
  //std::string lockFilePath = "/var/lock/" + argv[0];
  //daemonize("/var/lock/smartmonitor.lock");
  TrapSignals2();
  SMARTmonitorService/*<char>*/ SMARTmonitor;//(argc, argv);
  gp_SMARTmonitor = & SMARTmonitor;
  
  //TODO set command line args
  SMARTmonitor.SetCommandLineArgs(argc, argv);
  SMARTmonitor.InitializeLogger();
  
  //std::wstring stdwstrConfigPathWithoutExtension;
  //SMARTmonitor.ConstructConfigFilePath(stdwstrConfigPathWithoutExtension);
  SMARTmonitor.InitializeSMART();
  nativeThread_type clientConnThread;
  clientConnThread.start(SMARTmonitor.ClientConnThreadFunc, & SMARTmonitor );

  SMARTmonitor.StartAsyncUpdateThread();
  
  //Wait for the update thread to be finished
  SMARTmonitor.WaitForSignal();
  LOGN("ending service")
  return 0;
}

