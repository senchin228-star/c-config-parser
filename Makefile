CC = gcc
CFLAGS = -Wall -Wextra -g
all: main
main: main.c hardware.c hardware.h
	$(CC) $(CFLAGS) -o main main.c hardware.c
clean:
	rm -f main
memcheck: main
	valgrind --leak-check=full ./main
sanitize: main.c hardware.c hardware.h
	$(CC) $(CFLAGS) -fsanitize=address,undefined -o main_asan main.c hardware.c
	./main_asan
