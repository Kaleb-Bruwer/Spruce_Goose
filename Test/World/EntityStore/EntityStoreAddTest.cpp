#include <gtest/gtest.h>

#include <vector>

#include "ESHelpers.cpp"

using namespace std;

class EntityStoreAddTest :public ::testing::TestWithParam<tuple<int, int>>{
public:
    EntityStore store;

    vector<ns_es::DummyEntity*> entities;
    vector<ns_es::DummyPlayer*> players;
    int e = 0;
    int p = 0;


    virtual void SetUp(){
        auto params = GetParam();
        e = get<0>(params);
        p = get<1>(params);

        entities = vector<ns_es::DummyEntity*>(e, 0);
        for(int i=0; i<e; i++){
            entities[i] = makeEntity(i);
            store.addEntity(entities[i]);
        }

        players = vector<ns_es::DummyPlayer*>(p, 0);
        for(int i=0; i<p; i++){
            players[i] = makePlayerEntity(i);
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

    ns_es::DummyEntity* makeEntity(int n){
        ns_es::DummyEntity* result = new ns_es::DummyEntity(n*2);
        return result;
    }

    ns_es::DummyPlayer* makePlayerEntity(int n){
        ns_es::DummyPlayer* result = new ns_es::DummyPlayer(n*2 + 1);
        return result;
    }

};

TEST_P(EntityStoreAddTest, getByEid){

    // Check that getByEid returns correct value
    for(int i=0; i<e; i++){
        int eid = entities[i]->getEid();
        Entity* val = store.getByEid(eid);
        ASSERT_TRUE(val == entities[i]) << "Wrong entity returned";
    }
    for(int i=0; i<p; i++){
        int eid = players[i]->getEid();
        Entity* val = store.getByEid(eid);
        ASSERT_TRUE(val == players[i]) << "Wrong entity returned";
    }
}

TEST_P(EntityStoreAddTest, execAll){
    ns_es::FuncCounterEntity functor;
    store.executeFunctorAll(functor);
    ASSERT_TRUE(functor.counter == e + p) << "Total number of executions wrong";

    for(int i=0; i<e; i++){
        ASSERT_TRUE(entities[i]->counter > 0) << "Entity skipped over";
        ASSERT_TRUE(entities[i]->counter <= 1) << "Entity executed multiple time";
    }

    for(int i=0; i<p; i++){
        ASSERT_TRUE(players[i]->counter > 0) << "Player skipped over";
        ASSERT_TRUE(players[i]->counter <= 1) << "Player executed multiple time";
    }
}

TEST_P(EntityStoreAddTest, execPlayers){
    ns_es::FuncCounterPlayer functor;
    store.executeFunctorPlayers(functor);
    ASSERT_TRUE(functor.counter == p) << "Total number of executions wrong";

    for(int i=0; i<e; i++){
        ASSERT_TRUE(entities[i]->counter == 0) << "Entity executed";
    }

    for(int i=0; i<p; i++){
        ASSERT_TRUE(players[i]->counter > 0) << "Player skipped over";
        ASSERT_TRUE(players[i]->counter <= 1) << "Player executed multiple time";
    }
}


INSTANTIATE_TEST_CASE_P(EntityStoreAdd1, EntityStoreAddTest,
    ::testing::Values(
        make_tuple(0,0),
        make_tuple(1,0),
        make_tuple(0,1),
        make_tuple(1,1),
        make_tuple(5,5)
    ));
