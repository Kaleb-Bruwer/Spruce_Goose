#pragma once

#include "PacketWriter.h"

#include "../Datastructures/Coordinate/Coordinate.h"

#include "../World/Chunk/Block.h"

using namespace std;

class BlockData;

class AdvancedWriter : public PacketWriter{
private:

    void writeEntityHeadLook(int eid, double yaw);
    void writeAbsolutePos(Coordinate<double> pos);
public:
    AdvancedWriter(unsigned int size = 256);

    //state 1 packets
    void writeSLP(); //server list ping
    void writePong(long long);

    //state 2 packets
    void writeLoginSuccess(string uuid, string username);
    void writeJoinGame(int eid, short gamemode, short renderDistance);
    void writePlayerAbilities(int gamemode);
    void writeSpawnPos(Coordinate<int> pos);

    //state 3 packets
    void writeChangeHeldItem(int slot);
    void writeEntityStatus(int eid, int status);
    void writePlayerPos(Coordinate<double>, double pitch, double yaw, bool relative = false);

    //This sends an empty array
    void writePlayerInfo(int action);
    void writeUpdateViewPos(Coordinate<double> pos);
    void writeUnloadChunk(ChunkCoord c);
    void writeBlockChange(vector<Coordinate<int>> positions, vector<Block> blocks);

    void writeSpawnObject(int eid, Coordinate<double> pos, double yaw = 0, double pitch = 0);
    void writeEntityMetadata(int eid, Slot s);
    void writeSpawnPlayer(int eid, string name, Coordinate<double> pos, double yaw, double pitch, short item);

    void writeEntityRelativeMove(int eid, Coordinate<double> movement);
    void writeEntityRelativeMove(int eid, Coordinate<double> movement, double yaw, double pitch);
    void writeEntityTeleport(int eid, Coordinate<double> movement, double yaw, double pitch);
    void writeEntityLook(int eid, double yaw, double pitch);
    void writeDestroyEntity(int eid);
    void writeDestroyEntity(vector<int> eid);
    void writeCollectItem(int player, int item);

    void writeSetSlot(int windowID, short slotNum, Slot slot);
    void writeConfirmTransaction(int windowID, short actionNum, bool accepted);

    void writeChat(string message);

    void writeOpenWindow(short windowID, short invType, string title, int numSlots, bool useTitle, int eid = -1);
    bool writeWindowItems(BlockData* b);
    void writeCloseWindow(short windowID);
};
