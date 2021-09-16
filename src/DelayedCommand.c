/*
 * DelayedCommand.c
 *
 *  Created on: 9/09/2021
 *      Author: James Brown
 */

#include "shared/DelayedCommand.h"

#include <string.h>

#include <cmsis_os.h>
#include <fatfs.h>

#include "shared/Error.h"

#define DELAY_STACK_SIZE 1024

static void taskFunction(void const *arg);
static osThreadDef(commandHandlingThread, taskFunction, osPriorityNormal, 1, DELAY_STACK_SIZE);
static osThreadId threadId;

struct QueuedCommand;

typedef struct {
    char command [DELAY_MAX_ESTTC_SIZE];
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
} QueuedCommand_t;

static osMailQDef(submitQueue, 1, QueuedCommand_t);
static osMailQId submitQueueId;

static FIL *fp = NULL;

static void queueCommand(QueuedCommand_t *command) {
    FRESULT fresult = f_lseek(fp, f_size(fp));
    if (fresult != FR_OK) {
        goto error;
    }

    UINT length;
    fresult = f_write(fp, command, sizeof(*command), &length);
    if (length != sizeof(*command)) {
        goto error;
    }

    return;

error:
    ERR_LOG_ERROR_F("Unable to write command to file (%d)", fresult);
}

static void executeCommand(QueuedCommand_t *command) {
    EXT(DELAY_executeCommand)(command->command);

    // Remove the command from the executable list
    FRESULT fresult;
    for (unsigned int i = 0; ; i++) {
        QueuedCommand_t copy;
        UINT length;

        fresult = f_lseek(fp, sizeof(*command) * (i + 1));
        if (fresult != FR_OK) {
            goto error;
        }

        fresult = f_read(fp, &copy, sizeof(*command), &length);
        if (fresult != FR_OK) {
            goto error;
        }

        fresult = f_lseek(fp, sizeof(*command) * i);
        if (fresult != FR_OK) {
            goto error;
        }

        // We are done, EOF. The read-write head is now at the end of the last written
        // command. Truncate the file here.
        if (length != sizeof(*command)) {
            fresult = f_truncate(fp);
            if (fresult != FR_OK) {
                goto error;
            }

            return;
        }

        fresult = f_write(fp, &copy, sizeof(*command), &length);
        if (length != sizeof(*command)) {
            ERR_LOG_ERROR("Unable to write to command file");
            return;
        }

        if (fresult != FR_OK) {
            goto error;
        }
    }

error:
    ERR_LOG_ERROR_F("Unable to remove executed command (%d)", fresult);
}

static bool findNextCommand(QueuedCommand_t *command) {
    FRESULT fresult = f_rewind(fp);
    if (fresult != FR_OK) {
        goto error;
    }

    UINT length;
    fresult = f_read(fp, command, sizeof(*command), &length);
    if (fresult != FR_OK) {
        goto error;
    }

    return length == sizeof(*command);

error:
    ERR_LOG_ERROR_F("Unable to read next command from file (%d)", fresult);
    return false;
}

static bool shouldExecute(QueuedCommand_t *command) {
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    if (EXT(CORE_getTime)(&time, &date, RTC_FORMAT_BCD) != HAL_OK) {
        return false;
    }

#define DELAY_COMPARE(term) {           \
    if (term != command->term) {        \
        return term > command->term;    \
    }                                   \
}

    DELAY_COMPARE(date.Year)
    DELAY_COMPARE(date.Month)
    DELAY_COMPARE(date.Date)
    DELAY_COMPARE(time.Hours)
    DELAY_COMPARE(time.Minutes)
#undef DELAY_COMPARE

    return time.Seconds >= command->time.Seconds;
}

static bool openFile(void) {
    if (EXT(CORE_enterSDRegion)(osWaitForever) != osOK) {
        return false;
    }

    FRESULT fresult = f_open(fp, DELAY_COMMAND_FILE, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    if (fresult != FR_OK) {
        ERR_LOG_ERROR_F("Unable to open command file (%d)", fresult);
        EXT(CORE_exitSDRegion)();
        return false;
    }

    return true;
}

static void closeFile(void) {
    FRESULT fresult = f_close(fp);
    if (fresult != FR_OK) {
        ERR_LOG_ERROR_F("Unable to close command file (%d)", fresult);
    }

    EXT(CORE_exitSDRegion)();
}

static void taskFunction(void const *arg) {
    submitQueueId = osMailCreate(osMailQ(submitQueue), threadId);
    if (submitQueueId == NULL) {
        ERR_LOG_ERROR("Unable to create submission queue");
        return;
    }

    bool hasNextCommand = false;
    QueuedCommand_t nextCommand;

    while (true) {
        uint32_t wait = osWaitForever;

        if (hasNextCommand) {
            wait = shouldExecute(&nextCommand) ? 0 : osKernelSysTickFrequency;
        }

        osEvent event = osMailGet(submitQueueId, wait);

        switch (event.status) {
        case osOK:
        case osEventTimeout:
            if (!hasNextCommand || !shouldExecute(&nextCommand)) {
                // We waited the maximum time period, or there was no command. Wait again
                continue;
            }

            openFile();
            executeCommand(&nextCommand);
            break;

        case osEventMail:
            // We have a command to queue
            openFile();
            queueCommand((QueuedCommand_t *) event.value.p);
            osMailFree(submitQueueId, event.value.p);
            break;

        default:
            ERR_LOG_ERROR_F("Failed to wait (%d)", event.status);
            return;
        }

        hasNextCommand = findNextCommand(&nextCommand);
        closeFile();
    }
}

osThreadId EXT(DELAY_createThread)(void) {
    threadId = osThreadCreate(osThread(commandHandlingThread), NULL);
    if (threadId == NULL) {
        ERR_LOG_ERROR("Failed to create command execution thread");
    }

    return threadId;
}

void EXT(DELAY_queueCommand)(char const *command, RTC_TimeTypeDef const *time, RTC_DateTypeDef const *date) {
    osStatus osResult;

    // Include the null byte in the copy
    size_t length = strlen(command) + 1;
    if (length > DELAY_MAX_ESTTC_SIZE) {
        ERR_LOG_ERROR_F("ESTTC command string too long (%ul)", length);
        return;
    }

    QueuedCommand_t *mail = osMailAlloc(submitQueueId, DELAY_QUEUE_TIMEOUT);
    if (mail == NULL) {
        ERR_LOG_ERROR("Failed to submit command");
        return;
    }

    memcpy(mail->command, command, length);
    mail->time = *time;
    mail->date = *date;

    osResult = osMailPut(submitQueueId, mail);
    if (osResult != osOK) {
        ERR_LOG_ERROR_F("Failed to submit command (%d)", osResult);
        return;
    }
}
