#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>

uint64_t        *aPFNs;             // page info; holds opaque data
uint32_t        PFNArraySize;       // memory to request for PFN array
void            *lpMemReserved;     // AWE window
uint32_t        SysPageSize;        // System page size

int virtToPhy(
    uint64_t volatile *VirtualAddress,
    uint64_t volatile *PhysicalAddress
)
{
    size_t  PageOffset, PageIndex;

    if((aPFNs == NULL) || (lpMemReserved == NULL))
    {
        return EFAULT;
    }

    PageIndex = ((uint64_t)VirtualAddress - (uint64_t)lpMemReserved) / SysPageSize;
    PageOffset = ((uint64_t)VirtualAddress - (uint64_t)lpMemReserved) % SysPageSize;

    if(PageIndex > (PFNArraySize / sizeof(uint64_t)))
    {
        printf("Incorrect Page Index!\n");
        return EXIT_FAILURE;
    }

    *PhysicalAddress = aPFNs[PageIndex] << 12 | PageOffset;

    return EXIT_SUCCESS;
}

int prepareTestMemArea(
    size_t           *MemorySize,
    void    volatile **MemoryAddress
)
{
    SYSTEM_INFO     SysInfo;                // system information
    BOOL            bResult;                // generic Boolean value
    HANDLE          hProcess = GetCurrentProcess();         // Handle of current process
    uint64_t        NumberOfPages, NumberOfPagesInitial;    // number of pages to request

    GetSystemInfo(&SysInfo);  // fill the system information structure

    SysPageSize = SysInfo.dwPageSize;

    // Calculate the number of pages of memory to request.

    NumberOfPages = (*MemorySize)/SysPageSize;

    // Calculate the size of the user PFN array.

    PFNArraySize = (uint32_t)(NumberOfPages * sizeof (uint64_t));

    aPFNs = (uint64_t *) HeapAlloc(GetProcessHeap(), 0, PFNArraySize);

    if (aPFNs == NULL)
    {
        printf("Failed to allocate on heap.\n");
        return EXIT_FAILURE;
    }

    // Allocate the physical memory.

    NumberOfPagesInitial = NumberOfPages;
    bResult = AllocateUserPhysicalPages(hProcess,
                                       &NumberOfPages,
                                       aPFNs);

    if( bResult != TRUE )
    {
        printf("Cannot allocate physical pages (%u)\n", GetLastError());
        return EXIT_FAILURE;
    }

    if( NumberOfPagesInitial != NumberOfPages )
    {
        printf("Allocated only %lld pages.\n", NumberOfPages);
        PFNArraySize = (uint32_t)(NumberOfPages * sizeof (uint64_t));
    }

    // Reserve the virtual memory.

    lpMemReserved = VirtualAlloc(NULL,
                                *MemorySize,
                                MEM_RESERVE | MEM_PHYSICAL,
                                PAGE_READWRITE);

    if( lpMemReserved == NULL )
    {
        printf("Cannot reserve memory.\n");
        return EXIT_FAILURE;
    }

    // Map the physical memory into the window.

    bResult = MapUserPhysicalPages( lpMemReserved,
                                    NumberOfPages,
                                    aPFNs );

    if( bResult != TRUE )
    {
        printf("MapUserPhysicalPages failed (%u)\n", GetLastError());
        return EXIT_FAILURE;
    }

    *MemorySize = NumberOfPages * SysPageSize;
    *MemoryAddress = lpMemReserved;
    return EXIT_SUCCESS;
}

int AcquirePrivilidge(void)
{
    struct {
        uint32_t Count;
        LUID_AND_ATTRIBUTES Privilege [2];
    } Info;

    HANDLE  Token, hProcess = GetCurrentProcess();
    BOOL    Result;

    // Open the token.

    Result = OpenProcessToken ( hProcess,
                                TOKEN_ADJUST_PRIVILEGES,
                                &Token);

    if( Result != TRUE )
    {
        printf("Cannot open process token.\n");
        return EPERM;
    }

    // Enable Privilidge

    Info.Count = 2;
    Info.Privilege[0].Attributes = SE_PRIVILEGE_ENABLED;
    Info.Privilege[1].Attributes = SE_PRIVILEGE_ENABLED;

    // Get the LUID.

    Result = LookupPrivilegeValue ( NULL,
                                    SE_LOCK_MEMORY_NAME,
                                    &(Info.Privilege[0].Luid));

    if( Result != TRUE )
    {
        printf("Cannot get privilege for %s.\n", SE_LOCK_MEMORY_NAME);
        return EEXIST;
    }

    Result = LookupPrivilegeValue ( NULL,
                                    SE_SHUTDOWN_NAME,
                                    &(Info.Privilege[1].Luid));

    if( Result != TRUE )
    {
        printf("Cannot get privilege for %s.\n", SE_SHUTDOWN_NAME);
        return EEXIST;
    }

    // Adjust the privilege.

    Result = AdjustTokenPrivileges ( Token, FALSE,
                                    (PTOKEN_PRIVILEGES) &Info,
                                    0, NULL, NULL);

    // Check the result.

    if( Result != TRUE )
    {
        printf("Cannot adjust token privileges (%u)\n", GetLastError() );
        return EPERM;
    }
    else
    {
        if( GetLastError() != ERROR_SUCCESS )
        {
            printf("Cannot enable the SE_LOCK_MEMORY_NAME privilege; ");
            printf("please check the local policy.\n");
            return EPERM;
        }
    }

    CloseHandle( Token );

    return EXIT_SUCCESS;
}

