#include <stdlib.h>
#include <iostream>
#include <vector>
#include <sys/time.h>

using namespace std;

struct Node
{
    string state;
    Node *path;
    int g;
    int h;
    int f;
};

//Astar
void aStar(string initialState, int heuristicFunction);

//Heuristic functions/Dependancies
int sumDistances(string state);
int sumMisplacedItems(string state);
int sumRowsCols(string state);
int sumSequence(string state);

bool checkGoal(string state);

//movement functions
bool canMove(int way, string state);
string move(int way, string state);

void displayState(string state);
void endState(Node *node, double runtime, unsigned long int nodes, unsigned long int nodesExpanded, unsigned long int bytes);



int main(int argc, char const *argv[])
{
    string initialPosition1 = "283164 75";
    string initialPosition2 = "2164 8753";
    
    char answer = '-';
    do{
        cout << "Select a Heuristic function" << endl;
        cout << "1: H1n, " << endl;
        cout << "2: H2n, " << endl;
        cout << "3: Sum incorrect Rows/Columns, " << endl;
        cout << "4: Sum incorrect Sequence, " << endl;
        cout << "X/x to quit" << endl;
        cout << endl;
        cout << "Selection: ";
        cin >> answer;
        
        if(answer == '1' || answer == '2' || answer == '3' || answer == '4'){
            const char *constChar = &answer;
            int answerAsInteger = atoi(constChar);
            cout << endl;
            cout << "Starting aStar with selected Heuristic Function:" << endl;
            cout << "Here is the initial position: " << endl;
            displayState(initialPosition1);
            
            cout << "Running a* on initial position." << endl;
            cout << endl << endl;
            aStar(initialPosition1, answerAsInteger);
            
            cout << endl << endl << endl;
            
            cout << "Here is the initial position: " << endl;
            displayState(initialPosition2);
            
            cout << "Running a* on initial position." << endl;
            cout << endl << endl;
            aStar(initialPosition2, answerAsInteger);
            
            cout << endl << endl;
            cout << "aStar sequence ended." << endl;
            cout << endl << endl;
        }
        else if(answer == 'x' || answer == 'X'){}
        else{
            cout << endl;
            cout << "Invalid Response. Try again." << endl;
            cout << endl;
        }
    }while(answer != 'X' && answer != 'x');
    
    
    cout << endl << endl;
    cout << "This implementation of astar to solve 8 puzzle was developed by: " << endl;
    cout << "Blake Anderson" << endl;
    
    return 0;
}


void aStar(string initialState, int heuristicFunction)
{
    unsigned long int nodeCount = 0;
    unsigned long int nodesExpanded = 0;
    unsigned long int bytes = 0;
    timeval start, end;
    gettimeofday(&start, NULL);
    
    int (*functionPtr)(string);
    switch (heuristicFunction) {
        case 1:
            functionPtr = sumMisplacedItems;
            break;
        case 2:
            functionPtr = sumDistances;
            break;
        case 3:
            functionPtr = sumRowsCols;
            break;
        case 4:
            functionPtr = sumSequence;
            break;
        default:
            //fallback case to catch errors
            functionPtr = sumDistances;
            break;
    }
    
    vector<Node> open;
    vector<Node> closed;
    
    string currentState = initialState;
    Node startingNode = {
        currentState,
        NULL,
        0,
        functionPtr(currentState),
        0 + functionPtr(currentState)
    };
    //add startingNode memory usage to bytes
    bytes += sizeof(startingNode);
    
    open.push_back(startingNode);
    nodeCount++;
    nodesExpanded++;
    
    //While the open set has available items to pick from
    while(open.size() > 0){
        //Step 1, find lowest f node
        Node *bestNode = new Node();
        int lowestF = open[0].f;
        int bestNodeIndex = 0;
        for(int i = 0; i < open.size(); i++){
            if(open[i].f < lowestF){
                lowestF = open[i].f;
                bestNodeIndex = i;
            }
        }
        
        //copy contents from lowest index to bestNode
        if(open[bestNodeIndex].path != NULL)
            bestNode->path = open[bestNodeIndex].path;
        bestNode->state = open[bestNodeIndex].state;
        bestNode->g = open[bestNodeIndex].g;
        bestNode->h = open[bestNodeIndex].h;
        bestNode->f = open[bestNodeIndex].f;
        //add bestNode size to bytes used
        bytes += sizeof(bestNode);
        
        //Step 2, pop bestNode off open
        closed.push_back(open[bestNodeIndex]);
        open.erase(open.begin() + bestNodeIndex);
        nodesExpanded++;
        
        //Step 3, generate succesors
        for(int i = 0; i < 4; i++){
            //if the bestNode can move in the direction
            if(canMove(i, bestNode->state)){
                currentState = move(i, bestNode->state);
                //Creates a new node for the new state created
                Node currentNode = {
                    currentState,
                    bestNode,
                    bestNode->g + 1,
                    functionPtr(currentState),
                    bestNode->g + 1 + functionPtr(currentState)
                };
                //bytes is to add up memory usage for new node created in memory
                bytes += sizeof(currentNode);
                //nodeCount iterated to keep track of total nodes
                nodeCount++;
                
                //Check goal state
                if(checkGoal(currentState)) {
                    //bytes is to add the memory usage of lists used
                    bytes += sizeof(open);
                    bytes += sizeof(closed);
                    //gettimeofday will mark the end time
                    gettimeofday(&end, NULL);
                    //timeResults computes the time between start and end in seconds
                    double timeResults = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0;
                    
                    //Send completion data to endState for final statistcs
                    endState(&currentNode, timeResults, nodeCount, nodesExpanded, bytes);
                    return;
                }
                
                //Check open for same state. If same state has lower F value, skip current node
                bool canPostOpen = true;
                for(int i = 0; i < open.size(); i++)
                    if(open[i].state == currentState)
                        if(open[i].f < currentNode.f)
                            canPostOpen = false;
                
                
                //Check  closed for same state. If same state has lower F value, skip current node
                for(int i = 0; i < closed.size(); i++)
                    if(closed[i].state == currentState)
                        if(closed[i].f < currentNode.f)
                            canPostOpen = false;
                
                //if lower f in both cases above, or not in either, post to open
                if(canPostOpen)
                    open.push_back(currentNode);
                
            }
        }
    }
    cout << "Error: No more nodes in open set." << endl;
}

int sumMisplacedItems(string state){
    int sum = 0;
    string goalState = "1238 4765";
    for(int i = 0; i < 9; i++){
        if(state[i] != goalState[i])
            sum++;
    }
    return sum;
}

int sumSequence(string state){
    int sum = 0;
    string goalState = "1238 4765";
    for(int i = 0; i < 9; i++){
        char value = state[i];
        if(i == 4){ if(state[i] != ' ') sum += 1; }
        
        int findNextChar = '-';
        for(int i = 0; i < 9; i++){
            if(value == goalState[i]){
                if(i != 8) findNextChar = goalState[i+1];
                if(i == 8) findNextChar = goalState[0];
            }
        }
        if(state[i+1] != findNextChar) sum += 2;
    }
    
    return sum;
}


//gets total of rows that are accurate
int sumRowsCols(string state){
    int sum = 0;
    //check row
    for(int i = 0; i < 9; i++){
        int spot = -1;
        for(int j = 0; j < 9; j++){
            char aChar = '0' + i;
            if(state[j] == aChar){
                spot = j;
            }
        }
        
        int myX = spot / 3;
        int myY =  spot % 3;
        
        int goalX = i / 3;
        int goalY = i % 3;
        
        if(myX != goalX)
            sum += abs(goalX-myX);
        
        if(myY != goalY)
            sum += abs(goalY-myY);
        
        if(myX == goalX && myY == goalY)
            sum -= 1;
            
    }
    return sum;
}


//gets the total differences of all 9 places
//h2n
int sumDistances(string state){
    int sum = 0;
    string s = "1238 4665";
    
    for(int i = 0; i < 9; i++){
        int spot = -1;
        for(int j = 0; j < 9; j++){
            char aChar = '0' + i;
            if(state[j] == aChar){
                spot = j;
            }
        }
        int myX = spot / 3;
        int myY =  spot % 3;
        
        int goalX = i / 3;
        int goalY = i % 3;
        
        sum += abs(myY-goalY);
        sum += abs(myX-goalX);
        
    }
    return sum;
}

//Checks if goal state was found
bool checkGoal(string state){
    string goalState = "1238 4765";
    if(state == goalState)
        return true;
    return false;
}

//checks if a move if possible
bool canMove(int way, string state){
    //0: up, 1: right, 2: down, 3:left
    int index = -1;
    
    for (int i = 0; i < 9; i++)
        if (state[i] == ' ')
            index = i;
    switch (way){
        case 0://up
            if(index - 3 < 0) return false;
            break;
        case 1://right
            if(index % 3 == 2) return false;
            break;
        case 2://down
            if(index + 3 > 8) return false;
            break;
        case 3://left
            if(index % 3 == 0) return false;
            break;
        default:
            break;
    }
    
    return true;
}


//moves the blank space up down right or left
string move(int way, string state){
    //0: up, 1: right, 2: down, 3:left
    int index = -1;
    for (int i = 0; i < 9; i++)
        if (state[i] == ' ')
            index = i;
    
    if(!canMove(way, state)) return "ERRORERRO";
    switch (way){
        case 0://up
            state[index] = state[index - 3];
            state[index - 3] = ' ';
            break;
        case 1://right
            state[index] = state[index + 1];
            state[index + 1] = ' ';
            break;
        case 2://down
            state[index] = state[index + 3];
            state[index + 3] = ' ';
            break;
        case 3://left
            state[index] = state[index - 1];
            state[index - 1] = ' ';
            break;
        default:
            break;
    }
    return state;
}


//shows current state in readable format
void displayState(string state){
    cout << "___________" << endl;
    cout << "| " << state[0] << "  " << state[1] << "  " << state[2] << " |" << endl;
    cout << "| " << state[3] << "  " << state[4] << "  " << state[5] << " |" << endl;
    cout << "| " << state[6] << "  " << state[7] << "  " << state[8] << " |" << endl;
    cout << "|_________|" << endl;
}

//State to show after goal state was found
void endState(Node *node, double runtime, unsigned long int nodes, unsigned long int nodesExpanded, unsigned long int bytes){
    
    //Prints statistics about the astar seach
    cout << "Success." << endl;
    cout << "Runtime: " << runtime << " seconds." << endl;
    cout << "Total Number of Nodes: " << nodes << " generated nodes." << endl;
    cout << "Total Number of Expanded Nodes: " << nodesExpanded << " expanded nodes." << endl;
    cout << "Depth to find solution: " << node->g << " nodes deep." << endl;
    cout << "Effective Branching Factor: ~" << nodes/node->g << endl;
    cout << "Memory Used: " << bytes << " bytes used." << endl;
    
    cout << "Here is the path to resolve solve the puzzle." << endl;
    
    //node that can be iterated through to preserve endState Node
    Node *newNode = node;
    //Backwards for loop to print the linked list states in order
    for(int i = node->g; i >=0; i--){
        newNode = node;
        for(int j = 0; j < i; j++){
            newNode = newNode->path;
        }
        displayState(newNode->state);
    }
}
