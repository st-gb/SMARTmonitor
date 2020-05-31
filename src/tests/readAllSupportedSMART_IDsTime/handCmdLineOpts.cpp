void handleCmdLineArgs(int argCount, char * argVec [], std::string & device)
{
  if( argCount < 2){
    device = "/dev/sda";
    std::cout << "no devive specified->using " << device << std::endl;
  }
  else
    device = argVec[1];
}
