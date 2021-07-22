#include <gtest/gtest.h>
#include <string>

#include "../../src/TerrainPort/GenPlayerConnection.h"

// Will obviously only work while world-gen server is running

TEST(GenPlayerConnection, connect){
    GenPlayerConnection connection;
    connection.openConnection((std::string)"127.0.0.1", 10000);
    ASSERT_TRUE(connection.getIsOpen()) << "Couldn't open connection";
}
