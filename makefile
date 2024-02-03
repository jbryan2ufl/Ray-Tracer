CXX = g++
EXE = main
INCLUDE = -Iinclude
LINK = -Llib -lglew32 -lglfw3 -lopengl32 -lgdi32
FLAGS = -Wall -g -std=c++2b

TARGETS = $(subst src/, , $(wildcard src/*.cpp))

all: clean $(TARGETS:.cpp=.o)
	$(CXX) *.o -o $(EXE) $(LINK) $(FLAGS)

# clean2:
# 	$(CXX) -c imgui/*.cpp $(INCLUDE) $(LINK)

%.o: src/%.cpp
	$(CXX) -c $< -o $@ $(INCLUDE) $(FLAGS)

clean:
	rm -f *.exe $(TARGETS:.cpp=.o)
