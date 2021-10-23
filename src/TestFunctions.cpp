#include "TestFunctions.h"

#include <iostream>
#include <fstream>
#include <vector>

#include "./zlib/zlib.h"

#include "./Datastructures/NBT/Tag_Compound.h"
#include "./Datastructures/Graph.h"
#include "./World/Chunk/Chunk.h"
#include "./Protocol/PacketWriter.h"

using namespace std;

void makeSimpleChunk(){
    Chunk origin(0,0);

    //origin.setBlockRange(Coordinate(0,0,0), Coordinate(15,5,15), 1);
}

void testGraph(){
    cout << "Starting graph test\n";
    Graph<char> g;
    vector<char> vertices = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k'};
    g.setVertices(vertices);

    g.addEdge('a', 'b');
    g.addEdge('a', 'd');
    g.addEdge('b', 'd');
    g.addEdge('d', 'f');
    g.addEdge('d', 'g');
    g.addEdge('h', 'a');
    g.addEdge('h', 'b');
    g.addEdge('h', 'd');

    g.addEdge('c', 'e');
    g.addEdge('c', 'j');
    g.addEdge('j', 'k');

    vector<vector<char>> result = g.getClosedPartitions();
    cout << "Received partitions\n\n";

    int rSize = result.size();
    for(int i=0; i<rSize; i++){
        cout << "Partition " << i << ": ";
        for(char a : result[i]){
            cout << a << " ";
        }
        cout << endl;
    }
}

void testPacketWriter(){
    string filename = "bigtest.nbt";
    ifstream file(filename);

    char* array = new char[1600];
    file.read(array, 1600);
    int index =0;
    Tag_Compound tc(array+1,index);
    file.close();

    int size = tc.getTransmitSize();
    char* array2 = new char[size];
    int index2 = 0;
    tc.getTransmittable(array2,index2);
    PacketWriter pw;
    pw << tc;
    pw << tc;
    cout << "Packet length: " << pw.getIndex() << endl;
}
