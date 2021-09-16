/*
 * Error.h
 *
 *  Created on: 9/09/2021
 *      Author: James Brown
 */

#ifndef SHARED_ERROR_H_
#define SHARED_ERROR_H_

#include <stdio.h>

#include "shared/Shared.h"

#ifndef ERR_MAX_LENGTH
/*!
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @brief The maximum string length that will be logged
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#define ERR_MAX_LENGTH 64
#endif

#define ERR_LOG_ERROR(msg) ERR_logError(msg, __FILE__, __LINE__)

#ifndef ERR_NO_HANDLING

#define ERR_LOG_ERROR_F(msg, ...) { \
    char formattedMessage [ERR_MAX_LENGTH + 1]; \
    snprintf(formattedMessage, sizeof(formattedMessage), msg, __VA_ARGS__); \
    ERR_LOG_ERROR(formattedMessage); \
}

#else

#define ERR_LOG_ERROR_F(...)

#endif

#ifndef ERR_NO_HANDLING
/*!
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @brief Logs an error (MUST BE IMPLEMENTED BY LIBRARY CONSUMER)
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* @param[input]     char const *msg - The error message to log
* @param[input]     char const *file - The file the error occurred in
* @param[input]     unsigned int line - The line number
* @return           nothing
* @note             The strings will be truncated if they are longer than ERR_MAX_LENGTH excluding '\0'
*                   The file string will be truncated in reverse if needed; the last ERR_MAX_LENGTH characters will
*                   be kept.
* @note             This function must be implemented by each individual mission
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
void EXT(ERR_logError)(char const *msg, char const *file, unsigned int line);

#else

void EXT(ERR_logError)(char const *msg, char const *file, unsigned int line) {
    // Do nothing
}

#endif /* ERR_NO_HANDLING  */

#endif /* SHARED_ERROR_H_ */
