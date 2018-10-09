/*
 ============================================================================
 Name        : OS Program 2
 Author      : Rishab Vaishya
 SMU ID      : 47505527
 ============================================================================
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

	int numberOfSec;			// Number of seconds
	pid_t processID;			// Process ID

	if (argc == 2)
	{
		numberOfSec = atoi(argv[1]);
	}
	else
	{
		numberOfSec = 10;
	}
	if (numberOfSec == 0)
	{
		printf(" The specified number is invalid, program Terminated !\n");
		return 0;
	}
	processID = fork();
	if (processID == -1)
	{
		// -1 indicates an Error occurred

		fprintf(stderr, "can't fork, error %d\n", errno);
		exit(EXIT_FAILURE);
	}
	else if (processID == 0)
	{
		// 0 indicates we are in the child process.

		for (int j = 1; j <= numberOfSec; j++)
		{
			sleep(1);
			printf("Child Process with PID:   %d%s%d%s%d\n", getpid(),
					"  and PPID:  ", getppid(), "   tick:  ", j);
		}
		_exit(0);
	}
	else
	{
		// A positive number indicates we are in the parent process

		for (int i = 1; i <= numberOfSec; i++)
		{
			sleep(1);
			printf("Original Process with PID:  %d%s%d%s%d\n", getpid(),
					"  and PPID:  ", getppid(), "   tick:  ", i);
		}
		int returnStatus;

		// Parent process waits here for child to terminate.
		waitpid(processID, &returnStatus, 0);

		// Verify child process terminated without error.
		if (returnStatus == 0)
		{
			printf("Child process terminating\n");
		}
		else if (returnStatus == 1)
		{
			printf("The child process terminated with an error!");
		}
		printf("Parent process terminating\n");
		exit(0);
	}
	return 0;
}



/*
 *

=======>  PART I

1. Using a simple five-state process model, what states do you think each process goes through during its lifetime?

Answer - Each process goes through New state (processID = fork) to Ready state and then Running state,
as soon as we call sleep() function process goes to Blocked state for given time,
then again Blocked process goes to Ready state and then from Ready to Running state until loop ends.



=======>  PART II

1. Describe what you see using comments in your source code.  Is there anything unusual about the PPID in the child? Do some research and explain what is happening.

Answer - When we comment Sleep() & Wait() in parent process,
it is observed that parent process execute first without sleep() & the child process follows the loop.

 *
 * */



/*
 *

=======>  OUTPUT in genuse server

Parent Process with processID:  15066  and PPID:  14984   tick:  1
Child Process with processID:   15067  and PPID:  15066   tick:  1
Parent Process with processID:  15066  and PPID:  14984   tick:  2
Child Process with processID:   15067  and PPID:  15066   tick:  2
Parent Process with processID:  15066  and PPID:  14984   tick:  3
Child Process with processID:   15067  and PPID:  15066   tick:  3
Parent Process with processID:  15066  and PPID:  14984   tick:  4
Child Process with processID:   15067  and PPID:  15066   tick:  4
Parent Process with processID:  15066  and PPID:  14984   tick:  5
Child Process with processID:   15067  and PPID:  15066   tick:  5
Parent Process with processID:  15066  and PPID:  14984   tick:  6
Child Process with processID:   15067  and PPID:  15066   tick:  6
Parent Process with processID:  15066  and PPID:  14984   tick:  7
Child Process with processID:   15067  and PPID:  15066   tick:  7
Parent Process with processID:  15066  and PPID:  14984   tick:  8
Child Process with processID:   15067  and PPID:  15066   tick:  8
Parent Process with processID:  15066  and PPID:  14984   tick:  9
Child Process with processID:   15067  and PPID:  15066   tick:  9
Parent Process with processID:  15066  and PPID:  14984   tick:  10
Child Process with processID:   15067  and PPID:  15066   tick:  10
Child process terminating
Parent process terminating

 *
 * */
