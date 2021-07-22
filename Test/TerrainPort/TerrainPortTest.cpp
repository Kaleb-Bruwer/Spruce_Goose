#include <gtest/gtest.h>

#include "../../src/TerrainPort/TerrainPort.h"

TEST(TerrainPort, construct){
    TerrainPort* tp = TerrainPort::getInstance();
    ASSERT_TRUE(tp != 0) << "Can't get TerrainPort instance";
}
