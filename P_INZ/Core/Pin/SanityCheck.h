/*
 * SanityCheck.h
 *
 *  Created on: 6 sty 2020
 *      Author: Mateusz Jurczak
 */

#ifndef INC_SANITYCHECK_H_
#define INC_SANITYCHECK_H_
#include "configuration.h"


#ifndef POWER_FIRST
#error "Non power based scheduling not implemented yet, and most likely will not be as it does not make too much sense. Please enable POWER_FIRST in configuration.h"
#endif

#if NoOfDevices > 4
#error "Number of BLE devices can't be greater than 4. Please change NoOfDevices in configuration.h"
#endif

#if MAX_NO_OF_DISCOVERABLE_DEVICES > 20
#error "MAX_NO_OF_DISCOVERABLE_DEVICES is higher than 20,please change it in configuration.h"
#endif

#endif /* INC_SANITYCHECK_H_ */
