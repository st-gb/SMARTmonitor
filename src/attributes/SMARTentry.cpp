#include "SMARTentry.hpp"

bool operator < (const SMARTentry & left, const SMARTentry & right)
{
  return left.id < right.id;
}
