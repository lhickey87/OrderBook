
#pragma once
#include <stdint.h>

#if defined(__APPLE__) || defined(__MACH__)
    #include <libkern/OSByteOrder.h>

    #define be16toh(x) OSSwapBigToHostInt16(x)
    #define be64toh(x) OSSwapBigToHostInt64(x)
    #define htobe64(x) OSSwapHostToBigInt64(x)
    #define be32toh(x) OSSwapBigToHostInt32(x)
    #define htobe32(x) OSSwapHostToBigInt32(x)

#elif defined(__linux__) || defined(__CYGWIN__)
//this should work for linux
    #include <endian.h>
    // but they can be kept for consistency if needed.

#else
//figure out for windows

#endif
