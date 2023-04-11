build:
	gcc -std=c99 -Wall -o disasm src/*.c

exec:
	./disasm

clean:
	rm disasm
