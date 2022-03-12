#include<gtest/gtest.h>

#include "../../src/Protocol/PacketReader.h"

TEST(PacketReader, append){
    const int dataSize = 8;
    char data[dataSize];
    for(int i=0; i<dataSize; i++){
        data[i] = i;
    }

    PacketReader reader(&(data[0]), dataSize);


    const int data2Size = 4;
    char data2[data2Size];
    for(int i=0; i<data2Size; i++){
        data2[i] = i + dataSize;
    }

    int a = reader.readInt();
    ASSERT_TRUE(a == 66051);

    reader.append(&(data2[0]), data2Size);


    int b = reader.readInt();
    ASSERT_TRUE(b == 67438087);

    int c = reader.readInt();
    ASSERT_TRUE(c == 134810123);
    ASSERT_TRUE(reader.reachedEnd());
}

TEST(PacketReader, append2){
    const int dataSize = 8;
    char data[dataSize];
    for(int i=0; i<dataSize; i++){
        data[i] = i;
    }

    PacketReader reader(&(data[0]), dataSize);


    const int data2Size = 8;
    char data2[data2Size];
    for(int i=0; i<data2Size; i++){
        data2[i] = i + dataSize;
    }

    int a = reader.readInt();
    ASSERT_TRUE(a == 66051);
    ASSERT_TRUE(reader.lenRemaining() == 4);

    reader.append(&(data2[0]), data2Size);
    ASSERT_TRUE(reader.lenRemaining() == 4 + data2Size);


    int b = reader.readInt();
    ASSERT_TRUE(b == 67438087);

    int c = reader.readInt();
    ASSERT_TRUE(c == 134810123);
    ASSERT_FALSE(reader.reachedEnd());

    reader.readInt();
    ASSERT_TRUE(reader.reachedEnd());
}

TEST(PacketReader, append3){
    // Append a packetReader instead of direct data

    const int dataSize = 8;
    char data[dataSize];
    for(int i=0; i<dataSize; i++){
        data[i] = i;
    }

    PacketReader reader(&(data[0]), dataSize);


    const int data2Size = 4;
    char data2[data2Size];
    for(int i=0; i<data2Size; i++){
        data2[i] = i + dataSize;
    }

    int a = reader.readInt();
    ASSERT_TRUE(a == 66051);

    PacketReader reader2(&(data2[0]), data2Size);

    reader2.readInt(); //This shouldn't affect anything
    reader.append(reader2);


    int b = reader.readInt();
    ASSERT_TRUE(b == 67438087);

    int c = reader.readInt();
    ASSERT_TRUE(c == 134810123);
    ASSERT_TRUE(reader.reachedEnd());
}

TEST(PacketReader, swapGuts){
    // Append a packetReader instead of direct data

    const int dataSize = 8;
    char data[dataSize];
    for(int i=0; i<dataSize; i++){
        data[i] = i;
    }

    PacketReader reader(&(data[0]), dataSize);

    const int data2Size = 4;
    char data2[data2Size];
    for(int i=0; i<data2Size; i++){
        data2[i] = i + dataSize;
    }

    PacketReader reader2(&(data2[0]), data2Size);

    reader.swapGuts(reader2);

    int a = reader2.readInt();
    ASSERT_TRUE(a == 66051);

    int b = reader2.readInt();
    ASSERT_TRUE(b == 67438087);
    ASSERT_TRUE(reader2.reachedEnd());

    int c = reader.readInt();
    ASSERT_TRUE(c == 134810123);
    ASSERT_TRUE(reader.reachedEnd());
}
