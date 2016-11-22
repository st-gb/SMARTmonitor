/*
 * SMARTuniqueIDandValues.hpp
 *
 *  Created on: 05.08.2016
 *      Author: sg
 */

#ifndef ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_
#define ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_

#include "SMARTuniqueID.hpp" //struct SMARTuniqueID
#include <stdint.h> //uint64_t

#define NUM_DIFFERENT_SMART_ENTRIES 256

class SMARTuniqueIDandValues
{
private:
  SMARTuniqueID m_SMARTuniqueID;
public:
  /*uint64_t*/ long int m_SMARTrawValues[NUM_DIFFERENT_SMART_ENTRIES];
  long int m_successfullyReadSMARTrawValue[NUM_DIFFERENT_SMART_ENTRIES];
  SMARTuniqueIDandValues (const SMARTuniqueID & _SMARTuniqueID);
  SMARTuniqueIDandValues () {}
  SMARTuniqueIDandValues( const SMARTuniqueIDandValues &);
  virtual
  ~SMARTuniqueIDandValues ();

  const SMARTuniqueID & getSMARTuniqueID() const { return m_SMARTuniqueID; }
  void SetDataCarrierID(const SMARTuniqueID & sMARTuniqueID) {
    m_SMARTuniqueID = sMARTuniqueID;
  }
};

bool operator < (const SMARTuniqueIDandValues & left,
                 const SMARTuniqueIDandValues & right);

#endif /* ATTRIBUTES_SMARTUNIQUEIDANDVALUES_HPP_ */
