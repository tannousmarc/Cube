#ifndef __PHILO_H
#define __PHILO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>

#include "PL011.h"

#include "libc.h"
#include "hilevel.h"


typedef struct {
    int id;
    // pseudo-boolean values
    int left;
    int right;
    int hungry;
} phil_t;



#endif
