#include <iostream>
#include <stack>
#include <vector>

using namespace std;

// Structure representing a frame in the stack for DFS
struct Frame {
    int node;             // Current node
    int nextChildIdx;     // Index of the next child to process
    int discoveryTime;    // Discovery time of the node
    bool hasAddedToStack; // Flag to check if the node has been added to the stack
};

class Graph {
private:
    int nPeople;                                    // Number of people    
    vector<vector<int>> adjList;                    // Adjacency list
    vector<int> discoveryTimes, lowLinks, sccIds;   // Discovery times, low links and SCC ids
    stack<int> mutualFriendsStack;                  // Stack to store mutual friends
    vector<bool> inStack;                           // Flag to check if a node is in the stack
    int timeCounter, sccCounter;                    // Time counter and SCC counter
    vector<int> maxRelationMemo;                    // Memoization vector for max relations

    // Method to find mutual friends using modified DFS
    void findMutualFriends(int startPerson) {
        
        // Initialize a stack of frames for DFS
        stack<Frame> frames;
        frames.push({startPerson, 0, ++timeCounter, false});

        while (!frames.empty()) {
            Frame &frame = frames.top();
            int node = frame.node;

            // Initial processing of a node
            if (!frame.hasAddedToStack) {
                discoveryTimes[node] = lowLinks[node] = frame.discoveryTime;
                mutualFriendsStack.push(node);
                inStack[node] = true;
                frame.hasAddedToStack = true;
            }

            // Flag to determine if we should return from the current frame
            bool shouldReturn = true;

            // Iterate through the children of the current node
            while (frame.nextChildIdx < static_cast<int>(adjList[node].size())) {
                int child = adjList[node][frame.nextChildIdx++];

                // If the child node is not discovered, push it to the stack and continue
                if (discoveryTimes[child] == -1) {
                    frames.push({child, 0, ++timeCounter, false});
                    shouldReturn = false;
                    break;
                }
                // If the child is in the stack, update low link values
                else if (inStack[child]) {
                    lowLinks[node] = min(lowLinks[node], lowLinks[child]);
                }
            }

            // Post-processing of a node
            if (shouldReturn) {
                if (lowLinks[node] == discoveryTimes[node]) {
                    int topNode;
                    do {
                        topNode = mutualFriendsStack.top();
                        sccIds[topNode] = sccCounter;
                        inStack[topNode] = false;
                        mutualFriendsStack.pop();
                    } while (topNode != node);
                    sccCounter++;
                }

                frames.pop();

                if (!frames.empty()) {
                    Frame &parentFrame = frames.top();
                    lowLinks[parentFrame.node] = min(lowLinks[parentFrame.node], lowLinks[node]);
                }
            }
        }
    }


public:

    // Constructor for graph
    Graph(int nPeople) : nPeople(nPeople), adjList(nPeople) {}

    // Method to add an edge to the graph
    void addEdge(int x, int y) {
        adjList[x].push_back(y);
    }

    // Method to find mutual friends
    void getRelationsCycles() {

        discoveryTimes.assign(nPeople, -1);
        lowLinks.assign(nPeople, -1);
        sccIds.assign(nPeople, -1);
        inStack.assign(nPeople, false);
        timeCounter = 0;
        sccCounter = 0;

        for (int i = 0; i < nPeople; i++) {
            if (discoveryTimes[i] == -1) {
                findMutualFriends(i);
            }
        }
    }

    // Method to create an updated graph with SCCs as nodes
    Graph createUpdatedGraph() {

        Graph sccGraph(sccCounter);

        for (int i = 0; i < nPeople; i++) {
            for (int j : adjList[i]) {
                if (sccIds[i] != sccIds[j]) {
                    sccGraph.addEdge(sccIds[i], sccIds[j]);
                }
            }
        }
        return sccGraph;
    }

    // Method to calculate the maximum number of relations
    int getMaxRelations() {

        maxRelationMemo.assign(nPeople, -1);

        int maxRelations = 0;

        for (int i = 0; i < nPeople; i++) {
            int relations = calculateMaxRelations(i);
            maxRelations = max(maxRelations, relations);
        }
        return maxRelations;
    }
    
    // Method to calculate the maximum number of relations for each node
    int calculateMaxRelations(int node) {

        // If the value is already calculated, return it
        if (maxRelationMemo[node] != -1) {
            return maxRelationMemo[node];
        }

        int maxRelations = 0;

        // Iterate through the children of the current node
        for (int adjNode : adjList[node]) {
            maxRelations = max(maxRelations, 1 + calculateMaxRelations(adjNode));
        }
        
        maxRelationMemo[node] = maxRelations;
        
        return maxRelations;
    }
};

int main() {

    // Improve I/O performance
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    
    int nPeople, nRelations;

    // Read number of people and relations
    cin >> nPeople >> nRelations;

    Graph g(nPeople);

    // Read relations
    for (int i = 0; i < nRelations; ++i) {
        int x, y;
        cin >> x >> y;
        g.addEdge(x-1, y-1);
    }

    // Find mutual friends
    g.getRelationsCycles();

    // Create updated graph with SCCs as nodes
    Graph updatedGraph = g.createUpdatedGraph();

    // Find maximum number of relations
    int maxRelations = updatedGraph.getMaxRelations();
    cout << maxRelations << endl;

    return 0;
}
