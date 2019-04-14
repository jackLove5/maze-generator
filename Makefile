all: maze_generator

clean:
	rm *.o ./maze_generator

maze_generator: maze.o output.o maze_generator.o
	g++ -o maze_generator maze.o output.o maze_generator.o

maze.o: maze.h maze.cpp
	g++ -c maze.cpp

output.o: maze.h output.h output.cpp
	g++ -c output.cpp

maze_generator.o: maze.h output.h maze_generator.cpp
	g++ -c maze_generator.cpp
