/*
 * SMARTuniqueIDandValues.hpp
 *
 *  Created on: 05.08.2016
 *      Author: sg
 */

#ifndef ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_
#define ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_

#include "SMARTuniqueID.hpp" //struct SMARTuniqueID

class SMARTuniqueIDandValues
{
private:
  SMARTuniqueID m_SMARTuniqueID;
public:
  uint64_t m_SMARTrawValues[255];
  SMARTuniqueIDandValues ();
  virtual
  ~SMARTuniqueIDandValues ();

  const SMARTuniqueID & getSMARTuniqueID() const { return m_SMARTuniqueID; }
};

bool operator < (const SMARTuniqueIDandValues & left,
                 const SMARTuniqueIDandValues & right);

#endif /* ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_ */
