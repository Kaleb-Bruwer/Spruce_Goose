#include <gtest/gtest.h>

#include "OctreeTestHelpers.cpp"
#include "../../../src/Datastructures/Octree/OctRoot.h"

using namespace std;

class OctRootInsertTest :public ::testing::TestWithParam<int>{
public:
    OctRoot root;
    virtual void SetUp(){};

    virtual void TearDown(){};

};

TEST_P(OctRootInsertTest, insert1){
    // Test that a new insert remains accessible
    int sector = GetParam();

    ns_oc::PosCounter* pos = new ns_oc::PosCounter();
    Coordinate<double> temp = Coordinate<double>((bool)(sector & 1),
        (bool)(sector & 2), (bool)(sector & 4));
    pos->position = temp*4 - Coordinate<double>(2,2,2);

    // cout << pos->position << endl;

    root.insert(pos, false);

    ns_oc::FuncCounter functor;

    root.execFunc(functor, false);

    ASSERT_TRUE(pos->counter == 1)
        << "pos became unreachable after insert at " << pos->position;
}

TEST_P(OctRootInsertTest, insert2){
    // Insert in existing child passed on correctly
    int sector = GetParam();

    ns_oc::PosCounter* pos = new ns_oc::PosCounter();
    Coordinate<double> temp = Coordinate<double>((bool)(sector & 1),
        (bool)(sector & 2), (bool)(sector & 4));
    pos->position = temp*4 - Coordinate<double>(2,2,2);

    Coordinate<double> low((bool)(sector & 1), (bool)(sector & 2),
        (bool)(sector & 4));
    low = low - Coordinate<double>(1,1,1);
    low = low*256;
    Coordinate<double> high = low + Coordinate<double>(256,256,256);

    // cout << "[" << sector << "] " << low << ",  " << high << endl;

    ns_oc::DummyNode* dummy = new ns_oc::DummyNode(low, high);
    root.children[sector] = dummy;

    root.insert(pos, false);

    ASSERT_TRUE(dummy->pos.size() == 1) << "Didn't insert";
    ASSERT_TRUE(dummy->pos[0] == pos) << "Didn't insert";

    delete pos;
    delete dummy;
}

TEST_P(OctRootInsertTest, insert3){
    // Trigger expandChild
    int sector = GetParam();

    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    Coordinate<double> temp = Coordinate<double>((bool)(sector & 1),
        (bool)(sector & 2), (bool)(sector & 4));
    pos1->position = temp*4 - Coordinate<double>(2,2,2);

    pos2->position = pos1->position * 128;
    // cout << pos->position << endl;

    root.insert(pos1, false);
    root.insert(pos2, false);

    ns_oc::FuncCounter functor;
    root.execFunc(functor, false);

    ASSERT_TRUE(pos1->counter == 1) << "pos became unreachable";
    ASSERT_TRUE(pos2->counter == 1) << "pos became unreachable";
}

TEST_P(OctRootInsertTest, insert4){
    // Trigger expandChild
    int sector = GetParam();

    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    Coordinate<double> temp = Coordinate<double>((bool)(sector & 1),
        (bool)(sector & 2), (bool)(sector & 4));
    pos1->position = temp*4 - Coordinate<double>(2,2,2);

    pos2->position = pos1->position * 256;
    // cout << pos->position << endl;

    root.insert(pos2, false);
    root.insert(pos1, false);

    ns_oc::FuncCounter functor;
    root.execFunc(functor, false);

    ASSERT_TRUE(pos1->counter == 1) << "pos became unreachable";
    ASSERT_TRUE(pos2->counter == 1) << "pos became unreachable";
}

TEST_P(OctRootInsertTest, insert5){
    // Trigger expandChild
    int sector = GetParam();

    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos3 = new ns_oc::PosCounter();
    Coordinate<double> temp = Coordinate<double>((bool)(sector & 1),
        (bool)(sector & 2), (bool)(sector & 4));
    pos1->position = temp*4 - Coordinate<double>(2,2,2);

    pos2->position = pos1->position * 256;
    pos3->position = pos2->position;
    // cout << pos->position << endl;

    root.insert(pos2, false);
    root.insert(pos1, false);
    root.insert(pos3, false);

    ns_oc::FuncCounter functor;
    root.execFunc(functor, false);

    ASSERT_TRUE(pos1->counter == 1) << "pos became unreachable";
    ASSERT_TRUE(pos2->counter == 1) << "pos became unreachable";
    ASSERT_TRUE(pos3->counter == 1) << "pos became unreachable";
}

INSTANTIATE_TEST_CASE_P(OctRootInsert1, OctRootInsertTest,
        ::testing::Values(0,1,2,3,4,5,6,7));
