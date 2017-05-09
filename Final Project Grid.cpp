// Final Project Grid.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <assert.h>
#include <random>
#include <time.h>


using namespace std;



class state
{
public:
	int stateNum;
	bool wall;
	bool goal;

	void init();
};

void state::init()
{
	// originally initialize every state as a wall and not a goal
	stateNum = 0;
	wall = true;
	goal = false;
}

int main()
{
	srand(time(NULL));

	int xMax = 9;
	int yMax = 9;
	int xGoal = 1;
	int yGoal = 1;

	// make 10*10 grid 
	vector<state> Grid;

	for (int i = 0; i < xMax+1; i++)
	{
		for (int j = 0; j < yMax+1; j++)
		{
			state S;
			S.init();
			S.stateNum = i + j*(xMax + 1);
			Grid.push_back(S);
		}
	}

	for (int k = 0; k < (Grid.size() - 1); k++)
	{
		assert(Grid.at(k).stateNum != Grid.at(k + 1).stateNum); //all states should be unique
	}

	// Randomly pick half of states to not have walls
	for (int clearWallsIndex = 0; clearWallsIndex < Grid.size() / 2; clearWallsIndex++)
	{
		int clearState = rand() % Grid.size();
		//cout << clearState << endl;

		Grid.at(clearState).wall = false;
	}

	// Now identify which state has goal (call it the middle state)

	int goalState = (xMax/2) + (yMax/2)*(xMax + 1);
	Grid.at(goalState).goal = true;
	Grid.at(goalState).wall = false;

	// Now create optimal path to goal from lower left corner and upper right corner
	// Find state on lowest level halfway to being below goal state
	int firstRefState = (xMax / 4);

	// Find state halfway in the x and all the way in the y to goal state
	int secondRefState = (xMax / 4) + (yMax / 2)*(xMax + 1);

	int currentState = 0;

	while (currentState != firstRefState)
	{
		Grid.at(currentState).wall = false;
		currentState++;
	}

	while (currentState != secondRefState)
	{
		Grid.at(currentState).wall = false;
		currentState = currentState + xMax+1;
	}

	while (currentState != goalState)
	{
		Grid.at(currentState).wall = false;
		currentState++;
	}

	int thirdRefState = xMax * 3 / 4 + (yMax)*(xMax+1);
	int fourthRefState = (xMax * 3 / 4) + (yMax / 2)*(xMax + 1);

	currentState = xMax+yMax*(xMax + 1);

	while (currentState != thirdRefState)
	{
		Grid.at(currentState).wall = false;
		currentState--;
	}

	while (currentState != fourthRefState)
	{
		Grid.at(currentState).wall = false;
		currentState = currentState - (xMax + 1);
	}

	while (currentState != goalState)
	{
		Grid.at(currentState).wall = false;
		currentState--;
	}

	// Show representation of grid
	
	int numRows = yMax + 1;

	for (int i = numRows; i > 0; i--)
	{
		int jStart = (xMax + 1)*(yMax + 1) + (i - numRows - 1) - xMax*(numRows - i + 1);
		for (int j = jStart; j < jStart+xMax+1; j++)
		{
			if (Grid.at(j).wall == true)
			{
				cout << "-" << "\t";
			}

			else if (Grid.at(j).goal == true)
			{
				cout << "*" << "\t";
			}

			else
			{
				cout << Grid.at(j).stateNum << "\t";
			}
		}
		cout << endl;
	}

	system("pause");
    return 0;
}

