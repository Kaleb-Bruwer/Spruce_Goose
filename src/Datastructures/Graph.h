#ifndef GRAPH_H
#define GRAPH_H

#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

//T specifies data type for vertices
//edges do not have data in this version, add it later if nessesary

//Graph represented using adjacency matrix

//Strict usage: must declare vertices ahead with setVertices
//Then edges can be added one by one

template <class T>
class Graph{
private:
    vector<T> vertices;
    vector<bool> vertPresent; //Set to false when vertex is removed
    vector<vector<int>> adjacent;

    bool empty(){
        for(bool b : vertPresent)
            if(b)
                return false;

        int size = vertices.size();
        for(int i=0; i<size; i++){
            if(adjacent[i].empty())
                continue;

            for(int a : adjacent[i]){
                if(a != -1) //-1 can be used to remove edges
                    return false;
            }
        }

        return true;
    };

    bool hasEdge(vector<int> &vert){
        for(int v : vert){
            if(adjacent[v].empty())
                continue;

            for(int a : adjacent[v]){
                if(a != -1) //-1 can be used to remove edges
                    return true;
            }
        }
        return false;
    }

    //Returns arbitrary vertex in graph
    int getSomeVertex(){
        int size = vertPresent.size();
        for(int i=0; i<size; i++){
            if(vertPresent[i])
                return i;
        }
        return -1; //Won't happen if you use it right
    }

    vector<int> getAdjacent(int src){
        vector<int> result;
        for(int dest : adjacent[src]){
            result.push_back(dest);
        }
        return result;
    }

    vector<int> getAdjacent(vector<int> &vert){
        vector<int> result;

        for(int a : vert){
            vector<int> temp = getAdjacent(a);
            for(int b : temp){
                if(find(result.begin(), result.end(), b) == result.end()){
                    result.push_back(b);
                }
            }
        }

        return result;
    }

    //Remover every edge connected to input vertices
    void remEdges(vector<int> &vert){
        for(int src : vert){
            for(int dest : adjacent[src]){
                remove(adjacent[dest].begin(), adjacent[dest].end(), src);
            }
            adjacent[src].clear();
        }
    }

    void remVertices(vector<int> &vert){
        for(int a : vert){
            vertPresent[a] = false;
        }
    };

public:
    Graph(){};

    void setVertices(vector<T> &v){
        clear();
        int vSize = v.size();
        vertices.resize(vSize);
        vertPresent.resize(vSize);

        for(int i=0; i < vSize; i++){
            vertices[i] = v[i];
            vertPresent[i] = true;
        }

        adjacent.resize(vSize);
    };

    void addEdge(T src, T dest){
        int s = -1, d = -1;

        int i=0;
        for(T vert : vertices){
            if(vert == src)
                s = i;
            if(vert == dest)
                d = i;

            i++;
        }

        if(s == d)
            return;

        if(s == -1 || d == -1)
            return;

        addEdge(s,d);
    }

    void printGraph(){
        cout << "Vertices: ";
        for(T a : vertices){
            cout << a << ", ";
        }
        cout << endl;

        cout << "Adjacencies:\n";
        int size = vertices.size();
        for(int i=0; i<size; i++){
            cout << i << ": ";
            for(int a : adjacent[i]){
                cout << a << ", ";
            }
            cout << endl;
        }
    }

    //Using this one directly is allowed, but might be dodgy
    void addEdge(int src, int dest){
        int small = min(src, dest);
        int large = max(src, dest);

        if(small < 0 || large >= vertices.size()){
            cout << "Invalid vertex in Graph\n";
        }

        adjacent[small].push_back(large);
        adjacent[large].push_back(small);
    }

    vector<vector<T>> getClosedPartitions(){
        vector<vector<int>> parts; //partitions
        //Return value is a list of partitions
        int p = 0;
        while(!empty()){
            parts.resize(p + 1);

            //Pick some vertex in the graph
            int a = getSomeVertex();
            parts[p].push_back(a);

            while(hasEdge(parts[p])){
                //Get all adjacent vertices
                vector<int> adj = getAdjacent(parts[p]);
                remEdges(parts[p]);

                for(int b : adj){
                    if(find(parts[p].begin(), parts[p].end(), b) == parts[p].end()){
                        parts[p].push_back(b);
                    }
                }
                //Remove edges that led to these vertices
            }
            remVertices(parts[p]);
            p++;
        }

        vector<vector<T>> result;
        int pSize = parts.size();
        result.resize(pSize);

        for(int i = 0; i<pSize; i++){
            for(int a : parts[i]){
                result[i].push_back(vertices[a]);
            }
        }

        return result;
    }

    void clear(){
        vertices.clear();
        adjacent.clear();
    };
};

#endif
