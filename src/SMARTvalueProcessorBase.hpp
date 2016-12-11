/*
 * SMARTvalueProcessorBase.hpp
 *
 *  Created on: 05.08.2016
 *      Author: sg
 */

#ifndef SMARTVALUEPROCESSORBASE_HPP_
#define SMARTVALUEPROCESSORBASE_HPP_

#ifdef _WINDOWS
  #include "SmartReader.h" //class CSmartReader
#endif
#ifdef __linux__
  #include <libATA_SMART/SMARTaccess.hpp>
  typedef libatasmart::SMARTaccess SMARTaccess_type;
#endif

class SMARTvalueProcessorBase
{
protected:
  SMARTaccess_type m_SMARTaccess;
public:
  SMARTvalueProcessorBase ();
  virtual
  ~SMARTvalueProcessorBase ();
  SMARTaccess_type & getSMARTaccess() const { return (SMARTaccess_type &) m_SMARTaccess; }
};

#endif /* SMARTVALUEPROCESSORBASE_HPP_ */
