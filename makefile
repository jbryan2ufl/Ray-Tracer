CXX = g++
EXE = main
INCLUDE = -Iglfw\include -Iglew\include -Iglm
LINK = -Lglfw\lib -Lglew\lib -lglfw3 -lglew32 -lopengl32 -lgdi32
FLAGS = -Wall -g

TARGETS = $(subst src/, , $(wildcard src/*.cpp))

all: clean $(TARGETS:.cpp=.o)
	$(CXX) $(TARGETS:.cpp=.o) -o $(EXE) $(LINK) $(FLAGS)

%.o: src/%.cpp
	$(CXX) -c $< -o $@ $(INCLUDE) $(FLAGS)

clean:
	rm -f *.exe *.o