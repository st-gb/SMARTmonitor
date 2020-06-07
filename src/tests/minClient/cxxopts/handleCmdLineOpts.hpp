#pragma once///Include guard.

/** Needs header file from https://github.com/jarro2783/cxxopts
* For steps see https://github.com/jarro2783/cxxopts/blob/master/README.md */
#include <cxxopts.hpp>///class cxxopts::Options from Jarryd Beck
#include <libraries/cxxopts/handleArg.hpp>///cxxopts::handleArg(...)

namespace cxxopts{
inline int HandleCmdLineOpts(int argCount, char * args[], std::string & srvHost,
  int & port, unsigned & timeoutInUs){
  cxxopts::Options cmdLineOpts(args[0]);
  cmdLineOpts.add_options()///Indirectly calls OptionAdder::operator()
    ///->calls m_options.add_option
    ///->calls options.options.emplace_back(
    ("p,port","port number to connect to", cxxopts::value<int>())
    ("h,host","host (name/address) to connect to",cxxopts::value<std::string>())
    ("s,timeoutInUs","timeout for connect/send [us]",cxxopts::value<unsigned>())
    ;
  if( argCount == 1)
    std::cout << cmdLineOpts.help() << std::endl;
  else
  try{
    auto cmdLineOptsParseRslt = cmdLineOpts.parse(argCount, args);
    handleArg(cmdLineOptsParseRslt, "port", port);
    handleArg(cmdLineOptsParseRslt, "host", srvHost);
    handleArg(cmdLineOptsParseRslt, "timeoutInUs", timeoutInUs);
  }catch(cxxopts::OptionException & e){
    std::cerr << "cxxopts error:" << e.what() << std::endl;}
}
}
