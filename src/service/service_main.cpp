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
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)

using namespace std;

//static SMARTmonitorService/*<char>*/ SMARTmonitor;//(argc, argv);
static SMARTmonitorService/*<char>*/ * gp_SMARTmonitor;//(argc, argv);

void SMARTmonitorService::End(const char signalName [])
{
  LOGN("received " << signalName << " signal")
  //TODO fails to shutdown service:
//2017-03-14 19:52:01,265 INFO [Thread-16645] :: End received interruptsignal
//2017-03-14 19:52:01,265 INFO [Thread-16645] SMARTmonitorService EndUpdateSMARTvaluesThread ending get SMART values loop
//2017-03-14 19:52:01,284 INFO [Thread-16648] SMARTmonitorService AfterGetSMARTvaluesLoop closing server socket file descr.4
//2017-03-14 19:52:01,284 INFO [Thread-16648] SMARTmonitorService AfterGetSMARTvaluesLoop successfully shut down server socket
//2017-03-14 19:52:01,284 INFO [Thread-16648] SMARTmonitorService AfterGetSMARTvaluesLoop locking mutex for signalling
//2017-03-14 19:52:01,284 INFO [Thread-16648] SMARTmonitorService AfterGetSMARTvaluesLoop signalling the main thread to end
//2017-03-14 19:52:01,284 INFO [Thread-16648] SMARTmonitorService AfterGetSMARTvaluesLoop pthread signal succeeded
//2017-03-14 19:52:01,284 INFO [Thread-16648] SMARTmonitorService AfterGetSMARTvaluesLoop Unlocking mutex for signalling
//2017-03-14 19:52:01,284 INFO [Thread-16648] SMARTmonitorService AfterGetSMARTvaluesLoop end
//2017-03-14 19:52:01,285 ERROR [Thread-16645] SMARTmonitorService ClientConnThreadFunc Failed to accept client
//2017-03-14 19:52:01,285 INFO [Thread-16645] SMARTmonitorService ClientConnThreadFunc after "wait for socket connection" loop
//2017-03-14 19:52:01,285 INFO [Thread-16645] SMARTmonitorService ClientConnThreadFunc end
//2017-03-14 19:52:01,285 INFO [Thread-16645] SMARTmonitorService WaitForSignal locking signal mutex
//2017-03-14 19:52:01,285 INFO [Thread-16645] SMARTmonitorService WaitForSignal Waiting for signal  gp_SMARTmonitor->EndUpdateSMARTvaluesThread();
    
  pthread_cond_signal(& p_SMARTmonSvc->cond);
  EndUpdateSMARTvaluesThread();
}

///Specific to UNIX/Linux.
static void signal_handler(int signum)//TODO Unix-specific 
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
      SMARTmonitorService::End("term"); break;
    case SIGKILL:
      SMARTmonitorService::End("kill"); break;
    case SIGINT:
      SMARTmonitorService::End("interrupt"); break;
  }
}

void TrapSignals2()//TODO Unix-specific 
{
  /* Trap signals that we expect to receive */
  signal(SIGCHLD, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGKILL, signal_handler);
  signal(SIGUSR1, signal_handler);
  signal(SIGALRM, signal_handler);
  signal(SIGPIPE, signal_handler);
  //SIGHUP = Reload config signal.
  signal(SIGHUP, signal_handler);
  signal(SIGINT, signal_handler);
}

/*
 * 
 */
int main(int argc, char** argv) {
  ///Neded for SetLogLevel() in ProcessCommandLineArgs()
  LogLevel::CreateLogLevelStringToNumberMapping();
  //std::string lockFilePath = "/var/lock/" + argv[0];
  //TODO use daemonize(...)
  //daemonize("/var/lock/smartmonitor.lock");
  TrapSignals2();
  SMARTmonitorService/*<char>*/ SMARTmonitor;//(argc, argv);
  gp_SMARTmonitor = & SMARTmonitor;
  
  //TODO set command line args
  SMARTmonitor.SetCommandLineArgs(argc, argv);
  if( SMARTmonitor.GetCommandLineArgs().GetArgumentCount() < 2 )
    SMARTmonitor.OutputUsage();
  ///Has to be called before InitializeLogger() as it gets the log folder path.
  SMARTmonitor.ProcessCommandLineArgs();
  if( ! SMARTmonitor.InitializeLogger() )
    return 2;
  //std::wstring stdwstrConfigPathWithoutExtension;
  //SMARTmonitor.ConstructConfigFilePath(stdwstrConfigPathWithoutExtension);
  const fastestUnsignedDataType SMARTinitResult = SMARTmonitor.InitializeSMART();
  if( SMARTinitResult == SMARTmonitorBase::success && 
      SMARTmonitor.BindAndListenToSocket() == 
      SMARTmonitorService::listeningToSocket )
  {
    
    //TODO needs only to be done if at least 1 client connected?
    SMARTmonitor.StartAsyncUpdateThread(& SMARTmonitorBase::
      Upd8SMARTvalsDrctlyThreadSafe);
    
    /** Client connection handling can be done in the main thread because the
     *  service can be exiting via signals (that are software interrupts
     * and are handled within the [main] thread.). */
    SMARTmonitor.ClientConnThreadFunc(& SMARTmonitor);
    
    ///accept() is blocking, so start in a different thread?
    //nativeThread_type clientConnThread;
    //clientConnThread.start(SMARTmonitor.ClientConnThreadFunc, & SMARTmonitor );

    /** Wait for the update thread to be finished */
    //SMARTmonitor.WaitForSignal();
  }
  LOGN("ending service")
  return 0;
}
