CC = gcc
CFLAGS = -Wall -std=c11 -g 

BIN = ./bin/
SRC = ./src/
INC = ./include/

all: clean list parser main

run: $(BIN)program
	$(BIN)program
		
parser: $(BIN)GEDCOMparser.o $(BIN)GEDCOMutilities.o
	ar rcs $(BIN)libparser.a $(BIN)GEDCOMparser.o $(BIN)GEDCOMutilities.o
		
list: $(BIN)LinkedListAPI.o
	ar rcs $(BIN)liblist.a $(BIN)LinkedListAPI.o
	
clean:
	rm -f $(BIN)*.o
	rm -f $(BIN)*.a
	
test: 
	$(CC) $(CFLAGS)  -Iinclude $(SRC)main.c $(BIN)libparser.a $(BIN)liblist.a
	
	
$(BIN)LinkedListAPI.o: $(SRC)LinkedListAPI.c
	$(CC) $(CLFAGS) -c $(SRC)LinkedListAPI.c -Iinclude -o $(BIN)LinkedListAPI.o
	
$(BIN)GEDCOMparser.o: $(SRC)GEDCOMparser.c
	$(CC) $(CLFAGS) -c $(SRC)GEDCOMparser.c -Iinclude -o $(BIN)GEDCOMparser.o
	
$(BIN)GEDCOMutilities.o: $(SRC)GEDCOMutilities.c
	$(CC) $(CFLAGS) -c $(SRC)GEDCOMutilities.c -Iinclude -o $(BIN)GEDCOMutilities.o
	
main: $(BIN)libparser.a $(BIN)liblist.a
	$(CC) $(CFLAGS) -Iinclude $(SRC)main.c $(BIN)libparser.a $(BIN)liblist.a -o $(BIN)program


