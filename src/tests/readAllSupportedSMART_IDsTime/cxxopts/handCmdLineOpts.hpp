#include <libraries/cxxopts/handleArg.hpp>

namespace cxxopts{
inline int HandleCmdLineOpts(int argCount, char * args[],std::string & dvcPath){
  ///Needs header file from https://github.com/jarro2783/cxxopts
  ///For steps see https://github.com/jarro2783/cxxopts/blob/master/README.md
  cxxopts::Options cmdLineOpts(args[0]);
  cmdLineOpts.add_options()///Indirectly calls OptionAdder::operator()
    ///->calls m_options.add_option
    ///->calls options.options.emplace_back(
    ("d,device", "device path", cxxopts::value<std::string>())
    ;
  if( argCount == 1)
    std::cout << cmdLineOpts.help() << std::endl;
  try{
    auto cmdLineOptsParseRslt = cmdLineOpts.parse(argCount, args);
    cxxopts::handleArg(cmdLineOptsParseRslt, "device", dvcPath);
  }catch(cxxopts::OptionException & e){}
}
}
