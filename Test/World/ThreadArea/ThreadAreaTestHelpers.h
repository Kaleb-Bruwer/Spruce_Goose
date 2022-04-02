#pragma once

#include <gtest/gtest.h>
#include <vector>

#include "../../../src/World/ThreadArea.h"

// Uses fake chunks, don't use this if Chunks actually need to be used
void tAreaSetChunks(ThreadArea &tArea, std::vector<ChunkCoord> coords);
// Prevents a segfault upon ThreadArea destruction
void tAreaUnsetChunks(ThreadArea &tArea, std::vector<ChunkCoord> coords);

// Only a positive check, doesn't check for the lack of other chunks
void verifyChunks(const ThreadArea &tArea, std::vector<ChunkCoord> coords);
void verifyChunksNull(const ThreadArea &tArea, std::vector<ChunkCoord> coords);
