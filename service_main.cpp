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
  #include <Linux/daemon/daemon.h>
#endif

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
  
  //std::string lockFilePath = "/var/lock/" + argv[0];
  daemonize("/var/lock/smartmonitor.lock");
  return 0;
}

