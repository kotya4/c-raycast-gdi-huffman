LINKER = -mwindows -std=c99

all:
	gcc main.c $(LINKER) -o ray -DERR_MSG
	./ray
	
d:
	gcc main.c $(LINKER) -o ray -O0 -g -DERR_MSG

r:
	gcc main.c $(LINKER) -o ray -O2 -s -DNDEBUG

t:
	gcc test.c $(LINKER) -o test
	./test
