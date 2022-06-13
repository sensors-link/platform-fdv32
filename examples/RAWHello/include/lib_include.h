#ifndef __LIB_INCLUDE__
#define __LIB_INCLUDE__

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#if defined FDV32S301
#include "phnx02.h"
#elif defined FDV32S302
#include "phnx04.h"
#elif defined FDV32F003
#include "phnx05.h"
#else
#error "FDV32S301/FDV32S302/FDV32F003 should be defined!"
#endif

// HRC_FSEL 1=1M 2=2M 3=4M 4=8M 5=16M
#define HRC_FSEL 4
extern unsigned int SystemCoreClock;
#define TIME_DIFF(s,e) ((s)-(int)(e))

#endif /* __LIB_INCLUDE__ */
