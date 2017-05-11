// FinalProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <assert.h>
#include <random>
#include <time.h>
#include <fstream>
#include <algorithm>


using namespace std;

#define SmallRand (double)rand()/RAND_MAX*.01
#define Direction (double)rand()/RAND_MAX*3
#define RAND (double)rand()/RAND_MAX

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

class QTable {
public:

	double epsilon = .1; //Greedy variable
	double alpha = .1; //Learning varaible
	double gamma = .9;  //Q-Learning variable

	vector<double> actions;
	vector<vector<double>> StartQT;
	vector<vector<double>> QT;

	void init(vector<state> Grid) {

		for (int i = 0; i < Grid.size(); i++) {
			for (int j = 0; j < 4; j++) {
				double random = SmallRand;
				actions.push_back(random);
			}
			StartQT.push_back(actions);
			actions.clear();
		}
		QT = StartQT;
	}

	void updateQ(double action, vector<state> Grid, int NewestState, int State) {
		//Updates the old position of the agent with new Q data after moving one state 
		double maxQ = *max_element(QT[NewestState].begin(), QT[NewestState].end());
		QT[State][action] = QT[State][action] + alpha*(Grid.at(NewestState).reward + gamma*maxQ - QT[State][action]);
		State = NewestState;
	}

};

//Creating the agent class that decides how to move within the grid based on a goal
class agent {
public:
	int Startx;
	int Starty;
	int positionx;  //Setting the agent's x and y position
	int positiony;
	int StartingState;
	int State;
	int moves;

	int Episodecounter;

	double epsilon = .1; //Greedy variable
	double alpha = .1; //Learning varaible
	double gamma = .9;  //Q-Learning variable



	void init() {
		//initializing the agent's position
		positionx = -10;
		positiony = -10;
		State = -1;
		moves = 0;
		Episodecounter = 0;
	};

	void place(int x, int y, int xMax) {
		positionx = x;
		positiony = y;

		Startx = x;
		Starty = y;

		StartingState = positionx + positiony*(xMax+1);
		State = StartingState;
	};

	void bumpercheck(vector<state> Grid, int xMax, int yMax, int agentxpos, int agentypos, int Movetaken, QTable &QTable) {
		//Checks to see whether the agent's x and y coordinate position is not off the grid and if so
		//puts the agent back on the grid and gives the Q-Table a negative value of -100 for that state and direction movement
		if (agentxpos < 0) {
			QTable.QT[State][Movetaken] = -100;
			positionx = 0;
		}
		else if (agentxpos > xMax) {
			QTable.QT[State][Movetaken] = -100;
			positionx = xMax ;
		}
		else {
			positionx = agentxpos;
		}
		if (agentypos < 0) {
			QTable.QT[State][Movetaken] = -100;
			positiony = 0;
		}
		else if (agentypos > yMax) {
			QTable.QT[State][Movetaken] = -100;
			positiony = yMax;
		}
		else {
			positiony = agentypos;
		}

		int newState = positionx + positiony*(xMax + 1);

		int stateIndex = 0;
		while (Grid.at(stateIndex).stateNum != newState)
		{
			stateIndex++;
		}

		if (Grid.at(stateIndex).wall == true)
		{
			if (Movetaken == 0)
			{
				positiony--;
			}
			else if (Movetaken == 1)
			{
				positionx--;
			}
			else if (Movetaken == 2)
			{
				positiony++;
			}
			else if (Movetaken == 3)
			{
				positionx++;
			}
		}

	};

	void agent_moves(int xMax, int yMax, vector<state> Grid, QTable &QTable) {
		double Greed = RAND;
		int Move = 0;  //Initialize a random move up


		State = positionx + positiony*(xMax+1);

		if (Greed < epsilon) {
			//Random movement in one of the four directions
			Move = rand() % 4;
		}
		else {
			//Take the best action in the direction dictated by the Q-table
			double BestQ = *max_element(QTable.QT[State].begin(), QTable.QT[State].end());
			for (int i = 0; i <= 3; i++) {
				//cout << QTable[State][i] << endl;
				if (QTable.QT[State][i] == BestQ) {
					Move = i;
				}
			}

		}

		if (Move == 0) {
			//Move Up
			positiony++;
		}
		else if (Move == 1) {
			//Move Right
			positionx++;
		}
		else if (Move == 2) {
			//Move Down
			positiony--;
		}
		else {
			//Move Left
			positionx--;
		}

		//Adds one to the number of moves
		moves++;

		//Resets the agent if it hits a wall on the outer lines of the grid
		bumpercheck(Grid, xMax, yMax, positionx, positiony, Move, QTable);

		//Updates the agent's position in order to update the QTable of its move
		int NextState = positionx + positiony*(xMax+1);

		//Now the QTable is updated with what the agent just did
		QTable.updateQ(Move, Grid, NextState, State);

		State = NextState;
	};



	void Runlearner(vector<state> Matrix, state goalState, int max_x, int max_y, int Runs, int Episodes, ofstream &statout, int TotalEpisodes, QTable &QTable) {


		while (State != goalState.stateNum) {
			agent_moves(max_x, max_y, Matrix, QTable);
			cout << positionx << '\t' << positiony << '\t' << State << '\t' << goalState.stateNum << endl;
		}

		statout << Runs << '\t' << Episodes << '\t' << moves << endl;

		//Sets the agent back to its starting position
		positionx = Startx;
		positiony = Starty;
		State = StartingState;
		//Sets an Episode counter to determine the total moves of the agent on the last 10 steps
		Episodecounter++;
		
		//Sets the moves counter back to zero
		moves = 0;
	}

};



vector<state> gridSetup(int xMax, int yMax, ofstream& fout);

int main()
{
	srand(time(NULL));
	ofstream  fout;

	/// GRID PARAMETERS ///
	int xMax = 19;
	int yMax = 19;
	int Episodes = 250;

	// Create maze
	fout.clear();
	fout.open("Grid.txt");
	vector<state> Maze = gridSetup(xMax, yMax, fout);
	fout.close();

	/// Run Q-Learner with One Agent
	fout.clear();
	fout.open("LearningCurve.txt");
	
	fout << "Run:" << '\t' << "Episode:" << '\t' << "Moves:" << endl;
	
	// Initialize Agent
	agent Main;
	Main.init();
	int initialX = 0;
	int initialY = 0;
	Main.place(initialX, initialY, xMax);

	// Initialize Q-Table
	QTable Master;
	Master.init(Maze);

	cout << "Agent's State: " << Main.State << endl;
	int goalState = 0;
	while (Maze.at(goalState).goal == false)
	{
		goalState++;
	}
	cout << "Goal State: " << Maze.at(goalState).stateNum << endl;

	// Run N Generations 
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < Episodes; j++) {

			while (Main.State != goalState) {
				Main.agent_moves(xMax, yMax, Maze, Master);
				//cout << Main.positionx << '\t' << Main.positiony << '\t' << Main.State << '\t' << goalState << endl;
			}

			fout << i << '\t' << j << '\t' << Main.moves << endl;

			//Sets the agent back to its starting position
			Main.positionx = Main.Startx;
			Main.positiony = Main.Starty;
			Main.State = Main.StartingState;
			//Sets an Episode counter to determine the total moves of the agent on the last 10 steps
			Main.Episodecounter++;

			//Sets the moves counter back to zero
			Main.moves = 0;
		}
		//resets the parameters for Test E 
		Main.Episodecounter = 0;
		Master.QT = Master.StartQT;
	}
		
	fout.close();

	/// Run Q-Learner with Two Agents
	fout.clear();
	fout.open("LearningCurve2.txt");

	// Clear Q-Table
	Master.QT = Master.StartQT;

	// Initialize Two Agents
	agent Main2;
	Main2.init();
	Main2.place(initialX, initialY, xMax);

	agent Main3;
	Main3.init();
	Main3.place(initialX, initialY, xMax);

	// Run N Generations
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < Episodes; j++) {

			while (Main2.State != goalState || Main3.State != goalState) {
				Main2.agent_moves(xMax, yMax, Maze, Master);
				Main3.agent_moves(xMax, yMax, Maze, Master);
				//cout << Main.positionx << '\t' << Main.positiony << '\t' << Main.State << '\t' << goalState << endl;
			}

			fout << i << '\t' << j << '\t' << Main2.moves << "\t" << Main3.moves <<endl;

			//Sets the agent back to its starting position
			Main2.positionx = Main2.Startx;
			Main2.positiony = Main2.Starty;
			Main2.State = Main2.StartingState;

			Main3.positionx = Main3.Startx;
			Main3.positiony = Main3.Starty;
			Main3.State = Main3.StartingState;
			//Sets an Episode counter to determine the total moves of the agent on the last 10 steps
			Main2.Episodecounter++;
			Main3.Episodecounter++;

			//Sets the moves counter back to zero
			Main2.moves = 0;
			Main3.moves = 0;
		}
		//resets the parameters for Test E 
		Main2.Episodecounter = 0;
		Main3.Episodecounter = 0;
		Master.QT = Master.StartQT;
	}

	fout.close();

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
	for (int clearWallsIndex = 0; clearWallsIndex < Grid.size() * 17 / 32; clearWallsIndex++)
	{
		int clearState = rand() % Grid.size();

		while (Grid.at(clearState).wall == false)
		{
			clearState = rand() % Grid.size();
		}

		Grid.at(clearState).wall = false;
	}

	// Now identify which state has goal (call it the middle state)

	int goalState = (xMax * 4 / 5) + (yMax * 4 / 5)*(xMax + 1);
	Grid.at(goalState).goal = true;
	Grid.at(goalState).wall = false;


	// Now create optimal path to goal from lower left corner and upper right corner
	// Find state on lowest level halfway to being below goal state
	int firstRefState = (xMax / 3);

	// Find state halfway in the x and all the way in the y to goal state
	int secondRefState = (xMax / 3) + (yMax * 4 / 5)*(xMax + 1);

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

	int thirdRefState = (yMax * 4 / 5)*(xMax + 1);

	currentState = 0;

	while (currentState != thirdRefState)
	{
		Grid.at(currentState).wall = false;
		currentState = currentState + xMax + 1;
	}

	while (currentState != goalState)
	{
		Grid.at(currentState).wall = false;
		currentState++;
	}

	int fourthRefState = (xMax * 4 / 5);

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
				Grid.at(j).reward = -100;
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