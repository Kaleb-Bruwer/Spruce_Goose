#include<gtest/gtest.h>

#include<fstream>

#include "../../src/Datastructures/NBT/Tag_Compound.h"

// Some of these tests rely on specific test data (aren't written to handle changed data)
// so they will read from Test/Data/... which contains data files that must never change

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
    cout << "DimensionCodec.nbt:" << size << endl;

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

    string filename = "../Test/Data/recipes.nbt";
    ifstream file(filename);

    char* array = new char[64000];
    file.read(array, 64000);

    int index =0;
    Tag_Compound tc(array+1,index);
    // cout << tc.printableStr() << endl;
    file.close();

    int size = tc.getTransmitSize() + 1;
    cout << "recipes.nbt:" << size << endl;
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

TEST_F(NBTReadTest, tightBuffer){
    // Buffer size is exactly long enough
    try{
        string filename = "../Test/Data/recipes.nbt";
        ifstream file(filename);

        char* array = new char[36785];
        file.read(array, 36785);

        int index = 0;
        Tag_Compound tc(array+1,index, 36784);
        // cout << tc.printableStr() << endl;
        file.close();

        int size = tc.getTransmitSize();
        ASSERT_TRUE(size == 36784) << "getTransmitSize wrong\n";
    }
    catch(int e){
        ASSERT_TRUE(false) << "Tag_Compound couldn't read with exactly correct buffer size\n";
    };
}

TEST_F(NBTReadTest, smallBuffer1){
    // Buffer size is too small (by one byte)
    try{
        string filename = "../Test/Data/recipes.nbt";
        ifstream file(filename);

        char* array = new char[36785];
        file.read(array, 36785);
        file.close();

        int index = 0;
        Tag_Compound tc(array+1,index, 36783);
        ASSERT_TRUE(false) << "Tag_Compound read out of bounds\n";
    }
    catch(int e){};
}

TEST_F(NBTReadTest, smallBuffer2){
    // Buffer size is too small (by a lot)
    try{
        string filename = "../Test/Data/recipes.nbt";
        ifstream file(filename);

        char* array = new char[36785];
        file.read(array, 36785);
        file.close();

        int index = 0;
        Tag_Compound tc(array+1,index, 500);
        ASSERT_TRUE(false) << "Tag_Compound read out of bounds\n";
    }
    catch(int e){};
}
