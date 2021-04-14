#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "phnx02.h"

// HRC_FSEL 1=1M 2=2M 3=4M 4=8M 5=16M
#define HRC_FSEL 4
extern unsigned int SystemCoreClock;
#define TIME_DIFF(s,e) ((s)-(int)(e))
