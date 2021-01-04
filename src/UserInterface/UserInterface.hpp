/** UserInterface.hpp
 *  Created on: 20.02.2015
 *  Author: Stefan Gebauer, M.Sc.Comp.Sc.*/

#ifndef USERINTERFACE_USERINTERFACE_HPP_
#define USERINTERFACE_USERINTERFACE_HPP_

#include <string> //class std::string
#include <stdint.h>///uint64_t

/** Forward declarations */
struct tm;

class UserInterface
{
public:
  class MessageType{
    public:
    enum messageTypes {success, error, warning, info};
  };
  ~UserInterface() { }
  virtual void ShowMessage(const char * const ) const = 0;
  ///messageType e.g. for filtering/highlighting errors coloured (red etc.).
  virtual void ShowMessage(const char * const, enum MessageType::messageTypes msg) const = 0;
  virtual void ShowConnectionState(const char * const pch, int timeOut) {}
  
  static std::string GetTimeAsString(const struct tm & timeOfLastSMARTvaluesUpdate);
  inline static void FmtViaOSS(const uint64_t timeInMs, std::string &);
  static void FormatTime(
    /** Needs to be uint64_t in order to work if built as 32 bit program and
     *  large values such as power-on time in ms.*/
    const uint64_t timeInMs, 
    std::string & timeFormatString);
};

#endif /* USERINTERFACE_USERINTERFACE_HPP_ */
