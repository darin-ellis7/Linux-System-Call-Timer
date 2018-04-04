//Darin Ellis - 3/24/2018

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BILLION 1000000000

/**
 * Takes an array and its size and calculates the
 * arithmetic mean.
 *
 * @param an_array[] any array of doubles
 * @param size       the integer size of the above array
 * @return           the mean
 */
double mean(double an_array[], int size)
{
	double sum = 0;
	for(int i = 0; i < size; i++)
	{
		sum += an_array[i];
	}

	return sum / size;
}

/**
 * Takes an array and its size and calculates the
 * standard deviation. Also invokes mean() and reports
 * the mean.
 *
 * @param an_array[] any array of doubles
 * @param size       the integer size of the above array
 * @return           the standard deviation
 */
double std_deviation(double an_array[], int size)
{
	double average = mean(an_array, size);
	printf("Mean: \t \t   %f nanoseconds\n", average);

	double calc_array[size];
	for(int i = 0; i < size; i++)
	{
		calc_array[i] = pow(an_array[i] - average, 2);
	}

	return sqrt(mean(calc_array, size));
}

/**
 * Simply opens a file and then closes it. This is to
 * prepare the system for open calls - the first call
 * takes longer than the rest, so this filters that out.
 */
void init_open()
{
       	char *filename = "./test";
        int fd = open(filename, O_CREAT | O_RDONLY);
	//failure case - warn user
	if(fd == -1)
	{
		perror("open() system call failure.");
	}
        close(fd);
	return;
}

/**
 * Similar to init_open(), this just prepares the system
 * for fork calls.
 */
void init_fork()
{
	int fork_condition = fork();
	if(fork_condition == 0)
        {
                exit(EXIT_SUCCESS);
        }
	else if(fork_condition == -1)
        {
                perror("fork() system call failure: ");
        }
        else
        {
		wait(NULL);
        }
	return;
}

/**
 * This function times the opening of a file (assumed to not
 * exist) using open. To record
 * time immediately before and after the sytem call, I use clock_gettime()
 * from time.h. It then returns the difference in nanoseconds.
 *
 * @return           The difference between the start of the call
 *                   and the end.
 */
double open_wrapper()
{
	struct timespec start, end;
        char *filename = "./test";

	clock_gettime(CLOCK_MONOTONIC, &start);
        int fd = open(filename, O_CREAT | O_RDONLY);
	clock_gettime(CLOCK_MONOTONIC, &end);

	if(fd == -1)
        {
                perror("open() system call failure.");
        }
        close(fd);

	return (end.tv_nsec - start.tv_nsec);
}

/**
 * This function times forking. The child processes do nothing
 * but exit immediately. To record
 * time immediately before and after the sytem call, I use clock_gettime()
 * from time.h. It then returns the difference in nanoseconds.
 *
 * @return           The difference between the start of the call
 *                   and the end.
 */
double fork_wrapper()
{
	struct timespec start, end;

	clock_gettime(CLOCK_MONOTONIC, &start);
	int fork_condition = fork();

	//child case
	if(fork_condition == 0)
	{
		exit(EXIT_SUCCESS);
	}
	//failure case - send warning, but continue
        else if(fork_condition == -1)
	{
		perror("fork() system call failure.");
		clock_gettime(CLOCK_MONOTONIC, &end);
	}
	//parent case - just wait
	else
	{
		wait(NULL);
		clock_gettime(CLOCK_MONOTONIC, &end);
	}
	return (end.tv_nsec - start.tv_nsec);
}

/**
 * This function simply performs the timing process, but without
 * anything enclosed within start and end. Because of the way
 * I time the system calls, there is not much to filter out.
 *
 * @return		The difference between start and end
 */
double dummy_open()
{
	struct timespec start, end;

	clock_gettime(CLOCK_MONOTONIC, &start);
	clock_gettime(CLOCK_MONOTONIC, &end);

	return (end.tv_nsec - start.tv_nsec);
}

/**
 * This function simply performs the timing process, but without
 * anything enclosed within start and end. Because of the way
 * I time the system calls, there is not much to filter out.
 *
 * @return 		The difference between start and end
 */
double dummy_fork()
{
	struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC, &start);
	if (0)
	{
		//this will not happen
	}
	else if (0)
	{
		//this shouldnt happen either
	}
	else
	{
		clock_gettime(CLOCK_MONOTONIC, &end);
	}

	return (end.tv_nsec - start.tv_nsec);
}

/**
 * This function is the bulk of the program. It takes several
 * function pointers as arguments. These function pointers handle
 * the parts of the program unique to the type of system call
 * (such as "open" or "fork"). driver() takes them and runs the
 * number of tests specified by the other arguments. It then also
 * determines the mean and standard deviation of the tests by
 * invoking std_deviation().
 *
 * @param *init            This function prepares the system to
 *                         run the tests efficiently.
 * @param *callwrapper     This function does the actual timing
 *                         of the system call, as well as misc
 *                         upkeep.
 * @param *dummy           This function exists to compare to
 *                         *callwrapper to avoid timing inaccurate
 *                         timing.
 * @param runsInSingleTest This integer indicates how many times
 *                         to time a system call within one test.
 *                         This can be useful due to the small time
 *                         it takes to execute a single call.
 * @param numberOfTests    This integer indicates how many tests
 *                         to run.
 * @return                 the mean
 */
void driver(
	void (*init)(),
	double (*callwrapper)(),
	double (*dummy)(),
	int runsInSingleTest,
	int numberOfTests
){
	(*init)(); //do necessary initialization for given system call

	double test_times[numberOfTests];
	double call_time = 0;
	double dummy_time = 0;
	for(int i = 0; i < numberOfTests; i++)
	{
		for(int j = 0; j < runsInSingleTest; j++)
		{
			//time system call
			call_time += (*callwrapper)();
			//time corresponding dummy
			dummy_time += (*dummy)();
		}

		double time = call_time - dummy_time; //compare
		test_times[i] = time;
	}

	double stddev = std_deviation(test_times, numberOfTests);
	printf("Standard Deviation: %f nanoseconds\n", stddev);

	return;
}

/**
 * main() mostly handles the program input and the setup to
 * execute driver()
 *
 * @param argc    This should be equal to 4. If not, the program ends.
 * @param argv[1] This should be the name of the system call that
 *                needs to be timed - only open() and fork() are
 *                implemented at the moment.
 * @param argv[2] This should be the number of system calls to be
 *                considered part of one test.
 * @param argv[3] This should be the numbers of tests
 */
int main(int argc, char *argv[])
{

	if(argc != 4)
	{
		fprintf(stderr, "Error: Wrong number of arguments. Expected 3.\n");
		return -1;
	}

	int runsInSingleTest = atoi(argv[2]);
	int numberOfTests = atoi(argv[3]);

	if(runsInSingleTest == 0  || numberOfTests == 0)
	{
		fprintf(stderr, "Error: Zero or non-number parameter\n");
		return -1;
	}

	if (strcmp(argv[1], "open") == 0)
	{
		printf("Open\n");
		driver(init_open, open_wrapper, dummy_open, runsInSingleTest, numberOfTests);
	}
	else if (strcmp(argv[1], "fork") == 0)
	{
		printf("Fork\n");
		driver(init_fork, fork_wrapper, dummy_fork, runsInSingleTest, numberOfTests);
	}
	else
	{
		printf("Unrecognized system call.\n");
	}

	return 0;
}
