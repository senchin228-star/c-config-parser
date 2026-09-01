CC = gcc
CFLAGS = -Wall -Wextra -g
all: main
main: main.c hardware.c hardware.h utils.c utils.h
	$(CC) $(CFLAGS) -I/opt/cuda/include -o main main.c hardware.c utils.c -lnvidia-ml 
clean:
	rm -f main
memcheck: main
	valgrind --leak-check=full ./main
sanitize: main.c hardware.c hardware.h utils.c utils.h
	$(CC) $(CFLAGS) -fsanitize=address,undefined -I/opt/cuda/include -o main_asan main.c hardware.c utils.c -lnvidia-ml 
	./main_asan
