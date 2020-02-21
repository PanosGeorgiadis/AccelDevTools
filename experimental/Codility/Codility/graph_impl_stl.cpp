// directed graph implementation with C++ STL

#include <vector>

using namespace std;

typedef int NodeValueType;
// can be also done with a template<T> Edge
struct Edge
{
    NodeValueType source;
    NodeValueType destination;
};

class Graph
{
public:
    vector<vector<NodeValueType>> adjacencyList;

    Graph(const vector<Edge>& edges, int NumberOfNodes);
    {
        adjacencyList.resize(NumberOfNodes);
        for (auto& edge: edges)
        {
            adjacencyList[edge.source].push_back(edge.destination);

            // for undirected graph:
            // adjacencyList[edge.destination].push_back(edge.source);
        }
    }
};

void printGraph(Graph const& graph, int N)
{
    for (int i = 0; i < N; i++)
    {
        // print current vertex number
        cout << i << " --> ";

        // print all neighboring vertices of vertex i
        for (int v : graph.adjList[i])
            cout << v << " ";
        cout << endl;
    }
}

int main(int argc, char[] *argv)
{
    vector<Edge> edges;// = { {1, 2}, {...},... }
    int N = 6;
    Graph myGraph(edges, N);

    printGraph(graph, N);

    return 0;
}

Output:

0 -> 1
1 -> 2
2 -> 0 1
3 -> 2
4 -> 5
5 -> 4