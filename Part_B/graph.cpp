#include <iostream>
#include <limits.h>
#include "d_except.h"
#include <list>
#include <stack>
#include <queue>
#include <fstream>
#include "d_matrix.h"
#include "graph.h"
using namespace std;
class maze
{
public:
    maze(ifstream &fin);
    void print(int, int, int, int);
    bool isLegal(int i, int j);
    void setMap(int i, int j, int n);
    int getMap(int i, int j) const;
    int getRow();
    int getCol();
    void mapMazeToGraph(maze &m, graph &g);

private:
    int rows; // number of rows in the maze
    int cols; // number of columns in the maze
    matrix<bool> value;
    matrix<int> map; // Mapping from maze (i,j) values to node index values
};

void maze::setMap(int i, int j, int n)
// Set mapping from maze cell (i,j) to graph node n.
{
    map[i][j] = n;
}
int maze ::getMap(int i, int j) const
// Return mapping of maze cell (i,j) in the graph.
{
    return map[i][j];
}

int maze ::getRow()
// Return mapping of maze cell (i,j) in the graph.
{
    return rows;
}

int maze ::getCol()
// Return mapping of maze cell (i,j) in the graph.
{
    return cols;
}

maze::maze(ifstream &fin)
// Initializes a maze by reading values from fin. Assumes that the
// number of rows and columns indicated in the file are correct.
{
    fin >> rows;
    fin >> cols;
    char x;
    value.resize(rows, cols);
    for (int i = 0; i <= rows - 1; i++)
        for (int j = 0; j <= cols - 1; j++)
        {
            fin >> x;
            if (x == 'O')
                value[i][j] = true;
            else
                value[i][j] = false;
        }
    map.resize(rows, cols);
}
void maze::print(int goalI, int goalJ, int currI, int currJ)
// Print out a maze, with the goal and current cells marked on the
// board.
{
    cout << endl;
    if (goalI < 0 || goalI > rows || goalJ < 0 || goalJ > cols)
        throw rangeError("Bad value in maze::print");
    if (currI < 0 || currI > rows || currJ < 0 || currJ > cols)
        throw rangeError("Bad value in maze::print");
    for (int i = 0; i <= rows - 1; i++)
    {
        for (int j = 0; j <= cols - 1; j++)
        {
            if (i == goalI && j == goalJ)
                cout << "*";
            else if (i == currI && j == currJ)
                cout << "+";
            else if (value[i][j])
                cout << " ";
            else
                cout << "X";
        }
        cout << endl;
    }
    cout << endl;
}
bool maze::isLegal(int i, int j)
// Return the value stored at the (i,j) entry in the maze.
{
    if (i < 0 || i > rows || j < 0 || j > cols)
        throw rangeError("Bad value in maze::isLegal");
    return value[i][j];
}

void maze::mapMazeToGraph(maze &m, graph &g)
// Create a graph g that represents the legal moves in the maze m.
{
    for (int i = 0; i <= rows - 1; i++)
    {
        for (int j = 0; j <= cols - 1; j++)
        {
            if (isLegal(i, j))
            {
                int index = g.addNode(1);
                setMap(i, j, index);
            }
        }
    }
    for (int i = 0; i <= rows - 1; i++)
    {
        for (int j = 0; j <= cols - 1; j++)
        {
            if (isLegal(i, j))
            {
                if ((i - 1) >= 0 && isLegal(i - 1, j))
                {
                    g.addEdge(getMap(i, j), getMap(i - 1, j), 1);
                }
                if ((i + 1) <= (rows - 1) && isLegal(i + 1, j))
                {
                    g.addEdge(getMap(i, j), getMap(i + 1, j), 2);
                }
                if ((j - 1) >= 0 && isLegal(i, j - 1))
                {
                    g.addEdge(getMap(i, j), getMap(i, j - 1), 3);
                }
                if ((j + 1) <= (cols - 1) && isLegal(i, j + 1))
                {
                    g.addEdge(getMap(i, j), getMap(i, j + 1), 4);
                }
            }
        }
    }
}

bool findPathNonRecursive(int start, graph &g)
{
    stack<node> s;
    node s_node = g.getNode(start);
    s.push(s_node);
    while (!s.empty())
    {
        node n = s.top();
        s.pop();
        if (n.getId() == g.numNodes() - 1)
            return true;
        if (!g.isVisited(n.getId()))
        {
            g.visit(n.getId());
            for (int i = 0; i < g.numNodes(); i++)
            {
                if (g.isEdge(n.getId(), i) && !g.isVisited(i))
                {
                    s.push(g.getNode(i));
                }
            }
        }
    }
    return false;
}

bool findShortestPath1(graph &g, stack<string> &s)
{
    int n_count = g.numNodes();
    vector<int> parents(n_count, -1);
    vector<int> distance(n_count, 1e9);
    distance[0] = 0;
    queue<node> q;
    node s_node = g.getNode(0);
    q.push(s_node);
    while (!q.empty())
    {
        node n = q.front();
        q.pop();
        if (!g.isVisited(n.getId()))
        {
            g.visit(n.getId());
            for (int i = 0; i < n_count; i++)
            {
                if (g.isEdge(n.getId(), i) && !g.isVisited(i))
                {
                    parents[i] = n.getId();
                    distance[i] = distance[n.getId()] + 1;
                    q.push(g.getNode(i));
                }
            }
        }
    }
    if (distance[n_count - 1] == 1e9)
    {
        return false;
    }
    int currentNode = n_count - 1;
    while (parents[currentNode] != -1)
    {
        int temp = currentNode;
        currentNode = parents[currentNode];
        string move;
        if (g.getEdgeWeight(currentNode, temp) == 1)
        {
            move = "Go Up";
        }
        if (g.getEdgeWeight(currentNode, temp) == 2)
        {
            move = "Go Down";
        }
        if (g.getEdgeWeight(currentNode, temp) == 3)
        {
            move = "Go Left";
        }
        if (g.getEdgeWeight(currentNode, temp) == 4)
        {
            move = "Go Right";
        }
        s.push(move);
    }
    return true;
}

bool findShortestPath2(graph &g, stack<string> &s)
{
    return true;
}

int main()
{
    char x;
    ifstream fin;
    // Read the maze from the file.
    string fileName = "maze3-1.txt";
    fin.open(fileName.c_str());
    if (!fin)
    {
        cerr << "Cannot open " << fileName << endl;
        exit(1);
    }
    try
    {
        while (fin && fin.peek() != 'Z')
        {
            graph g;
            stack<string> s1, s2;
            maze m(fin);
            m.print(0, 0, m.getRow() - 1, m.getCol() - 1);
            m.mapMazeToGraph(m, g);
            if (findShortestPath1(g, s1))
            {
                cout << "Solution Shortest Path Found Using Queue-based BFS Requires " << s1.size() << " Steps:" << endl;
                while (!s1.empty())
                {
                    cout << s1.top() << endl;
                    s1.pop();
                }
            }
            else
            {
                cout << "No Path Exists..." << endl;
            }
            cout << endl;
            g.clearVisit();
            if (findShortestPath2(g, s2))
            {
                cout << "Solution Shortest Path Found Using Queue-based BFS Requires " << s2.size() << " Steps:" << endl;
                while (!s1.empty())
                {
                    cout << s1.top() << endl;
                    s1.pop();
                }
            }
            else
            {
                cout << "No Path Exists..." << endl;
            }
        }
    }
    catch (indexRangeError &ex)
    {
        cout << ex.what() << endl;
        exit(1);
    }
    catch (rangeError &ex)
    {
        cout << ex.what() << endl;
        exit(1);
    }
}