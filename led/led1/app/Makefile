###########################################
#Makefile for simple programs
###########################################

CC=arm-linux-gnueabihf-gcc
CC_FLAG=-Wall 

PRG=ledApp
OBJ=ledApp.o

$(PRG):$(OBJ)
	$(CC)  -o $@ $(OBJ) 
	
.SUFFIXES: .c .o .cpp
.cpp.o:
	$(CC)  -c $*.cpp -o $*.o

.PRONY:clean
clean:
	@echo "Removing linked and compiled files......"
	rm -f $(OBJ) $(PRG)
