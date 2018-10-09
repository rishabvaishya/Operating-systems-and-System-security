/*
 ============================================================================
 Name        : Rishab Vaishya
 SMU ID      : 47505527
 reference1  : https://brennan.io/2015/01/16/write-a-shell-in-c/
 reference2  : https://www.geeksforgeeks.org/quick-sort/
 ============================================================================
 */

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <pthread.h>
#include <stdbool.h>


// definition of methods ****
int lsh_exit(char **args);
int lsh_dir(char **args);
int lsh_clr(char **args);
int lsh_environ(char **args);
int lsh_frand(char **args);
int lsh_fsort(char **args);
void *fileCreation(void *context);
void *readInteger(void *context);
void swap(int*, int*);
int partition(int[], int, int);
void quickSort(int[], int, int);

extern char **environ;

struct childThreadParams childParams;
int *input;
char *file_name;
int NumberOfRandomValues;

// all built-in function created.
char *builtin_str[] =
{ "quit", "dir", "clr", "environ", "frand", "fsort" };

int (*builtin_func[])(char **) =
{
	&lsh_exit,
	&lsh_dir,
	&lsh_clr,
	&lsh_environ,
	&lsh_frand,
	&lsh_fsort
};

int lsh_num_builtins()
{
	return sizeof(builtin_str) / sizeof(char *);
}

int lsh_dir(char **args)
{
	system("ls -al");
	return 1;
}

int lsh_clr(char **args)
{
	system("clear");
	return 1;

}

int lsh_environ(char **args)
{
	char ** env = environ;
	while (*env)
		printf("%s\n", *env++);
	return 1;

}

struct BoundedBuffer
{
	pthread_t childThread;
	pthread_mutex_t mBuffer;
} buffer2;

struct childThreadParams
{
	struct BoundedBuffer buff;
	char* data;
	int count;
};

int lsh_frand(char **args)
{
	printf("Start frand");


	if (args[1] == NULL || args[2] == NULL)
	{
		return 0; // return if no parameters are inputed.
	}
	int ret;
	NumberOfRandomValues = atoi(args[2]);
	file_name = (char *) args[1];

	input = (int *) malloc(sizeof(int));

	pthread_t tid;
	pthread_create(&tid, NULL, fileCreation, input);
	pthread_join(tid, NULL);

	return 1;

}

void *fileCreation(void *context)
{

	FILE *file_pointer;
	file_pointer = fopen(file_name, "w");
	unsigned int temp;
	for (int i = 0; i < NumberOfRandomValues; i = i + 1)
	{
		temp = rand();
		fprintf(file_pointer, "%u\n", temp);
	}

	fclose(file_pointer);
	return 0;
}

void *readInteger(void *context)
{
	FILE *file = fopen(file_name, "r");
	unsigned int integers[10000];

	int i = 0;
	int num;
	int count = 0;
	while (fscanf(file, "%i", &num) > 0)
	{
		integers[i] = num;
		i++;
		count++;
	}
	fclose(file);
	quickSort(integers, 0, count - 1);

	FILE *file_pointer;

	file_pointer = fopen(file_name, "w");
	for (int i = 0; i < count; i = i + 1)
	{
		fprintf(file_pointer, "%u\n", integers[i]);
	}
	fclose(file_pointer);
	return 0;
}


int lsh_fsort(char **args)
{
	if (args[1] == NULL)
	{
		return 0; // return if no parameters are inputed.
	}
	file_name = (char *) args[1];
	pthread_t tid;
	pthread_create(&tid, NULL, readInteger, input);
	pthread_join(tid, NULL);
	return 1;
}

int lsh_exit(char **args)
{
	return 0;
}

int lsh_launch(char **args)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == 0)
	{
		// Child process
		if (execvp(args[0], args) == -1)
		{
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0)
	{
		// Error forking
		perror("lsh");
	} else
	{
		// Parent process
		do
		{
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

int lsh_execute(char **args)
{
	int i;

	if (args[0] == NULL)
	{
		// An empty command was entered.
		return 1;
	}

	for (i = 0; i < lsh_num_builtins(); i++)
	{
		if (strcmp(args[0], builtin_str[i]) == 0)
		{
			return (*builtin_func[i])(args);
		}
	}

	return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024

char *lsh_read_line(void)
{
	int bufsize = LSH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

	if (!buffer)
	{
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		// Read a character
		c = getchar();

		if (c == EOF)
		{
			exit(EXIT_SUCCESS);
		} else if (c == '\n')
		{
			buffer[position] = '\0';
			return buffer;
		} else
		{
			buffer[position] = c;
		}
		position++;

		// If we have exceeded the buffer, reallocate.
		if (position >= bufsize)
		{
			bufsize += LSH_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer)
			{
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

char **lsh_split_line(char *line)
{
	int bufsize = LSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char));
	char *token, **tokens_backup;

	if (!tokens)
	{
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, LSH_TOK_DELIM);
	while (token != NULL)
	{
		tokens[position] = token;
		position++;

		if (position >= bufsize)
		{
			bufsize += LSH_TOK_BUFSIZE;
			tokens_backup = tokens;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens)
			{
				free(tokens_backup);
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, LSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

void lsh_loop(void)
{
	char *line;
	char **args;
	int status;

	do
	{
		printf("Rishab's shell> ");
		line = lsh_read_line();
		args = lsh_split_line(line);
		status = lsh_execute(args);

		free(line);
		free(args);
	} while (status);
}

int main(int argc, char **argv)
{
	// Load config files, if any.
	//	// Run command loop.

	lsh_loop();

	// Perform any shutdown/cleanup.

	return EXIT_SUCCESS;

	exit(0);
}


void swap(int* a, int* b)
{
	int t = *a;
	*a = *b;
	*b = t;
}

int partition(int arr[], int low, int high)
{
	int pivot = arr[high]; // pivot
	int i = (low - 1); // Index of smaller element

	for (int j = low; j <= high - 1; j++)
	{
		// If current element is smaller than or
		// equal to pivot
		if (arr[j] <= pivot)
		{
			i++; // increment index of smaller element
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}

void quickSort(int arr[], int low, int high)
{
	if (low < high)
	{
		/* pi is partitioning index, arr[p] is now
		 at right place */
		int pi = partition(arr, low, high);

		// Separately sort elements before
		// partition and after partition
		quickSort(arr, low, pi - 1);
		quickSort(arr, pi + 1, high);
	}
}

