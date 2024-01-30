all:
# rm -f main.exe
# g++ -c simpleTexture.cpp -o simpleTexture.o -Iglew\include -Iglfw\include -Iglm -Wall -g
# g++ simpleTexture.o -o main -Lglfw\lib -Lglew\lib -lglfw3 -lglew32 -lopengl32 -lgdi32 -Wall -g

	rm -f main.exe
	g++ simpleTexture.cpp -o main -Iglew\include -Iglfw\include -Iglm -Lglfw\lib -Lglew\lib -lglfw3 -lglew32 -lopengl32 -lgdi32