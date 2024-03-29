#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

/// Other Functions File ///

// Project Members: Daniel Langer, Alan George, and Kenneth Procacci //

void findDims()
{
	// scan a single row to find x_dim
	char rowForRowDim[100];

	fgets(rowForRowDim, 100, input);
	int i;
	for (i = 0; i < strlen(rowForRowDim); i++)
	{
		if (rowForRowDim[i] == '*' || rowForRowDim[i] == ' ')
		{
			x_dim++;
		}
		else if (isdigit(rowForRowDim[i]))
		{
			while (isdigit(rowForRowDim[i + 1]))
			{
				i++;
			}
			x_dim++;
		}
	}
	fclose(input);

	// scan entire file to get y_dim
	input = fopen("maze.txt", "r");
	while (fgets(rowForRowDim, 100, input) != NULL)
	{
		y_dim++;
	}
	fclose(input);

	// printf("x = %i, y = %i", x_dim, y_dim);
}

void createArrays()
{
	// create maze array
	mazeArray = (char *)malloc(((x_dim) * (y_dim)) * sizeof(char *));

	// create deed and pheremone array
	deedArray = (int *)malloc(((x_dim) * (y_dim)) * sizeof(int *));
	pheromoneArray = (int *)malloc(((x_dim) * (y_dim)) * sizeof(int *));

	// initialize both to zero
	int i;
	for (i = 0; i < (x_dim * y_dim); i++)
	{
		deedArray[i] = 0;
		pheromoneArray[i] = 0;
	}
}

void scan()
{
	// Open file
	input = fopen("maze.txt", "r");

	// Scans the input file into the necessary arrays
	int i, j;
	for (i = 0; i < y_dim; i++)
	{
		char line[1000];
		fgets(line, 1000, input);
		int xthCharacter = 0;
		for (j = 0; j < strlen(line); j++)
		{
			// if its a number (deed value)
			if (isdigit(line[j]))
			{
				char temp[100];
				int k = 0;
				do
				{
					temp[k] = line[j];
					temp[k + 1] = '\0';
					k++;
					j++;
				} while (isdigit(line[j]));
				j--;
				deedArray[xthCharacter + (x_dim * i)] = atoi(temp);
				mazeArray[xthCharacter + (x_dim * i)] = ' ';
				xthCharacter++;
			}
			// otherwise its a wall or a space
			else if (line[j] == '*' || line[j] == ' ')
			{
				mazeArray[xthCharacter + (x_dim * i)] = line[j];
				xthCharacter++;
			}
		}
	}
}

void findStart()
{
	int i;

	// checks the left and right columns to check for the entrance
	for (i = 0; i < y_dim; i++)
	{
		// check the left column
		if (mazeArray[i * x_dim] == ' ')
		{
			currentPos.x = 0, currentPos.y = i; // Good
		}
		// check the right column
		else if (mazeArray[(i * x_dim) + x_dim - 1] == ' ')
		{
			currentPos.x = x_dim - 1, currentPos.y = i; //
		}
	}

	// checks the top and bottom rows to check for the entrance
	for (i = 0; i < x_dim; i++)
	{
		// check the top row
		if (mazeArray[i] == ' ')
		{
			currentPos.x = i, currentPos.y = 0; // Good
		}
		// check the bottom row
		else if (mazeArray[((y_dim - 1) * x_dim) + i] == ' ')
		{
			currentPos.x = i, currentPos.y = y_dim - 1; // Good
		}
	}

	// printf("starting position = (%i, %i)\n", currentPos.x, currentPos.y);
}

void checkSurroundings()
{
	int left = CWL(), right = CWR(), up = CWF(), down = CWB();
	// If there are more than 1 possible moves, save this coordinate to come back to later
	if (left + up + right + down > 1)
	{
		fprintf(output, "Multiple paths detected\n");
		PUSH();
	}

	// Use the priority to decide the general direction to head towards, with some random variation to keep the path different run to run
	switch (priority)
	{
	// Default priority, finds which direction to head towards, which gives you the relative location, ie top/right/bottom/left of maze, so prioritize moving towards the center. If you are at top of maze, only move is to go down, so continue to prioritize going generally downward
	case 0:
	{ // default
		if (left)
		{
			priority = 1;
			MOVE_L();
		}
		else if (up)
		{
			priority = 2;
			MOVE_F();
		}
		else if (right)
		{
			priority = 3;
			MOVE_R();
		}
		else if (down)
		{
			priority = 4;
			MOVE_B();
		}
		else
		{
			MARK();
			finished();
		}
	}
	break;

	case 1:
	{ // generally ←
		bool moved = false;
		int rand = random(1, 4);
		switch (rand)
		{
		case 1:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_L\nMOVE_F\nMOVE_B\nMOVE_R\n) 10\nBACKTRACK");
			break;
		}
		case 2:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_L\nMOVE_B\nMOVE_F\nMOVE_R\n) 10\nBACKTRACK");
			break;
		}
		case 3:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_F\nMOVE_L\nMOVE_B\nMOVE_R\n) 10\nBACKTRACK");
			break;
		}
		case 4:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_B\nMOVE_L\nMOVE_F\nMOVE_R\n) 10\nBACKTRACK");
			break;
		}
		}
	}
	break;

	case 2:
	{ // generally ↑
		bool moved = false;
		int rand = random(1, 4);
		switch (rand)
		{
		case 1:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_F\nMOVE_L\nMOVE_R\nMOVE_B\n) 10\nBACKTRACK");
			break;
		}
		case 2:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_F\nMOVE_R\nMOVE_L\nMOVE_B\n) 10\nBACKTRACK");
			break;
		}
		case 3:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_L\nMOVE_F\nMOVE_R\nMOVE_B\n) 10\nBACKTRACK");
			break;
		}
		case 4:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_R\nMOVE_F\nMOVE_L\nMOVE_B\n) 10\nBACKTRACK");
			break;
		}
		}
	}
	break;

	case 3:
	{ // generally →
		bool moved = false;
		int rand = random(1, 4);
		switch (rand)
		{
		case 1:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_R\nMOVE_F\nMOVE_B\nMOVE_L\n) 10\nBACKTRACK");
			break;
		}
		case 2:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_R\nMOVE_B\nMOVE_F\nMOVE_L\n) 10\nBACKTRACK");
			break;
		}
		case 3:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_F\nMOVE_R\nMOVE_B\nMOVE_L\n) 10\nBACKTRACK");
			break;
		}
		case 4:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_B\nMOVE_R\nMOVE_F\nMOVE_L\n) 10\nBACKTRACK");
			break;
		}
		}
	}
	break;

	case 4:
	{ // generally ↓
		bool moved = false;
		int rand = random(1, 4);
		switch (rand)
		{
		case 1:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_B\nMOVE_R\nMOVE_L\nMOVE_F\n) 10\nBACKTRACK");
			break;
		}
		case 2:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_B\nMOVE_L\nMOVE_R\nMOVE_F\n) 10\nBACKTRACK");
			break;
		}
		case 3:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_R\nMOVE_B\nMOVE_L\nMOVE_F\n) 10\nBACKTRACK");
			break;
		}
		case 4:
		{
			fprintf(intelligence, "RP (\nCWL\nCWR\nCWB\nCWF\nMARK\nMOVE_L\nMOVE_B\nMOVE_R\nMOVE_F\n) 10\nBACKTRACK");
			break;
		}
		}
	}
	break;
	}
}

void logic()
{
	// While there are steps available, check surroundings and move
	while (steps < MAX_NUMBER_OF_STEPS)
	{
		// Close and reopen intelligence file to loop through it
		fclose(intelligence);
		intelligence = fopen("intelligence.txt", "r+");

		// Scan through it line by line, doing each command
		char line[8];
		while (fscanf(intelligence, "%s", line) != EOF)
		{

			if (strcmp(line, "RP") == 0)
			{
				char RParray[20][20];
				int r = 0;
				char *temp;
				while (true)
				{
					fscanf(intelligence, "%s", temp);
					if (isdigit(temp[0]))
					{
						RP(RParray, atoi(temp) - 1);
						break;
					}
					else
					{
						strcpy(RParray[r], temp);
						RParray[r + 1][0] = '\0';
						r++;
					}
				}
			}
			else if (strcmp(line, "PUSH") == 0)
			{
				PUSH();
			}
			else if (strcmp(line, "POP") == 0)
			{
				POP();
			}
			else if (strcmp(line, "CLEAR") == 0)
			{
				CLEAR();
			}
			else if (strcmp(line, "PEEK") == 0)
			{
				PEEK();
			}
			else if (strcmp(line, "CJPI") == 0)
			{
				CJPI();
			}
			else if (strcmp(line, "BJPI") == 0)
			{
				BJPI();
			}
			else if (strcmp(line, "BACKTRACK") == 0)
			{
				BACKTRACK();
			}
		}

		// break;
	}
	finished();
}

void reviewPriorities()
{
	int i;
	int m = stack2Position - 1;

	// First bubble sort stack2 by increasing point value
	bool sorted = false;
	deed temp;
	while (!sorted)
	{
		sorted = true;
		for (i = 0; i < m; i++)
		{
			if (stack2[i].value < stack2[i + 1].value)
			{
				temp = stack2[i];
				stack2[i] = stack2[i + 1];
				stack2[i + 1] = temp;
				sorted = false;
			}
		}
	}

	// If multiple are found to be true, then itll pick the one that is first in the order
	int _left = 0, _right = 0, _up = 0, _down = 0;

	for (i = 0; i < stack2Position - 1; i++)
	{
		// See if x continuously increases, if so then prioritize moving left
		if (stack2[i].location.x >= stack2[i + 1].location.x)
		{
			_left++;
		}
		// See if x continuously decreases, if so then prioritize moving right
		else if (stack2[i].location.x <= stack2[i + 1].location.x)
		{
			_right++;
		}
		// See if y continuously increases, if so then prioritize moving up
		else if (stack2[i].location.y <= stack2[i + 1].location.y)
		{
			_up++;
		}
		// See if y continuously decreases, if so then prioritize moving down
		else if (stack2[i].location.y >= stack2[i + 1].location.y)
		{
			_down++;
		}
	}

	if (_left == m)
	{
		fprintf(output, "Changed priority from %i to 1\n", priority);
		priority = 1;
		return;
	}
	else if (_right == m)
	{
		fprintf(output, "Changed priority from %i to 3\n", priority);
		priority = 3;
		return;
	}
	else if (_up == m)
	{
		fprintf(output, "Changed priority from %i to 2\n", priority);
		priority = 2;
		return;
	}
	else if (_down == m)
	{
		fprintf(output, "Changed priority from %i to 4\n", priority);
		priority = 4;
	}
}

void checkIfOnDeed()
{
	// Checks to see if you are on the same space as a deed
	if (deedArray[currentPos.x + (currentPos.y * x_dim)] > 0)
	{
		fprintf(output, "Found a Deed worth %i at (%i, %i)\n", deedArray[currentPos.x + (currentPos.y * x_dim)], currentPos.x, currentPos.y);
		// If so collect that point value
		deedTotal += deedArray[currentPos.x + (currentPos.y * x_dim)];
		// Add the location as well as deed value to stack2
		stack2[stack2Position].location = currentPos;
		stack2[stack2Position].value = deedArray[currentPos.x + (currentPos.y * x_dim)];
		// Remove the deed from the current spot (it has already been picked up, and cant be picked up multiple times)
		deedArray[currentPos.x + (currentPos.y * x_dim)] = 0;
		stack2Position++;
		// If you have collected at least two deeds, there is potential to find a pattern, so try to find a pattern for them
		if (stack2Position > 1)
		{
			// reviewPriorities();
		}
	}
}

int random(int min, int max)
{
	// returns a random value between int min and int max, inclusive
	if (srandCall == 0)
	{
		srand(time(NULL));
		srandCall = 1;
	}

	return ((rand() % max) + min);
}

void printMaze()
{
	int i;
	for (i = 0; i < x_dim * y_dim; i++)
	{
		if (i % (x_dim) == 0)
		{
			printf("\n");
		}
		if (pheromoneArray[i] == 1)
		{
			printf("|X|%i| ", deedArray[i]);
		}
		else
		{
			printf("|%c|%i| ", mazeArray[i], deedArray[i]);
		}
	}
}

void printDeed()
{
	int i, j;
	for (i = 0; i < y_dim; i++)
	{
		for (j = 0; j < x_dim; j++)
		{
			printf("%i ", deedArray[i + j * x_dim]);
		}
		printf("\n");
	}
	printf("\n\n");
}

void printPheromone()
{
	int i, j;
	for (i = 0; i < y_dim; i++)
	{
		for (j = 0; j < x_dim; j++)
		{
			printf("%i ", pheromoneArray[i + j * x_dim]);
		}
		printf("\n");
	}
	printf("\n\n");
}

void finished()
{
	fprintf(output, "\nEither the max number of steps has been reached or there are no more possible moves\n");
	fprintf(output, "Point total is %i", deedTotal);

	printMaze();
	printf("\n\n");
	printf("Point total is %i\n(Find the actions taken in this maze in the output.txt file)", deedTotal);
	exit(3);
}
