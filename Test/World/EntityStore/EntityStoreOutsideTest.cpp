#include <gtest/gtest.h>

#include <vector>

#include "ESHelpers.cpp"

using namespace std;

class EntityStoreOutsideTest :public ::testing::TestWithParam<int>{
public:
    EntityStore store;

    vector<ns_es::DummyPlayer*> players;
    int p = 0;


    virtual void SetUp(){
        p = GetParam();

        players = vector<ns_es::DummyPlayer*>(p, 0);
        for(int i=0; i<p; i++){
            players[i] = makePlayerEntity(i);
            store.addExternalPlayer(players[i]);
        }

    };

    virtual void TearDown(){
        for(int i=0; i<p; i++){
            delete players[i];
        }
    };

    ns_es::DummyEntity* makeEntity(int n){
        ns_es::DummyEntity* result = new ns_es::DummyEntity(n*2);
    }

    ns_es::DummyPlayer* makePlayerEntity(int n){
        ns_es::DummyPlayer* result = new ns_es::DummyPlayer(n*2 + 1);
    }

};

TEST_P(EntityStoreOutsideTest, execOutside){

    ns_es::FuncCounterPlayer functor;

    store.executeFunctorOutsidePlayers(functor);

    ASSERT_TRUE(functor.counter == p) << "Wrong total execution count";

    for(int i=0; i<p; i++){
        ASSERT_TRUE(players[i]->counter == 1) << "Wrong execution count";
    }
}

INSTANTIATE_TEST_CASE_P(EntityStoreOutside1, EntityStoreOutsideTest,
    ::testing::Values(0,1,2,5));
