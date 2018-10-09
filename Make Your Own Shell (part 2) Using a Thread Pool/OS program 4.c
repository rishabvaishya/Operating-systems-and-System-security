/*
 ============================================================================
 Assignment  : OS program 4
 Author      : Rishab Vaishya
 SMU_ID      : 47505527
 Reference1  : https://computing.llnl.gov/tutorials/pthreads/
 Reference2  : https://www.geeksforgeeks.org/quick-sort/
 Reference3  : https://www.geeksforgeeks.org/queue-set-1introduction-and-array-implementation/

 ============================================================================
 */

#include <curses.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// A structure of command
struct command_data
{
	char *commandName;
	char *argument1;
	char *argument2;
};

// A structure of a queue
struct Queue
{
	int first;
	int last;
	int size;
	struct command_data* array;
};

// where all the environ variables are stored
extern char **environ;

// this is the numnber of thread in the thread pool & also the message buffer size
int threadCount = 5;

// this is used to locking the shared buffer
pthread_mutex_t queue_mutex;

// used to send and recieving signals betwwen boss and worker threads
pthread_cond_t new_input_in_queue;

// creates & returns a queue.
struct Queue* instantiateQueue()
{
	struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
	queue->first = queue->size = 0;
	queue->last = threadCount - 1;
	queue->array = (struct command_data*) malloc(
			threadCount * sizeof(struct command_data));
	return queue;
}

// this is used to create a instance of struct command from line input
struct command_data getCommandStructFromInput(char currentCommand[200])
{
	struct command_data command;
	command.commandName = malloc(sizeof(char) * 50);
	command.argument1 = malloc(sizeof(char) * 50);
	command.argument2 = malloc(sizeof(char) * 50);
	char* token = malloc(sizeof(char) * 50);
	// get the first word by splitting
	token = strtok(currentCommand, " ");

	strcpy(command.commandName, token);
	token = strtok(NULL, " ");

	if (token != NULL)
	{
		strcpy(command.argument1, token);
	}
	token = strtok(NULL, " ");

	if (token != NULL)
	{
		strcpy(command.argument2, token);
	}
	return command;
}

// adds new element in the queue.
void addToQueue(struct Queue* queue, struct command_data command)
{
	// if queue is full
	if (queue->size == threadCount)
	{
		return;
	}
	queue->last = (queue->last + 1) % threadCount;
	queue->array[queue->last] = command;
	queue->size = queue->size + 1;
}

// returns the first element in the queue.
struct command_data getQueueNext(struct Queue* queue)
{
	// if queue is empty
	if (queue->size == 0)
	{
		struct command_data emptyCommand;
		emptyCommand.commandName = "empty";
		return emptyCommand;
	}
	struct command_data item = queue->array[queue->first];
	queue->first = (queue->first + 1) % threadCount;
	queue->size = queue->size - 1;

	return item;
}

// implementation of frand command, accepts 2 parameters - filename and no of random variables
// command.argument1 is the file name
// command.argument2 is the Random Number Count
void frand(struct command_data command)
{
	int RandomNumberCount;
	unsigned int temperaryInteger;
	RandomNumberCount = atoi(command.argument2);

	// function returns if parameters are not properly inputed
	if (command.argument1 == NULL || RandomNumberCount == 0)
	{
		printf("parameters are NOT properly inputed");
		return;
	}

	// this is used to remove the nect line charatcer from the argument
	strtok(command.argument1, "\n");

	// this is used to append .txt extension to file name
	strcat(command.argument1, ".txt");

	FILE *file;
	file = fopen(command.argument1, "w");

	// generate random numbers and write it to the file.
	for (int i = 0; i < RandomNumberCount; i = i + 1)
	{
		temperaryInteger = rand();
		fprintf(file, "%u\n", temperaryInteger);
	}
	fclose(file);
}

// ******************************* Quick-Sort Section open  ***************************************
// code in this section is imported from this Reference  : https://www.geeksforgeeks.org/quick-sort/

// swaps the values inside pointers of element1 and element 2
void interchange(int* element1, int* element2)
{
	int temperaryInteger = *element1;
	*element1 = *element2;
	*element2 = temperaryInteger;
}

// used in quick sort implementation to divide the array into further smaller arrays, returns the partition index
int divide(int array[], int left, int right)
{
	int high = array[right];
	int i = (left - 1);

	for (int j = left; j <= right - 1; j++)
	{
		// If current element is less than or equal to high
		if (array[j] <= high)
		{
			i++;
			interchange(&array[i], &array[j]);
		}
	}
	interchange(&array[i + 1], &array[right]);
	return (i + 1);
}

// implementation of quick sort
void quickSort(int array[], int left, int right)
{
	if (left < right)
	{
		// array is now at right place
		int partitioningIndex = divide(array, left, right);

		// sort elements from 0 till the partition index (left partition of the array)
		quickSort(array, left, partitioningIndex - 1);

		// sort elements from partition index till the end (right partition of the array)
		quickSort(array, partitioningIndex + 1, right);
	}
}

// ******************************* Quick-Sort Section close  ***************************************

// implementation of fsort command, accepts 1 parameters - filename
// command.argument1 is the file name
void fsort(struct command_data command)
{
	// function returns if parameters is not properly inputed
	if (command.argument1 == NULL)
	{
		printf("no parameters are inputed");
		return;
	}

	// this is used to remove the nect line charatcer from the argument
	strtok(command.argument1, "\n");

	// this is used to append .txt extension to file name
	strcat(command.argument1, ".txt");

	FILE *file;
	if ((file = fopen(command.argument1, "r")) == NULL) // checks to see if file exists, retuns if not present
	{
		printf("File not found\n");
		return;
	}

	// assuming file has 1000 integers values in it.
	unsigned int integers[1000];

	int i = 0;
	int num;
	int count = 0;
	// fetches values from file and inserts into the integers array
	while (fscanf(file, "%i", &num) > 0)
	{
		integers[i] = num;
		i++;
		count++;
	}
	fclose(file);

	// sorts the integer array
	quickSort(integers, 0, count - 1);

	// now once file is sorted, opens file in write mode and insert all sorted integers
	file = fopen(command.argument1, "w");
	for (int i = 0; i < count; i = i + 1)
	{
		fprintf(file, "%u\n", integers[i]);
	}

	fclose(file);

}

// implemented by all threads, it is used to execute the commands inputed by the user
// input parameter is a queue which is the shared buffer holding all the commands
void* executeCommand(void* voidQueue)
{
	struct Queue* queue = (struct Queue*) voidQueue;
	char empty[] = "empty";
	struct command_data command_struct_new;

	// thread runs till program exists
	while (1)
	{
		// allocate lock on the shared buffer, which in our case is a queue
		pthread_mutex_lock(&queue_mutex);
		do
		{
			// waits for the signal from boss thread when a new command is inputed in the shared buffer
			pthread_cond_wait(&new_input_in_queue, &queue_mutex);

			// gets the newly inputed command from the shared buffer
			command_struct_new = getQueueNext(queue);
		}
		// checks whether the new command is empty or not, IfEmpty then go back to wait stage.
		while (!strcmp(command_struct_new.commandName, empty));

		// unlocks the queue mutex
		pthread_mutex_unlock(&queue_mutex);

		// list of all commands
		char *custom_command_list[] =
		{ "dir", "clr", "environ", "frand", "fsort" };

		// this is used to remove the nect line charatcer from the commandName
		strtok(command_struct_new.commandName, "\n");

		// 'dir' command
		if (!strcmp(command_struct_new.commandName, custom_command_list[0]))
		{
			system("ls -al");

		}
		// 'clr' command
		else if (!strcmp(command_struct_new.commandName,
				custom_command_list[1]))
		{
			system("clear");

		}
		// 'environ' command
		else if (!strcmp(command_struct_new.commandName,
				custom_command_list[2]))
		{
			char ** env = environ;
			while (*env)
			{
				printf("%s\n", *env++);
			}
		}
		// 'frand' command
		else if (!strcmp(command_struct_new.commandName,
				custom_command_list[3]))
		{
			frand(command_struct_new);
		}
		// 'fsort' command
		else if (!strcmp(command_struct_new.commandName,
				custom_command_list[4]))
		{
			fsort(command_struct_new);
		}
		// for all other commands
		else
		{
			system(command_struct_new.commandName);
		}
	}

}

// initialize all worker threads , threadcount is the number of threads to be running in the thread pool
void initializeThreads(int threadCount, pthread_t threads[threadCount],
		struct Queue* queue)
{
	int t;
	for (t = 0; t < threadCount; t++)
	{
		pthread_create(&threads[t], NULL, executeCommand, (void *) queue);
	}
}

// ******************************* MAIN implementation  ********************************************

int main(int argc, char *argv[])
{
	int result = 0;

	// command which is inputed from user
	char currentCommand[200];

	// initialize queue and get it's instance
	struct Queue* queue = instantiateQueue();

	pthread_t threads[threadCount];

	// initialize threads at start of the program
	initializeThreads(threadCount, threads, queue);
	printf("\nRishab's Shell>");

	// till exit condition is not met, keep accepting user inputs
	while (1)
	{

		fgets(currentCommand, 200, stdin);
		char exit[] = "exit";
		char quit[] = "quit";

		strtok(currentCommand, "\n");

		// exit if user inputs 'quit'
		if (!strcmp(currentCommand, quit))
		{
			break;
		}
		// exit if user inputs 'exit'
		else if (!strcmp(currentCommand, exit))
		{
			break;
		}
		// add command in queue for all other conditions
		else
		{
			// fetch command structure instance from command line input
			struct command_data command_struct = getCommandStructFromInput(
					currentCommand);
			// lock queue mutex
			pthread_mutex_lock(&queue_mutex);

			// add command to queue, which is the shared buffer
			addToQueue(queue, command_struct);

			// broadcast signal to all worker threads that a new command has been added into shared buffer
			pthread_cond_broadcast(&new_input_in_queue);

			// unlock queue mutex
			pthread_mutex_unlock(&queue_mutex);
		}

		// make thread sleep for 100 micro seconds so that user gets ready to input the next command
		usleep(100000);
		printf("\nRishab's Shell> ");
	}
	printf("\nGood-Bye !!!\n\n");
	exit(0);

}

