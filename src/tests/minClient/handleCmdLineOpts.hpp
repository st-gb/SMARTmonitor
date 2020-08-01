#pragma once///Include guard.

void HandleCmdLineOpts(int argCount, char * argVec [], std::string & srvHost,
  int & port)
{
  if( argCount < 2)
    std::cout << "no server specified as 1st command line argument->using \""
      << srvHost << "\"" <<std::endl;
  else
    srvHost = argVec[1];
}
