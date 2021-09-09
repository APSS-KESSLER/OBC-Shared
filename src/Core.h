/*
 * Core.h
 *
 *  Created on: 9/09/2021
 *      Author: James Brown
 */

#ifndef CORE_H_
#define CORE_H_

#include "Shared.h"

/**
 * Mangles a function or variable name to the external form
 */
#define EXT(name) SHARED_PREFIX ## name

#endif /* CORE_H_ */
