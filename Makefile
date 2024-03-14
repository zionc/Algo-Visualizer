CC=clang
OSX_OPT = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib.a
OSX_OUT = -o main
CFILES = main.c graph.c game.c
CFLAGS = -Wall -Wextra -I./include
main: main.c 
	$(CC) $(CFLAGS) $(CFILES) $(OSX_OUT) $(OSX_OPT)
clean:
	rm -f main
	