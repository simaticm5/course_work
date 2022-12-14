
#include "read_file.h"
#define UPPER_CASE(x)	   ( ( (x)>='A' ) && ( (x)<='Z' ) )
#define TO_LOWER_CASE(x)   (UPPER_CASE(x) ? ((x)-'A'+'a'):(x))
#define DateTouint64(y,m,d,h,minute)((uint64_t)y << 32 | m << 24 | d << 16 | h << 8 | minute) /* перевод даты и времени в одно число*/
enum{year, month, day, hour, minute, temp};

//***************************************************************************************************************************************
//                             Добавление данных в структуру из буфера строки, посимвольно ее разбирая
//***************************************************************************************************************************************
_Bool add_data(struct data* info, char* buff, _Bool* up) 
{
	short add[6], counter = 0, size = 0;
	static _Bool flag_data = false;
	char str[5]; //числовой буфер
	_Bool minus = false; // флаг отрицательного числа
	*up = true;          // флаг проверка на временную последовательность (значения времени и даты идут строго по возрастанию)
	while(*buff++)
	{
		if(!(is_simbol(*(buff-1)) || is_numeral(*(buff-1))))//проверка на допустимые символы строки файла сохраненные в символьном буфере
			return false;
		
		
		if(*(buff-1) == '-' || *(buff-1) == '+')// отрицательное число, положительное - если попадётся знак '+'
		{
			if(is_numeral(*buff))
				minus =	(*(buff-1) == '-') ? true:false;
			else 
				return false;
		}
		while(is_numeral(*(buff-1)))//пока идут символьные цифры загружаем их в символьный буфер 
		{	
			str[size++] = *(buff-1);            // буфер символьных цифр
			if(input_condition(*buff, counter)) // в функции создаётся условие для разделения данных
			{
				 
				add[counter] = str_is_number(str, size, minus);//конвертация и сохранение данных в числовом буфере для последующей загрузки в струкуру
				minus = false; 
				if(interval(add[counter], counter))// фильтрация на корректные значения даты, времени и температуры
					counter++;
				else 
					return false;
				size = 0;
			}
			if(*buff == '\0')
				break;
			buff++;
		 }
		 if(size)
			return false;
	}
	if(counter != 6) // несоответствие полей данных для загрузки значений в структуру
		return false;
	

	if(flag_data) // проверка на временную последовательность(значения времени и даты идут строго по возрастанию)
	{
		info--;
		if( DateTouint64(info->year, info->month, info->day, info->hour, info->minute) >= 
		    DateTouint64(add[year], add[month], add[day], add[hour], add[minute]) )// проверяем текущие времменые данные с предыдущими
		{
			*up = false;// флаг учёта последовательности(нужен для основной функции передается значение по адресу)
			return false;
		}
		info++;
	}
	*info = (struct data){add[year], add[month], add[day], //загрузка подготовленных данных в структуру
		                  add[hour], add[minute], add[temp]};
	flag_data = true;// флаг указывает на то что у нас произошла первая загрузка данных в структуру, чтобы можно было сравнивать текущие данные структуры с предыдущими
	return true;
}

//***************************************************************************************************************************
//***************************************************************************************************************************
 _Bool is_letter(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

//***************************************************************************************************************************
//                                  Проверка на раширения файла
//***************************************************************************************************************************
_Bool file_extension(char* name)
{
	char *pstr;
	if(strlen(name) > 4)
	{
		while((pstr = strstr(name, ".csv")) != NULL)
		{
			if(*(pstr+4) == '\0')
				return true;
			name =(pstr+4);
		}
	}
	return false;
}

//********************************************************************************************************************************
//        Распознования ввода месяца в произвольном виде ( можно числом или буквавми не менее 3-х для распознавания)
//********************************************************************************************************************************
_Bool month_name(char* month, short* number) 
{
	_Bool flag;
	char const *str_month[] = {"january","february","march","april","may","june","july",
	                              "august","september","october","november","december"};
								  
	char const str_letter[] = "jfmasond";
	short sum = 0;
	if(is_letter(*month))
		flag = true;
	else if(is_numeral(*month))
		flag = false;
	else
		return false;
	for(short i = 0; month[i]; i++)
	{
		if(flag)
		{
			if(is_letter(month[i]))
			{
				sum++;
				month[i] = TO_LOWER_CASE(month[i]);
			}
			else
				return false;
			
		}
		else
		{
			if(!is_numeral(month[i]))
				return false;
		}
	}
	char tmp[2]; tmp[0] = *month; tmp[1] = '\0';
	if(flag && sum >2 && strstr(str_letter, tmp) != NULL)
	{
		for(short i = 0; i < 12; i++)
		{
			if(strstr(str_month[i], month) != NULL)
			{
				*number = i+1;
				return true;
			}
		}
	}
	else if(!flag)
	{
		*number = (short)strtol(month, NULL, 10);
		if(*number < 13 && *number > 0)
			return true;
	}
	return false;	
}

//**************************************************************************************************************************
//                                   Перевод строковых символов в число 
//************************************************************************************************************************** 
 short str_is_number(char* buff, short size, _Bool minus) 
{		
	short multip = 1, number = 0;
	for(short i = size-1; i >= 0; i-- )
	{
		number += multip*(buff[i] - '0');  
		multip *= 10;
	}
	return number *= minus ? -1:1;
}
	
//***************************************************************************************************************************
//                                              Это символьные цифры
//***************************************************************************************************************************
_Bool is_numeral(char c) 
{
	return (c >= '0' && c <= '9');
}	

//***************************************************************************************************************************
//        Допустимые символы которые присутствуют в файле (; или , будут учитыватся как разделители данных другой функцией)
//***************************************************************************************************************************
_Bool is_simbol(char c) 
{
	return (c == ' ' || c == '\t' || c == '-' || c == '+');
}

//********************************************************************************************************************************
//Условие для разделение данных, где n - указывает на считанное поле строки (0 -> 1-е значение строки ... 5 -> последнее значение)
//*********************************************************************************************************************************
_Bool input_condition(char c, int n) 
{
	return (((c == ';'  || c == ',') && n != 5) || (n == 5 &&(c == '\0' || c == ' ' || c == '\t')));
}

//**************************************************************************************************************************
//                        Проверка на допустимые значения даты, времени и температуры
//**************************************************************************************************************************
_Bool interval(short num, short counter) 
{
	short arr[6][2] = {{1000, 9999}, {1, 12}, {1, 31},
		                    {0, 23}, {0, 59}, {-99, 99}};
							
	return (num >= arr[counter][0] && num <= arr[counter][1]);
}

//*****************************************************************************************************************************
         /******************************************************************************************************
		  *                                           Основная функция.                                        *
		  *           Чтение и запись данных из фаила в структуру и сохранение в масив ошибочных строк         *
          ******************************************************************************************************/
 //*****************************************************************************************************************************
 
int parseFile(struct data* info, char* filename, int* err_list, string* array_err)
{
	size_t accounted_list = 0, all_list = 0;
	string simbol_buff; // переменная структрного типа для построчного считывания из файла
	char c;
	_Bool overflow_buf = false, up;// флаг set когда строка длинее допустимого, up - отслеж-е времменой послед.
	FILE *f_read = NULL;
	if((f_read = fopen(filename, "r")) != NULL)
	{
		while(fscanf(f_read, "%30[^\n]%c", simbol_buff.str, &c) != EOF) // считываем строку
		{
			while(c != '\n')          // строка полностью не поместилась в заданный интервал
			{
				c = fgetc(f_read);
				overflow_buf = true; // флаг ошибки - слишком длинная строка для её учёта
			}
			all_list++; 
			
			if(!*(simbol_buff.str)) //условие пропуска серии символ новой строки если они попадутся в файле будем дальше считываем данные
			{
				c=fgetc(f_read);
				continue;
			}
			
			if (add_data(info + accounted_list, simbol_buff.str, &up) && !overflow_buf) //добовляем  данные в структуру из считанной строки
				accounted_list++; //количество учтенных строк
			else
			{
				if(overflow_buf) // если строка больше 30 символов
				{
					strcpy(simbol_buff.str, "--->long string"); //это выводим на экран 
					overflow_buf = false;
				}                                                  
				else if(!up)// флаг указывает когда данные находятся не в порядке возрастания даты и времени(его состояние генерирует функция add_data)
					strcpy(simbol_buff.str, "incorrect time sequence");// это  выводим на экран
				*err_list++ = all_list;     //сохраненяем номера строк, где выявлены ошибки
				*array_err++ = simbol_buff;  //сохраняем сами строки, где выявлены ошибки (array_err и simbol_buff имеют тип структуры string)
			}
			*(simbol_buff.str) = '\0';
		}
	}
	else 
		return -1;//возвращаем, если файл не открылся
	fclose(f_read);// закрываим файл
	*err_list = '\0'; //помечаем конец массива ошибочных номеров
	return accounted_list; // возвращаем количество учтенных строк
}















