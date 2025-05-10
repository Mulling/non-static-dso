CFLAGS := -Wall -Wextra -Wpedantic -g

all: main lib1.so lib2.so payload.bin

main: main.c
	$(CC) $(CFLAGS) -o $@ $^

lib1.so: lib1.c
	$(CC) $(CFLAGS) -o $@ $^ -shared -fpic


lib2.so: lib2.c
	$(CC) $(CFLAGS) -o $@ $^ -shared -fpic

payload.bin: payload.asm
	nasm -f bin $^ -o $@

clean:
	$(RM) *.o *.so main compile_commands.json payload.bin

.PHONY: clean
