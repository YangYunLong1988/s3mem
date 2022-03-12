
#ifndef _OSDEPSLIB_H_
#define _OSDEPSLIB_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int prepareTestMemArea(
    size_t           *MemorySize,
    void    volatile **MemoryAddress
);

int suspendSystem(uint64_t Delay);

int virtToPhy(
    uint64_t volatile *VirtualAddress,
    uint64_t volatile *PhysicalAddress
);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _OSDEPSLIB_H_