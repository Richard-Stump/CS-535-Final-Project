#ifndef PROJECT_H
#define PROJECT_H

// include common lib
#include "fp_lib.hpp"

// other stuff
#include "stdio.h"

#include <stdio.h>

// Any of the SDK headers that are not part of GCC's libraries need to be in an
// extern "C" block to prevent C++ name mangling. 
#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>
#include <unistd.h>

#ifdef __cplusplus
}
#endif//__cplusplus

#endif