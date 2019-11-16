/**
 * based on common.h,v 1.2 2007/06/18 17:58:52 lordjaxom
 *
 * version by Midas
 *
 */

#ifndef VDR_BLOCK_COMMON_H
#define VDR_BLOCK_COMMON_H

#if VDRVERSNUM >= 10318
inline uint64_t time_ms() { return cTimeMs::Now(); }
#endif

#endif // VDR_BLOCK_COMMON_H
