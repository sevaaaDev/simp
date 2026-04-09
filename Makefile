simp: main.c simp_backend.o miniaudio.o
	gcc -Wall -Wextra -o simp main.c simp_backend.o miniaudio.o -lm -lpthread

simp_backend.o: simp_backend.c simp_backend.h
	gcc -Wall -Wextra -c simp_backend.c

miniaudio.o: miniaudio.c miniaudio.h
	gcc -Wall -Wextra -c miniaudio.c
