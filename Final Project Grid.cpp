// Final Project Grid.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <assert.h>
#include <random>
#include <time.h>
#include <fstream>


using namespace std;



class state
{
public:
	int stateNum;
	int reward;
	bool wall;
	bool goal;

	void init();
};

void state::init()
{
	// originally initialize every state as a wall and not a goal
	stateNum = 0;
	reward = 0;
	wall = true;
	goal = false;
}

vector<state> gridSetup(int xMax, int yMax, ofstream& fout);

int main()
{
	srand(time(NULL));
	ofstream  fout;

	fout.clear();
	fout.open("Grid.txt");

	int xMax = 19;
	int yMax = 19;

	// Create maze
	vector<state> Maze = gridSetup(xMax, yMax, fout);
	

	system("pause");
    return 0;
}

vector<state> gridSetup(int xMax, int yMax, ofstream& fout)
{
	// make 10*10 grid 
	vector<state> Grid;

	for (int i = 0; i < xMax + 1; i++)
	{
		for (int j = 0; j < yMax + 1; j++)
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
	for (int clearWallsIndex = 0; clearWallsIndex < Grid.size() *17/32; clearWallsIndex++)
	{
		int clearState = rand() % Grid.size();
		
		while (Grid.at(clearState).wall == false)
		{
			clearState = rand() % Grid.size();
		}

		Grid.at(clearState).wall = false;
	}

	// Now identify which state has goal (call it the middle state)

	int goalState = (xMax *4/5) + (yMax *4/5)*(xMax + 1);
	Grid.at(goalState).goal = true;
	Grid.at(goalState).wall = false;

	
	// Now create optimal path to goal from lower left corner and upper right corner
	// Find state on lowest level halfway to being below goal state
	int firstRefState = (xMax / 3);

	// Find state halfway in the x and all the way in the y to goal state
	int secondRefState = (xMax / 3) + (yMax *4/5)*(xMax + 1);

	int currentState = 0;

	while (currentState != firstRefState)
	{
		Grid.at(currentState).wall = false;
		currentState++;
	}

	while (currentState != secondRefState)
	{
		Grid.at(currentState).wall = false;
		currentState = currentState + xMax + 1;
	}

	while (currentState != goalState)
	{
		Grid.at(currentState).wall = false;
		currentState++;
	}

	int thirdRefState =  (yMax*4/5)*(xMax + 1);

	currentState = 0;

	while (currentState != thirdRefState)
	{
		Grid.at(currentState).wall = false;
		currentState = currentState+xMax+1;
	}

	while (currentState != goalState)
	{
		Grid.at(currentState).wall = false;
		currentState++;
	}

	int fourthRefState = (xMax * 4 / 5) ;

	currentState = 0;

	while (currentState != fourthRefState)
	{
		Grid.at(currentState).wall = false;
		currentState++;
	}

	while (currentState != goalState)
	{
		Grid.at(currentState).wall = false;
		currentState = currentState + xMax + 1;
	}
	
	// Show representation of grid

	int numRows = yMax + 1;

	for (int i = numRows; i > 0; i--)
	{
		int jStart = (xMax + 1)*(yMax + 1) + (i - numRows - 1) - xMax*(numRows - i + 1);

		cout << jStart << endl;
		for (int j = jStart; j < jStart + xMax + 1; j++)
		{
			if (Grid.at(j).wall == true)
			{
				Grid.at(j).reward = -1;
				fout << "-" << "\t";
			}

			else if (Grid.at(j).goal == true)
			{
				Grid.at(j).reward = 100;
				fout << "*" << "\t";
			}

			else
			{
				Grid.at(j).reward = -1;
				fout << Grid.at(j).stateNum << "\t";
			}
		}
		fout << endl;
	}

	return Grid;
}