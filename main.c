#pragma warning(disable:4996)
//#define LEAK_TEST
#ifdef LEAK_TEST
#define _CRTDBG_MAP_ALLOC 
#endif // LEAK_TEST

#include <locale.h>
#include <stdlib.h>

#ifdef LEAK_TEST
	#include <crtdbg.h> 
#endif // LEAK_TEST
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "calctypes.h"
#include "calcmacros.h"
#include "calc.h"
#define BUFFER_SIZE 1
#define INCASESAY(file, error) case error: \
fprintf(file,#error); \
break

int isEnd( char chr)
{
	return chr == '\n' || chr == '\r' || chr == '\0';
}

void __printerr__(FILE* file, flag_t flag)
{
	fprintf(file,"ERROR: x%i | ", ((int)flag));
	switch (flag)
	{
		INCASESAY(file, SUCCESS);
		INCASESAY(file, FILE_OPPENING_ERROR);
		INCASESAY(file, NUMBER_OF_ARGUMENTS_ERROR);
		INCASESAY(file, MEMORY_ERROR);
		INCASESAY(file, READING_ERROR);
		INCASESAY(file, WRITING_ERROR);
		INCASESAY(file, COMMENT_STR);
		INCASESAY(file, EMPTY_STR);
		INCASESAY(file, EXPRESSION_STR);
		INCASESAY(file, END_STR);
		INCASESAY(file, EMPTY_STACK);
		INCASESAY(file, LEX_ERROR);
		INCASESAY(file, BRACKETS_ERROR);
		INCASESAY(file, PARSING_ERROR);
		INCASESAY(file, CALCULATING_ERROR);
	}
	fprintf(file, " |");
}

static int endstream = 0;

char skipLine(FILE* file)
{
	char chr = (char)getc(file);
	while (!isEnd(chr) && chr != (char)EOF)
		chr = (char)getc(file);

	return chr;
}

flag_t ReadLine(FILE* file, char* arr[], int* slen)
{
	char chr;
	char* str;
	char* _str;
	int iW = 0;
	int size = BUFFER_SIZE + 1;
	chr = (char)getc(file);

	if (isEnd(chr) || chr == (char)EOF)
	{
		*slen = 0;
		*arr = NULL;
		if (chr == (char)EOF)
			endstream = 1;
		return EMPTY_STR;
	}
	str = Malloc(size);
	if (str == NULL)
	{
		*arr = NULL;
		chr = skipLine(file);
		if (chr == (char)EOF)
		{
			endstream = 1;
		}
		return MEMORY_ERROR;
	}
	
	while (!isEnd(chr) && chr!= (char)EOF)
	{
		if (iW == size - 2)
		{
			size += BUFFER_SIZE;
			_str = Realloc(str, size);
			if (_str == NULL)
			{
				*arr = NULL;
				free(str);
				*slen = 0;
				chr = skipLine(file);
				if (chr == (char)EOF)	
				{
					endstream = 1;
				}
				return MEMORY_ERROR;
			}
			else str = _str;
		}
		str[iW++] = chr;
		chr = (char)getc(file);
	}
	str[iW] = 0;
	*arr = str;
	*slen = iW;
	if (chr == (char)EOF)
		endstream = 1;
	return SUCCESS;
}

flag_t Process( char* str, FILE* output)
{
	int i;
	flag_t type = EMPTY_STR;
	for (i = 0;	str[i] !=0 ; i++)
	{
		if (type == EMPTY_STR)
		{
			if (str[i] == '/' && str[i + 1] == '/')
				type = COMMENT_STR;
			else if (!isspace((unsigned char)str[i]))
				type = EXPRESSION_STR;
		}
		fprintf(output,"%c", str[i]);
	}
	if (type == EXPRESSION_STR)
	{
		double res = 0;
		flag_t resflag = calc(str, &res);
		if (resflag != SUCCESS){
			fprintf(output, " == ");
			PRINTERROR(output, resflag);
		}
		else
		{
			fprintf(output, " == %lg", res);
		}
		//fprintf(output, "%s\n", str);
	}
	return type;
}

flag_t start(FILE* input, FILE* output)
{
	char* str;
	str = NULL;
	int slen = 0;
	flag_t flag;
	flag = calcInit();
	if (flag != SUCCESS)
		return flag;
	flag = ReadLine(input, &str, &slen);
	while (!endstream)
	{
		if (flag != MEMORY_ERROR)
		{
			if (flag != EMPTY_STR)
			{
				Process(str, output);
				free(str);
			}
			str = NULL;
			fprintf(output, "\n");
		}
		else
		{
			PRINTERROR(output, MEMORY_ERROR);
			fprintf(output, "\n");
		}
		flag = ReadLine(input, &str, &slen);
	}

	if (flag != MEMORY_ERROR)
	{
		if (flag != EMPTY_STR)
		{
			Process(str, output);
			printf("\n");
			free(str);
		}
	}
	else
	{
		PRINTERROR(output, MEMORY_ERROR);
		fprintf(output, "\n");
	}
	//calcDestroy();
	return SUCCESS;
}

int main(int argc, char *args[])
{
	int error;
	FILE* input;
	srand(0);
#ifdef LEAK_TEST
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // LEAK_TEST
	//setlocale(LC_CTYPE, "rus");
	if (argc == 1)
	{
		input = stdin;
	}
	else if (argc == 2)
	{
		input = fopen(args[1], "r");
		if (input == NULL)
			EXITWITH(FILE_OPPENING_ERROR);
	}
	else
		EXITWITH(NUMBER_OF_ARGUMENTS_ERROR);
	error = start(input,stdout);
	fclose(input);

#ifdef LEAK_TEST
	  _CrtDumpMemoryLeaks();
#endif // LEAK_TEST
	return error;
}