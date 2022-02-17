#include <gtest/gtest.h>

#include <vector>

#include "ESHelpers.cpp"

using namespace std;

class EntityStoreRemoveFunctorTest :public ::testing::TestWithParam<tuple<
        int,
        int,
        vector<int>, //entities to remove by index
        vector<int>  //players to remove by index
    >>{
public:
    EntityStore store;

    vector<ns_es::DummyEntity*> entities;
    vector<bool> eFlags;
    vector<ns_es::DummyPlayer*> players;
    vector<bool> pFlags;
    int e = 0;
    int p = 0;

    vector<int> mustRemoveE;
    vector<int> mustRemoveP;

    int numRemovedE = 0;
    int numRemovedP = 0;

    virtual void SetUp(){
        auto params = GetParam();
        e = get<0>(params);
        p = get<1>(params);
        mustRemoveE = get<2>(params);
        mustRemoveP = get<3>(params);

        numRemovedE = mustRemoveE.size();
        numRemovedP = mustRemoveP.size();

        entities = vector<ns_es::DummyEntity*>(e, 0);
        eFlags = vector<bool>(e,false);
        for(int i=0; i<e; i++){
            entities[i] = makeEntity(i);
            store.addEntity(entities[i]);
        }

        players = vector<ns_es::DummyPlayer*>(p, 0);
        pFlags = vector<bool>(p,false);
        for(int i=0; i<p; i++){
            players[i] = makePlayerEntity(i);
            store.addEntity(players[i]);
        }

        for(int i=0; i<mustRemoveE.size(); i++)
            eFlags[mustRemoveE[i]] = true;

        for(int i=0; i<mustRemoveP.size(); i++)
            pFlags[mustRemoveP[i]] = true;
    };

    virtual void TearDown(){
        for(int i=0; i<e; i++){
            if(!eFlags[i])
                delete entities[i];
        }
        for(int i=0; i<p; i++){
            if(!pFlags[i])
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

    void execRemFunc(){
        ns_es::FuncRemove functor;

        for(int i=0; i<entities.size(); i++){
            if(eFlags[i])
                functor.hitlist.push_back(entities[i]);
        }

        for(int i=0; i<players.size(); i++){
            if(pFlags[i])
                functor.hitlist.push_back(players[i]);
        }

        store.executeFunctorAll(functor);
    }

    void execRemFuncPlayers(){
        ns_es::FuncRemovePlayers functor;

        for(int i=0; i<players.size(); i++){
            if(pFlags[i])
                functor.hitlist.push_back(players[i]);
        }

        store.executeFunctorPlayers(functor);
    }

};

class EntityStoreRemoveOutsideFunctorTest :public ::testing::TestWithParam<tuple<
        int,
        vector<int>  //players to remove by index
    >>{
public:
    EntityStore store;

    vector<ns_es::DummyPlayer*> players;
    vector<bool> pFlags;
    int p = 0;

    int numRemovedP = 0;
    vector<int> mustRemoveP;

    virtual void SetUp(){
        auto params = GetParam();
        p = get<0>(params);
        mustRemoveP = get<1>(params);

        numRemovedP = mustRemoveP.size();

        players = vector<ns_es::DummyPlayer*>(p, 0);
        pFlags = vector<bool>(p,false);
        for(int i=0; i<p; i++){
            players[i] = makePlayerEntity(i);
            store.addExternalPlayer(players[i]);
        }

        for(int i=0; i<numRemovedP; i++){
            pFlags[mustRemoveP[i]] = true;
            store.deleteEntity(players[mustRemoveP[i]]);
        }

    };

    virtual void TearDown(){
        for(int i=0; i<p; i++){
            if(!pFlags[i])
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

    void execRemFunc(){
        ns_es::FuncRemovePlayers functor;

        for(int i=0; i<players.size(); i++){
            if(pFlags[i])
                functor.hitlist.push_back(players[i]);
        }

        store.executeFunctorOutsidePlayers(functor);
    }

};

TEST_P(EntityStoreRemoveFunctorTest, getByEid1){
    execRemFunc();

    // Check that getByEid returns correct value
    for(int i=0; i<e; i++){
        if(eFlags[i]){
            int eid = i * 2;
            Entity* val = store.getByEid(eid);
            ASSERT_TRUE(val == 0) << "Removed entity returned";
        }
    }
    for(int i=0; i<p; i++){
        if(pFlags[i]){
            int eid = i * 2 + 1;
            Entity* val = store.getByEid(eid);
            ASSERT_TRUE(val == 0) << "Removed player returned";
        }
    }
}

TEST_P(EntityStoreRemoveFunctorTest, getByEid2){
    execRemFuncPlayers();

    // Check that getByEid returns correct value
    for(int i=0; i<p; i++){
        if(pFlags[i]){
            int eid = i * 2 + 1;
            Entity* val = store.getByEid(eid);
            ASSERT_TRUE(val == 0) << "Removed player returned";
        }
    }
}

TEST_P(EntityStoreRemoveFunctorTest, execAll1){
    execRemFunc();

    ns_es::FuncCounterEntity functor;
    store.executeFunctorAll(functor);

    int expected = e + p - numRemovedE - numRemovedP;

    // EntityStoreAddTest already assures that nothing is skipped, so we can
    // assume that is the total matches, the right ones are executed
    ASSERT_TRUE(functor.counter == expected) << "Total number of executions wrong";
}

TEST_P(EntityStoreRemoveFunctorTest, execAll2){
    execRemFuncPlayers();

    ns_es::FuncCounterEntity functor;
    store.executeFunctorAll(functor);

    int expected = e + p - numRemovedP;

    // EntityStoreAddTest already assures that nothing is skipped, so we can
    // assume that is the total matches, the right ones are executed
    ASSERT_TRUE(functor.counter == expected) << "Total number of executions wrong";
}

TEST_P(EntityStoreRemoveFunctorTest, execPlayers1){
    execRemFunc();

    ns_es::FuncCounterPlayer functor;
    store.executeFunctorPlayers(functor);

    int expected = p - numRemovedP;

    ASSERT_TRUE(functor.counter == expected) << "Total number of executions wrong";
}

TEST_P(EntityStoreRemoveFunctorTest, execPlayers2){
    execRemFuncPlayers();

    ns_es::FuncCounterPlayer functor;
    store.executeFunctorPlayers(functor);

    int expected = p - numRemovedP;

    ASSERT_TRUE(functor.counter == expected) << "Total number of executions wrong";
}

INSTANTIATE_TEST_CASE_P(EntityStoreRemoveFunctor1, EntityStoreRemoveFunctorTest,
    ::testing::Values(
        make_tuple(1,0,
            vector<int>{0},
            vector<int>{}
        ),
        make_tuple(0,1,
            vector<int>{},
            vector<int>{0}
        ),
        make_tuple(1,1,
            vector<int>{0},
            vector<int>{}
        ),
        make_tuple(1,1,
            vector<int>{0},
            vector<int>{0}
        ),
        make_tuple(1,1,
            vector<int>{},
            vector<int>{0}
        ),
        make_tuple(5,5,
            vector<int>{0,2},
            vector<int>{1,3}
        )
    ));

TEST_P(EntityStoreRemoveOutsideFunctorTest, execOutside){
    ns_es::FuncCounterPlayer functor;

    store.executeFunctorOutsidePlayers(functor);

    ASSERT_TRUE(functor.counter == p - numRemovedP) << "Wrong total execution count";
}

INSTANTIATE_TEST_CASE_P(EntityStoreRemoveOutsideFunctor1, EntityStoreRemoveOutsideFunctorTest,
    ::testing::Values(
        make_tuple(1,
            vector<int>{0}
        ),
        make_tuple(5,
            vector<int>{1}
        ),
        make_tuple(5,
            vector<int>{1, 3, 4}
        )
    ));
