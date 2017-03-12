/*
 * UserInterface.hpp
 *
 *  Created on: 20.02.2015
 *      Author: mr.sys
 */

#ifndef USERINTERFACE_USERINTERFACE_HPP_
#define USERINTERFACE_USERINTERFACE_HPP_

#include <string> //class std::string

/** Forward declarations */
struct tm;

class UserInterface
{
public:
  ~UserInterface() { }
  virtual void ShowMessage(const char * const ) const = 0;
  virtual void ShowConnectionState(const char * const pch, int timeOut) {}
  
  static std::string GetTimeAsString(const struct tm & timeOfLastSMARTvaluesUpdate);
  static void FormatTimeOfLastUpdate(
    long int timeStampOfRetrievalIn1ks, 
    std::string & timeFormatString);
};

#endif /* USERINTERFACE_USERINTERFACE_HPP_ */
