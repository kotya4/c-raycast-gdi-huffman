LINKER = -mwindows -lwinmm -std=c99
NOWARN = -Wno-return-type -Wno-parentheses -Wno-unused-variable

all:
	gcc main.c $(LINKER) -o ray -DWARNMEPLS -Wall $(NOWARN)
	./ray

d:
	gcc main.c $(LINKER) -o ray -O0 -g -DWARNMEPLS

r:
	gcc main.c $(LINKER) -o ray -O2 -s -DNDEBUG

t:
	gcc test.c $(LINKER) -o test
	./test

i:
	python imgp.py
	gcc imgc.c -std=c99 -o imgc -Wall $(NOWARN)
	./imgc

husk:
	gcc main2.c $(LINKER) -o husk

