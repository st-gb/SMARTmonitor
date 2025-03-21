/** SMARTattributeNameAndID.hpp
 * (c) Stefan Gebauer(TU Berlin matriculation number 361095)
 *  Created on: 28.10.2016
 * @author: Stefan Gebauer(Computer Science Master from TU Berlin) */

#ifndef ATTRIBUTES_SMARTATTRIBUTENAMEANDID_HPP_
#define ATTRIBUTES_SMARTATTRIBUTENAMEANDID_HPP_

///C(++) standard library header files:
 #include <set>///class std::set
 #include <string>///class std::string

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
