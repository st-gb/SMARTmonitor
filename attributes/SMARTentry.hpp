/*
 * SMARTentry.hpp
 *
 *  Created on: 06.08.2016
 *      Author: sg
 */

#ifndef ATTRIBUTES_SMARTENTRY_HPP_
#define ATTRIBUTES_SMARTENTRY_HPP_

#include <stddef.h> //size_t in <atasmart.h>
#include <atasmart.h> //struct SkSmartAttributeParsedData

typedef struct SkSmartAttributeParsedData SMARTentry;

bool operator < (const SMARTentry & left, const SMARTentry & right);

#endif /* ATTRIBUTES_SMARTENTRY_HPP_ */
