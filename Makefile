LINKER = -mwindows -lwinmm -std=c99
NOWARN = -Wno-return-type -Wno-parentheses -Wno-unused-variable

all:
	gcc main.c $(LINKER) -o ray -DERR_MSG -Wall $(NOWARN)
	./ray

d:
	gcc main.c $(LINKER) -o ray -O0 -g -DERR_MSG

r:
	python imgp.py
	gcc imgc.c -std=c99 -o imgc -Wall $(NOWARN)
	./imgc
	gcc main.c $(LINKER) -o ray -O2 -s -DNDEBUG

t:
	gcc test.c $(LINKER) -o test
	./test

imgc:
	gcc imgc.c -std=c99 -o imgc -Wall $(NOWARN)
	./imgc
