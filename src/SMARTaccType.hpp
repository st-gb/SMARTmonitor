/** File:   SMARTaccType.hpp
 * Author: S. Gebauer
 * Created on 29. Mai 2020, 20:37 */

#ifndef SMARTACCTYPE_HPP
#define SMARTACCTYPE_HPP

#ifdef _WIN32
  #include "SmartReader.h" //class CSmartReader
#endif
#ifdef __linux__
  #include <libATA_SMART/SMARTaccess.hpp>
  typedef libatasmart::SMARTaccess SMARTaccess_type;
#endif

#endif /* SMARTACCTYPE_HPP */
