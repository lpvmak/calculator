#ifndef LEXEMEDATA_H_INCLUDED__
#define LEXEMEDATA_H_INCLUDED__ 

#include "calc.h"
#include "lexeme.h"

typedef struct lexeme_stack_t
{
	int size;
	int last;
	lexeme_t* arr;
} lexeme_stack_t;

flag_t LexemeStackCreate(lexeme_stack_t* stk);

flag_t LexemeStackPush(lexeme_stack_t* stk, lexeme_t* lex);

flag_t LexemeStackPop(lexeme_stack_t* stk, lexeme_t* lex);

typedef struct lexeme_list_t
{
	int size;
	lexeme_t* arr;
} lexeme_list_t;

flag_t LexemeListGet(lexeme_list_t* list, char* name, lexeme_t* lex);

//flag_t LexemeListAdd(lexeme_list_t* list, lexeme_t* lex);
#endif