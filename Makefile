game:
	g++ renderer.cc -Wall -Wextra -pedantic -Ofast -std=c++20 -I include -L lib -l SDL2-2.0.0
clean:
	-rm play