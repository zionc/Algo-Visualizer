CC=clang
OSX_OPT = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib.a
OSX_OUT = -o main
CFILES = main.c
CFLAGS = -Wall -Werror
main: main.c 
	$(CC) $(CFLAGS) $(CFILES) $(OSX_OUT) $(OSX_OPT)

	