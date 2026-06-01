#compila (precisa do raylib instalado via homebrew)
all:
	gcc main.c -o jogo -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

#roda
run:
	./jogo
