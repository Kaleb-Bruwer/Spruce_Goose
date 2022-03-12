#include <gtest/gtest.h>

#include <vector>

#include "OctreeTestHelpers.cpp"
#include "../../../src/Datastructures/Octree/OctLeaf.h"

using namespace std;

class OctLeafTest : public ::testing::Test{
protected:
    ns_oc::DummyNode *parent = 0;
    OctNode *leaf = 0;

    // Get threshold in an easily accessible variable
    inline static int threshold;
    static void SetUpTestCase(){
        OctLeaf temp(Coordinate<double>(), Coordinate<double>(), 0);
        threshold = temp.threshold;
    };

    virtual void SetUp(){
        parent = new ns_oc::DummyNode(
            Coordinate<double>(0.0,0.0,0.0), Coordinate<double>(16.0,16.0,16.0));

        leaf = new OctLeaf(Coordinate<double>(0.0,0.0,0.0),
                Coordinate<double>(8.0,8.0,8.0), parent);
    };

    virtual void TearDown(){
        delete parent;
        if(leaf)
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
    // Up to threshold, all on same position

    vector<ns_oc::PosCounter*> pos(threshold, 0);
    for(int i=0; i<threshold; i++){
        pos[i] = new ns_oc::PosCounter();
        pos[i]->position = Coordinate<double>(1, 1, 1);

        leaf = leaf->insert(pos[i], false);
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

TEST_F(OctLeafTest, remove1){
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);

    leaf = leaf->remove(pos2);

    ns_oc::FuncCounter functor;
    leaf->execFunc(functor, false);

    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos1";
    ASSERT_TRUE(pos2->counter == 0) << "Executed on removed val";
    ASSERT_TRUE(functor.counter == 1) << "Total num of executions wrong";

    delete pos1;
    delete pos2;
}

TEST_F(OctLeafTest, remove2){
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);

    leaf->insert(pos1, false);

    leaf = leaf->remove(pos1);

    ASSERT_TRUE(leaf == 0) << "Empty leaf didn't delete";
    delete pos1;
}

TEST_F(OctLeafTest, execFunc1){
    // Execute twice
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
    // Ignore pos with wrong flag
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

TEST_F(OctLeafTest, execFunc3){
    // Element moves out of bounds
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);

    // Move pos2 out of leaf's bounds
    ns_oc::FuncPosChange functor;
    functor.target.push_back(pos2);
    functor.destination.push_back(Coordinate<double>(12, 1, 1));

    leaf->execFunc(functor, false);

    ASSERT_TRUE(parent->pos.size() == 1) << "Wrong number of removed elements";
    ASSERT_TRUE(parent->pos[0] == pos2) << "Removed wrong element";

    delete pos1;
    delete pos2;
}

TEST_F(OctLeafTest, execFunc4){
    // remove element
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);

    ns_oc::FuncPosRemove functor;
    functor.target.push_back(pos2);

    leaf->execFunc(functor, false);

    ASSERT_TRUE(pos2->mustRemove == false) << "Didn't reset mustRemove flag";

    ns_oc::FuncCounter functor2;
    leaf->execFunc(functor2, true);


    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos1";
    ASSERT_TRUE(pos2->counter == 0) << "Executed on removed element";
    ASSERT_TRUE(functor2.counter == 1) << "Total num of executions wrong";

    delete pos1;
    delete pos2;
}

TEST_F(OctLeafTest, execFunc5){
    // delete element
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);

    ns_oc::FuncPosDelete functor;
    functor.target.push_back(pos2);

    leaf->execFunc(functor, false);

    ASSERT_TRUE(pos2->mustRemove == false) << "Didn't reset mustRemove flag";

    ns_oc::FuncCounter functor2;
    leaf->execFunc(functor2, true);


    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos1";
    ASSERT_TRUE(functor2.counter == 1) << "Total num of executions wrong";

    delete pos1;
}

TEST_F(OctLeafTest, execBox1){
    // Test in and out bounds
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos3 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);
    pos3->position = Coordinate<double>(1.5, 1.0, 3.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);
    leaf->insert(pos3, false);


    ns_oc::FuncCounter functor;
    pair<Coordinate<double>, Coordinate<double>> resetBounds;
    resetBounds = leaf->execInBounds(Coordinate<double>(0.5, 0.9, 0.9),
        Coordinate<double>(1.6, 1.1, 1.1), functor, false);


    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos1";
    ASSERT_TRUE(pos2->counter == 1) << "Didn't execute on pos2";
    ASSERT_TRUE(pos3->counter == 0) << "Executed outside box";
    ASSERT_TRUE(functor.counter == 2) << "Total num of executions wrong";

    // Test that flags are reset properly

    leaf->resetFlagsInBounds(resetBounds.first, resetBounds.second, false);
    leaf->execFunc(functor, false);

    ASSERT_TRUE(pos1->counter == 2) << "Didn't execute on pos1";
    ASSERT_TRUE(pos2->counter == 2) << "Didn't execute on pos2";
    ASSERT_TRUE(pos3->counter == 1) << "Executed outside box";
    ASSERT_TRUE(functor.counter == 5) << "Total num of executions wrong";

    delete pos1;
    delete pos2;
    delete pos3;
}

TEST_F(OctLeafTest, execBox2){
    // Test in and out bounds (upper bound should be excluded)
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos3 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);
    pos3->position = Coordinate<double>(1.5, 1.0, 3.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);
    leaf->insert(pos3, false);


    ns_oc::FuncCounter functor;
    pair<Coordinate<double>, Coordinate<double>> resetBounds;
    resetBounds = leaf->execInBounds(Coordinate<double>(0.5, 0.9, 0.9),
        Coordinate<double>(1.6, 1.1, 3.0), functor, false);


    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos1";
    ASSERT_TRUE(pos2->counter == 1) << "Didn't execute on pos2";
    ASSERT_TRUE(pos3->counter == 0) << "Executed outside box";
    ASSERT_TRUE(functor.counter == 2) << "Total num of executions wrong";

    delete pos1;
    delete pos2;
    delete pos3;
}

TEST_F(OctLeafTest, execBox3){
    // Move elements
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos3 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);
    pos3->position = Coordinate<double>(1.5, 1.0, 3.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);
    leaf->insert(pos3, false);

    ns_oc::FuncPosChange functor;
    functor.target.push_back(pos2);
    functor.destination.push_back(Coordinate<double>(2.0, 1.0, 1.0));

    pair<Coordinate<double>, Coordinate<double>> resetBounds;
    resetBounds = leaf->execInBounds(Coordinate<double>(0.5, 0.9, 0.9),
        Coordinate<double>(1.6, 1.1, 1.1), functor, false);

    ASSERT_TRUE(pos2->position == Coordinate<double>(2.0, 1.0, 1.0))
            << "Position didn't update, " << pos2->position;


    // Test that movement was applied properly
    leaf->resetFlagsInBounds(resetBounds.first, resetBounds.second, false);
    ns_oc::FuncCounter functor2;
    resetBounds = leaf->execInBounds(Coordinate<double>(0.5, 0.9, 0.9),
        Coordinate<double>(1.6, 1.1, 1.1), functor2, false);


    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos1 ";
    ASSERT_TRUE(pos2->counter == 0) << "Executed outside box";
    ASSERT_TRUE(pos3->counter == 0) << "Executed outside box";
    ASSERT_TRUE(functor2.counter == 1) << "Total num of executions wrong";

    delete pos1;
    delete pos2;
    delete pos3;
}

TEST_F(OctLeafTest, execBox4){
    // Move out of bounds
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos3 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);
    pos3->position = Coordinate<double>(1.5, 1.0, 3.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);
    leaf->insert(pos3, false);

    ns_oc::FuncPosChange functor;
    functor.target.push_back(pos2);
    functor.destination.push_back(Coordinate<double>(12, 1.0, 2.0));

    pair<Coordinate<double>, Coordinate<double>> resetBounds;
    resetBounds = leaf->execInBounds(Coordinate<double>(0.5, 0.9, 0.9),
        Coordinate<double>(1.6, 1.1, 1.1), functor, false);

    ASSERT_TRUE(parent->pos.size() == 1) << "Wrong number of removed elements: " << parent->pos.size();
    ASSERT_TRUE(parent->pos[0] == pos2) << "Removed wrong element";


    // Test that removal was applied properly
    leaf->resetFlagsInBounds(resetBounds.first, resetBounds.second, false);
    ns_oc::FuncCounter functor2;
    leaf->execFunc(functor2, false);


    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos1 ";
    ASSERT_TRUE(pos2->counter == 0) << "Executed on removed element";
    ASSERT_TRUE(pos3->counter == 1) << "Didn't execute on pos3";
    ASSERT_TRUE(functor2.counter == 2) << "Total num of executions wrong";

    delete pos1;
    delete pos2;
    delete pos3;
}

TEST_F(OctLeafTest, execBox5){
    // Remove element
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos3 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);
    pos3->position = Coordinate<double>(1.5, 1.0, 3.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);
    leaf->insert(pos3, false);

    ns_oc::FuncPosRemove functor;
    functor.target.push_back(pos2);

    pair<Coordinate<double>, Coordinate<double>> resetBounds;
    resetBounds = leaf->execInBounds(Coordinate<double>(0.5, 0.9, 0.9),
        Coordinate<double>(1.6, 1.1, 1.1), functor, false);

    ASSERT_TRUE(pos2->mustRemove == false) << "Didn't reset mustRemove";

    // Test that removal was applied properly
    leaf->resetFlagsInBounds(resetBounds.first, resetBounds.second, false);
    ns_oc::FuncCounter functor2;
    resetBounds = leaf->execInBounds(Coordinate<double>(0.5, 0.9, 0.9),
        Coordinate<double>(1.6, 1.1, 1.1), functor2, false);


    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos1 ";
    ASSERT_TRUE(pos2->counter == 0) << "Executed on removed element";
    ASSERT_TRUE(pos3->counter == 0) << "Didn't execute on pos3";
    ASSERT_TRUE(functor2.counter == 1) << "Total num of executions wrong";

    delete pos1;
    delete pos2;
    delete pos3;
}

TEST_F(OctLeafTest, execBox6){
    // Delete element
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos3 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);
    pos3->position = Coordinate<double>(1.5, 1.0, 3.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);
    leaf->insert(pos3, false);

    ns_oc::FuncPosDelete functor;
    functor.target.push_back(pos2);

    pair<Coordinate<double>, Coordinate<double>> resetBounds;
    resetBounds = leaf->execInBounds(Coordinate<double>(0.5, 0.9, 0.9),
        Coordinate<double>(1.6, 1.1, 1.1), functor, false);

    // Test that removal was applied properly
    leaf->resetFlagsInBounds(resetBounds.first, resetBounds.second, false);
    ns_oc::FuncCounter functor2;
    resetBounds = leaf->execInBounds(Coordinate<double>(0.5, 0.9, 0.9),
        Coordinate<double>(1.6, 1.1, 1.1), functor2, false);


    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos1 ";
    ASSERT_TRUE(pos3->counter == 0) << "Didn't execute on pos3";
    ASSERT_TRUE(functor2.counter == 1) << "Total num of executions wrong";

    delete pos1;
    delete pos3;
}

TEST_F(OctLeafTest, execRadius1){
    // Test in and out bounds
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos3 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);
    pos3->position = Coordinate<double>(1.5, 1.0, 3.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);
    leaf->insert(pos3, false);


    ns_oc::FuncCounter functor;

    double resetRadius;
    resetRadius = leaf->execFuncInRadius(Coordinate<double>(1,1,1), 2.0, functor, false);


    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos1";
    ASSERT_TRUE(pos2->counter == 1) << "Didn't execute on pos2";
    ASSERT_TRUE(pos3->counter == 0) << "Executed outside box";
    ASSERT_TRUE(functor.counter == 2) << "Total num of executions wrong";

    // Test that flags are reset properly

    leaf->resetFlagsInRadius(Coordinate<double>(1,1,1), resetRadius, false);
    leaf->execFunc(functor, false);

    ASSERT_TRUE(pos1->counter == 2) << "Didn't execute on pos1";
    ASSERT_TRUE(pos2->counter == 2) << "Didn't execute on pos2";
    ASSERT_TRUE(pos3->counter == 1) << "Executed outside box";
    ASSERT_TRUE(functor.counter == 5) << "Total num of executions wrong";

    delete pos1;
    delete pos2;
    delete pos3;
}

TEST_F(OctLeafTest, execRadius2){
    // Test moving element out of bounds
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos3 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);
    pos3->position = Coordinate<double>(1.5, 1.0, 3.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);
    leaf->insert(pos3, false);


    ns_oc::FuncPosChange functor;
    functor.target.push_back(pos2);
    functor.destination.push_back(Coordinate<double>(12, 1, 1));

    double resetRadius;
    resetRadius = leaf->execFuncInRadius(Coordinate<double>(1,1,1), 2.0, functor, false);

    ASSERT_TRUE(parent->pos.size() == 1) << "Wrong number of removed elements: " << parent->pos.size();
    ASSERT_TRUE(parent->pos[0] == pos2) << "Removed wrong element";

    // Test that flags are reset properly

    ns_oc::FuncCounter functor2;

    leaf->resetFlagsInRadius(Coordinate<double>(1,1,1), resetRadius, false);
    leaf->execFunc(functor2, false);

    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos1";
    ASSERT_TRUE(pos2->counter == 0) << "Executed outside box";
    ASSERT_TRUE(pos3->counter == 1) << "Didn't execute on pos3";
    ASSERT_TRUE(functor2.counter == 2) << "Total num of executions wrong";

    delete pos1;
    delete pos2;
    delete pos3;
}

TEST_F(OctLeafTest, execRadius3){
    // Test removing an element
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos3 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);
    pos3->position = Coordinate<double>(1.5, 1.0, 3.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);
    leaf->insert(pos3, false);


    ns_oc::FuncPosRemove functor;
    functor.target.push_back(pos2);

    double resetRadius;
    resetRadius = leaf->execFuncInRadius(Coordinate<double>(1,1,1), 2.0, functor, false);

    ASSERT_TRUE(pos2->mustRemove == false) << "Didn't reset mustRemove flag";

    // Test that flags are reset properly
    ns_oc::FuncCounter functor2;

    leaf->resetFlagsInRadius(Coordinate<double>(1,1,1), resetRadius, false);
    leaf->execFunc(functor2, false);

    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos1";
    ASSERT_TRUE(pos2->counter == 0) << "Executed on removed element";
    ASSERT_TRUE(pos3->counter == 1) << "Didn't execute on pos3";
    ASSERT_TRUE(functor2.counter == 2) << "Total num of executions wrong";

    delete pos1;
    delete pos2;
    delete pos3;
}

TEST_F(OctLeafTest, execRadius4){
    // Test removing an element
    ns_oc::PosCounter* pos1 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos2 = new ns_oc::PosCounter();
    ns_oc::PosCounter* pos3 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1.0, 1.0, 1.0);
    pos2->position = Coordinate<double>(1.5, 1.0, 1.0);
    pos3->position = Coordinate<double>(1.5, 1.0, 3.0);

    leaf->insert(pos1, false);
    leaf->insert(pos2, false);
    leaf->insert(pos3, false);


    ns_oc::FuncPosDelete functor;
    functor.target.push_back(pos2);

    double resetRadius;
    resetRadius = leaf->execFuncInRadius(Coordinate<double>(1,1,1), 2.0, functor, false);

    // Test that flags are reset properly
    ns_oc::FuncCounter functor2;

    leaf->resetFlagsInRadius(Coordinate<double>(1,1,1), resetRadius, false);
    leaf->execFunc(functor2, false);

    ASSERT_TRUE(pos1->counter == 1) << "Didn't execute on pos1";
    ASSERT_TRUE(pos3->counter == 1) << "Didn't execute on pos3";
    ASSERT_TRUE(functor2.counter == 2) << "Total num of executions wrong";

    delete pos1;
    delete pos3;
}
