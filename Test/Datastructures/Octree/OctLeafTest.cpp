#include <gtest/gtest.h>

#include <vector>

#include "OctreeTestHelpers.cpp"
#include "../../../src/Datastructures/Octree/OctLeaf.h"

using namespace std;

class OctLeafTest : public ::testing::Test{
protected:
    ns_oc::DummyParent *parent = 0;
    OctNode *leaf = new OctLeaf(Coordinate<double>(0.0,0.0,0.0),
            Coordinate<double>(8.0,8.0,8.0), parent);

    // Get threshold in an easily accessible variable
    inline static int threshold;
    static void SetUpTestCase(){
        OctLeaf temp(Coordinate<double>(), Coordinate<double>(), 0);
        threshold = temp.threshold;
    };

    virtual void SetUp(){
        parent = new ns_oc::DummyParent(
            Coordinate<double>(0.0,0.0,0.0), Coordinate<double>(16.0,16.0,16.0));

        leaf = new OctLeaf(Coordinate<double>(0.0,0.0,0.0),
                Coordinate<double>(8.0,8.0,8.0), parent);
    };

    virtual void TearDown(){
        delete parent;
        delete leaf;
    };

};

TEST_F(OctLeafTest, insert1){
    ns_oc::PosCounter* pos = new ns_oc::PosCounter();
    pos->position = Coordinate<double>(1.0, 1.0, 1.0);

    leaf->insert(pos, false);

    ns_oc::FuncCounter functor;
    leaf->execFunc(functor, false);

    ASSERT_TRUE(pos->counter == 1) << "Didn't execute on pos";
    ASSERT_TRUE(functor.counter == 1) << "Total num of executions wrong";

    delete pos;
}

TEST_F(OctLeafTest, insert2){
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);

    ns_oc::FuncCounter functor;
    leaf->execFunc(functor, false);

    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos";
    ASSERT_TRUE(pos2->counter == 1) << "Didn't execute on pos";
    ASSERT_TRUE(functor.counter == 2) << "Total num of executions wrong";

    delete pos1;
    delete pos2;
}

TEST_F(OctLeafTest, insert3){
    // Up to threshold

    vector<ns_oc::PosCounter*> pos(threshold, 0);
    for(int i=0; i<threshold; i++){
        pos[i] = new ns_oc::PosCounter();
        double x = ((double)i)/threshold;
        pos[i]->position = Coordinate<double>(x, 1, 1);

        leaf->insert(pos[i], false);
    }

    ns_oc::FuncCounter functor;
    leaf->execFunc(functor, false);

    for(int i=0; i<threshold; i++)
        ASSERT_TRUE(pos[i]->counter == 1) << "Didn't execute on pos " << i;


    ASSERT_TRUE(functor.counter == threshold) << "Total num of executions wrong";

    for(int i=0; i<threshold; i++){
        delete pos[i];
    }
}

TEST_F(OctLeafTest, insert4){
    // One over threshold (non-unique)
    vector<ns_oc::PosCounter*> pos(threshold + 1, 0);
    for(int i=0; i<threshold; i++){
        pos[i] = new ns_oc::PosCounter();

        double x = ((double)i)/threshold;
        pos[i]->position = Coordinate<double>(x, 1, 1);

        leaf->insert(pos[i], false);
    }
    // Last two have the same position
    pos[threshold] = new ns_oc::PosCounter();
    pos[threshold]->position = pos[threshold-1]->position;
    leaf->insert(pos[threshold], false);

    ns_oc::FuncCounter functor;
    leaf->execFunc(functor, false);

    for(int i=0; i<threshold + 1; i++)
        ASSERT_TRUE(pos[i]->counter == 1) << "Didn't execute on pos " << i;


    ASSERT_TRUE(functor.counter == threshold + 1) << "Total num of executions wrong";

    for(int i=0; i<threshold + 1; i++){
        delete pos[i];
    }
}

TEST_F(OctLeafTest, insert5){
    // Over the threshold (unique)

    OctNode* leafStart = leaf;

    vector<ns_oc::PosCounter*> pos(threshold, 0);
    for(int i=0; i<threshold + 1; i++){
        pos[i] = new ns_oc::PosCounter();
        double x = ((double)i)/(threshold + 1);
        pos[i]->position = Coordinate<double>(x, 1, 1);

        leaf = leaf->insert(pos[i], false);
    }

    ASSERT_TRUE(leafStart != leaf) << "Leaf didn't split";

    // NOTE: From here the test slightly overlaps with OctInternal
    ns_oc::FuncCounter functor;
    leaf->execFunc(functor, false);

    for(int i=0; i<threshold + 1; i++)
        ASSERT_TRUE(pos[i]->counter == 1) << "Didn't execute on pos " << i;


    ASSERT_TRUE(functor.counter == threshold + 1) << "Total num of executions wrong";

    for(int i=0; i<threshold; i++){
        delete pos[i];
    }
}

TEST_F(OctLeafTest, insert6){
    // Up to threshold

    vector<ns_oc::PosCounter*> pos(threshold, 0);
    for(int i=0; i<threshold; i++){
        pos[i] = new ns_oc::PosCounter();
        pos[i]->position = Coordinate<double>(1, 1, 1);

        leaf->insert(pos[i], false);
    }

    ns_oc::FuncCounter functor;
    leaf->execFunc(functor, false);

    for(int i=0; i<threshold; i++)
        ASSERT_TRUE(pos[i]->counter == 1) << "Didn't execute on pos " << i;


    ASSERT_TRUE(functor.counter == threshold) << "Total num of executions wrong";

    for(int i=0; i<threshold; i++){
        delete pos[i];
    }
}

TEST_F(OctLeafTest, execFunc1){
    ns_oc::PosCounter* pos = new ns_oc::PosCounter();
    pos->position = Coordinate<double>(1.0, 1.0, 1.0);

    leaf->insert(pos, false);

    ns_oc::FuncCounter functor;
    leaf->execFunc(functor, false);
    leaf->execFunc(functor, true);

    ASSERT_TRUE(pos->counter == 2) << "Didn't execute on pos";
    ASSERT_TRUE(functor.counter == 2) << "Total num of executions wrong";

    delete pos;
}

TEST_F(OctLeafTest, execFunc2){
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, true);

    ns_oc::FuncCounter functor;
    leaf->execFunc(functor, false);

    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos";
    ASSERT_TRUE(pos2->counter == 0) << "Executed on wrong flag";
    ASSERT_TRUE(functor.counter == 1) << "Total num of executions wrong";

    delete pos1;
    delete pos2;
}
