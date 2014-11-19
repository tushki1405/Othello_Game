/*****************************************************************
--------Foundation of Artificial Intelligence
--------Homework 2
--------Submitted by : Tushar Gupta
******************************************************************/
#include <iostream>
#include <queue>
#include <stack>
#include <list>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

using namespace std;
//global variables
int heuristic[8][8] = {
	{99,  -8,   8,   6,   6,   8,  -8,  99},
	{-8, -24,  -4,  -3,  -3,  -4, -24,  -8},
	{ 8,  -4,   7,   4,   4,   7,  -4,   8},
	{ 6,  -3,   4,   0,   0,   4,  -3,   6},
	{ 6,  -3,   4,   0,   0,   4,  -3,   6},
	{ 8,  -4,   7,   4,   4,   7,  -4,   8},
	{-8, -24,  -4,  -3,  -3,  -4, -24,  -8},
	{99,  -8,   8,   6,   6,   8,  -8,  99}};
int gameState[8][8];    //* = 0 ; X = 1; O = 2;
int playerXO;
int cutOffDepth;
const int infinity=32767;
int noMove=0;
//Structure for every node of graph
struct node
{
	int nodeNumber;
	int parentNode;
	vector<int> childNodes;
	bool traversed;
	int depth;
	int state[8][8];
	int nextTurn;
	int cost;
	bool terminal;
	string mark;
	node() 
	{
		parentNode = -2;
		traversed = false;
		depth = -2;
		terminal=false;
	}
};
vector<node> nodes;
vector<node>::iterator nodeIt;
list<string> path;

//Priority queue sorting method
class orderLegalMoves
{
public:
	bool operator()(node& n1, node& n2)
	{
		if(n1.mark[1] > n2.mark[1]) return true;
		else if(n1.mark[1] == n2.mark[1] && n1.mark[0] > n2.mark[0]) return true;
		return false;
	}
};

//global functions
//function to compute legal moves
//void computeLegalMoves(int array[8][8], int xOrO);
void computeLegalMoves(node currentNode);
//Check legal moves in the specified direction
bool checkDirection(int i, int j, int array[8][8], int xOrO, int direction, int visitedNodeNumber);
//create node and add to nodes
void createAddNode(int array[8][8]);
//Method to copy one array to another
void copyArray(int arrayFrom[8][8], int arrayTo[8][8]);
//Method to evaluate cost of the state
int evaluateCost(node currentNode);
//Method to calculate the point which is marked
string Location(int row, int column);
//Method for max-value
int MaxValue(node currentNode);
//Method for min-value
int MinValue(node currentNode);
//Method for max-alphaBeta
int MaxValueAB(node currentNode, int alpha, int beta);
//Method for max-alphaBeta
int MinValueAB(node currentNode, int alpha, int beta);
//Method to replace string (infinity)
string replaceInfinity(string value);
//Method to check if location already visited
int visitedNode(string location, int parentNodeNumber);
//Method to convert int to string
string to_string_manual(int data);
//Method to check if only one type of coins are on the board
bool bothPlayersOnBoard(node currentNode);
//Method to check if board is full
bool isBoardFull(node currentNode);

int main() {
	//INITIALIZATIONS & DECLARATIONS
	bool logging=false;
	int inputFlag=1;
	int method=0;
	int ctr=0;
	string playerXOraw;
	string data;
	stringstream readWrite;
	string symbol;
	string::iterator stringIt;
	vector<int>::iterator intIt;

	ifstream inputFile ("input.txt");

	if(inputFile.is_open())
	{
		while(getline(inputFile, data))
		{
			//Reading initial information
			switch (inputFlag)
			{
			case 1:
				readWrite << data;
				readWrite >> method; 
				readWrite.clear();
				break;
			case 2: playerXOraw = data;
				if(playerXOraw == "*")
					playerXO=0;
				else if(playerXOraw == "X")
					playerXO=1;
				else if(playerXOraw == "O")
					playerXO=2;
				break;
			case 3: 
				readWrite << data;
				readWrite >> cutOffDepth; 
				readWrite.clear();
				break;
			default:
				break;
			}

			//Reading game state
			if(inputFlag > 3 && inputFlag <= 11)
			{
				for(stringIt=data.begin(), ctr=0; stringIt!=data.end(); stringIt++, ctr++)
				{
					if(*stringIt == '*')
					{
						gameState[inputFlag-4][ctr]=0;
					}
					else if(*stringIt == 'X')
					{
						gameState[inputFlag-4][ctr]=1;
					}
					else if(*stringIt == 'O')
					{
						gameState[inputFlag-4][ctr]=2;
					}
				}
			}
			inputFlag++;
		}
	}
	inputFile.close();

	//Adding the parent node
	createAddNode(gameState);

	//Greedy: Minimax with cut-off depth= 1
	//Minimax
	if(method==1 || method == 2)
	{
		string lastOutput="";
		string current;
		int nodeToPrint;
		bool noOutput=true;

		nodes[0].depth = 0;
		nodes[0].nextTurn = playerXO;
		nodes[0].mark = "root";
		if(method==1)
			cutOffDepth=1;
		MaxValue(nodes[0]);
		//Output state
		for(vector<int>::iterator intIt=nodes[0].childNodes.begin(); intIt!=nodes[0].childNodes.end();intIt++)
		{
			if(nodes[*intIt-1].cost == nodes[0].cost && nodes[0].cost != infinity && nodes[0].cost != -infinity)
			{
				nodeToPrint=nodes[*intIt-1].nodeNumber-1;
				noOutput=false;
				break;
			}
		}
		//handle no moves at start of game
		if(noOutput || path.size()==2)
		{
			nodeToPrint=0;
			path.clear();
			path.push_back(nodes[0].mark+","+to_string_manual(nodes[0].depth)+","+to_string_manual(nodes[0].cost));
		}

		ofstream outputfile("output.txt");
		for(int i=0; i<8; i++)
		{
			for(int j=0;j<8;j++)
			{
				if(nodes[nodeToPrint].state[i][j]==0)
					outputfile<<"*";
				else if(nodes[nodeToPrint].state[i][j]==1)
					outputfile<<"X";
				else if(nodes[nodeToPrint].state[i][j]==2)
					outputfile<<"O";
			}
			outputfile<<endl;
		}
		if(method==2)
		{
			//Output tree
			outputfile<<"Node,Depth,Value"<<endl;
			for(list<string>::iterator it=path.begin(); it!=path.end(); it++)
			{
				current=*it;
				if(current.substr(0,4) != "root" && current.substr(0,4)!= "pass" && current.substr(0,4) != lastOutput.substr(0,4))
				{
					lastOutput=*it;
					outputfile<<replaceInfinity(current)<<endl;
				}
				else if((current.substr(0,4) == "root" || current.substr(0,4) == "pass") && current.substr(0,6) != lastOutput.substr(0,6))
				{
					lastOutput=*it;
					outputfile<<replaceInfinity(current)<<endl;
				}
			}
		}
		outputfile.close();
	}
	//Alpha beta pruning
	else if(method == 3)
	{
		string lastOutput="";
		string current;
		int nodeToPrint;
		bool noOutput=true;

		nodes[0].depth = 0;
		nodes[0].nextTurn = playerXO;
		nodes[0].mark = "root";
		if(method==1)
			cutOffDepth=1;
		MaxValueAB(nodes[0],-infinity,infinity);
		//Output state
		for(vector<int>::iterator intIt=nodes[0].childNodes.begin(); intIt!=nodes[0].childNodes.end();intIt++)
		{
			if(nodes[*intIt-1].cost == nodes[0].cost && nodes[0].cost != infinity && nodes[0].cost != -infinity)
			{
				nodeToPrint=nodes[*intIt-1].nodeNumber-1;
				noOutput=false;
				break;
			}
		}
		//handle no moves at start of game
		if(noOutput || path.size()==2)
		{
			nodeToPrint=0;
			path.clear();
			path.push_back(nodes[0].mark+","+to_string_manual(nodes[0].depth)+","+to_string_manual(nodes[0].cost)+","+to_string_manual(-infinity)+","+to_string_manual(infinity));
		}

		ofstream outputfile("output.txt");
		for(int i=0; i<8; i++)
		{
			for(int j=0;j<8;j++)
			{
				if(nodes[nodeToPrint].state[i][j]==0)
					outputfile<<"*";
				else if(nodes[nodeToPrint].state[i][j]==1)
					outputfile<<"X";
				else if(nodes[nodeToPrint].state[i][j]==2)
					outputfile<<"O";
			}
			outputfile<<endl;
		}

		//Output tree
		outputfile<<"Node,Depth,Value,Alpha,Beta"<<endl;
		for(list<string>::iterator it=path.begin(); it!=path.end(); it++)
		{
			current=*it;
			if(current.substr(0,4) != "root" && current.substr(0,4)!= "pass" && current.substr(0,4) != lastOutput.substr(0,4))
			{
				lastOutput=*it;
				outputfile<<replaceInfinity(current)<<endl;
			}
			else if((current.substr(0,4) == "root" || current.substr(0,4) == "pass") && current.substr(0,6) != lastOutput.substr(0,6))
			{
				lastOutput=*it;
				outputfile<<replaceInfinity(current)<<endl;
			}
		}
		outputfile.close();
	}
	return 0;
}

//Method to replace string (infinity)
string replaceInfinity(string value)
{
	string current=value;
	string val=to_string_manual(infinity);
	if(current.find(val)!= string::npos)
	{
		while(current.find(val)!= string::npos)
		{
			current.replace(current.find(val), 5, "Infinity");
		}
	}
	return current;
}

//Method to compute max-value
int MaxValue(node currentNode)
{
	//Initializations
	int currentCost;
	int nextCost;
	vector<int> loop;
	//Return if cut off
	if(currentNode.depth == cutOffDepth || !bothPlayersOnBoard(currentNode) || noMove == 2 || isBoardFull(currentNode))
	{
		int thisCost = evaluateCost(currentNode);
		path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(thisCost));
		noMove=0;
		return thisCost;
	}
	currentCost=-infinity;
	//Assign cost and display
	nodes[currentNode.nodeNumber-1].cost = currentCost;
	//Compute legal moves and add
	computeLegalMoves(currentNode);
	//If no legal moves then pass with values
	if(nodes[currentNode.nodeNumber-1].childNodes.size() == 0)
	{
		
			noMove++;
			path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost));
			nodes.push_back(currentNode);
			nodes[nodes.size()-1].nodeNumber = nodes.size();
			nodes[nodes.size()-1].parentNode = currentNode.nodeNumber;
			nodes[nodes.size()-1].depth = currentNode.depth+1;
			if(currentNode.nextTurn == 1)
				nodes[nodes.size()-1].nextTurn = 2;
			else
				nodes[nodes.size()-1].nextTurn = 1;
			nodes[nodes.size()-1].mark = "pass";
			nodes[currentNode.nodeNumber-1].childNodes.push_back(nodes[nodes.size()-1].nodeNumber);
			currentCost=MinValue(nodes[nodes.size()-1]);
			nodes[currentNode.nodeNumber-1].cost=currentCost;
			path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost));
	}
	else
	{
		noMove=0;
		path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost));
		//Create another vector because original changes when nodes are added
		for(vector<int>::iterator intIt=nodes[currentNode.nodeNumber-1].childNodes.begin(); intIt!=nodes[currentNode.nodeNumber-1].childNodes.end();intIt++)
			loop.push_back(*intIt-1);
		//loop through
		for(vector<int>::iterator intIt=loop.begin(); intIt!=loop.end();intIt++)
		{
			nextCost=MinValue(nodes[*intIt]);
			if(nextCost > currentCost)
			{
				currentCost=nextCost;
				nodes[nodes[*intIt].parentNode-1].cost=currentCost;
			}
			path.push_back(nodes[nodes[*intIt].parentNode-1].mark+","+to_string_manual(nodes[nodes[*intIt].parentNode-1].depth)+","+to_string_manual(nodes[nodes[*intIt].parentNode-1].cost));
		}
	}
	//Assign
	nodes[currentNode.nodeNumber-1].cost=currentCost;
	return currentCost;
}

//Method to compute min-value
int MinValue(node currentNode)
{
	//Initializations
	int currentCost;
	int nextCost;
	vector<int> loop;
	//Return if cut off
	if(currentNode.depth == cutOffDepth || !bothPlayersOnBoard(currentNode) || noMove == 2 || isBoardFull(currentNode))
	{
		int thisCost = evaluateCost(currentNode);
		path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(thisCost));
		noMove=0;
		return thisCost;
	}
	currentCost=infinity;
	//Assign cost and display
	nodes[currentNode.nodeNumber-1].cost = currentCost;
	//Compute legal moves and add
	computeLegalMoves(currentNode);
	//If no legal moves then pass with values
	if(nodes[currentNode.nodeNumber-1].childNodes.size() == 0)
	{
			noMove++;
			path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost));
			nodes.push_back(currentNode);
			nodes[nodes.size()-1].nodeNumber = nodes.size();
			nodes[nodes.size()-1].parentNode = currentNode.nodeNumber;
			nodes[nodes.size()-1].depth = currentNode.depth+1;
			if(currentNode.nextTurn == 1)
				nodes[nodes.size()-1].nextTurn = 2;
			else
				nodes[nodes.size()-1].nextTurn = 1;
			nodes[nodes.size()-1].mark = "pass";
			nodes[currentNode.nodeNumber-1].childNodes.push_back(nodes[nodes.size()-1].nodeNumber);
			currentCost=MaxValue(nodes[nodes.size()-1]);
			nodes[currentNode.nodeNumber-1].cost=currentCost;
			path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost));
	}
	else
	{
		noMove=0;
		path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost));
		//Create another vector because original changes when nodes are added
		for(vector<int>::iterator intIt=nodes[currentNode.nodeNumber-1].childNodes.begin(); intIt!=nodes[currentNode.nodeNumber-1].childNodes.end();intIt++)
			loop.push_back(*intIt-1);
		//loop through
		for(vector<int>::iterator intIt=loop.begin(); intIt!=loop.end();intIt++)
		{
			nextCost=MaxValue(nodes[*intIt]);
			if(nextCost < currentCost)
			{
				currentCost=nextCost;
				nodes[nodes[*intIt].parentNode-1].cost=currentCost;
			}
			path.push_back(nodes[nodes[*intIt].parentNode-1].mark+","+to_string_manual(nodes[nodes[*intIt].parentNode-1].depth)+","+to_string_manual(nodes[nodes[*intIt].parentNode-1].cost));
		}
	}
	//Assign
	nodes[currentNode.nodeNumber-1].cost=currentCost;
	return currentCost;
}

//Method for max-alphaBeta
int MaxValueAB(node currentNode, int alpha, int beta)
{
	//Initializations
	int currentCost;
	int nextCost;
	vector<int> loop;
	//Return if cut off
	if(currentNode.depth == cutOffDepth || !bothPlayersOnBoard(currentNode) || noMove == 2 || isBoardFull(currentNode))
	{
		int thisCost = evaluateCost(currentNode);
		path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(thisCost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
		noMove=0;
		return thisCost;
	}
	currentCost=-infinity;
	//Assign cost and display
	nodes[currentNode.nodeNumber-1].cost = currentCost;
	//Compute legal moves and add
	computeLegalMoves(currentNode);
	//If no legal moves then pass with values
	if(nodes[currentNode.nodeNumber-1].childNodes.size() == 0)
	{
		noMove++;
		nodes.push_back(currentNode);
		nodes[nodes.size()-1].nodeNumber = nodes.size();
		nodes[nodes.size()-1].parentNode = currentNode.nodeNumber;
		nodes[nodes.size()-1].depth = currentNode.depth+1;
		if(currentNode.nextTurn == 1)
			nodes[nodes.size()-1].nextTurn = 2;
		else
			nodes[nodes.size()-1].nextTurn = 1;
		nodes[nodes.size()-1].mark = "pass";
		nodes[currentNode.nodeNumber-1].childNodes.push_back(nodes[nodes.size()-1].nodeNumber);
		path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
		currentCost=MinValueAB(nodes[nodes.size()-1],alpha,beta);
		nodes[currentNode.nodeNumber-1].cost=currentCost;
		if(currentCost>=beta)
		{
			path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
			return currentCost;
		}
		if(alpha<currentCost)
			alpha=currentCost;
		path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
	}
	else
	{
		noMove=0;
		path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
		//Create another vector because original changes when nodes are added
		for(vector<int>::iterator intIt=nodes[currentNode.nodeNumber-1].childNodes.begin(); intIt!=nodes[currentNode.nodeNumber-1].childNodes.end();intIt++)
			loop.push_back(*intIt-1);
		//loop through
		for(vector<int>::iterator intIt=loop.begin(); intIt!=loop.end();intIt++)
		{
			nextCost=MinValueAB(nodes[*intIt],alpha,beta);
			if(nextCost > currentCost)
			{
				currentCost=nextCost;
				nodes[nodes[*intIt].parentNode-1].cost=currentCost;
			}
			if(currentCost>=beta)
			{
				path.push_back(nodes[nodes[*intIt].parentNode-1].mark+","+to_string_manual(nodes[nodes[*intIt].parentNode-1].depth)+","+to_string_manual(nodes[nodes[*intIt].parentNode-1].cost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
				return currentCost;
			}
			if(alpha<currentCost)
				alpha=currentCost;
			path.push_back(nodes[nodes[*intIt].parentNode-1].mark+","+to_string_manual(nodes[nodes[*intIt].parentNode-1].depth)+","+to_string_manual(nodes[nodes[*intIt].parentNode-1].cost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
		}
	}
	//Assign
	nodes[currentNode.nodeNumber-1].cost=currentCost;
	return currentCost;
}

//Method for min-alphaBeta
int MinValueAB(node currentNode, int alpha, int beta)
{
	//Initializations
	int currentCost;
	int nextCost;
	vector<int> loop;
	//Return if cut off
	if(currentNode.depth == cutOffDepth || !bothPlayersOnBoard(currentNode) || noMove == 2 || isBoardFull(currentNode))
	{
		int thisCost = evaluateCost(currentNode);
		path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(thisCost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
		noMove=0;
		return thisCost;
	}
	currentCost=infinity;
	//Assign cost and display
	nodes[currentNode.nodeNumber-1].cost = currentCost;
	//Compute legal moves and add
	computeLegalMoves(currentNode);
	//If no legal moves then pass with values
	if(nodes[currentNode.nodeNumber-1].childNodes.size() == 0)
	{
		noMove++;
		nodes.push_back(currentNode);
		nodes[nodes.size()-1].nodeNumber = nodes.size();
		nodes[nodes.size()-1].parentNode = currentNode.nodeNumber;
		nodes[nodes.size()-1].depth = currentNode.depth+1;
		if(currentNode.nextTurn == 1)
			nodes[nodes.size()-1].nextTurn = 2;
		else
			nodes[nodes.size()-1].nextTurn = 1;
		nodes[nodes.size()-1].mark = "pass";
		nodes[currentNode.nodeNumber-1].childNodes.push_back(nodes[nodes.size()-1].nodeNumber);
		path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
		currentCost=MaxValueAB(nodes[nodes.size()-1],alpha,beta);
		nodes[currentNode.nodeNumber-1].cost=currentCost; 
		if(currentCost<=alpha)
		{
			path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
			return currentCost;
		}
		if(beta>currentCost)
			beta=currentCost;
		path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
	}
	else
	{
		noMove=0;
		path.push_back(nodes[currentNode.nodeNumber-1].mark+","+to_string_manual(nodes[currentNode.nodeNumber-1].depth)+","+to_string_manual(nodes[currentNode.nodeNumber-1].cost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
		//Create another vector because original changes when nodes are added
		for(vector<int>::iterator intIt=nodes[currentNode.nodeNumber-1].childNodes.begin(); intIt!=nodes[currentNode.nodeNumber-1].childNodes.end();intIt++)
			loop.push_back(*intIt-1);
		//loop through
		for(vector<int>::iterator intIt=loop.begin(); intIt!=loop.end();intIt++)
		{
			nextCost=MaxValueAB(nodes[*intIt],alpha,beta);
			if(nextCost < currentCost)
			{
				currentCost=nextCost;
				nodes[nodes[*intIt].parentNode-1].cost=currentCost;
			}
			if(currentCost<=alpha)
			{
				path.push_back(nodes[nodes[*intIt].parentNode-1].mark+","+to_string_manual(nodes[nodes[*intIt].parentNode-1].depth)+","+to_string_manual(nodes[nodes[*intIt].parentNode-1].cost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
				return currentCost;
			}
			if(beta>currentCost)
				beta=currentCost;
			path.push_back(nodes[nodes[*intIt].parentNode-1].mark+","+to_string_manual(nodes[nodes[*intIt].parentNode-1].depth)+","+to_string_manual(nodes[nodes[*intIt].parentNode-1].cost)+","+to_string_manual(alpha)+","+to_string_manual(beta));
		}
	}
	//Assign
	nodes[currentNode.nodeNumber-1].cost=currentCost;
	return currentCost;
}

//Method to calculate cost
int evaluateCost(node currentNode)
{
	int cost=0;
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<8;j++)
		{
			if(currentNode.state[i][j]!=0 && currentNode.state[i][j]==playerXO)
				cost+=heuristic[i][j];
			else if(currentNode.state[i][j]!=0 && currentNode.state[i][j]!=playerXO)
				cost-=heuristic[i][j];
		}
	}
	nodes[currentNode.nodeNumber-1].cost = cost;
	return cost;
}

//Method to copy array
void copyArray(int arrayFrom[8][8], int arrayTo[8][8])
{
	for(int i=0;i<8;i++)
		for(int j=0;j<8;j++)
			arrayTo[i][j] = arrayFrom[i][j];
}

//Method to create and add new nodes.
void createAddNode(int array[8][8])
{
	//Create a new node and initialize
	node newNode = node();
	for(int i=0;i<8;i++)
		for(int j=0;j<8;j++)
			newNode.state[i][j] = array[i][j];

	//add to the node list and assign values to variables
	nodes.push_back(newNode);
	nodes[nodes.size()-1].nodeNumber=nodes.size();
}

//Method to check which location is marked
string Location(int row, int column)
{
	string value;
	switch(column)
	{
	case 1: value="a"; break;
	case 2: value="b"; break;
	case 3: value="c"; break;
	case 4: value="d"; break;
	case 5: value="e"; break;
	case 6: value="f"; break;
	case 7: value="g"; break;
	case 8: value="h"; break;
	}
	value += to_string_manual(row);
	return value;
}

//Method to compute legal moves given a state in array form
void computeLegalMoves(node currentNode)
{
	int array[8][8];
	int xOrO;
	int nextTurn;
	priority_queue<node, vector<node>, orderLegalMoves> orderQueue;;

	copyArray(currentNode.state,array);
	xOrO=currentNode.nextTurn;
	if(xOrO == 1)
		nextTurn=2;
	else if(xOrO == 2)
		nextTurn=1;

	for(int i=0; i<8; i++)
	{
		for(int j=0;j<8;j++)
		{
			if(array[i][j]!=0 && array[i][j]!=xOrO)
			{
				//Spot 1
				if(i-1>=0 && j-1>=0 && array[i-1][j-1]==0 && checkDirection(i,j, array, xOrO, 1, visitedNode(Location(i,j),currentNode.nodeNumber)))
				{
					nodes[nodes.size()-1].parentNode = currentNode.nodeNumber;
					nodes[nodes.size()-1].depth = currentNode.depth+1;
					nodes[nodes.size()-1].nextTurn = nextTurn;
					nodes[nodes.size()-1].mark = Location(i,j);
					orderQueue.push(nodes[nodes.size()-1]);
				}
				copyArray(currentNode.state,array);
				//Spot 2
				if(i-1>=0 && array[i-1][j]==0 && checkDirection(i,j, array, xOrO, 2, visitedNode(Location(i,j+1),currentNode.nodeNumber)))
				{
					nodes[nodes.size()-1].parentNode = currentNode.nodeNumber;
					nodes[nodes.size()-1].depth = currentNode.depth+1;
					nodes[nodes.size()-1].nextTurn = nextTurn;
					nodes[nodes.size()-1].mark = Location(i,j+1);
					orderQueue.push(nodes[nodes.size()-1]);
				}
				copyArray(currentNode.state,array);
				//Spot 3
				if(i-1>=0 && j+1<8 && array[i-1][j+1]==0 && checkDirection(i,j, array, xOrO, 3, visitedNode(Location(i,j+2),currentNode.nodeNumber)))
				{
					nodes[nodes.size()-1].parentNode = currentNode.nodeNumber;
					nodes[nodes.size()-1].depth = currentNode.depth+1;
					nodes[nodes.size()-1].nextTurn = nextTurn;
					nodes[nodes.size()-1].mark = Location(i,j+2);
					orderQueue.push(nodes[nodes.size()-1]);
				}
				copyArray(currentNode.state,array);
				//Spot 4
				if(j-1>=0 && array[i][j-1]==0 && checkDirection(i,j, array, xOrO, 4, visitedNode(Location(i+1,j),currentNode.nodeNumber)))
				{
					nodes[nodes.size()-1].parentNode = currentNode.nodeNumber;
					nodes[nodes.size()-1].depth = currentNode.depth+1;
					nodes[nodes.size()-1].nextTurn = nextTurn;
					nodes[nodes.size()-1].mark = Location(i+1,j);
					orderQueue.push(nodes[nodes.size()-1]);
				}
				copyArray(currentNode.state,array);
				//Spot 5
				if(j+1<8 && array[i][j+1]==0 && checkDirection(i,j, array, xOrO, 5, visitedNode(Location(i+1,j+2),currentNode.nodeNumber)))
				{
					nodes[nodes.size()-1].parentNode = currentNode.nodeNumber;
					nodes[nodes.size()-1].depth = currentNode.depth+1;
					nodes[nodes.size()-1].nextTurn = nextTurn;
					nodes[nodes.size()-1].mark = Location(i+1,j+2);
					orderQueue.push(nodes[nodes.size()-1]);
				}
				copyArray(currentNode.state,array);
				//Spot 6
				if(i+1<8 && j-1>=0 && array[i+1][j-1]==0 && checkDirection(i,j, array, xOrO, 6, visitedNode(Location(i+2,j),currentNode.nodeNumber)))
				{
					nodes[nodes.size()-1].parentNode = currentNode.nodeNumber;
					nodes[nodes.size()-1].depth = currentNode.depth+1;
					nodes[nodes.size()-1].nextTurn = nextTurn;
					nodes[nodes.size()-1].mark = Location(i+2,j);
					orderQueue.push(nodes[nodes.size()-1]);
				}
				copyArray(currentNode.state,array);
				//Spot 7
				if(i+1<8 && array[i+1][j]==0 && checkDirection(i,j, array, xOrO, 7, visitedNode(Location(i+2,j+1),currentNode.nodeNumber)))
				{
					nodes[nodes.size()-1].parentNode = currentNode.nodeNumber;
					nodes[nodes.size()-1].depth = currentNode.depth+1;
					nodes[nodes.size()-1].nextTurn = nextTurn;
					nodes[nodes.size()-1].mark = Location(i+2,j+1);
					orderQueue.push(nodes[nodes.size()-1]);
				}
				copyArray(currentNode.state,array);
				//Spot 8
				if(i+1<8 && j+1<8 && array[i+1][j+1]==0 && checkDirection(i,j, array, xOrO, 8, visitedNode(Location(i+2,j+2),currentNode.nodeNumber)))
				{
					nodes[nodes.size()-1].parentNode = currentNode.nodeNumber;
					nodes[nodes.size()-1].depth = currentNode.depth+1;
					nodes[nodes.size()-1].nextTurn = nextTurn;
					nodes[nodes.size()-1].mark = Location(i+2,j+2);
					orderQueue.push(nodes[nodes.size()-1]);
				}
				copyArray(currentNode.state,array);
			}
		}
	}
	//extract from priority queue and add to child nodes of root
	while(!orderQueue.empty())
	{
		nodes[currentNode.nodeNumber-1].childNodes.push_back(orderQueue.top().nodeNumber);
		orderQueue.pop();
	}
}

//Method to check if location already visited
int visitedNode(string location, int parentNodeNumber)
{
	for(int k=0; k<nodes.size();k++)
	{
		if(nodes[k].mark == location && nodes[k].parentNode==parentNodeNumber)
			return nodes[k].nodeNumber-1;
	}
	return 0;
}

//Method working as extension for computeLegalMoves
/*----------1 2 3-------------- 
------------4 N 5-------------- 
------------6 7 8--------------
*/
bool checkDirection(int i, int j, int array[8][8], int xOrO, int direction, int visitedNodeNumber)
{
	int m,n;
	//which direction to check
	if(direction == 1)
	{
		//if going out of bounds or if same symbol not present at the end of direction
		if((j+1>=8 || i+1>=8) || array[i+1][j+1]==0)
			return false;
		//if legal move
		if(array[i+1][j+1]==xOrO)
		{
			m=i;n=j;
			//flip all tiles in between
			while(array[i][j]!=0 && array[i][j]!=xOrO)
			{
				array[i][j]=xOrO;
				if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; }
				i--; j--;
			}
			array[i][j]=xOrO;
			if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; return false;}
			else { createAddNode(array); }
			return true;
		}
		return checkDirection(i+1,j+1,array,xOrO,1,visitedNodeNumber);
	}
	else if(direction == 2)
	{
		if(i+1>=8 || array[i+1][j]==0)
			return false;
		if(array[i+1][j]==xOrO)
		{
			m=i;n=j;
			while(array[i][j]!=0 && array[i][j]!=xOrO)
			{
				array[i][j]=xOrO;
				if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; }
				i--;
			}
			array[i][j]=xOrO;
			if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; return false;}
			else { createAddNode(array); }
			return true;
		}
		return checkDirection(i+1,j,array,xOrO,2,visitedNodeNumber);
	}
	else if(direction == 3)
	{   
		if((i+1>=8 || j-1<0) || array[i+1][j-1]==0)
			return false;
		if(array[i+1][j-1]==xOrO)
		{
			m=i;n=j;
			while(array[i][j]!=0 && array[i][j]!=xOrO)
			{
				array[i][j]=xOrO;
				if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; }
				i--;j++;
			}
			array[i][j]=xOrO;
			if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; return false;}
			else { createAddNode(array); }
			return true;
		}
		return checkDirection(i+1,j-1,array,xOrO,3,visitedNodeNumber);
	}
	else if(direction == 4)
	{
		if(j+1>=8 || array[i][j+1]==0)
			return false;
		if(array[i][j+1]==xOrO)
		{
			m=i;n=j;
			while(array[i][j]!=0 && array[i][j]!=xOrO)
			{
				array[i][j]=xOrO;
				if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; }
				j--;
			}
			array[i][j]=xOrO;
			if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; return false;}
			else { createAddNode(array); }
			return true;
		}
		return checkDirection(i,j+1,array,xOrO,4,visitedNodeNumber);
	}
	else if(direction == 5)
	{
		if(j-1<0 || array[i][j-1]==0)
			return false;
		if(array[i][j-1]==xOrO)
		{
			m=i;n=j;
			while(array[i][j]!=0 && array[i][j]!=xOrO)
			{
				array[i][j]=xOrO;
				if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; }
				j++;
			}
			array[i][j]=xOrO;
			if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; return false;}
			else { createAddNode(array); }
			return true;
		}
		return checkDirection(i,j-1,array,xOrO,5,visitedNodeNumber);
	}
	else if(direction == 6)
	{
		if((i-1<0 || j+1>=8) || array[i-1][j+1]==0)
			return false;
		if(array[i-1][j+1]==xOrO)
		{
			m=i;n=j;
			while(array[i][j]!=0 && array[i][j]!=xOrO)
			{
				array[i][j]=xOrO;
				if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; }
				i++;j--;
			}
			array[i][j]=xOrO;
			if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; return false;}
			else { createAddNode(array); }
			return true;
		}
		return checkDirection(i-1,j+1,array,xOrO,6,visitedNodeNumber);
	}
	else if(direction == 7)
	{
		if(i-1<0 || array[i-1][j]==0)
			return false;
		if(array[i-1][j]==xOrO)
		{
			m=i;n=j;
			while(array[i][j]!=0 && array[i][j]!=xOrO)
			{
				array[i][j]=xOrO;
				if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; }
				i++;
			}
			array[i][j]=xOrO;
			if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; return false;}
			else { createAddNode(array); }
			return true;
		}
		return checkDirection(i-1,j,array,xOrO,7,visitedNodeNumber);
	}
	else if(direction == 8)
	{
		if((j-1<0 || i-1<0) || array[i-1][j-1]==0)
			return false;
		if(array[i-1][j-1]==xOrO)
		{
			m=i;n=j;
			while(array[i][j]!=0 && array[i][j]!=xOrO)
			{
				array[i][j]=xOrO;
				if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; }
				i++;j++;
			}
			array[i][j]=xOrO;
			if(visitedNodeNumber>0) { nodes[visitedNodeNumber].state[i][j]=xOrO; return false;}
			else { createAddNode(array); }
			return true;
		}
		return checkDirection(i-1,j-1,array,xOrO,8,visitedNodeNumber);
	}
	return false;
}

//Method to convert int to string
string to_string_manual(int data){
	stringstream s;
	s<<data;
	return s.str();
}

//Method to check if only one type of coins are on the board
bool bothPlayersOnBoard(node currentNode)
{
	bool playerX=false;
	bool playerY=false;
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<8;j++)
		{
			if(currentNode.state[i][j]!=0 && currentNode.state[i][j]==playerXO && !playerX)
			{
				playerX=true;
				if(playerX && playerY) return true;
			}
			else if(currentNode.state[i][j]!=0 && currentNode.state[i][j]!=playerXO && !playerY)
			{
				playerY=true;
				if(playerX && playerY) return true;
			}
		}
	}
	return false;
}

//Method to check if board is full
bool isBoardFull(node currentNode)
{
	bool emptyLocation=false;
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<8;j++)
		{
			if(currentNode.state[i][j] == 0)
				return false;
		}
	}
	return true;
}
