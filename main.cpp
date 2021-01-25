#include <bits/stdc++.h>

using namespace std;

struct Node{
    const int id;
    Node * sccRepresentant = nullptr;
    int size = 1;//used in sccGraph
    vector<Node *> outConnections;
    vector<Node *> inConnections;

    vector<Node *> sccOutConnections; //if node is scc - these are connections to other sccs
    vector<Node *> sccInConnections; //if node is scc - these are connections to other sccs
    Node(int id):id(id){}
};

void lowLinkDFS(Node * n, vector<Node *> &nodes, vector<bool> &visited, stack<int> &stack, vector<bool> &onStack, vector<int> &entry, vector<int> &lowLink, int &timeStamp){
    visited[n->id] = true;
    entry[n->id] = timeStamp;
    lowLink[n->id] = timeStamp;
    ++timeStamp;

    stack.push(n->id);
    onStack[n->id] = true;
    for(auto c : n->outConnections){
        if(!visited[c->id]){
            lowLinkDFS((c), nodes, visited, stack, onStack, entry, lowLink, timeStamp);
        }
        if(onStack[c->id] && lowLink[c->id] < lowLink[n->id]){
            lowLink[n->id] = lowLink[c->id];
        }
    }
    if(entry[n->id] == lowLink[n->id]){
        while(!stack.empty()){
            int stackNodeID = stack.top();
            stack.pop();

            onStack[stackNodeID] = false;
            nodes[stackNodeID]->sccRepresentant = n;

            if(stackNodeID == n->id){
                break;
            }
        }
    }
}

void findSCCs(vector<Node *> &nodes){
    vector<bool> visited(nodes.size(), false); 
    stack<int> stack; 
    vector<bool> onStack(nodes.size(), false);
    vector<int> entry(nodes.size(), -1);
    vector<int> lowLink(nodes.size(), -1);
    int timeStamp = 0;
    for(auto n : nodes){
        if(!visited[n->id]){
            lowLinkDFS(n, nodes, visited, stack, onStack, entry, lowLink, timeStamp);
        }
    }
    cout<<"";
}

void getSCCConnections(const Node * start, unordered_set<Node *> &outSCCs, unordered_set<Node *> &inSCCs, unordered_set<int> &visited){
    for(auto n : start->outConnections){
        if(n->sccRepresentant == start->sccRepresentant){
            if(visited.find(n->id) == visited.end()){
                visited.insert(n->id);
                getSCCConnections(n, outSCCs, inSCCs, visited);
            }
        }else{
            outSCCs.insert(n->sccRepresentant);
        }
    }
    for(auto n : start->inConnections){
        if(n->sccRepresentant != start->sccRepresentant){
            inSCCs.insert(n->sccRepresentant);
        }
    }
}

//weakly connected components helper
void dfs0(Node * start, unordered_set<int> &visited){
    visited.insert(start->id);
    for(auto n : start->sccOutConnections){
        if(n->sccRepresentant == start->sccRepresentant){
            if(visited.find(n->id) == visited.end()){
                dfs0(n, visited);
            }
        }
    }
    for(auto n : start->sccInConnections){
        if(n->sccRepresentant == start->sccRepresentant){
            if(visited.find(n->id) == visited.end()){
                dfs0(n, visited);
            }
        }
    }
}

//get weakly connected components
vector< vector<Node *> > weaklyConnectedComponents(unordered_map<Node *, vector<Node *>> &containers){
    vector< vector<Node *> > graphs; 

    unordered_set<int> visited;
    for(auto container : containers){
        Node * n = container.first;
        if(visited.find(n->id) == visited.end()){
            graphs.push_back(vector<Node *> ());
            dfs0(n, visited);
        }
        graphs.back().push_back(n);
    }

    return graphs;
}


int main(){
    int nodesCount, edgesCount;
    cin >> nodesCount >> edgesCount;

    vector<Node *> nodes;
    for(int i = 0; i < nodesCount; i ++){
        nodes.push_back(new Node(i));
    }
    for(int i = 0; i < edgesCount; i ++){
        int n1, n2;
        cin >> n1 >> n2;
        n1--, n2--;
        nodes[n1]->outConnections.push_back(nodes[n2]);
        nodes[n2]->inConnections.push_back(nodes[n1]);
    }
    findSCCs(nodes);
    unordered_map<Node *, vector<Node *>> sccContainers;
    for(auto n : nodes){
        if(sccContainers.find(n->sccRepresentant) == sccContainers.end()){
            sccContainers[n->sccRepresentant] = vector<Node *>();
        }
        sccContainers[n->sccRepresentant].push_back(n);
        n->sccRepresentant->size++;
    }
    for(auto container : sccContainers){
        unordered_set<Node *> outConns;
        unordered_set<Node *> inConns;
        unordered_set<int> visited;

        Node * representant = container.first;
        getSCCConnections(representant, outConns, inConns, visited);
        for(auto c : outConns){
            representant->sccOutConnections.push_back(c);
        }
        for(auto c : inConns){
            representant->sccInConnections.push_back(c);
        }
    }

    for(auto n : nodes){
        n->inConnections.clear();
        n->outConnections.clear();
    }

    //returns weakly connected scc graphs - DAGs!
    vector< vector<Node *> > graphs = weaklyConnectedComponents(sccContainers);

    cout<<"";
    for(auto n : nodes){
        delete n;
    }
}