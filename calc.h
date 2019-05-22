#ifndef CALC_H_INCLUDED__
#define CALC_H_INCLUDED__ 
#pragma once

#include "calctypes.h"
#include "lexeme.h"
int num;

void* Malloc(size_t _Size);
void* Realloc(void* _Block, size_t _Size);

flag_t calcInit(void);
flag_t calc( char* str, double* res);

static flag_t RPN(lexeme_t *exp, int size, double* res);

#endif // CALC_H_INCLUDED__