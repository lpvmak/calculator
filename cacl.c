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
#define _USE_MATH_DEFINES
#include <math.h>
#define PI_CONST M_PI 
#define E_CONST M_E
lexeme_list_t lexemeList_g;

static lexeme_t _lexeme_arr[100];
static char _nameSin[] = "sin";
static char _nameCos[] = "cos";
static char _nameArcCos[] = "arccos";
static char _nameArcSin[] = "arcsin";
static char _nameCtg[] = "ctg";
static char _nameTg[] = "tg";
static char _nameLn[] = "ln";
static char _nameArcTg[] = "arctg";
static char _nameFloor[] = "floor"; 
static char _nameCeil[] = "ceil";
static char _nameSqrt[] = "sqrt";
static char _namePi[] = "pi";
static char _nameE[] = "e";
#ifdef MEMORY_TEST
int num = 0;
#endif // !1
void* Malloc(size_t _Size)
{
#ifdef MEMORY_TEST


	num++;
	if ((rand() % 40) != 1)
	{
#endif // !1
			return malloc(_Size);
#ifdef MEMORY_TEST
	}
	if (num == 184)
		printf("HUI!!!");
	printf("[ERROR %d]", num);
	return NULL;
#endif // !1
}
void* Realloc(void* _Block, size_t _Size) 
{
#ifdef MEMORY_TEST


	num++;
	if ((rand() % 40) != 1)
	{
#endif // !1
		return realloc(_Block, _Size);
#ifdef MEMORY_TEST
	}
	if (num == 184)
		printf("HUI!!!");
	printf("[ERROR %d]", num);
	return NULL;
#endif // !1
}

static double ctg(double phi)
{
	if (tan(phi)*tan(phi) < fabs(tan(PI_CONST)))
		return NAN;
	return 1/tan(phi);
}

flag_t calcInit(void)
{
	LEXEME_T(lex);
	lexemeList_g.size = 0;
	lexemeList_g.arr = _lexeme_arr;
	
	lex.type = FUNC_1;
	lex.name = _nameSin;
	lex.ess.func = sin;
	lexemeList_g.arr[lexemeList_g.size++] = lex;
	
	lex.type = FUNC_1;
	lex.name = _nameCos;
	lex.ess.func = cos;
	lexemeList_g.arr[lexemeList_g.size++] = lex;

	lex.type = FUNC_1;
	lex.name = _nameArcCos;
	lex.ess.func = acos;
	lexemeList_g.arr[lexemeList_g.size++] = lex;

	lex.type = FUNC_1;
	lex.name = _nameArcSin;
	lex.ess.func = asin;
	lexemeList_g.arr[lexemeList_g.size++] = lex;

	lex.type = FUNC_1;
	lex.name = _nameLn;
	lex.ess.func = log;
	lexemeList_g.arr[lexemeList_g.size++] = lex;

	lex.type = FUNC_1;
	lex.name = _nameTg;
	lex.ess.func = tan;
	lexemeList_g.arr[lexemeList_g.size++] = lex;

	lex.type = FUNC_1;
	lex.name = _nameCtg;
	lex.ess.func = ctg;
	lexemeList_g.arr[lexemeList_g.size++] = lex;

	lex.type = FUNC_1;
	lex.name = _nameArcTg;
	lex.ess.func = atan;
	lexemeList_g.arr[lexemeList_g.size++] = lex;

	lex.type = FUNC_1;
	lex.name = _nameFloor;
	lex.ess.func = floor;
	lexemeList_g.arr[lexemeList_g.size++] = lex;

	lex.type = FUNC_1;
	lex.name = _nameCeil;
	lex.ess.func = ceil;
	lexemeList_g.arr[lexemeList_g.size++] = lex;

	lex.type = FUNC_1;
	lex.name = _nameSqrt;
	lex.ess.func = sqrt;
	lexemeList_g.arr[lexemeList_g.size++] = lex;

	lex.type = VAR;
	lex.name = _namePi;
	lex.ess.var = PI_CONST;
	lexemeList_g.arr[lexemeList_g.size++] = lex;

	lex.type = VAR;
	lex.name = _nameE;
	lex.ess.var = E_CONST;
	lexemeList_g.arr[lexemeList_g.size++] = lex;

	return SUCCESS;
}

flag_t calc(char* str, double *res)
{
	lexeme_t *seq = NULL;
	flag_t flag = SUCCESS;
//	int i;
	int size = 0;
	errno = 0;
	flag = LexemeSeqParse(str, &seq, &size);
	if (flag == SUCCESS)
	{
		flag = RPN(seq, size, res);
		free(seq);
		return flag;
	}
	else
		return flag;
}

static void _printLexemeArr(lexeme_t* arr, int size)
{
	int i;
	printf("\n");
	for (i = 0; i < size; i++)
	{
		LexemeWrite(stdout, arr[i]);
		printf(" ");
	}
	printf("\n");
}

flag_t RPN_PARSE(lexeme_t *exp, int size, lexeme_stack_t *stk, lexeme_stack_t *outstring)
{
	flag_t flag = 0; 
	flag_t flag1 = 0;
	flag_t flag2 = 0;
	LEXEME_T(lex);
	int i;
	for (i = 0; flag == SUCCESS && i < size; i++)
	{
		if (LexemeIsVar(exp[i]))
		{
			flag = LexemeStackPush(outstring, &exp[i]);
		}
		else if (LexemeIsFunc(exp[i]))
		{
			flag = LexemeStackPush(stk, &exp[i]);
		}
		else if (LexemeIsOper(exp[i]))
		{
			if (exp[i].ess.oper == '(')
			{
				flag = LexemeStackPush(stk, &exp[i]);
			}
			else if (exp[i].ess.oper == ')')
			{
				while (flag2 == SUCCESS && stk->last != -1 && stk->arr[stk->last].ess.oper != '(')
				{
					flag1 = LexemeStackPop(stk, &lex);
					flag2 = LexemeStackPush(outstring, &lex);
					if (flag1 != SUCCESS || flag2 != SUCCESS)
					{
						return PARSING_ERROR;
					}				
				}
				if (flag2 != SUCCESS)
					return flag2;
				if (stk->arr[stk->last].ess.oper != '(')
					return PARSING_ERROR;
				flag1 = LexemeStackPop(stk, &lex);
			}
			else if (isRightOper(exp[i].ess.oper))
			{
				int lexpr = LexemeOperPriority(exp[i].ess.oper);
				while (flag2 == SUCCESS && stk->last != -1 && ((LexemeIsFunc(stk->arr[stk->last]) && exp[i].ess.oper != UNAR_MINUS) || (LexemeOperPriority(stk->arr[stk->last].ess.oper) > lexpr)))
				{
					flag1 = LexemeStackPop(stk, &lex);
					flag2 = LexemeStackPush(outstring, &lex);
					if (flag1 != SUCCESS || flag2 != SUCCESS)
					{
						return PARSING_ERROR;
					}
				}
				if (flag2 != SUCCESS)
					return flag2;
				flag = LexemeStackPush(stk, &exp[i]);
				if (flag != SUCCESS)
					return flag;
			}
			else
			{
				int lexpr = LexemeOperPriority(exp[i].ess.oper);
				while (flag2 == SUCCESS && stk->last != -1 && ((LexemeIsFunc(stk->arr[stk->last]) && exp[i].ess.oper!=UNAR_MINUS) || (LexemeOperPriority(stk->arr[stk->last].ess.oper) >= lexpr)))
				{
					flag1 = LexemeStackPop(stk, &lex);
					flag2 = LexemeStackPush(outstring, &lex);
					if (flag1 != SUCCESS || flag2 != SUCCESS)
					{
						return PARSING_ERROR;
					}
				}
				if (flag2 != SUCCESS)
					return flag2;
				flag = LexemeStackPush(stk, &exp[i]);
			}
		}
	}
	while (stk->last != -1 && stk->arr[stk->last].ess.oper != '(' && stk->arr[stk->last].ess.oper != ')')
	{
		flag1 = LexemeStackPop(stk, &lex);
		flag2 = LexemeStackPush(outstring, &lex);
		if (flag1 != SUCCESS || flag2 != SUCCESS)
		{
			return PARSING_ERROR;
		}
	}
	if (stk->last!=-1 && (stk->arr[stk->last].ess.oper == '(' || stk->arr[stk->last].ess.oper == ')'))
	{
		return BRACKETS_ERROR;
	}
	return SUCCESS;
}

static int _checkOutOfAcceptability(lexeme_t func, double var)
{
	return isnan(var)|| isinf(var) ||
		(func.ess.func == sqrt && var < 0) ||
		(func.ess.func == log && var < 0) ||
		(func.ess.func == acos && (var < -1 || var > 1)) ||
		(func.ess.func == asin && (var < -1 || var > 1)) ||
		(func.ess.func == tan && cos(var) == 0) ||
		(func.ess.func == ctg && sin(var) == 0);
		
}

flag_t RPN(lexeme_t *exp, int size, double *res)
{
	flag_t flag;
	LEXEME_T(lex1);
	LEXEME_T(lex2);
	flag_t flag1 = 0, flag2 = 0;
	lexeme_stack_t stk;
	lexeme_stack_t outstring;
	int i;
	flag = LexemeStackCreate(&stk);
	if (flag != SUCCESS)
		return flag;
	flag = LexemeStackCreate(&outstring);
	if (flag != SUCCESS)
	{
		free(stk.arr);
		return flag;
	}
	flag = RPN_PARSE(exp, size, &stk, &outstring);
	
	if (flag != SUCCESS)
	{
		free(stk.arr);
		free(outstring.arr);
		return flag;
	}
	//_printLexemeArr(outstring.arr, outstring.size);
	for (i = 0; i <= outstring.last; i++)
	{
		if (outstring.arr[i].type == VAR)
		{
			flag = LexemeStackPush(&stk, &(outstring.arr[i]));
		}
		else if (outstring.arr[i].type == FUNC_1)
		{
			int lasterr = errno;
			lexeme_t lex;
			lex.type = VAR;
			flag1 = LexemeStackPop(&stk, &lex1);
			if (flag1!=SUCCESS || lex1.type != VAR)
			{
				free(stk.arr);
				free(outstring.arr);
				return PARSING_ERROR;
			}
			if (!_checkOutOfAcceptability(outstring.arr[i], lex1.ess.var))
			{
				lex.ess.var = (*outstring.arr[i].ess.func)(lex1.ess.var);
			}
			else
			{
				errno = lasterr;
				free(stk.arr);
				free(outstring.arr);
				return CALCULATING_ERROR;
			}
			if(errno || isnan(lex.ess.var) || isinf(lex.ess.var))
			{
				errno = lasterr;
				free(stk.arr);
				free(outstring.arr);
				return CALCULATING_ERROR;
			}
			flag = LexemeStackPush(&stk, &lex);
			if (flag == MEMORY_ERROR)
			{
				free(stk.arr);
				free(outstring.arr);
				return MEMORY_ERROR;
			}
		}
		else if (outstring.arr[i].type == OPER)
		{
			lexeme_t lex3;
			lex3.type = VAR;
			if (isBinaryOper(outstring.arr[i].ess.oper))
			{
				flag1 = LexemeStackPop(&stk, &lex1);
				flag2 = LexemeStackPop(&stk, &lex2);

				if (flag1 != SUCCESS || flag2 != SUCCESS || lex1.type != VAR || lex2.type != VAR)
				{
					free(stk.arr);
					free(outstring.arr);
					return PARSING_ERROR;
				}
				switch (outstring.arr[i].ess.oper)
				{
				case '+':
					lex3.ess.var = lex1.ess.var + lex2.ess.var;
					break;
				case '-':
					lex3.ess.var = lex2.ess.var - lex1.ess.var;
					break;
				case '*':
					lex3.ess.var = lex2.ess.var * lex1.ess.var;
					break;
				case '/':
					if (lex1.ess.var == 0)
					{
						free(stk.arr);
						free(outstring.arr);
						return CALCULATING_ERROR;
					}
					lex3.ess.var = lex2.ess.var / lex1.ess.var;
					break;
				case '^':
					if (lex2.ess.var < 0 && floor(lex1.ess.var) != lex1.ess.var)
					{
						free(stk.arr);
						free(outstring.arr);
						return CALCULATING_ERROR;
					}
					lex3.ess.var = pow(lex2.ess.var, lex1.ess.var);
					break;
				default:
					free(stk.arr);
					free(outstring.arr);
					return CALCULATING_ERROR;
				}
			}
			else if(outstring.arr[i].ess.oper == UNAR_MINUS)
			{
				flag1 = LexemeStackPop(&stk, &lex1);

				if (flag1 != SUCCESS || lex1.type != VAR)
				{
					free(stk.arr);
					free(outstring.arr);
					return PARSING_ERROR;
				}
				lex3.ess.var = -lex1.ess.var;
			}
			else
			{
				free(stk.arr);
				free(outstring.arr);
				return CALCULATING_ERROR;
			}
			flag = LexemeStackPush(&stk, &lex3);
			if (flag == MEMORY_ERROR)
			{
				free(stk.arr);
				free(outstring.arr);
				return MEMORY_ERROR;
			}
		}
	}			
	flag = LexemeStackPop(&stk, &lex1);
	if (flag != SUCCESS || stk.last!= -1 || lex1.type != VAR)
	{
		free(stk.arr);
		free(outstring.arr);
		return PARSING_ERROR;
	}
	free(stk.arr);
	free(outstring.arr);
	*res = lex1.ess.var;
	return SUCCESS;
}