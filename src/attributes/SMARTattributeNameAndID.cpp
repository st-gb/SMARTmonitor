/** SMARTattributeNameAndID.cpp
 *  Created on: 28.10.2016
 *  Author: Stefan Gebauer, M.Sc.Comp.Sc. (TU Berlin)*/

#include <attributes/SMARTattributeNameAndID.hpp>

//SMARTattributeNameAndID::SMARTattributeNameAndID ()
//{
//  // TODO Auto-generated constructor stub
//}

SMARTattributeNameAndID::~SMARTattributeNameAndID ()
{
  // TODO Auto-generated destructor stub
}

///For associative/sorting  STL containers etc.
bool operator < (const SMARTattributeNameAndID & left,
  const SMARTattributeNameAndID & right)
{
  return left.GetID() < right.GetID();
}
