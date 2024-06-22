build:
	clang -Wall -g -pg -march=native -std=c99 ./src/*.c -lSDL2 -lm -o renderer

build-prod:
	clang -Wall -O3 -funroll-loops -march=native -std=c99 ./src/*.c -lSDL2 -lm -o renderer

build-mac:
	arch -x86_64 gcc -Wall -g -std=c99 ./src/*.c -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -l SDL2 -lm -o renderer

run:
	./renderer

clean:
	rm renderer
