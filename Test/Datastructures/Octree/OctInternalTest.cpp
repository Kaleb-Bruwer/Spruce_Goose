#include <gtest/gtest.h>

#include <vector>

#include "OctreeTestHelpers.cpp"
#include "../../../src/Datastructures/Octree/OctInternal.h"

using namespace std;

class OctInternalTest : public ::testing::Test{
protected:
    ns_oc::DummyNode *parent = 0;
    OctInternal *node = 0;

    // Get threshold in an easily accessible variable
    inline static int threshold;
    static void SetUpTestCase(){
        OctInternal temp(Coordinate<double>(), Coordinate<double>(), 0);
        threshold = temp.threshold;
    };

    virtual void SetUp(){
        parent = new ns_oc::DummyNode(
            Coordinate<double>(0.0,0.0,0.0), Coordinate<double>(16.0,16.0,16.0));

        node = new OctInternal(Coordinate<double>(0.0,0.0,0.0),
                Coordinate<double>(8.0,8.0,8.0), parent);
    };

    virtual void TearDown(){
        delete parent;
        if(node)
            delete node;
    };

};

class OctInternalInsertTest :public ::testing::TestWithParam<int>{
public:
    OctInternal *node = 0;
    virtual void SetUp(){
        node = new OctInternal(Coordinate<double>(0.0,0.0,0.0),
                Coordinate<double>(8.0,8.0,8.0), 0);
    };

    virtual void TearDown(){
        if(node)
            delete node;
    };
};

class OctInternalExecRadiusTest : public
    ::testing::TestWithParam<tuple<Coordinate<double>, double, unsigned char>>{
public:
    OctInternal *node = 0;
    virtual void SetUp(){
        node = new OctInternal(Coordinate<double>(0.0,0.0,0.0),
                Coordinate<double>(8.0,8.0,8.0), 0);
    };

    virtual void TearDown(){
        if(node)
            delete node;
    };
};

class OctInternalExecBoxTest : public
    ::testing::TestWithParam<tuple<Coordinate<double>, Coordinate<double>, unsigned char>>{
public:
    OctInternal *node = 0;
    virtual void SetUp(){
        node = new OctInternal(Coordinate<double>(0.0,0.0,0.0),
                Coordinate<double>(8.0,8.0,8.0), 0);
    };

    virtual void TearDown(){
        if(node)
            delete node;
    };
};

TEST_F(OctInternalTest, insert1){
    ns_oc::PosCounter* pos = new ns_oc::PosCounter();
    pos->position = Coordinate<double>(1.0, 1.0, 1.0);

    node->insert(pos, false);

    ASSERT_TRUE(node->children[0] != 0) << "Didn't create leaf";

}

TEST_F(OctInternalTest, insert2){
    ns_oc::PosCounter* pos = new ns_oc::PosCounter();
    pos->position = Coordinate<double>(100.0, 1.0, 1.0);

    node->insert(pos, false);

    ASSERT_TRUE(parent->pos.size() > 0) << "Parent didn't receive pos";
    ASSERT_TRUE(parent->pos.size() == 1) << "Parent received wrong number of inserts";
}

TEST_F(OctInternalTest, remove1){
    ns_oc::PosCounter* pos = new ns_oc::PosCounter();
    pos->position = Coordinate<double>(1.0, 1.0, 1.0);

    ns_oc::DummyNode *dummy = new ns_oc::DummyNode(Coordinate<double>(0,0,0),
            Coordinate<double>(4,4,4));
    node->addChild(dummy);

    node = (OctInternal*) node->remove(pos);

    ASSERT_TRUE(dummy->pos.size() > 0) << "Parent didn't receive pos";
    ASSERT_TRUE(dummy->pos.size() == 1) << "Parent received wrong number of inserts";

    ASSERT_TRUE(node->children[0] == dummy) << "leaf removed";
}

TEST_F(OctInternalTest, remove2){
    ns_oc::PosCounter* pos = new ns_oc::PosCounter();
    pos->position = Coordinate<double>(1.0, 1.0, 1.0);

    ns_oc::DummyNode *dummy = new ns_oc::DummyNode(Coordinate<double>(0,0,0),
            Coordinate<double>(4,4,4));
    dummy->finalRemove = true;

    node->addChild(dummy);

    node = (OctInternal*)node->remove(pos);

    // This is still accessible since it's the leaf's responsibility to delete
    // itself
    ASSERT_TRUE(dummy->pos.size() > 0) << "Parent didn't receive pos";
    ASSERT_TRUE(dummy->pos.size() == 1) << "Parent received wrong number of inserts";

    ASSERT_TRUE(node == 0) << "node didn't delete itself";
}

TEST_F(OctInternalTest, remove3){
    ns_oc::PosCounter* pos = new ns_oc::PosCounter();
    pos->position = Coordinate<double>(1.0, 1.0, 1.0);

    ns_oc::DummyNode *dummy = new ns_oc::DummyNode(Coordinate<double>(0,0,0),
            Coordinate<double>(4,4,4));
    ns_oc::DummyNode *dummy2 = new ns_oc::DummyNode(Coordinate<double>(4,0,0),
            Coordinate<double>(8,4,4));
    dummy->finalRemove = true;

    node->addChild(dummy);
    node->addChild(dummy2);

    node = (OctInternal*)node->remove(pos);

    ASSERT_TRUE(node != 0) << "node deleted itself prematurely";
    ASSERT_TRUE(node->children[0] == 0) << "empty child";
    ASSERT_TRUE(node->children[1] != 0) << "deleted wrong child";
}

// Doing this paramaterized test for one test is enough to prove that index()
// and the children array work properly, doing it for everything is redundant
TEST_P(OctInternalInsertTest, insert){
    int sector = GetParam();

    Coordinate<double> low((bool)(sector & 1) * 4, (bool)(sector & 2) * 4, (bool)(sector & 4) * 4);
    Coordinate<double> high = low + Coordinate<double>(4,4,4);

    // cout << "sector " << sector << low << ",   " << high;

    ns_oc::DummyNode *dummy = new ns_oc::DummyNode(low, high);
    node->addChild(dummy);

    ASSERT_TRUE(node->children[sector] == dummy) << "Didn't create leaf";

    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    pos1->position = low + Coordinate<double>(1.0, 1.0, 1.0);

    node->insert(pos1, false);

    ASSERT_TRUE(dummy->pos.size() == 1) << "Didn't insert";
    ASSERT_TRUE(dummy->pos[0] == pos1) << "Inserted wrong element";

}

INSTANTIATE_TEST_CASE_P(OctInternalInsert1, OctInternalInsertTest,
        ::testing::Values(0,1,2,3,4,5,6,7));

TEST_F(OctInternalTest, exec1){

    ns_oc::DummyNode *dummies[8];
    for(int i=0; i<8; i++){
        Coordinate<double> low((bool)(i & 1) * 4, (bool)(i & 2) * 4, (bool)(i & 4) * 4);
        Coordinate<double> high = low + Coordinate<double>(4,4,4);

        dummies[i] = new ns_oc::DummyNode(low, high);
        node->addChild(dummies[i]);
    }

    ns_oc::FuncCounter functor;

    node->execFunc(functor, false);

    for(int i=0; i<8; i++){
        ASSERT_TRUE(dummies[i]->execCount == 1) << "Exec count mismatch at " << i;
    }


    for(int i=0; i<8;i++){
        delete dummies[i];
    }
}

TEST_F(OctInternalTest, exec2){

    ns_oc::DummyNode *dummies[3];
    for(int i=0; i<3; i++){
        Coordinate<double> low((bool)(i & 1) * 4, (bool)(i & 2) * 4, (bool)(i & 4) * 4);
        Coordinate<double> high = low + Coordinate<double>(4,4,4);

        dummies[i] = new ns_oc::DummyNode(low, high);
        node->addChild(dummies[i]);
    }

    ns_oc::FuncCounter functor;

    node->execFunc(functor, false);

    for(int i=0; i<3; i++){
        ASSERT_TRUE(dummies[i]->execCount == 1) << "Exec count mismatch at " << i;
    }


    for(int i=0; i<3;i++){
        delete dummies[i];
    }
}

TEST_P(OctInternalExecRadiusTest, exec){
    auto params = GetParam();
    Coordinate<double> origin = get<0>(params);
    double r = get<1>(params);
    unsigned char expected = get<2>(params);

    ns_oc::DummyNode *dummies[8];
    for(int i=0; i<8; i++){
        Coordinate<double> low((bool)(i & 1) * 4, (bool)(i & 2) * 4, (bool)(i & 4) * 4);
        Coordinate<double> high = low + Coordinate<double>(4,4,4);

        dummies[i] = new ns_oc::DummyNode(low, high);
        node->addChild(dummies[i]);
    }

    ns_oc::FuncCounter functor;
    node->execFuncInRadius(origin, r, functor, false);


    for(int i=0; i<8; i++){
        int count = 0;
        if((expected >> i) &1)
            count = 1;
        ASSERT_TRUE(dummies[i]->execCount == count) << "Exec count mismatch at " << i;
    }


    for(int i=0; i<8;i++){
        delete dummies[i];
    }

}

INSTANTIATE_TEST_CASE_P(OctInternalExecRadius1, OctInternalExecRadiusTest,
        ::testing::Values(make_tuple(Coordinate<double>(1,1,1), 5, 0b01111111),
                          make_tuple(Coordinate<double>(2,2,2), 1, 0b00000001),
                          make_tuple(Coordinate<double>(4,4,4), 3, 0b11111111),
                          make_tuple(Coordinate<double>(-1,-1,-1), 3, 0b00000001),
                          make_tuple(Coordinate<double>(-1,0,7), 3, 0b00010000),
                          make_tuple(Coordinate<double>(-1,0,70), 3, 0b00000000)

    ));


TEST_P(OctInternalExecBoxTest, exec){
    auto params = GetParam();
    Coordinate<double> low = get<0>(params);
    Coordinate<double> high = get<1>(params);
    unsigned char expected = get<2>(params);

    ns_oc::DummyNode *dummies[8];
    for(int i=0; i<8; i++){
        Coordinate<double> low((bool)(i & 1) * 4, (bool)(i & 2) * 4, (bool)(i & 4) * 4);
        Coordinate<double> high = low + Coordinate<double>(4,4,4);

        dummies[i] = new ns_oc::DummyNode(low, high);
        node->addChild(dummies[i]);
    }

    ns_oc::FuncCounter functor;
    node->execInBounds(low, high, functor, false);

    for(int i=0; i<8; i++){
        int count = 0;
        if((expected >> i) &1)
            count = 1;
        ASSERT_TRUE(dummies[i]->execCount == count) << "Exec count mismatch at " << i;
    }

    for(int i=0; i<8;i++){
        delete dummies[i];
    }

}

INSTANTIATE_TEST_CASE_P(OctInternalExecBox1, OctInternalExecBoxTest,
        ::testing::Values(
            make_tuple(Coordinate<double>(0,0,0), Coordinate<double>(8,8,8), 0b11111111),
            make_tuple(Coordinate<double>(0,0,0), Coordinate<double>(1,1,1), 0b00000001),
            make_tuple(Coordinate<double>(0,0,0), Coordinate<double>(3.9,3.9,3.9), 0b00000001),
            make_tuple(Coordinate<double>(8,0,0), Coordinate<double>(9,1,1), 0b00000000),
            make_tuple(Coordinate<double>(1,1,5), Coordinate<double>(5,1,6), 0b00110000),
            make_tuple(Coordinate<double>(-50,-10,-10), Coordinate<double>(3,3,3), 0b00000001),
            make_tuple(Coordinate<double>(0,0,0), Coordinate<double>(4,4,4), 0b11111111)

    ));
