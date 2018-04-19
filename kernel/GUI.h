#ifndef __GUI_H
#define __GUI_H

// Include functionality relating to newlib (the standard C library).

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>

// Include functionality relating to the platform.

#include   "GIC.h"
#include "PL011.h"
#include "SP804.h"

// Include functionality relating to the   kernel.

#include "lolevel.h"
#include     "int.h"
#include "hilevel.h"




extern void options(int selected, int level, int toggleDash);
extern void displayDashboard(int processes, pcb_t pcb[]);
#endif
