#include <gtest/gtest.h>

#include "OctreeTestHelpers.cpp"
#include "../../../src/Datastructures/Octree/Octree.h"


class OctreeTest : public ::testing::Test{
protected:
    Octree* oc = new Octree();

    virtual void SetUp(){
        if(oc != 0)
            delete oc;
        oc = new Octree();
    };

    virtual void TearDown(){
        if(oc != 0){
            delete oc;
            oc = 0;
        }
    };
};

TEST_F(OctreeTest, test1){
    // Simple case

    ns_oc::PosCounter* dummy = new ns_oc::PosCounter();
    dummy->position = Coordinate<double>(1.0, 1.0, 1.0);

    oc->insert(dummy);

    ns_oc::FuncCounter functor;
    oc->execFunc(functor);

    ASSERT_TRUE(dummy->counter == 1) << "functor didn't execute\n";

}

TEST_F(OctreeTest, test2){
    // Don't execute on removed

    ns_oc::PosCounter* dummy = new ns_oc::PosCounter();
    dummy->position = Coordinate<double>(1.0, 1.0, 1.0);

    oc->insert(dummy);

    ns_oc::FuncCounter functor;
    oc->execFunc(functor);

    ASSERT_TRUE(dummy->counter == 1) << "functor didn't execute\n";

    oc->remove(dummy);
    oc->execFunc(functor);
    ASSERT_TRUE(dummy->counter == 1);
}
