#include <gtest/gtest.h>

#include <vector>

#include "ESHelpers.cpp"

using namespace std;

class EntityStoreGetTest :public ::testing::TestWithParam<tuple<
        vector<Coordinate<double>>, //Entity starting positions
        vector<Coordinate<double>>, //player starting positions
        vector<ChunkCoord> //Which chunks to request
    >>{
public:
    EntityStore store;

    vector<ns_es::DummyEntity*> entities;
    vector<ns_es::DummyPlayer*> players;
    int e = 0;
    int p = 0;

    vector<Coordinate<double>> posE;
    vector<Coordinate<double>> posP;

    vector<ChunkCoord> toRequest;
    int playersInChunks = 0;
    int entitiesInChunks = 0;

    virtual void SetUp(){
        auto params = GetParam();
        posE = get<0>(params);
        posP = get<1>(params);
        toRequest = get<2>(params);

        e = posE.size();
        p = posP.size();

        entities = vector<ns_es::DummyEntity*>(e, 0);
        for(int i=0; i<e; i++){
            entities[i] = makeEntity(posE[i], i);
            store.addEntity(entities[i]);
        }

        players = vector<ns_es::DummyPlayer*>(p, 0);
        for(int i=0; i<p; i++){
            players[i] = makePlayerEntity(posP[i], i);
            store.addEntity(players[i]);
        }

    };

    virtual void TearDown(){
        for(int i=0; i<e; i++){
            delete entities[i];
        }
        for(int i=0; i<p; i++){
            delete players[i];
        }
    };

    ns_es::DummyEntity* makeEntity(Coordinate<double> pos, int n){
        ns_es::DummyEntity* result = new ns_es::DummyEntity(pos, n*2);
        return result;
    }

    ns_es::DummyPlayer* makePlayerEntity(Coordinate<double> pos, int n){
        ns_es::DummyPlayer* result = new ns_es::DummyPlayer(pos, n*2 + 1);
        return result;
    }

    void calcInChunks(){
        for(int i=0; i<e; i++){
            ChunkCoord target = posE[i].getContainingChunk();

            for(int j=0 ; j<toRequest.size(); j++){
                if(toRequest[j] == target){
                    entitiesInChunks++;
                    break;
                }
            }
        }

        for(int i=0; i<p; i++){
            ChunkCoord target = posP[i].getContainingChunk();

            for(int j=0 ; j<toRequest.size(); j++){
                if(toRequest[j] == target){
                    playersInChunks++;
                    break;
                }
            }
        }
    }

};

TEST_P(EntityStoreGetTest, getAllPlayers){
    vector<PlayerEntity*> result = store.getAllPlayers();

    ASSERT_TRUE(result.size() == p) << "Wrong number of players returned";

    for(int i=0; i<p; i++){
        bool found = false;
        for(int j=0; j<p; j++){
            if(players[i] == result[j]){
                found = true;
                break;
            }
        }

        ASSERT_TRUE(found) << "Player " << i << " not found";
    }
}

TEST_P(EntityStoreGetTest, getSomePlayers){
    calcInChunks();
    vector<PlayerEntity*> result = store.getAllPlayers(toRequest);

    ASSERT_TRUE(result.size() == playersInChunks) << "Wrong number of entities returned";

    for(int i=0; i<p; i++){
        bool found = false;
        bool returned = false;
        ChunkCoord target = posP[i].getContainingChunk();

        for(int j=0; j<toRequest.size(); j++){
            if(target == toRequest[j]){
                found = true;
                break;
            }
        }
        for(int j=0; j<result.size(); j++){
            if(players[i] == result[j]){
                returned = true;
                break;
            }
        }

        ASSERT_TRUE(found == returned) << "Returned-expect mismatch";
    }
}

TEST_P(EntityStoreGetTest, getInsideChunks){
    calcInChunks();
    vector<Entity*> result = store.getInsideChunks(toRequest);

    ASSERT_TRUE(result.size() == playersInChunks + entitiesInChunks)
        << "Wrong number of entities returned";

    for(int i=0; i<e; i++){
        bool found = false;
        bool returned = false;
        ChunkCoord target = posE[i].getContainingChunk();

        for(int j=0; j<toRequest.size(); j++){
            if(target == toRequest[j]){
                found = true;
                break;
            }
        }
        for(int j=0; j<result.size(); j++){
            if(entities[i] == result[j]){
                returned = true;
                break;
            }
        }

        ASSERT_TRUE(found == returned) << "Returned-expect mismatch";
    }

    for(int i=0; i<p; i++){
        bool found = false;
        bool returned = false;
        ChunkCoord target = posP[i].getContainingChunk();

        for(int j=0; j<toRequest.size(); j++){
            if(target == toRequest[j]){
                found = true;
                break;
            }
        }
        for(int j=0; j<result.size(); j++){
            if(players[i] == result[j]){
                returned = true;
                break;
            }
        }

        ASSERT_TRUE(found == returned) << "Returned-expect mismatch";
    }
}

INSTANTIATE_TEST_CASE_P(EntityStoreGet1, EntityStoreGetTest,
    ::testing::Values(
        make_tuple(
            vector<Coordinate<double>>(),
            vector<Coordinate<double>>{
                Coordinate<double>(0,0,0),
                Coordinate<double>(0,5,0)},
            vector<ChunkCoord>()
        ),
        make_tuple(
            vector<Coordinate<double>>{
                Coordinate<double>(1,1,1),
                Coordinate<double>(1,1,0),
            },
            vector<Coordinate<double>>{
                Coordinate<double>(0,0,0),
                Coordinate<double>(0,5,0)},
            vector<ChunkCoord>()
        ),
        make_tuple(
            vector<Coordinate<double>>{
                Coordinate<double>(1,1,1),
                Coordinate<double>(1,1,0),
            },
            vector<Coordinate<double>>{
                Coordinate<double>(0,0,0),
                Coordinate<double>(0,5,0)},
            vector<ChunkCoord>{
                ChunkCoord(0,0)
            }
        ),
        make_tuple(
            vector<Coordinate<double>>{
                Coordinate<double>(1,1,1),
                Coordinate<double>(1,1,0),
            },
            vector<Coordinate<double>>{
                Coordinate<double>(0,0,0),
                Coordinate<double>(0,5,0)},
            vector<ChunkCoord>{
                ChunkCoord(1,0)
            }
        ),
        make_tuple(
            vector<Coordinate<double>>{
                Coordinate<double>(20,1,1),
                Coordinate<double>(1,1,0),
            },
            vector<Coordinate<double>>{
                Coordinate<double>(0,0,0),
                Coordinate<double>(20,5,0)},
            vector<ChunkCoord>{
                ChunkCoord(1,0)
            }
        )
    ));
