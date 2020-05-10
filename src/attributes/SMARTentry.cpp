#include "SMARTentry.hpp"

bool operator < (const SMARTattrDef & left, const SMARTattrDef & right)
{
  return left./*id*/GetAttributeID() < right./*id*/GetAttributeID();
}
