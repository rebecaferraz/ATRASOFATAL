all:
	gcc main.c cli-lib/src/screen.c cli-lib/src/keyboard.c cli-lib/src/timer.c -I cli-lib/include -o jogo

run:
	./jogo
