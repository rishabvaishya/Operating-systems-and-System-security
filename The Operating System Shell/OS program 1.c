/******************************************************************************

CSE 7343-Operating Systems - Program 1
Name - Rishab Vaishya
SMU ID - 47505527

*******************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char*argv[])
{

    int NoOfSeconds,tick;
    pid_t process_ID;
    process_ID = getpid();


    if (argc == 2) // if count is provided by the user
    {
        NoOfSeconds = atoi(argv[1]);    // Get 2nd parameter from list

    }
    else
    {
    	NoOfSeconds=5;
    }

    if (NoOfSeconds <= 0)   // No Of Seconds cannot be 0 or less
    {
        printf(" Count cannot be 0 or less!\n");
        return 0;
    }

    printf("Starting process %d - Sleepy %d\n", process_ID, NoOfSeconds);

    for (tick = 1; tick <= NoOfSeconds; tick++) 
    {
        sleep(1); // Sleep for 1 second
		printf("process %d - tick %d \n",process_ID,tick);
    }

    return 0;
}
