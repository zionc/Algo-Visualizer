CC=clang
OSX_OPT = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib.a
OSX_OUT = -o main
CFILES = main.c

main: main.c 
	$(CC) $(CFILES) $(OSX_OUT) $(OSX_OPT)

	