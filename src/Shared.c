/*
 * Core.c
 *
 *  Created on: 9/09/2021
 *      Author: James Brown
 */

#include "shared/Shared.h"
#include "shared/Error.h"

#include "MCU_Init.h"

HAL_StatusTypeDef EXT(CORE_getTime)(RTC_TimeTypeDef *time, RTC_DateTypeDef *date, uint32_t format) {
    // Must be read in this order, reading getTime prepares the date variable to be read
    // Disable interrupts during this call to avoid time-slicing the RTC reads
    taskENTER_CRITICAL();
    HAL_StatusTypeDef timeStatus = HAL_RTC_GetTime(&hrtc, time, format);
    HAL_StatusTypeDef dateStatus = HAL_RTC_GetDate(&hrtc, date, format);
    taskEXIT_CRITICAL();

    if (timeStatus != HAL_OK) {
        ERR_LOG_ERROR_F("Unable to read RTC time (%d)", timeStatus);
        return timeStatus;
    }


    if (dateStatus != HAL_OK) {
        ERR_LOG_ERROR_F("Unable to read RTC date (%d)", dateStatus);
        return dateStatus;
    }

    return HAL_OK;
}
