/*
                     Функции для формирования и вывода статистических значений 
*/
#ifndef TEMP_FUNCTIONS_H
#define TEMP_FUNCTIONS_H
#include "read_file.h"


_Bool start_end_list(struct data*, size_t, short, short ,int*, int*);//ищет начало и конец строки заданного года или месяца.
_Bool statistics(struct data*, size_t, short, short, float*, short*, short*); // вычисляет статистику за месяц или год
/*********************************************************************************************************/
/*********************************************************************************************************/
void print_err(int*, string*);// функция выводит ошибочные строки в три столбца автоматически выравнивая их
_Bool print_statistics(struct data*, size_t, short, short*); // выводим статистику за месяц или год
void  printHelp();
#endif