#include <gtest/gtest.h>

#include "OctreeTestHelpers.cpp"
#include "../../../src/Datastructures/Octree/OctLeaf.h"


class OctLeafTest : public ::testing::Test{
protected:
    ns_oc::DummyParent *parent = 0;
    OctLeaf *leaf = new OctLeaf(Coordinate<double>(0.0,0.0,0.0),
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
}
