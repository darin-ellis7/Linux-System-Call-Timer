harnessmake: harness.c
	gcc -o harness harness.c -lm -Wall

clean:
	rm harness
