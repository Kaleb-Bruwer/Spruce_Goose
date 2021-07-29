#include <gtest/gtest.h>
#include <string>
#include <chrono>
#include <vector>
#include <thread>

#include "../../src/TerrainPort/GenPlayerConnection.h"

// Will obviously only work while world-gen server is running

TEST(GenPlayerConnection, connect){
    GenPlayerConnection connection;
    connection.openConnection((std::string)"127.0.0.1", 10000, "bot0");
    ASSERT_TRUE(connection.getIsOpen()) << "Couldn't open connection";


    connection.readMessage();
    this_thread::sleep_for(chrono::milliseconds(100));
    connection.readMessage();
    this_thread::sleep_for(chrono::milliseconds(500));
    cout << "FINAL ROUND\n";
    connection.readMessage();

    ASSERT_TRUE(connection.getHasSpawned()) << "Login sequence failed";
}
