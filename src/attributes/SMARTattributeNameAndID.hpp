/** SMARTattributeNameAndID.hpp
 *  Created on: 28.10.2016
 *  Author: Stefan Gebauer, M.Sc.Comp.Sc. (TU Berlin) */

#ifndef ATTRIBUTES_SMARTATTRIBUTENAMEANDID_HPP_
#define ATTRIBUTES_SMARTATTRIBUTENAMEANDID_HPP_

#include <string> //class std::string
#include <set>///class std::set

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

///For ordered/associative std::container
//TODO faster if this is an inline function?!
bool operator < (const SMARTattributeNameAndID & left,
  const SMARTattributeNameAndID & right);

///Define here, so it can be used without SMARTaccessBase class
typedef std::set<SMARTattributeNameAndID> suppSMART_IDsType;

#endif /* ATTRIBUTES_SMARTATTRIBUTENAMEANDID_HPP_ */
