#pragma once///Include guard.

/** Needs header file from https://github.com/jarro2783/cxxopts
* For steps see https://github.com/jarro2783/cxxopts/blob/master/README.md */
///class cxxopts::Options from Jarryd Beck
#include <cxxopts.hpp>///https://github.com/jarro2783/cxxopts/blob/master/INSTALL
#include <libraries/cxxopts/handleArg.hpp>///cxxopts::handleArg(...)

namespace cxxopts{
  enum HndleCmdLneOptsRslt{success=0, tooFewCmdLineArgs, cxxoptsExc};
template <typename timeoutType> inline int HandleCmdLineOpts(
  int argCount, const char * args[],
  std::string & srvHost, int & port, timeoutType & timeoutInUs)
{
  cxxopts::Options cmdLineOpts(args[0]);
  cmdLineOpts.add_options()///Indirectly calls OptionAdder::operator()
    ///->calls m_options.add_option
    ///->calls options.options.emplace_back(
    ("p,port","port number to connect to", cxxopts::value<int>())
    ("h,host","host (name/address) to connect to",cxxopts::value<std::string>())
    ("s,timeoutInUs","timeout for connect/send [us]",cxxopts::value<timeoutType>
      ())
    ;
  if( argCount == 1){
    std::cout << cmdLineOpts.help() << std::endl;
    return tooFewCmdLineArgs;
  }
  else
  try{
    auto cmdLineOptsParseRslt = cmdLineOpts.parse(argCount, args);
    handleArg(cmdLineOptsParseRslt, "port", port);
    handleArg(cmdLineOptsParseRslt, "host", srvHost);
    handleArg(cmdLineOptsParseRslt, "timeoutInUs", timeoutInUs);
  }catch(cxxopts::OptionException & e){
    std::cerr << "cxxopts error:" << e.what() << std::endl;
    return cxxoptsExc;
  }
  return success;
}
}
