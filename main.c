

#include "temp_functions.h"
#include <unistd.h>

int err_list[10000];      // массив для сохранения чисел ошибочных строк 
struct data info[1000000];// массив структурного типа для заполнения данными из файла
string array_err[10000];  // массив структурного типа для сохранения ошибочных строк

int main(int argc, char *argv[])
{
	short year = 0, counter = 0;
	static short month[12] = {0};
	static char fileName[50], buff_month[20];
	char c;
	size_t list;
	int rez=0;
    opterr=0;
	while ( (rez = getopt(argc,argv,"hf:m:")) != -1)
	{
		switch (rez)
		{
			static _Bool f = false, f_print = true,  m = true; //флаги управления
			case 'h': printHelp();
			return 0;
			break;
			case 'f':
				if(f){
					if(f_print) printf("YOU HAVE ENTERED SEVERAL KEYS -f. ONLY THE FIRST COMMAND WILL BE PROCESSED.\n"); 
					f_print = false; break;} // условие обработки команд если их несколько
				f = true;
				if(strlen(optarg) > 25)
				{
					printf("FILENAME TOO LONG! 25 CHARACTERS MAX.\n");
            WRONG_EXTENSION:
					while(true)// проверка на длину строки в названии файла, 
							   // если оно больше 25 символов то нам заного предоставляется ее ввести
					{
						printf("ENTER FILE NAME: ");
						for(short i = 0; i < 26; i++)
						{
							if((fileName[i] = getchar()) == '\n' && i >0)
							{
								fileName[i] = '\0';
								goto OK_LONG;
							}
							else if(fileName[0] == '\n')
							{
								printf("YOU DID NOT ENTER ANYTHING\n"); // если ничего не ввели, выводит сообщение
								goto WRONG_EXTENSION;
							}
						}
						while((c=getchar()) != '\n');
						printf("FILENAME TOO LONG! 25 CHARACTERS MAX.\n");
				     }
				}
				else 
					strcpy(fileName, optarg);
            OK_LONG:
				if(!file_extension(fileName))// проверяем на расширение файла
				{
					printf("INCORRECT FILE EXTENSION OR NAME.\n");
					goto WRONG_EXTENSION;                         // если в имени файла неверное расширение, то заного вводим имя файла
				}                                             
				do
				{
					printf("\nEnter the year value in four formats \nto generate statistics for this time period: ");
					scanf("%hd%c", &year, &c);  // после ввода имени файла, вводим год за который будет  сформирована статистика
					if(year < 2000 || year > 3000)
						printf("\nWRONG YEAR VALUE.\n");
				}
				while(year < 2000 || year > 3000);
			break;
			case 'm': //в аргументе команды мы проверям название месяца (число или название не мене 3-х символов для определения месяца)
				if(strstr(optarg, "all") != NULL || !m) // обработка аргумента  <all> при совпадении (выводит статистику за все месяца выбранного года)
				{
					for(short i = 0; i < 12; i++)
						month[i] = i+1;
					m = false;
					break;
				}
				if (counter >11){
					if(m) printf("YOU ENTERED MORE THAN 12 -m KEYS\nONLY THE FIRST 12 KEYS WILL BE PROCESSED\n"); 
					m = false; break;} //условие обработки команд если их несколько (не больше 12, исключение для <all> она обработается как - 13, если были предыдущие 12 )
				if(strlen(optarg) > 9) // <all> отменяет все команда данного ключа и выводит статистику за все месяца выбранного года
				{
					printf("YOU ENTERED AN INCORRECT MONTH NAME.\n");
			WRONG_MONTH:
					while(true) // здесь то же самое организован повторный ввод ошибочных значений
					{
						printf("ENTER MONTH NAME: ");
						for(short i = 0; i < 11; i++)
						{
							if((buff_month[i] = getchar()) == '\n' && i >0)
							{
								buff_month[i] = '\0';
								if(month_name(buff_month, &month[counter]))
								   goto OK_MONTH;
								else{
									printf("YOU ENTERED AN INCORRECT MONTH NAME\n");
								   goto WRONG_MONTH;
								}
							}
							else if(buff_month[0] == '\n')
							{
								printf("YOU DID NOT ENTER ANYTHING\n");
									goto WRONG_MONTH;
							}
						}
						while((c=getchar()) != '\n');
					}
					
				}
				else if(!month_name(optarg, &month[counter])) // лог. функция определения месяца, в массив month заносим число месяца от команд-ключей
					{
						printf("YOU ENTERED AN INCORRECT MONTH NAME %hd KEYS -m\n", counter+1); // выводит сообщение о некорректном аргументе команды, 
						                                                                         //указывая в каком месте данного  ключа была ошибка и заного вводим значение
						goto WRONG_MONTH;
					}
		OK_MONTH:
			counter++;
			break;
			case '?': printf("\nINCORRECT KEY OR KEY WITHOUT ARGUMENT. USE -h TO SEE A LIST OF AVAILABLE KEYS.\n"); 
			return 0;
			break;
			default: return 0;
		}
	}

	
	if (!year) 
		printf("\nYOU DID NOT SPECIFY THE SOURCE FILE FOR PROCESSING\nPLEASER USE -h KEY FOR HELP\nEXITING..\n");
	else if((list = parseFile(info, fileName, err_list, array_err)) == -1)//Чтение и запись данных из фаила в структуру и сохранение в масив ошибочных строк
		printf("\nFILE OPEN ERROR\n");                                    // функция возвращает количество учтенных строк или -1 если ошибка открытия фаила
	else if(print_statistics(info, list, year, month)) //выводим годовую, если она есть, или по выбранным месяцам статистику, а так же месяца где не была сформирована статистика
		print_err(err_list, array_err);//выводим номер и строку с ошибками
	
	
	return 0;

}


// mingw32-make clean
// temperature_small.csv
// temperature_big.csv    temperature_big1.csv  temperature_example.csv   