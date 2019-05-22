#ifndef LEXEME_H_INCLUDED__
#define LEXEME_H_INCLUDED__ 
#pragma once
#include "calc.h"

#define UNAR_MINUS ('~')
#define LEXEME_NAME_LEN 8
typedef char oper_t;

#define LEXEME_T(lex) lexeme_t lex; lex.name = NULL


typedef double (*func_t)(double);


typedef enum lexeme_type
{
	END = 0, VAR, OPER, FUNC_1, FUNC_2
} lexeme_type_t;

typedef union lexeme_essence
{
	double var;
	oper_t oper;
	func_t func;
} lexeme_essence_t;

typedef struct lexeme_t
{
	char* name;
	lexeme_type_t type;
	lexeme_essence_t ess;
} lexeme_t;

int isBinaryOper(char c);

int isRightOper(oper_t c);

int isOperChar(char c);

int LexemeOperPriority(oper_t c);

int LexemeIsOper(lexeme_t lex);

int LexemeIsFunc(lexeme_t lex);

int LexemeIsVar(lexeme_t lex);

void LexemeWrite(FILE* file, lexeme_t lex);

lexeme_t LexemeCreateOper(char c);

lexeme_t LexemeCreateNumber(double var);

lexeme_t LexemeCreateVar(char* name, double var);

lexeme_t LexemeCreateFunc(char* name, func_t func);

flag_t LexemeParse(char *str, lexeme_t *lex, int *i, int isPrevOper);

flag_t LexemeSeqParse(char *str, lexeme_t** lexseq, int* size);

#endif // LEXEME_H_INCLUDED__ 