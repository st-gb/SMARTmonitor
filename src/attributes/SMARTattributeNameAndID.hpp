/*
 * SMARTattributeNameAndID.hpp
 *
 *  Created on: 28.10.2016
 *      Author: root
 */

#ifndef ATTRIBUTES_SMARTATTRIBUTENAMEANDID_HPP_
#define ATTRIBUTES_SMARTATTRIBUTENAMEANDID_HPP_

#include <string> //class std::string

class SMARTattributeNameAndID
{
  std::string name;
  unsigned id;
public:
  SMARTattributeNameAndID (const char * const name, unsigned id)
  {
    this->name = name;
    this->id = id;
  }
  virtual
  ~SMARTattributeNameAndID ();
  unsigned GetID() const { return id; }
  const std::string & getName() const { return name; }
};

#endif /* ATTRIBUTES_SMARTATTRIBUTENAMEANDID_HPP_ */
