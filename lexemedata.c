#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "calctypes.h"
#include "calcmacros.h"
#include "calc.h"
#include "lexeme.h"
#include "lexemedata.h"
#include <math.h>

flag_t LexemeStackCreate(lexeme_stack_t* stk)
{
	stk->arr = (lexeme_t*)Malloc(sizeof(lexeme_t));
	if (stk->arr == NULL)
	{
		return MEMORY_ERROR;
	}
	stk->size = 1;
	stk->last = -1;
	return SUCCESS;
}

flag_t LexemeStackPush(lexeme_stack_t* stk, lexeme_t* lex)
{
	lexeme_t* arr;
	stk->last += 1;
	if (stk->last == stk->size)
	{
		stk->size += 1;
		arr = Realloc(stk->arr, stk->size * sizeof(lexeme_t));
		if (arr == NULL)
		{
			return MEMORY_ERROR;
		}
		stk->arr = arr;
	}
	stk->arr[stk->last] = *lex;
	return SUCCESS;
}

flag_t LexemeStackPop(lexeme_stack_t* stk, lexeme_t* lex)
{
	if (stk->last < 0)
		return EMPTY_STACK;
	*lex = stk->arr[stk->last];
	(stk->last)--;
	return SUCCESS;
}

flag_t LexemeListGet(lexeme_list_t* list, char* name, lexeme_t *lex)
{
	int i;
	for (i = 0; i < list->size; i++)
	{
		if (list->arr[i].name != NULL && list->arr[i].name[0]!= 0 && strcmp(list->arr[i].name, name) == 0)
		{
			*lex = list->arr[i];
			return SUCCESS;
		}
	}
	return LEX_ERROR;
}
/*
flag_t LexemeListAdd(lexeme_list_t* list, lexeme_t* lex)
{
	lexeme_t* arr;
	list->size += 1;
	if(list->size == 1)
		arr = Malloc(sizeof(lexeme_t));
	else
		arr = Realloc(list->arr, list->size * sizeof(lexeme_t));
	if (arr == NULL)
	{
		if (list->size != 1)
			free(list->arr);
		return MEMORY_ERROR;
	}
	list->arr = arr;
	list->arr[list->size-1] = *lex;
	return SUCCESS;
}
*/