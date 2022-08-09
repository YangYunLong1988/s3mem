
#ifndef _COMMONLIB_H_
#define _COMMONLIB_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void wPrint(
    wchar_t  *Format,
    ...
);

void print(
    char  *Format,
    ...
);

int closeLogFile(
    void
);

int openLogFile(
    char    *logFilePath
);

int fillMem(
    uint64_t volatile  *bufa,
    uint64_t volatile  *bufb,
    uint64_t           testData,
    size_t             count
);

int verifyMem(
    uint64_t volatile  *bufa,
    uint64_t volatile  *bufb,
    uint64_t           testData,
    size_t             count
);

int SleepInSeconds(
    uint32_t Seconds
);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _COMMONLIB_H_
