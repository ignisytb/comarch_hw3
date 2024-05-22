CC = g++

all: Exe clean

Exe : main.o CPU.o Memory.o Register.o ALU.o
	$(CC) -o runfile CPU.o Memory.o Register.o ALU.o main.o

main.o : main.cpp CPU.h Memory.h Register.h ALU.h
	$(CC) -c main.cpp

CPU.o : CPU.cpp CPU.h Memory.h Register.h ALU.h
	$(CC) -c CPU.cpp

Memory.o : Memory.cpp Memory.h
	$(CC) -c Memory.cpp

Register.o : Register.cpp Register.h ALU.h
	$(CC) -c Register.cpp

ALU.o : ALU.cpp ALU.h
	$(CC) -c ALU.cpp

clean:
	rm -rf *.o