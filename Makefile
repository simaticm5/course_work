all: main

main: main.o read_file.o temp_functions.o
	gcc -O1 -g3 -Wall main.o read_file.o temp_functions.o -o main

main.o: main.c temp_functions.h
	gcc -O1 -g3 -Wall -c main.c

read_file.o:	read_file.c read_file.h
	gcc -O1 -g3 -Wall -c read_file.c

temp_functions.o: temp_functions.c temp_functions.h
	gcc -O1 -g3 -Wall -c temp_functions.c
	
clean:
	del rm -rf *.o *.exe