/*
Функции для чтения и записи данных из файла в структуру. Данные разделяются между собой  ';' или ',' можно смешано использовать разделители.
После числа идет строго разделитель иначе ошибка формата, если это не конец строки. В конце данных строки могут быть "белые символы" они не учитываются
 
*/
#ifndef READ_FILE_H
#define READ_FILE_H
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <stdbool.h>
#include <string.h>

struct data
{
	uint16_t year;
	uint8_t  month;
	uint8_t  day;
	uint8_t  hour;
	uint8_t  minute;
	int8_t   temp;
};

typedef struct
{
	char str[50];
} string;

//     Первые 5 функций вспомогательные для _Bool add_data(struct data*, char*, _Bool*) 
//     Для основной функции  int parseFile(struct data*, char*, int*, string*) - _Bool add_data(struct data*, char*, _Bool*) идет как вспомогательная

_Bool is_numeral(char);                                     // это цифра
_Bool is_simbol(char);                                      // допустимые символы в файле
_Bool interval(short, short);                               // проверка на корректность вводимых значений
_Bool input_condition(char, int);                           // создаёт условие для разделения данных
short str_is_number(char*, short, _Bool);                   // строка цифр в число
_Bool add_data(struct data*, char*, _Bool*); // добавление данных в структуру из буфера строки и проверка на ошибки формата и корректные значения
int parseFile(struct data*, char*, int*, string*);          // чтение файла, заполнение структуры и фиксация ошибок

/*******************************************************************************************************************/
/*                                  Функции для работы с аргументами командной строки                              */   
/*******************************************************************************************************************/
_Bool file_extension(char*);// проверка на расширение файла
_Bool month_name(char*, short*);// функция распознования: вводим наименование месяца  литералами или числом (если литералами то не меньше 3-х букв)
_Bool is_letter(char);          // это буквы

#endif

