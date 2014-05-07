warmup2: warmup2.o my402list.o timeCal.o
	gcc -o warmup2 -g warmup2.o my402list.o timeCal.o -lm -lpthread

warmup2.o: warmup2.c my402list.h
	gcc -g -c -Wall warmup2.c

my402list.o: my402list.c my402list.h
	gcc -g -c -Wall my402list.c

timeCal.o:timeCal.c
	gcc -g -c -Wall timeCal.c

clean:
	rm -f *.o warmup2
