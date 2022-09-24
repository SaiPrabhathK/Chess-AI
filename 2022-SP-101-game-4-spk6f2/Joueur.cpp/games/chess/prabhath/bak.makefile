CPP_FLAGS = -c
COMPILER = g++

main: parsingfen.o utils.o main.o
		$(COMPILER) -o ./bin/CHESS_BOARD.o ./bin/utils.o ./bin/parsingfen.o ./bin/main.o
main.o: 
		$(COMPILER) $(CPP_FLAGS) -o ./bin/main.o main.cpp
parsingfen.o: 
		$(COMPILER) $(CPP_FLAGS) -o ./bin/parsingfen.o parsingfen.cpp
utils.o: 
		$(COMPILER) $(CPP_FLAGS) -o ./bin/utils.o utils.cpp
clean: 
	rm -f ./bin/*.o