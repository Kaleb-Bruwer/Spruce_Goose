#include "EntityStore.h"

#include "../JobTickets/WorldToProtocol/EntityMove.h"
#include "../JobTickets/WorldToProtocol/DestroyEntityJob.h"
#include "../JobTickets/WorldToProtocol/SendWindowItem.h"

#include "../JobTickets/SendPacket.h"

#include "../Protocol/PacketWriterOld.h"
#include "../Protocol/AdvancedWriter.h"

#include "../Inventory/BlockData/Furnace.h"

#include "./Functors/GetAllF.h"

#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

EntityStore::~EntityStore(){
}

void EntityStore::insertInAllEnts(Entity* e){
    int eid = e->eid;
    eidLookup[eid] = e;


    // int eid = e->eid;
    //
    // //Do binary search
    // int low = 0;
    // int high = allEnts.size() - 1;
    // int mid = (high + low) / 2;
    //
    // if(high < 0){
    //     allEnts.push_back(e);
    //     return;
    // }
    //
    // if(allEnts[high]->eid < eid){
    //     allEnts.push_back(e);
    //     return;
    // }
    //
    // if(allEnts[0]->eid > eid){
    //     allEnts.insert(allEnts.begin(), e);
    //     return;
    // }
    //
    // //Since eids must be unique, it can be assumed there won't be matches
    // while(true){
    //     if(allEnts[mid]->eid < eid){
    //         if(low == mid){
    //             //Can't narrow down further, insert here
    //             //will always be on lower since c++ rounds down
    //             allEnts.insert(allEnts.begin() + low + 1, e);
    //             return;
    //         }
    //         else
    //             low = mid;
    //     }
    //     else{
    //         high = mid;
    //     }
    //     mid = (high + low) / 2;
    // }

}

void EntityStore::remFromAllEnts(Entity* e){
    int eid = e->eid;

    auto it = eidLookup.find(eid);
    if(it != eidLookup.end()){
        eidLookup.erase(it);
    }


    // int eid = e->eid;
    //
    // //Do binary search
    // int low = 0;
    // int high = allEnts.size() - 1;
    // int mid = (high + low) / 2;
    //
    // if(high < 0)
    //     return;
    //
    // if(eid < allEnts[0]->eid)
    //     return;
    //
    // //Since eids must be unique, it can be assumed there won't be matches
    // while(true){
    //     if(allEnts[mid]->eid < eid){
    //         if(low == mid){
    //             return;
    //         }
    //         else
    //             low = mid;
    //     }
    //     else if(allEnts[mid]->eid > eid){
    //         high = mid;
    //     }
    //     else{
    //         allEnts.erase(allEnts.begin() + mid);
    //         return;
    //     }
    //     mid = (high + low) / 2;
    // }
}

void EntityStore::addEntity(Entity* e){
    tree.insert(e);
    //eidLookup[e->eid] = e;
    insertInAllEnts(e);
    if(e->getType() == PLAYERENTITY){
        players.push_back((PlayerEntity*)e);
    }
}

bool EntityStore::addExternalPlayer(PlayerEntity* p){
    auto it = find(outsidePlayers.begin(), outsidePlayers.end(), p);
    if(it == outsidePlayers.end()){
        outsidePlayers.push_back(p);
        return true;
    }
    return false;
}

Entity* EntityStore::getByEid(int eid){
    auto it = eidLookup.find(eid);
    if(it != eidLookup.end()){
        return it->second;
    }
    return 0;

    // int low = 0;
    // int high = allEnts.size() - 1;
    // int mid = (high + low) / 2;
    //
    // if(high < 0)
    //     return 0;
    //
    // if(allEnts[0]->eid > eid){
    //     return 0;
    // }
    //
    // while(true){
    //     if(allEnts[mid]->eid < eid){
    //         if(low == mid){
    //             if(allEnts[mid+1]->eid != eid)
    //                 return 0;
    //             else{
    //                 return allEnts[mid+1];
    //             }
    //         }
    //         else
    //             low = mid;
    //     }
    //     else if (allEnts[mid]->eid > eid){
    //         high = mid;
    //     }
    //     else{
    //         return allEnts[mid];
    //     }
    //     mid = (high + low) / 2;
    // }

}

void EntityStore::deleteEntity(Entity* e){
    //eidLookup.erase(e->eid);
    remFromAllEnts(e);
    tree.remove(e);

    auto it = find(players.begin(), players.end(), (PlayerEntity*) e);
    if(it != players.end())
        players.erase(it);

    auto it2 = find(outsidePlayers.begin(), outsidePlayers.end(), (PlayerEntity*) e);
    if(it2 != outsidePlayers.end())
        outsidePlayers.erase(it2);

    delete e;
}

void EntityStore::deleteEntity(int eid){
    Entity* e = getByEid(eid);
    if(e)
        deleteEntity(e);
    return;
}

void EntityStore::sendPlayerPos(){
    //Sends positions of all players within this EntityStore to all
    //players within it and external, if they need it

    /*
    const int combineFactor = 2;

    auto insideCombinedCoord = [&](ChunkCoord c, PlayerEntity* e) -> bool{
        //c is the lower corner of combined Coord, not the actual Combined Coord
        ChunkCoord pc = e->position.getContainingChunk();
        pc.x /= combineFactor;
        pc.z /= combineFactor;

        c.x /= combineFactor;
        c.z /= combineFactor;

        return (c == pc);
    };

    map<ChunkCoord, PacketWriterOld> packets;
    map<ChunkCoord, vector<pair<int,int>>> playerIndices;
    int numPlayers = players.size();

    for(int i=0; i<numPlayers; i++){
        PlayerEntity* pla = players[i];
        if(pla->position == pla->oldPosition
            && !pla->changedLook)
            continue;

        ChunkCoord cCoord = pla->oldPosition.getContainingChunk();
        cCoord.x /= combineFactor;
        cCoord.z /= combineFactor;

        PacketWriterOld& p = packets[cCoord];
        vector<pair<int,int>>& pInd = playerIndices[cCoord];
        pInd.push_back(make_pair(pla->eid, p.getIndex()));

        if(pla->position != pla->oldPosition){
            p.writePacketID(0x22);
            p << (int) pla->eid;

            int x = pla->position.x * 32;
            int y = pla->position.y * 32;
            int z = pla->position.z * 32;

            p << x;
            p << y;
            p << z;

            p << (char) pla->yaw;
            p << (char) pla->pitch;
        }
        else{
            p.writePacketID(0x20);
            p << (int) pla->eid;
            p << (char) pla->yaw;
            p << (char) pla->pitch;
        }

        unsigned short heldItem = pla->inventory.getHeldItemID();
        if(heldItem != pla->lastHeldItem){
            p.writePacketID(0x05);

            p << (int) pla->eid;
            p << (short) 0;
            Slot* held = pla->inventory.getHeldItem();
            p << *held;
        }

    }

    int numPackets = packets.size();
    vector<pair<ChunkCoord,JobTicket*>> jobs(numPackets);

    int index = 0;
    for(auto it = packets.begin(); it != packets.end(); it++){
        jobs[index].first = it->first;
        jobs[index].second = new SendPacket(&(it->second));
        jobs[index].second->pickup();
    }

    int numOutside = outsidePlayers.size();
    for(int j=0; j < numPackets; j++){
        ChunkCoord c = jobs[j].first;
        c.x *= combineFactor;
        c.z *= combineFactor;
        ChunkCoord c2 = c;
        c2.x += combineFactor-1;
        c2.z += combineFactor-1;

        for(int i=0; i<numPlayers; i++){
            if(insideCombinedCoord(c, players[i])){
                cout << "Cutting out player\n";
                //Remove this player from packet
                //use playerIndices to figure out where to cut
                vector<pair<int,int>>& pInd = playerIndices[jobs[j].first];
                if(pInd.size() == 1)
                    continue; //Only player in chunk

                int pIndSize = pInd.size();
                int pStart = 0;
                int pEnd = 0;

                for(int k=0; k < pIndSize; k++){
                    if(pInd[k].first == players[i]->eid){
                        pStart = pInd[k].second;
                        if(k+1 < pIndSize){
                            pEnd = pInd[k+1].second;
                        }
                        break;
                    }
                }

                SendPacket* oldJ = (SendPacket*) jobs[j].second;

                int size = 0;
                char* buff = 0;
                if(pEnd == 0){
                    size = pStart;
                    buff = new char[pStart];
                    memcpy(buff, oldJ->data, pStart);
                }
                else if(pStart == 0){
                    size = oldJ->size - pEnd;
                    buff = new char[size];
                    memcpy(buff, oldJ->data, size);
                }
                else{
                    int size2 = oldJ->size - pEnd;
                    size = size2 + pStart;
                    buff = new char[size];
                    memcpy(buff, oldJ->data, pStart);
                    memcpy(&(buff[pStart]), &(oldJ->data[pEnd]), size2);
                }

                SendPacket* newJ = new SendPacket();
                newJ->data = buff;
                newJ->size = size;
                players[i]->pushJobToProtocol(oldJ);
                continue;
            }

            //Player not included in packet, so can send directly
            if(players[i]->hasChunk(c) || players[i]->hasChunk(c2))
                players[i]->pushJobToProtocol(jobs[j].second);
        }
        for(int i=0; i<numOutside; i++){
            if(outsidePlayers[i]->hasChunk(c) || outsidePlayers[i]->hasChunk(c2))
                outsidePlayers[i]->pushJobToProtocol(jobs[j].second);
        }
        jobs[j].second->drop();
    }
    */


    int numPlayers = players.size();
    for(int s = 0; s < numPlayers; s++){
        PlayerEntity* sending = players[s];

        if(sending->position == sending->oldPosition
            && !sending->changedLook)
            continue;

        ChunkCoord pos = sending->position.getContainingChunk();
        EntityMove* job = new EntityMove();
        job->pickup();

        //Set values
        job->eid = sending->eid;

        if(sending->position != sending->oldPosition){
            job->posIncluded = true;
            job->posRelative = false;

            job->movement = sending->position;
            sending->oldPosition = sending->position;
        }

        job->lookIncluded = true;
        sending->changedLook = false;

        double yaw = fmod(sending->yaw, 360);
        double pitch = fmod(sending->pitch, 360);

        job->yaw = yaw*((double)256/360);
        job->pitch = pitch*((double)256/360);

        //Held item
        unsigned short heldItem = sending->inventory.getHeldItemID();
        if(heldItem != sending->lastHeldItem){
            sending->lastHeldItem = heldItem;
            job->heldItem = new Slot(sending->inventory.getHeldItem());
        }

        //Iterate over players to see who needs it
        for(int r = 0; r < numPlayers; r++){
            if(r == s) //Don't send player to himself
                continue;
            if(players[r]->hasChunk(pos)){
                players[r]->pushJobToProtocol(job);
            }
        }

        //iterate over external players
        for(PlayerEntity *p : outsidePlayers){
            if(p->hasChunk(pos)){
                p->pushJobToProtocol(job);
            }
        }
        job->drop();
    }

}

void EntityStore::windowPropertyUpdate(unsigned long long tick){
    set<Furnace*> resetAltered;

    for(int i=0; i<players.size(); i++){
        BlockData* b = players[i]->inventory.getActiveBlock();
        if(!b)
            continue;

        switch(b->getType()){
        case FURNACE:
            Furnace* furnace = (Furnace*) b;
            AdvancedWriter writer;
            unsigned long long burnFinish = furnace->getBurnFinish();
            unsigned long long fuelFinish = furnace->getFuelFinish();

            if(furnace->altered){
                // Send a full update
                writer.writeWindowProperty(0, 0, burnFinish - tick);
                writer.writeWindowProperty(0, 1, min((int) (fuelFinish - tick), 200));

                SendWindowItem* job = new SendWindowItem();
                job->addSlot(0, furnace->slots[0]);
                job->addSlot(1, furnace->slots[1]);
                job->addSlot(2, furnace->slots[2]);
                players[i]->pushJobToProtocol(new SendPacket(&writer));

                resetAltered.insert(furnace);
            }
            else{
                // might need update on ui elements
                if(burnFinish != 0){
                    writer.writeWindowProperty(0, 0, burnFinish - tick);
                }

                if(fuelFinish != 0){
                    writer.writeWindowProperty(0, 1, min((int) (fuelFinish - tick), 200));
                }
            }


            if(writer.getIndex() > 0){
                players[i]->pushJobToProtocol(new SendPacket(&writer));
            }

            break;
        }
    }

    // This is neccesary for in case multiple players have the same furnace open
    for(auto it = resetAltered.begin(); it != resetAltered.end(); it++){
        (*it)->altered = false;
    }
}


void EntityStore::executeFunctorAll(Functor<Entity*> &f){
    FunctorAdapter<Entity*> f2(f);
    tree.execFunc(f2);
    removeFromF(f2);
}

void EntityStore::executeFunctorPlayers(Functor<PlayerEntity*> &f){
    vector<Positional*> removed;
    vector<pair<Positional*, Coordinate<double>>> moved;

    for(int i=0; i<players.size(); i++){
        Coordinate<double> pos = players[i]->position;
        f(players[i]);

        if(players[i]->mustDelete){
            delete players[i];
            removed.push_back(players[i]);
            players.erase(players.begin() + i);
            i--;
        }
        else if(players[i]->mustRemove){
            removed.push_back(players[i]);
            players.erase(players.begin() + i);
            i--;
        }
        else if(players[i]->position != pos){
            moved.push_back(make_pair(players[i], pos));
        }
    }

    /*
    for(auto it = players.begin(); it != players.end();){
        f(*it);
        if((*it)->mustDelete){
            delete *it;
            removed.push_back(*it);
            it = players.erase(it);
        }
        else if((*it)->mustRemove){
            removed.push_back(*it);
            it = players.erase(it);
        }
        else{
            it++;
        }
    }
    */

    for(Positional* p : removed){
        tree.remove(p);
        remFromAllEnts((Entity*) p);
    }

    int size = moved.size();
    for(int i=0; i<size; i++){
        tree.fixPos(moved[i].first, moved[i].second);
    }
}

void EntityStore::executeFunctorOutsidePlayers(Functor<PlayerEntity*> &f){
    for(auto it = outsidePlayers.begin(); it != outsidePlayers.end();){
        f(*it);
        // mustDelete is supported, but NOT recommended for outsidePlayers
        if((*it)->mustDelete){
            delete *it;
            it = outsidePlayers.erase(it);
        }
        else if((*it)->mustRemove){
            it = outsidePlayers.erase(it);
        }
        else{
            it++;
        }
    }
}

void EntityStore::execFuncInRadius(Coordinate<double> pos, double r, Functor<Entity*> &f){
    FunctorAdapter<Entity*> f2(f);

    tree.execFuncInRadius(pos, r, f2);
    removeFromF(f2);
}

void EntityStore::execFuncInChunk(ChunkCoord cCoord, Functor<Entity*> &f){
    FunctorAdapter<Entity*> f2(f);
    Coordinate<double> bound1(cCoord.x*16, 0, cCoord.z*16);
    Coordinate<double> bound2((cCoord.x+1)*16, 256, (cCoord.z+1)*16);
    tree.execInBounds(bound1, bound2, f2);
    removeFromF(f2);
}

void EntityStore::execFuncInBounds(Coordinate<double> lc, Coordinate<double> rc, Functor<Entity*> &f){
    FunctorAdapter<Entity*> f2(f);

    tree.execInBounds(lc,rc, f2);
    removeFromF(f2);
}


vector<PlayerEntity*> EntityStore::getAllPlayers(){
    vector<PlayerEntity*> result = players;
    return result;
}

vector<PlayerEntity*> EntityStore::getAllPlayers(vector<ChunkCoord> chunks){
    vector<PlayerEntity*> result;

    int size = players.size();
    for(int i = 0; i<size; i++){
        PlayerEntity* p = players[i];
        ChunkCoord c = p->position.getContainingChunk();
        auto it = find(chunks.begin(), chunks.end(), c);
        if(it != chunks.end())
            result.push_back(p);
    }

    return result;
}

vector<Entity*> EntityStore::getInsideChunks(vector<ChunkCoord> chunks){
    GetAllF f;

    for(ChunkCoord cCoord : chunks){
        Coordinate<double> bound1(cCoord.x*16, 0, cCoord.z*16);
        Coordinate<double> bound2((cCoord.x+1)*16, 256, (cCoord.z+1)*16);
        tree.execInBounds(bound1, bound2, f);
    }

    int size = f.values.size();
    vector<Entity*> result;
    if(size <= 0)
        return result;

    result.resize(size);
    for(int i = 0; i<size; i++){
        result[i] = (Entity*) f.values[i];
    }
    return result;
}
