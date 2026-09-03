CC = gcc
CFLAGS = -Wall -Wextra -g
all: main
main: main.c hardware.c hardware.h utils.c utils.h
	$(CC) $(CFLAGS) -lvulkan -o main main.c hardware.c utils.c 
clean:
	rm -f main
memcheck: main
	valgrind ./main
sanitize: main.c hardware.c hardware.h utils.c utils.h
	$(CC) $(CFLAGS) -fsanitize=address,undefined -lvulkan -o main_asan main.c hardware.c utils.c
	./main_asan
