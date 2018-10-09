/*
 ============================================================================
 Assignment  : OS Program 5
 Author      : Rishab Vaishya
 SMU ID      : 47505527
 Instruction : please keep 'testdata.txt' & 'results.txt' in the same directory as the 'OS Program 5.c' file
 (in order to find the files)
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// modifiable clock size
#define CLOCK_SIZE 4

// label for next pointer
#define nextFrameLabel "<- next frame"

// A structure of a Frame
struct Frame
{
	int frame;
	int page;
	int use;
	int modify;
};

// to point to the next frame.
int nextPointer = 0;

// for comparing the read/write characters for the modify flag
char write[] =
{ 'w', 'W' };

// writes the updates frames into the file
// takes output file and frames array as the input
void writeClockToFile(FILE* filePtr, struct Frame frames[])
{
	int i = 0;
	fprintf(filePtr, "\tFRAME\tPAGE\tUSE\t\tMODIFY\n");
	for (; i < CLOCK_SIZE; i++)
	{
		// when next pointer is pointing to the current frame. Append the arrow <- label text
		if (nextPointer == i)
		{
			fprintf(filePtr, "\t%d\t\t%d\t\t%d\t\t%d %s\n", frames[i].frame,
					frames[i].page, frames[i].use, frames[i].modify,
					nextFrameLabel);
		}
		else
		{
			fprintf(filePtr, "\t%d\t\t%d\t\t%d\t\t%d\n", frames[i].frame,
					frames[i].page, frames[i].use, frames[i].modify);
		}
	}

}
;

// initialize all frames to starting values.
void initializeFrame(struct Frame frames[CLOCK_SIZE])
{
	int t;
	for (t = 0; t < CLOCK_SIZE; t++)
	{
		frames[t].frame = t;
		frames[t].page = -1;
		frames[t].use = 0;
		frames[t].modify = 0;
	}
}

/*
 * Check to see if the page in the record is already in the clock and if it is update to appropriate flag(s).
 * (This will not reposition the next frame pointer)
 *
 * returns success or failure in step Two
 * */
bool stepTwo(struct Frame frames[CLOCK_SIZE], int page, char operation)
{
	int originalNextPointer = nextPointer;
	for (int i = 0; i < CLOCK_SIZE; i++)
	{
		// to reset the clock pointer to avoid out of bound
		if (nextPointer >= CLOCK_SIZE)
		{
			nextPointer = 0;
		}

		// when page is empty at the start
		if (frames[nextPointer].page == -1)
		{
			// update page flag
			frames[nextPointer].page = page;

			// update use flag
			frames[nextPointer].use = 1;

			// update modify flag
			if (operation == write[0] || operation == write[1])
			{
				frames[nextPointer].modify = 1;
			}
			nextPointer++;

			// to reset the clock pointer to avoid out of bound
			if (nextPointer >= CLOCK_SIZE)
			{
				nextPointer = 0;
			}
			return true;
		}
		// when page already exists in the frame
		else if (frames[nextPointer].page == page)
		{
			// update page flag
			frames[nextPointer].page = page;

			// update use flag
			frames[nextPointer].use = 1;

			// update modify flag & point to next frame only when write operation is performed
			if (operation == write[0] || operation == write[1])
			{
				frames[nextPointer].modify = 1;
				nextPointer++;
			}

			// to reset the clock pointer to avoid out of bound
			if (nextPointer >= CLOCK_SIZE)
			{
				nextPointer = 0;
			}
			return true;
		}
		nextPointer++;
	}
	return false;
}

/*
 * beginning at the current position of the next frame pointer scan the clock.
 * During this scan make no changes to the use bit.
 * The first frame encountered with (u = 0; m = 0) is selected for replacement.
 *
 * returns success or failure in step Three
 * */
bool stepThree(struct Frame frames[CLOCK_SIZE], int page, char operation)
{
	for (int i = 0; i < CLOCK_SIZE; i++)
	{
		// to reset the clock pointer to avoid out of bound
		if (nextPointer >= CLOCK_SIZE)
		{
			nextPointer = 0;
		}
		// when u = 0 and m = 1
		if (frames[nextPointer].use == 0 && frames[nextPointer].modify == 0)
		{
			// update page flag
			frames[nextPointer].page = page;

			// update use flag
			frames[nextPointer].use = 1;

			// update modify flag & point to next frame only when write operation is performed
			if (operation == write[0] || operation == write[1])
				frames[nextPointer].modify = 1;
			else
				frames[nextPointer].modify = 0;
			nextPointer++;

			// to reset the clock pointer to avoid out of bound
			if (nextPointer >= CLOCK_SIZE)
			{
				nextPointer = 0;
			}
			return true;
		}
		nextPointer++;
	}
	return false;
}


/*
 * scan the clock again.
 * The first frame encountered with (u = 0; m = 1) is selected for replacement.
 * During this scan, set the use bit to 0 for each frame that is bypassed.
 *
 * returns success or failure in step Four
 * */
bool stepFour(struct Frame frames[CLOCK_SIZE], int page, char operation)
{
	for (int i = 0; i < CLOCK_SIZE; i++)
	{
		if (nextPointer >= CLOCK_SIZE)
		{
			nextPointer = 0;
		}
		if (frames[nextPointer].use == 0 && frames[nextPointer].modify == 1)
		{
			frames[nextPointer].page = page;
			frames[nextPointer].use = 1;
			if (operation == write[0] || operation == write[1])
				frames[nextPointer].modify = 1;
			else
				frames[nextPointer].modify = 0;
			nextPointer++;
			if (nextPointer >= CLOCK_SIZE)
			{
				nextPointer = 0;
			}
			return true;
		}
		frames[nextPointer].use = 0;
		nextPointer++;
	}
	return false;
}

int main()
{
	// open "testdata.txt" to read all records
	char inFileName[] = "testdata.txt";
	FILE *inFilePtr = fopen(inFileName, "r");

	// if input file could not be found
	if (inFilePtr == NULL)
	{
		printf("File %s could not be opened.\n", inFileName);
		exit(1);
	}

	// open "results.txt" to read all records
	char outFileName[] = "results.txt";
	FILE *outFilePtr = fopen(outFileName, "w");

	// if output file could not be found
	if (outFilePtr == NULL)
	{
		printf("File %s could not be opened.\n", outFileName);
		exit(1);
	}

	// stores page value
	int page;

	// stores read/ write character value
	char operation;

	// frame array of size : clock size defined above
	struct Frame frames[CLOCK_SIZE];

	// initialze values of all frames
	initializeFrame(frames);

	// scan first record from input file
	fscanf(inFilePtr, "%d%c", &page, &operation);

	// scan till end of file is not reached
	while (!feof(inFilePtr))
	{
		// store success and failure of each step
		bool stepResult;

		// write Page referenced in output file
		fprintf(outFilePtr, "\nPage referenced: %d %c\n", page, operation);

		// perform step 2 as per the algorithm
		stepResult = stepTwo(frames, page, operation);
		do
		{
			// if step 2 fails
			if (!stepResult)
				// perform step 3 as per the algorithm
				stepResult = stepThree(frames, page, operation);
			// if step 2 & 3 fails,
			if (!stepResult)
				// perform step 4 as per the algorithm
				stepResult = stepFour(frames, page, operation);
		}
		// Repeat step 3 and, if necessary, step 4.
		// By time a frame will be found for replacement.
		while (!stepResult);

		// print frames values to output file after each iteration in required format
		writeClockToFile(outFilePtr, frames);

		// scan next record
		fscanf(inFilePtr, "%d%c", &page, &operation);

	} //end while

	// close input and out file
	fclose(inFilePtr);
	fclose(outFilePtr);
	return 0;
}
;

