/*
 * SMARTuniqueID.hpp
 *
 *  Created on: 05.08.2016
 *      Author: sg
 */

#ifndef ATTRIBUTES_SMARTUNIQUEID_HPP_
#define ATTRIBUTES_SMARTUNIQUEID_HPP_

#ifdef __linux__
  #include <stddef.h> // size_t in "<atasmart.h>"
  #include <atasmart.h>
  typedef SkIdentifyParsedData SMARTuniqueID;
#else
#ifdef _WIN32
  /** Same structure as in Linux' "atasmart.h" */
  struct SMARTuniqueID {
          char serial[21];
          char firmware[9];
          char model[41];
  }
#endif
#endif

  bool operator < (const SMARTuniqueID & left,
                   const SMARTuniqueID & right);


#endif /* ATTRIBUTES_SMARTUNIQUEID_HPP_ */
