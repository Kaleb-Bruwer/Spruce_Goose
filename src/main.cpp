#include <iostream>
#include <fstream>

#include "Server.h"
#include "TestFunctions.h"

#include "Datastructures/Octree/OctInternal.h"

using namespace std;

int main(int argc, char *argv[]){
    int socket = 25565;
    if(argc >=2){
        socket = atoi(argv[1]);
    }
    cout << "Socket set to " << socket << endl;

    Server spruceGoose(socket); //It all happens in constructor
    cout << "main() exits\n";
}
