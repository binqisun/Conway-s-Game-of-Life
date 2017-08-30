//Binqi Sun
//Parallel Programming project
//Conway’s Game(Cellular Automata) of Life with OpenMP
//CSCI 6300 


#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <windows.h> 
#include <time.h>
#include <omp.h>
#include<fstream>

using namespace std;

//global variables
char live = '0';
char dead = ' ';
int iterations = 0;
// random generator
std::mt19937 randGen(time(NULL));
std::uniform_int_distribution<int> roll(1, 100);
int seedLevel = 75;

const int HEIGHT = 4;
const int WIDTH = 2;


//functins
int getSize();
void initGame(std::vector<std::string> &board, std::vector<std::string> &change);
void seedGame(std::vector<std::string> &change);
void syncBoard(std::vector<std::string> &board, std::vector<std::string> &change);
void printBoard(std::vector<std::string> board);
void updateBoard(std::vector<std::string> &board, std::vector<std::string> &change);
void switchChar(std::vector<std::string> &change, int i, int j, int count);



int main(void)
{

	std::vector<std::string> board;
	std::vector<std::string> change;

	int size = getSize();


	//initial game board
	initGame(board, change);

	//function that random generate live cells on the board
	seedGame(change);

	// cover the origial game board with board after iterations
	syncBoard(board, change);


	//print the initial game board
	printBoard(board);
	std::cout << "The Randomly generate Matrixes is: " << size << " x " << size << endl;
	std::cout << "Enter iterations: ";
	std::cin >> iterations;

	clock_t start = clock();
	//for the iterations

	for (int i = 0; i < iterations; i++)
	{
		updateBoard(board, change);

		//print every iteration game board
		//if not print out the game board, the time will reduce significantly
		printBoard(board);
		//Sleep(250);
	}

	std::cout << "The time is : " << ((std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000)) / 25 << " ms" << std::endl;

	system("PAUSE");



	return 0;
}



int getSize()
{
	int array1[HEIGHT][WIDTH];
	
	int size;

	int i;
	int j;

	//read the matrixes from a text file
	ifstream inputFile;
	inputFile.open("matrix.txt");


	for (i = 0; i < HEIGHT; i++)
	{
		for (j = 0; j < WIDTH; j++)
		{
			inputFile >> array1[i][j];
		}
	}
	
	inputFile.close();

	srand(time(0));
	//Randomly generate 100x100, 500x500, 1000x1000, and 2000x2000 Matrixes of populated cells.
#pragma omp parallel for
		for (i = 0; i < HEIGHT; i++)
		{
			for (j = 0; j < WIDTH; j++)
			{
				//generate  random index number
				int indexH = rand() % HEIGHT;
				int indexW = rand() % WIDTH;

				size = array1[i][j];
				array1[i][j] = array1[indexH][indexW];
				array1[indexH][indexW] = size;
			}
		}
	
	return size;
}

//initial game board
void initGame(std::vector<std::string> &board, std::vector<std::string> &change)
{

	int size = getSize();

	std::string line;

	// get size for grid

	for (int i = 0; i<size; i++)
	{
		line += " ";

	}

	for (int i = 0; i<size; i++)
	{
		board.push_back(line);
		change.push_back(line);

	}


}


//function that random generate live cells on the board
void seedGame(std::vector<std::string> &change)
{
#pragma omp parallel for
		for (int i = 0; i < change.size(); i++)
		{
			for (int j = 0; j<change[i].size(); j++)
			{
				//generate random number for cells in every empty space,if number is larger then 75 its live cell.
				if (roll(randGen)>seedLevel)
				{
					change[i][j] = live;
				}
			}
		}
	
}


// sync the origial game board with board after iterations
//once the cells got changed, they would cover the original cells
void syncBoard(std::vector<std::string> &board, std::vector<std::string> &change)
{
#pragma omp parallel for
		for (int i = 0; i < change.size(); i++)
		{
			for (int j = 0; j < change[i].size(); j++)
			{
				board[i][j] = change[i][j];
			}
		}
	
}

//print out the game
void printBoard(std::vector<std::string> board)
{


	printf("%s", std::string(10, '\n').c_str());

#pragma omp parallel for
		for (int i = 0; i < board.size(); i++)
		{
			for (int j = 0; j < board[i].size(); j++)
			{
				std::cout << board[i][j];
			}
			std::cout << std::endl;
		}
	
}



// apply the game rules for the game
void updateBoard(std::vector<std::string> &board, std::vector<std::string> &change)
{
	int count;
#pragma omp parallel for
		for (int i = 0; i < board.size(); i++)
		{
			for (int j = 0; j < board[i].size(); j++)
			{


				// count as the populated cell (live neighbours)
				count = 0;
				//top first row
				if (i == 0)
				{
					//most left column( top left corner)
					if (j == 0)
					{
						//to be right
						if (board[i][j + 1] == live){ count++; }
						//below it
						if (board[i + 1][j] == live){ count++; }
						//below it and to be right
						if (board[i + 1][j + 1] == live){ count++; }

					}
					//top row most right column (top right corner)
					else if ((j + 1) == board[i].size())
					{
						// to be left
						if (board[i][j - 1] == live){ count++; }
						//below it
						if (board[i + 1][j] == live){ count++; }
						//below it and to the left
						if (board[i + 1][j - 1] == live){ count++; }

					}
					//in the mid (top row except corners)
					else
					{
						//to the right
						if (board[i][j + 1] == live){ count++; }
						//to the left 
						if (board[i][j - 1] == live){ count++; }
						//below it
						if (board[i + 1][j] == live){ count++; }
						//below and to the right
						if (board[i + 1][j + 1] == live){ count++; }
						//below and to the left
						if (board[i + 1][j - 1] == live){ count++; }
					}

					switchChar(change, i, j, count);
				}


				//bottom row
				else if ((i + 1) == board.size())
				{
					//bottom left corner
					if (j == 0)
					{
						//to be right
						if (board[i][j + 1] == live){ count++; }
						//above it
						if (board[i - 1][j] == live){ count++; }
						//above it and to the right
						if (board[i - 1][j + 1] == live){ count++; }

					}
					//bottom right corner
					else if ((j + 1) == board[i].size())
					{
						// to be left
						if (board[i][j - 1] == live){ count++; }
						//above it
						if (board[i - 1][j] == live){ count++; }
						//above it and to the left
						if (board[i - 1][j - 1] == live){ count++; }

					}
					//bottom mid
					else
					{
						//to the right
						if (board[i][j + 1] == live){ count++; }
						//to the left 
						if (board[i][j - 1] == live){ count++; }
						//above it
						if (board[i - 1][j] == live){ count++; }
						//above and to the right
						if (board[i - 1][j + 1] == live){ count++; }
						//above and to the left
						if (board[i - 1][j - 1] == live){ count++; }

					}
					switchChar(change, i, j, count);

				}


				//inside the grid
				else
				{
					//first column
					if (j == 0)
					{
						//to the right
						if (board[i][j + 1] == live){ count++; }
						//above it
						if (board[i - 1][j] == live){ count++; }
						//above and to the right
						if (board[i - 1][j + 1] == live){ count++; }
						//below it
						if (board[i + 1][j] == live){ count++; }
						//below it and to be right
						if (board[i + 1][j + 1] == live){ count++; }


					}
					//last column
					else if ((j + 1) == board[i].size())
					{
						//to the left
						if (board[i][j - 1] == live){ count++; }
						//above it
						if (board[i - 1][j] == live){ count++; }
						//above and to the left
						if (board[i - 1][j - 1] == live){ count++; }
						//below it
						if (board[i + 1][j] == live){ count++; }
						//below it and to be left
						if (board[i + 1][j - 1] == live){ count++; }

					}

					else
					{
						//above and to the left
						if (board[i - 1][j - 1] == live){ count++; }
						//above it
						if (board[i - 1][j] == live){ count++; }
						//above and to the right
						if (board[i - 1][j + 1] == live){ count++; }
						//to the left
						if (board[i][j - 1] == live){ count++; }
						//to the right
						if (board[i][j + 1] == live){ count++; }
						//below it and to be left
						if (board[i + 1][j - 1] == live){ count++; }
						//below it
						if (board[i + 1][j] == live){ count++; }
						//below it and to be right
						if (board[i + 1][j + 1] == live){ count++; }

					}
					switchChar(change, i, j, count);

				}

			}

		}
	
	syncBoard(board, change);


}

//set game rules
void switchChar(std::vector<std::string> &change, int i, int j, int count)
{
	//A live cell with less than 2 neighbors or more than 3 neighbors becomes a dead cell.
	if (change[i][j] == live)
	{
		if ((count<2) || (count>3))
		{
			change[i][j] = dead;
		}

	}
	//a dead cell with exactly 3 neighbors becomes alive cell
	else
	{
		if (count == 3)
		{
			change[i][j] = live;
		}

	}

}
