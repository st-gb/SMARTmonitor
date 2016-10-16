/*
 * SMARTuniqueID.hpp
 *
 *  Created on: 05.08.2016
 *      Author: sg
 */

#ifndef ATTRIBUTES_SMARTUNIQUEID_HPP_
#define ATTRIBUTES_SMARTUNIQUEID_HPP_

#include <iostream>
#include <sstream>

#ifdef __linux__
  #include <stddef.h> // size_t in "<atasmart.h>"
  #include <atasmart.h>
//  typedef SkIdentifyParsedData SMARTuniqueID;
//#else
//#ifdef _WIN32
#endif
  /** Same structure as in Linux' "atasmart.h" */
  struct SMARTuniqueID {
    char serial[21];
    char firmware[9];
    char model[41];
    SMARTuniqueID & operator = (const SMARTuniqueID & l);
    SMARTuniqueID() { }
#ifdef __linux__
    SMARTuniqueID(const SkIdentifyParsedData & l);
    ~SMARTuniqueID();
#endif
    /** For necessary function signature see
     *  https://msdn.microsoft.com/en-us/library/1z2f6c2k.aspx */
    friend std::ostream & operator << ( std::ostream & ostr,
      const SMARTuniqueID & obj)
    {
      ostr << obj.firmware << " " << obj.model << " " << obj.serial;
      return ostr;
    }
    std::string str() const;
  };
//#endif
//#endif

  bool operator < (const SMARTuniqueID & left,
                   const SMARTuniqueID & right);

#endif /* ATTRIBUTES_SMARTUNIQUEID_HPP_ */
