#pragma warning(disable:4996)
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

extern lexeme_list_t lexemeList_g;


int isOperChar(char c)
{
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '(' || c == ')' || c == UNAR_MINUS || c==',';
}

int isBinaryOper(char c)
{
	return isOperChar(c) && c != UNAR_MINUS && c != '(' && c != ')';
}

int LexemeIsOper(lexeme_t lex)
{
	return lex.type == OPER;
}

int LexemeIsFunc(lexeme_t lex)
{
	return lex.type == FUNC_1 || lex.type == FUNC_2;
}

int LexemeIsVar(lexeme_t lex)
{
	return lex.type == VAR;
}

int LexemeIsBinaryOper(lexeme_t lex)
{
	return LexemeIsOper(lex) && isBinaryOper(lex.ess.oper);
}

int LexemeOperPriority(oper_t c)
{
	switch (c)
	{
	case '(':
	case ')':
		return 0;
	case ',':
		return 1;
	case '+':
	case '-':
		return 2;
	case '*':
	case '/':
		return 3;
	case UNAR_MINUS:
	case '^':
		return 5;
	default:
		return 0;
	}
}

int isRightOper(oper_t c)
{
	return c == '^' || c == UNAR_MINUS;
}

lexeme_t LexemeCreateOper(char c)
{
	LEXEME_T(lex);
	lex.type = OPER;
	lex.ess.oper = c;
	lex.name = NULL;
	return lex;
}

lexeme_t LexemeCreateFunc(char* name, func_t func)
{
	LEXEME_T(lex);
	int i = 0;
	lex.type = FUNC_1;
	lex.ess.func = func;
	if (name != NULL)
	{
		lex.name = (char*)Malloc(LEXEME_NAME_LEN + 1);
		if (lex.name != NULL)
		{
			for (i = 0; name[i] != 0 && i < LEXEME_NAME_LEN; i++)
				lex.name[i] = name[i];
			lex.name[i] = 0;
		}
	}
	else
		lex.name = NULL;
	return lex;
}


lexeme_t LexemeCreateNumber(double var)
{
	return LexemeCreateVar(NULL, var);
}

lexeme_t LexemeCreateVar(char* name, double var)
{
	LEXEME_T(lex);
	int i = 0;
	lex.type = VAR;
	lex.ess.var = var;
	if (name != NULL)
	{
		lex.name = (char*)Malloc(LEXEME_NAME_LEN + 1);
		if (lex.name != NULL)
		{
			for (i = 0; name[i] != 0 && i < LEXEME_NAME_LEN; i++)
				lex.name[i] = name[i];
			lex.name[i] = 0;
		}
	}
	else
		lex.name = NULL;
	return lex;
}

void LexemeWrite(FILE* file, lexeme_t lex)
{
	if (LexemeIsOper(lex))
	{
		if (lex.ess.oper == UNAR_MINUS)
		{
			fprintf(file, "%c", '~');
		}
		else
			fprintf(file, "%c", lex.ess.oper);
	}
	else if (LexemeIsVar(lex))
		fprintf(file, "%lf", lex.ess.var);
	else if (LexemeIsFunc(lex))
		fprintf(file, "%s", lex.name);
}

static char* strtow(char* str, char** endptr)
{
	int i;
	int size = 1;
	char* word = Malloc(size);
	char* _word;
	if (word == NULL)
		return NULL;
	for (i = 0; str[i] != 0 && isalpha(str[i]); i++)
	{
		size = i + 2;
		_word = Realloc(word, size);
		if (_word == NULL)
		{
			free(word);
			return NULL;
		}
		else
			word = _word;
		word[i] = str[i];
	}
	word[i] = 0;
	if (endptr != NULL)
	{
		*endptr = &str[i];
	}
	return word;
}

flag_t LexemeParse(char *str, lexeme_t *lex, int *i, int isPrevOper)
{
	if (isdigit((unsigned char)str[*i]) || str[*i] == '.')
	{
		lex->type = VAR;
		char* endptr = NULL;
		lex->ess.var = strtod(&str[*i], &endptr);
		if (endptr == &str[*i])
			return LEX_ERROR;
		*i += endptr - &str[*i];
	}
	else if (isOperChar(str[*i]) && str[*i] != UNAR_MINUS)
	{
		if (isPrevOper && str[*i] == '-')
			*lex = LexemeCreateOper(UNAR_MINUS);
		else
			*lex = LexemeCreateOper(str[*i]);
		*i = *i + 1;
	}
	else if (isalpha((unsigned char)str[*i]))
	{
		char* endptr = NULL;
		char* word = strtow(&str[*i], &endptr);
		if (word == NULL)
		{
			return LEX_ERROR;
		}
		flag_t listflag = LexemeListGet(&lexemeList_g, word, lex);
		if (listflag != SUCCESS)
		{
			free(word);
			return LEX_ERROR;
		}
		*i += strlen(word);
		free(word);
	}
	else if (isspace((unsigned char)str[*i]))
	{
		*i = *i + 1;
	}
	else
		return LEX_ERROR;
	return SUCCESS;
}

static flag_t _lexemeSeqErrorCheck(const lexeme_t last, const lexeme_t now)
{
	if (LexemeIsOper(last) && LexemeIsOper(now))
	{
		if ((isBinaryOper(last.ess.oper) && isBinaryOper(now.ess.oper)) ||
			(isBinaryOper(last.ess.oper) && now.ess.oper == ')') ||
			(last.ess.oper == ')' && now.ess.oper == '(') ||
			(last.ess.oper == '(' && now.ess.oper == ')') ||
			(last.ess.oper == UNAR_MINUS && isBinaryOper(last.ess.oper)))
			return LEX_ERROR;
	}
	else if (LexemeIsVar(last) && LexemeIsVar(now))
		return LEX_ERROR;
	else if (LexemeIsVar(last) && LexemeIsOper(now))
	{
		if (now.ess.oper == '(')
			return LEX_ERROR;
	}
	else if (LexemeIsOper(last) && LexemeIsVar(now))
	{
		if (last.ess.oper == ')')
			return LEX_ERROR;
	}
	else if (LexemeIsFunc(last) && LexemeIsOper(now) && LexemeIsBinaryOper(now))
	{
		if (isBinaryOper(now.ess.oper) || now.ess.oper == ')')
			return LEX_ERROR;
	}
	else if (LexemeIsOper(last) && LexemeIsFunc(now) )
	{
		if (last.ess.oper == ')')
			return LEX_ERROR;
	}
	else if (LexemeIsVar(last) && LexemeIsFunc(now))
	{
		return LEX_ERROR;
	}
	return SUCCESS;
}

flag_t LexemeSeqParse(char *str, lexeme_t** lexseq, int* size)
{
	int i = 0;
	flag_t flag;
	LEXEME_T(lex);
	lexeme_stack_t stk;
	int isPrevOper = 1;
	flag = LexemeStackCreate(&stk);
	if (flag != SUCCESS)
		return flag;
	while (str[i] != 0)
	{
		if (isspace((unsigned char)str[i]))
		{
			i++; 
			continue;
		}
		flag = LexemeParse(str, &lex, &i, isPrevOper);
		if ((LexemeIsOper(lex) && lex.ess.oper!= ')') || LexemeIsFunc(lex))
			isPrevOper = 1;
		else
			isPrevOper = 0;
		if (stk.last != -1 && (_lexemeSeqErrorCheck(stk.arr[stk.last],lex) != SUCCESS))
		{
			free(stk.arr);
			return LEX_ERROR;
		}
		if (flag != LEX_ERROR)
		{
			flag = LexemeStackPush(&stk, &lex);
			if (flag == MEMORY_ERROR)
			{
				free(stk.arr);
				return MEMORY_ERROR;
			}
		}
		else
		{
			free(stk.arr);
			return LEX_ERROR;
		}
	}
	*lexseq = stk.arr;
	*size = stk.size;
	return SUCCESS;
}
