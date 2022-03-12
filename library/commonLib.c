#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <wchar.h>

#include "commonlib.h"

#define MAX_PRINT_BUFFER_LEN    0x200

static wchar_t  wprintBuffer[MAX_PRINT_BUFFER_LEN];
static char     printBuffer[MAX_PRINT_BUFFER_LEN];
static FILE     *logFile = NULL;

int openLogFile(
    char    *logFilePath
)
{
    logFile = fopen(logFilePath, "r");
    // If log file already exists, return error.
    if(logFile != NULL)
    {
        return EEXIST;
    }

    // open for log write
    logFile = fopen(logFilePath, "w");

    if(logFile == NULL)
    {
        return EIO;
    }

    return EXIT_SUCCESS;
}

int closeLogFile(void)
{
    if(logFile == NULL)
    {
        return EFAULT;
    }

    fflush(logFile);

    if(fclose(logFile))
    {
        return errno;
    }

    logFile = NULL;

    return EXIT_SUCCESS;
}

void wPrint(
    wchar_t  *Format,
    ...
)
{
    va_list ArgList;

    va_start(ArgList, Format);

    vswprintf(wprintBuffer, MAX_PRINT_BUFFER_LEN, Format, ArgList);

    va_end(ArgList);

    fwprintf(stderr, L"%s", wprintBuffer);
    if(logFile != NULL)
    {
        fwrite(wprintBuffer, wcsnlen(wprintBuffer, sizeof(wprintBuffer) * 2), 1, logFile);
    }

    return;
}

void print(
    char  *Format,
    ...
)
{
    va_list ArgList;

    va_start(ArgList, Format);

    vsnprintf(printBuffer, MAX_PRINT_BUFFER_LEN, Format, ArgList);

    va_end(ArgList);

    fprintf(stderr, "%s", printBuffer);
    if(logFile != NULL)
    {
        fwrite(printBuffer, strlen(printBuffer), 1, logFile);
    }

    return;
}