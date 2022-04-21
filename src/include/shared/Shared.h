
/*
 * Shared.h
 *
 *  Created on: 9/09/2021
 *      Author: James Brown
 */

#ifndef SHARED_H_
#define SHARED_H_

#include <stdbool.h>

#include <cmsis_os.h>
#include <stm32f4xx_hal.h>

#ifndef SHARED_PREFIX

/*!
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @brief A prefix for every externally accessible function name in this module
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define SHARED_PREFIX

#endif

#define _EXT_CONCAT(prefix, name) prefix ## name
#define _EXT(prefix, name) _EXT_CONCAT(prefix, name)

/**
 * Mangles a function or variable name to the external form
 */
#define EXT(name) _EXT(SHARED_PREFIX, name)

/*!
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @brief Gets the RTC time
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @param[output]    RTC_TimeTypeDef *time - The time
* @param[output]    RTC_DateTypeDef *date - The date
* @param[input]     uint32_t format - The format of the output data, must be RTC_FORMAT_BIN or RTC_FORMAT_BCD
* @return           HAL_StatusTypeDef - HAL_OK if the call succeeded. If anything else, the contents of date and time are undefined
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
HAL_StatusTypeDef EXT(CORE_getTime)(RTC_TimeTypeDef *time, RTC_DateTypeDef *date, uint32_t format);

/*!
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @brief Enters an exclusive access region (MUST BE IMPLEMENTED BY LIBRARY CONSUMER)
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @param[input]     uint32_t millisec - How long to wait to access the region, osWaitForever indicates
*                                       that the call should wait indefinitely. A value of zero indicates
*                                       that the call does not wait.
* @return           osStatus - osOK if the call succeeded
*                              osTimeoutResource if the timeout was exceeded and it was non-zero
*                              osResource if the region could not be entered and the timeout was zero
*                              any other value if the call failed for another reason
* @note             This must be reentrant capable.
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
osStatus EXT(CORE_enterExclusive)(uint32_t millisec);

/*!
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @brief Exits an exclusive access region (MUST BE IMPLEMENTED BY LIBRARY CONSUMER)
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @return           osStatus - osOK if the call succeeded
*                              osErrorResource the region was never entered
*                              any other value if the call failed for another reason
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
osStatus EXT(CORE_exitExclusive)(void);

#endif /* SHARED_H_ */
