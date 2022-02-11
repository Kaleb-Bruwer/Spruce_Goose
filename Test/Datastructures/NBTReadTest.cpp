#include<gtest/gtest.h>

#include<fstream>

#include "../../src/Datastructures/NBT/Tag_Compound.h"


class NBTReadTest : public ::testing::Test{
protected:
    virtual void SetUp(){

    };

    virtual void TearDown(){

    };

};

TEST_F(NBTReadTest, test0){
    // Read file into array
    // Interpret file
    // Convert back to binary
    // Compare

    string filename = "../Data/DimensionCodec.nbt";
    ifstream file(filename);

    char* array = new char[8000];
    file.read(array, 8000);

    int index =0;
    Tag_Compound tc(array+1,index);
    // cout << tc.printableStr() << endl;
    file.close();

    int size = tc.getTransmitSize() + 1;
    char* array2 = new char[size];
    int index2 = 0;
    tc.getTransmittable(array2,index2);

    bool match = true;
    for(int i=0; i<size; i++){
        if(array[i] != array2[i]){
            match = false;
            break;
        }
    }

    assert(size == index2);

    assert(match);

    delete [] array2;
    delete [] array;
}

TEST_F(NBTReadTest, test1){
    // Read file into array
    // Interpret file
    // Convert back to binary
    // Compare

    string filename = "../Data/recipes.nbt";
    ifstream file(filename);

    char* array = new char[64000];
    file.read(array, 64000);

    int index =0;
    Tag_Compound tc(array+1,index);
    // cout << tc.printableStr() << endl;
    file.close();

    int size = tc.getTransmitSize() + 1;
    char* array2 = new char[size];
    int index2 = 0;
    tc.getTransmittable(array2,index2);

    bool match = true;
    for(int i=0; i<size; i++){
        if(array[i] != array2[i]){
            match = false;
            break;
        }
    }

    assert(size == index2);
    assert(match);

    delete [] array2;
    delete [] array;
}
