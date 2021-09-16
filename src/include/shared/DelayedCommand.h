/*
 * DelayedCommand.h
 *
 *  Created on: 9/09/2021
 *      Author: James Brown
 */

#ifndef INCLUDE_DELAYEDCOMMAND_H_
#define INCLUDE_DELAYEDCOMMAND_H_

#include <cmsis_os.h>
#include <stm32f4xx_hal.h>

#include "shared/Shared.h"

#ifndef DELAY_COMMAND_FILE

#define DELAY_COMMAND_FILE "delay.txt"
#endif

#ifndef DELAY_MAX_ESTTC_SIZE
#include "main.h"

/*!
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @brief The maximum number of characters in an ESTTC command (including null termination)
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define DELAY_MAX_ESTTC_SIZE LINE_BUFFER_SIZE
#endif

#ifndef DELAY_QUEUE_TIMEOUT

/*!
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @brief The maximum number of ms that the command system will wait before failing to queue a
*        command.
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define DELAY_QUEUE_TIMEOUT 100
#endif

/*!
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @brief Creates a thread that runs the delayed command system
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @return           osThreadId - the ID of the thread, or NULL if creation of the thread failed
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
osThreadId EXT(DELAY_createThread)(void);

/*!
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @brief Queues a command for execution in the future
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @param[input]     char const *command - The ESTTC command to execute
* @param[input]     RTC_TimeTypeDef const *time - The time to execute the command after
* @param[input]     RTC_DateTypeDef const *date - The date to execute the command after
* @return           nothing
* @note             If the time specified has already passed the command will be immediately
*                   executed.
* @note             If the SD card is inaccessible for some reason the command list will
*                   be stored in memory
* @note             The date and time must be stored in BCD format
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
void EXT(DELAY_queueCommand)(char const *command, RTC_TimeTypeDef const *time, RTC_DateTypeDef const *date);

/*!
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @brief Executes the given command (MUST BE IMPLEMENTED BY LIBRARY CONSUMER)
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @param[input]     char const *command - The ESTTC command to execute
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
void EXT(DELAY_executeCommand)(char const *command);

#endif /* INCLUDE_DELAYEDCOMMAND_H_ */
