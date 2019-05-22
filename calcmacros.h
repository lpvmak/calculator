#ifndef CALCMACROS
#define CALCMACROS
#pragma once

#include <stdio.h>

#define EXITWITH(flag)\
{printf("ERROR: "#flag);\
return flag;}

#define PRINTERROR(out, flag) __printerr__(out, flag)

#endif