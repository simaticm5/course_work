
#include "temp_functions.h"
#define DateTouint16(year, month) (year << 8 | month)

//**************************************************************************************************************************
//                                        Поиск начало конца заданного периода
//**************************************************************************************************************************
_Bool start_end_list(struct data* info, size_t size, short year, short month, int* start_list, int* end_list)
{
	*start_list = *end_list = -1;
	int i = 0;
	int tmp, date = 0;
	if(year && month)
		date = DateTouint16(year, month);
	else 
		date = year;
	//здесь мы переключаем поиск статистики за месяц или год в зависимости от значений года и месяца
	while((tmp = (date > 256000) ? DateTouint16(info[i].year, info[i].month) : info[i].year) <= date && i < size)
	{
		if(*start_list == -1 && tmp == date)
			*start_list = i;
		i++;
	}
	*end_list = i;
	return (*start_list != -1);
}

//***********************************************************************************************************************************
//                                      Вычисление статистических значений за выбранный месяц или год
//***********************************************************************************************************************************
_Bool statistics(struct data* info, size_t size, short year, short month, float* average_temp, short* min_temp, short* max_temp)
{
	int start_list, end_list, sum = 0;
	if(start_end_list(info, size, year, month, &start_list, &end_list))
	{
		*max_temp = *min_temp = info[start_list].temp;
		for(size_t i = start_list; i < end_list; i++)
		{
			sum += info[i].temp;
			if(*min_temp > info[i].temp)
				*min_temp = info[i].temp;
			if(*max_temp < info[i].temp)
				*max_temp = info[i].temp;
		}
		*average_temp = (float)sum/(end_list - start_list);
	}
	else 
		return false;
	return true;
}

//****************************************************************************************************************************************
//                                Вывод ошибочных строк в три столбца и выравнивая их взависимости от длины
//****************************************************************************************************************************************
void print_err(int* err_list, string* array_err) 
{
	short max[3],len;
	size_t counter;
	for(counter = 0; err_list[counter]; counter++);
	for(short i = 0, dev = counter%3; i < 2; i++, dev--)
	{
		max[i] = 0;
		for(size_t j = 0, n = i; j < counter/3 + (dev > 0 ? 1:0); j++, n += 3)
			if(max[i] < (len = strlen(array_err[n].str)))
				max[i] = len;
	}
	for(short i = 0; i <  ((counter > 1) ? (max[0]+max[1]-2)/2 : (max[0]-10)/2); i++)
		printf(" ");
	printf("LIST ERROR ROWS:\n\n");

	for(size_t i = 0, n = 0 ; err_list[i]; i++, n++, n = (n == 3) ? 0:n )
	{
		printf("%06d:%s  ", err_list[i], array_err[i].str);
		if((i+1)%3 == 0)
		{
			printf("\n");
			continue;
		}
		for(short j = 0; j < max[n]-strlen(array_err[i].str); j++)
			printf(" ");

	}
	if(counter%3) printf("\n");
}

//**************************************************************************************************************************************************
//                                    Вывод статистики за выбранный год или месяца
//**************************************************************************************************************************************************
_Bool print_statistics(struct data* info, size_t accounted_list, short year, short* month)
{
	_Bool flag(short* month)
	{
		for(short i = 0; i < 12; i++)
			if (month[i]) return true;
		return false;
	}
	float average_temp;
	short min_temp, max_temp = 0;
	const char *month_name[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"}; 
	
	for(short i = 0; i < 11; i++)
	{
		for(short j = i+1; j < 12; j++)
		{
			if(month[i] == month[j]) month[j] = 0;
			if(month[i] > month[j]) {month[i] ^= month[j]; month[j] ^= month[i]; month[i] ^= month[j];}
		}
	}
	if(year)
	{
		if(!statistics(info, accounted_list, year, 0, &average_temp, &min_temp, &max_temp)) // условие что есть статистика за выбранный год
		{
			printf("\nNO STATISTICS FOR THE SELECTED YEAR\nEXITING...\n\n");
			return false;
		}
		if(flag(month)) // статистика за выбранные месяца данного года
		{
			printf("\n\t   STATISTICS FOR %hd YEAR FOR SELECTED MONTHS\n", year);
			printf("\n Month    Avr temperature    Min temperature    Max temperature\n");
			for(short i = 0; i < 12; i++)
			{
				if(!month[i]) continue;
				if(statistics(info, accounted_list, year, month[i], &average_temp, &min_temp, &max_temp)) // условие что есть статистика за выбранный мясяц
				{
					printf("   %s              %.2f                 %02hd                 %02hd\n", month_name[month[i] - 1], average_temp, min_temp, max_temp);
					month[i] = 0;// помечаем месяц со статистикой
				}
			}
			printf("\n");
			if(flag(month)) //из выбранных месяцов отсуствует статистика
			{
				printf("\n NO STATISTICS FOR: ");
				for(short i = 0; i < 12; i++)
				{
					if(!month[i]) continue;
					printf("%s, ", month_name[month[i] - 1]);
				}
				printf("\b\b \n\n");
			}
		}
		else //статистика за выбранный год
		{
			
			printf("\nSTATISTICS FORMED FOR: %hd YEAR\n\n", year);
			printf("Avr temperature: %5.2f\n", average_temp);
			printf("Min temperature: %02hd\n", min_temp);
			printf("Max temperature: %02hd\n", max_temp);
			printf("\n");
		}
			
		return true;
	}
	return false;
	
}	
//***************************************************************************************************************************************
void  printHelp()
{
	printf("\n -f <filename.csv> input .csv file to process.\n");
	printf(" -m <month name> if this key is given, then output only statistics for the specified month.\n");
	printf("    <month name> can be entered in the form of at least three letters or a number\n\n");
	printf(" You can generate statistics for selected months by entering several commands.\n");
	printf(" If the -m switch is set to the <all> argument, statistics for all 12 months will be displayed.\n\n");
	printf(" Additionally:\n");
	printf(" There is protection against incorrect input of command arguments,\n through a request for and their re-entry.\n");
	printf(" Checking for file extension, displaying months for which there were no statistics.\n");
	printf(" isplaying lines with data errors or incorrect format\n\n");
}








