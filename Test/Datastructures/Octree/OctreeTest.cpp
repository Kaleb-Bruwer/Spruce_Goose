#include <gtest/gtest.h>

#include <vector>
#include <iostream>

#include "OctreeTestHelpers.cpp"
#include "../../../src/Datastructures/Octree/Octree.h"

using namespace std;

class OctreeTest : public ::testing::Test{
protected:
    Octree* oc = new Octree();
    Octree tree;

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

class OcTreeInsertTest :public ::testing::TestWithParam<vector<Coordinate<double>>>{
public:
    Octree tree;
    virtual void SetUp(){};

    virtual void TearDown(){};

};

class OcTreeRemoveTest :public ::testing::TestWithParam
    <tuple<vector<Coordinate<double>>, vector<int>>>{
public:
    Octree tree;
    virtual void SetUp(){};

    virtual void TearDown(){};

};

class OcTreeExecRadiusTest :public ::testing::TestWithParam
    <tuple<
        Coordinate<double>, //origin
        double, //radius
        vector<Coordinate<double>>, //insert
        vector<bool> //expected
    >>{
public:
    Octree tree;
    virtual void SetUp(){};

    virtual void TearDown(){};

};

class OcTreeRemoveFuncTest :public ::testing::TestWithParam<Coordinate<double>>{
public:
    Octree tree;
    virtual void SetUp(){};

    virtual void TearDown(){};

};

class OcTreeFixPosTest :public ::testing::TestWithParam<
    pair<Coordinate<double>, Coordinate<double>>>{
public:
    Octree tree;
    virtual void SetUp(){};

    virtual void TearDown(){};

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

TEST_P(OcTreeInsertTest, insert){
    vector<Coordinate<double>> pos = GetParam();
    // cout << pos.size() << " ";
    // for(int i=0; i< pos.size(); i++){
    //     cout << pos[i] << " ";
    // }
    // cout << endl;

    vector<ns_oc::PosCounter*> elements(pos.size(),0);

    for(int i=0; i<pos.size(); i++){
        elements[i] = new ns_oc::PosCounter();
        elements[i]->position = pos[i];
        tree.insert(elements[i]);
    }

    ns_oc::FuncCounter functor;

    tree.execFunc(functor);

    for(int i=0; i<pos.size(); i++){
        ASSERT_TRUE(elements[i]->counter == 1) << "execFunc missed an element";
    }

    // 2nd execution to ensure execFunc didn't corrupt anything
    tree.execFunc(functor);

    for(int i=0; i<pos.size(); i++){
        ASSERT_TRUE(elements[i]->counter == 2) << "execFunc missed an element";
    }

    for(int i=0; i<elements.size(); i++){
        delete elements[i];
    }
}

INSTANTIATE_TEST_CASE_P(OcTreeInsert1, OcTreeInsertTest,
    ::testing::Values(
        vector<Coordinate<double>>{Coordinate<double>(1,1,1)},
        vector<Coordinate<double>>{Coordinate<double>(1,1,2),
            Coordinate<double>(1,1,5)
        },
        vector<Coordinate<double>>{Coordinate<double>(1,1,2),
            Coordinate<double>(1,1,5),
            Coordinate<double>(-1,-1,-5),
            Coordinate<double>(1,1,-5),
        },
        vector<Coordinate<double>>{Coordinate<double>(1,1,2),
            Coordinate<double>(50,200,500),
            Coordinate<double>(1,1,1)
        },
        vector<Coordinate<double>>{Coordinate<double>(1,1,2),
            Coordinate<double>(0,0,0),
            Coordinate<double>(1,1,1)
        },
        vector<Coordinate<double>>{Coordinate<double>(5000,30.1,3000.5),
            Coordinate<double>(-50,200,500),
            Coordinate<double>(1,1,1)
        }
    ));


TEST_P(OcTreeRemoveTest, remove){
    auto params = GetParam();
    vector<Coordinate<double>> insert = get<0>(params);
    vector<int> remove = get<1>(params);

    // cout << insert.size() << " " << remove.size();

    vector<ns_oc::PosCounter*> elements(insert.size(),0);

    for(int i=0; i<insert.size(); i++){
        elements[i] = new ns_oc::PosCounter();
        elements[i]->position = insert[i];
        tree.insert(elements[i]);
    }

    for(int i=0; i<remove.size(); i++){
        tree.remove(elements[remove[i]]);
    }

    ns_oc::FuncCounter functor;
    tree.execFunc(functor);

    int execCount = insert.size()- remove.size();
    ASSERT_TRUE(functor.counter == execCount) << "Wrong number of executions";

    for(int i=0; i<remove.size(); i++){
        ASSERT_TRUE(elements[remove[i]]->counter == 0) << "removed element still executed on";
    }

    for(int i=0; i<elements.size(); i++){
        delete elements[i];
    }
}

INSTANTIATE_TEST_CASE_P(OcTreeRemove1, OcTreeRemoveTest,
    ::testing::Values(
        make_tuple(
            vector<Coordinate<double>>{
                Coordinate<double>(1,1,1)
            },
            vector<int>{0}
        ),
        make_tuple(
            vector<Coordinate<double>>{
                Coordinate<double>(1,1,1),
                Coordinate<double>(1,1,1)
            },
            vector<int>{1}
        ),
        make_tuple(
            vector<Coordinate<double>>{
                Coordinate<double>(-1,-1,-1),
                Coordinate<double>(1,1,1)
            },
            vector<int>{0}
        ),
        make_tuple(
            vector<Coordinate<double>>{
                Coordinate<double>(1000,1000,1000),
                Coordinate<double>(1,1,1),
                Coordinate<double>(1,1,1),
            },
            vector<int>{1}
        ),
        make_tuple(
            vector<Coordinate<double>>{
                Coordinate<double>(1000,1000,1000),
                Coordinate<double>(700,800,900),
                Coordinate<double>(-1,-1,-1),
            },
            vector<int>{0}
        ),
        make_tuple(
            vector<Coordinate<double>>{
                Coordinate<double>(-1000,-1000,-1000),
                Coordinate<double>(-1,-1,-1),
                Coordinate<double>(1,1,1),
            },
            vector<int>{1}
        ),
        make_tuple(
            vector<Coordinate<double>>{
                Coordinate<double>(-1000,-1000,-1000),
                Coordinate<double>(-1,-1,-1),
                Coordinate<double>(1,1,1),
            },
            vector<int>{1, 0}
        ),
        make_tuple(
            vector<Coordinate<double>>{
                Coordinate<double>(-1000,-1000,-1000),
                Coordinate<double>(-1,-1,-1),
                Coordinate<double>(1,1,1),
            },
            vector<int>{0,1,2}
        )
    ));

TEST_P(OcTreeExecRadiusTest, execRadius1){
    auto params = GetParam();
    Coordinate<double> origin = get<0>(params);
    double radius = get<1>(params);
    vector<Coordinate<double>> insert = get<2>(params);
    vector<bool> expected = get<3>(params);

    vector<ns_oc::PosCounter*> elements(insert.size(),0);

    for(int i=0; i<insert.size(); i++){
        elements[i] = new ns_oc::PosCounter();
        elements[i]->position = insert[i];
        tree.insert(elements[i]);
    }

    ns_oc::FuncCounter functor;

    tree.execFuncInRadius(origin, radius, functor);

    for(int i=0; i<elements.size(); i++){
        if(expected[i])
            ASSERT_TRUE(elements[i]->counter == 1) << "missed an element";
        else
            ASSERT_TRUE(elements[i]->counter == 0) << "element wasn't skipped";
    }

    // 2nd execution to ensure nothing got corrupted
    tree.execFunc(functor);

    for(int i=0; i<elements.size(); i++){
        if(expected[i])
            ASSERT_TRUE(elements[i]->counter == 2) << "execFunc missed an element";
        else
            ASSERT_TRUE(elements[i]->counter == 1) << "execFunc missed an element";
    }

    for(int i=0; i<elements.size(); i++){
        delete elements[i];
    }
}

TEST_P(OcTreeExecRadiusTest, execRadius2){
    auto params = GetParam();
    Coordinate<double> origin = get<0>(params);
    double radius = get<1>(params);
    vector<Coordinate<double>> insert = get<2>(params);
    vector<bool> expected = get<3>(params);

    vector<ns_oc::PosCounter*> elements(insert.size(),0);

    for(int i=0; i<insert.size(); i++){
        elements[i] = new ns_oc::PosCounter();
        elements[i]->position = insert[i];
        tree.insert(elements[i]);
    }

    ns_oc::FuncCounter functor;

    tree.execFuncInRadius(origin, radius, functor);

    for(int i=0; i<elements.size(); i++){
        if(expected[i])
            ASSERT_TRUE(elements[i]->counter == 1) << "missed an element";
        else
            ASSERT_TRUE(elements[i]->counter == 0) << "element wasn't skipped";
    }

    // 2nd execution to ensure nothing got corrupted
    tree.execFuncInRadius(origin, radius, functor);


    for(int i=0; i<elements.size(); i++){
        if(expected[i])
            ASSERT_TRUE(elements[i]->counter == 2) << "missed an element";
        else
            ASSERT_TRUE(elements[i]->counter == 0) << "element wasn't skipped";
    }

    for(int i=0; i<elements.size(); i++){
        delete elements[i];
    }
}

INSTANTIATE_TEST_CASE_P(OcTreeExecRadius1, OcTreeExecRadiusTest,
    ::testing::Values(
        make_tuple(
            Coordinate<double>(2,2,2),
            3.0,
            vector<Coordinate<double>>{
                Coordinate<double>(1,1,1)
            },
            vector<bool>{true}
        ),
        make_tuple(
            Coordinate<double>(2,2,2),
            3.0,
            vector<Coordinate<double>>{
                Coordinate<double>(1,1,1),
                Coordinate<double>(15,1,1)
            },
            vector<bool>{true, false}
        ),
        make_tuple(
            Coordinate<double>(0,0,0),
            3.0,
            vector<Coordinate<double>>{
                Coordinate<double>(3,0,0),
                Coordinate<double>(15,1,1)
            },
            vector<bool>{true, false}
        ),
        make_tuple(
            Coordinate<double>(0,0,0),
            16.0,
            vector<Coordinate<double>>{
                Coordinate<double>(16,0,0)
            },
            vector<bool>{true}
        ),
        make_tuple(
            Coordinate<double>(2,0,0),
            3.0,
            vector<Coordinate<double>>{
                Coordinate<double>(-1,0,0),
                Coordinate<double>(2,1,1)
            },
            vector<bool>{true, true}
        )
    ));

TEST_F(OctreeTest, posChanger1){
    ns_oc::PosCounter *pos1 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1,1,1);

    tree.insert(pos1);

    ns_oc::FuncPosChange functor;
    functor.target.push_back(pos1);
    functor.destination.push_back(Coordinate<double>(2,2,2));

    tree.execFunc(functor);

    ASSERT_TRUE(pos1->position == Coordinate<double>(2,2,2)) << "element wasn't moved";

    delete pos1;
}

TEST_F(OctreeTest, posChanger2){
    ns_oc::PosCounter *pos1 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1,1,1);

    tree.insert(pos1);

    ns_oc::FuncPosChange functor;
    functor.target.push_back(pos1);
    functor.destination.push_back(Coordinate<double>(-2,-2,500));

    tree.execFunc(functor);

    ASSERT_TRUE(pos1->position == Coordinate<double>(-2,-2,500)) << "element wasn't moved";

    delete pos1;
}

TEST_F(OctreeTest, posChanger3){
    ns_oc::PosCounter *pos1 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1,1,1);

    tree.insert(pos1);

    ns_oc::FuncPosChange functor;
    functor.target.push_back(pos1);
    functor.destination.push_back(Coordinate<double>(2,2,2));

    tree.execFunc(functor);

    ns_oc::FuncCounter functor2;
    tree.execInBounds(Coordinate<double>(1.5,1.5,1.5),
        Coordinate<double>(2.1,2.1,2.1), functor2);

    ASSERT_TRUE(pos1->counter == 1) << "Lost after move";

    delete pos1;
}

TEST_F(OctreeTest, posChanger4){
    ns_oc::PosCounter *pos1 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1,1,1);

    tree.insert(pos1);

    ns_oc::FuncPosChange functor;
    functor.target.push_back(pos1);
    functor.destination.push_back(Coordinate<double>(-4,-5,-3));

    tree.execFunc(functor);

    ns_oc::FuncCounter functor2;
    tree.execInBounds(Coordinate<double>(-4.1,-5.1,-3.1),
        Coordinate<double>(-3.9,-4.9,-2.9), functor2);

    ASSERT_TRUE(pos1->counter == 1) << "Lost after move";

    delete pos1;
}

TEST_F(OctreeTest, posChanger5){
    ns_oc::PosCounter *pos1 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1,1,1);

    tree.insert(pos1);

    ns_oc::FuncPosChange functor;
    functor.target.push_back(pos1);
    functor.destination.push_back(Coordinate<double>(-4,-5,-3));

    tree.execInBounds(Coordinate<double>(0,0,0),
        Coordinate<double>(3,3,3), functor);

    ASSERT_TRUE(pos1->position == Coordinate<double>(-4,-5,-3)) << "wasn't moved";

    ns_oc::FuncCounter functor2;
    tree.execInBounds(Coordinate<double>(-4.1,-5.1,-3.1),
        Coordinate<double>(-3.9,-4.9,-2.9), functor2);

    ASSERT_TRUE(pos1->counter == 1) << "Lost after move";

    delete pos1;
}

TEST_F(OctreeTest, posChanger6){
    ns_oc::PosCounter *pos1 = new ns_oc::PosCounter();
    pos1->position = Coordinate<double>(1,1,1);

    tree.insert(pos1);

    ns_oc::FuncPosChange functor;
    functor.target.push_back(pos1);
    functor.destination.push_back(Coordinate<double>(-4,-5,-3));

    tree.execFuncInRadius(Coordinate<double>(1,1,1), 2, functor);

    ns_oc::FuncCounter functor2;
    tree.execInBounds(Coordinate<double>(-4.1,-5.1,-3.1),
        Coordinate<double>(-3.9,-4.9,-2.9), functor2);

    ASSERT_TRUE(pos1->counter == 1) << "Lost after move";

    delete pos1;
}

TEST_P(OcTreeRemoveFuncTest, execFuncR){
    ns_oc::PosCounter *pos1 = new ns_oc::PosCounter();
    pos1->position = GetParam();

    tree.insert(pos1);

    ns_oc::FuncPosRemove functor1;
    functor1.target.push_back(pos1);

    tree.execFunc(functor1);

    ns_oc::FuncCounter functor2;
    tree.execFunc(functor2);

    ASSERT_TRUE(pos1->counter == 0) << "element wasn't removed";

    delete pos1;
}

TEST_P(OcTreeRemoveFuncTest, execFuncD){
    ns_oc::PosCounter *pos1 = new ns_oc::PosCounter();
    pos1->position = GetParam();

    tree.insert(pos1);

    ns_oc::FuncPosDelete functor1;
    functor1.target.push_back(pos1);

    tree.execFunc(functor1);

    ns_oc::FuncCounter functor2;
    tree.execFunc(functor2);

    ASSERT_TRUE(functor2.counter == 0) << "element wasn't removed";
}

TEST_P(OcTreeRemoveFuncTest, execRadiusR){
    ns_oc::PosCounter *pos1 = new ns_oc::PosCounter();
    pos1->position = GetParam();

    tree.insert(pos1);

    ns_oc::FuncPosRemove functor1;
    functor1.target.push_back(pos1);

    tree.execFuncInRadius(pos1->position, 1, functor1);

    ns_oc::FuncCounter functor2;
    tree.execFunc(functor2);

    ASSERT_TRUE(pos1->counter == 0) << "element wasn't removed";

    delete pos1;
}

TEST_P(OcTreeRemoveFuncTest, execRadiusD){
    ns_oc::PosCounter *pos1 = new ns_oc::PosCounter();
    pos1->position = GetParam();

    tree.insert(pos1);

    ns_oc::FuncPosDelete functor1;
    functor1.target.push_back(pos1);

    tree.execFuncInRadius(pos1->position, 1, functor1);

    ns_oc::FuncCounter functor2;
    tree.execFunc(functor2);

    ASSERT_TRUE(functor2.counter == 0) << "element wasn't removed";
}

TEST_P(OcTreeRemoveFuncTest, execBoundsR){
    Coordinate coord = GetParam();

    ns_oc::PosCounter *pos1 = new ns_oc::PosCounter();
    pos1->position = coord;

    tree.insert(pos1);

    ns_oc::FuncPosRemove functor1;
    functor1.target.push_back(pos1);

    Coordinate<double> low = coord - Coordinate<double>(0.1,0.1,0.1);
    Coordinate<double> high = coord + Coordinate<double>(0.1,0.1,0.1);

    tree.execInBounds(low, high, functor1);

    ns_oc::FuncCounter functor2;
    tree.execFunc(functor2);

    ASSERT_TRUE(pos1->counter == 0) << "element wasn't removed";

    delete pos1;
}

TEST_P(OcTreeRemoveFuncTest, execBoundsD){
    Coordinate coord = GetParam();

    ns_oc::PosCounter *pos1 = new ns_oc::PosCounter();
    pos1->position = coord;

    tree.insert(pos1);

    ns_oc::FuncPosDelete functor1;
    functor1.target.push_back(pos1);

    Coordinate<double> low = coord - Coordinate<double>(0.1,0.1,0.1);
    Coordinate<double> high = coord + Coordinate<double>(0.1,0.1,0.1);

    tree.execInBounds(low, high, functor1);

    ns_oc::FuncCounter functor2;
    tree.execFunc(functor2);

    ASSERT_TRUE(functor2.counter == 0) << "element wasn't removed";
}

INSTANTIATE_TEST_CASE_P(OcTreeRemoveFunc1, OcTreeRemoveFuncTest,
    ::testing::Values(
        Coordinate<double>(0,0,0),
        Coordinate<double>(1,1,1),
        Coordinate<double>(1,1,-1),
        Coordinate<double>(1,-1,1),
        Coordinate<double>(1,-1,-1),
        Coordinate<double>(-1,1,1),
        Coordinate<double>(1000,500,1),
        Coordinate<double>(1000,50,333),
        Coordinate<double>(-0.0001,0,-0.001)
    ));

TEST_P(OcTreeFixPosTest, fixPos){
    auto params = GetParam();
    Coordinate<double> start = get<0>(params);
    Coordinate<double> end = get<1>(params);

    ns_oc::PosCounter *pos1 = new ns_oc::PosCounter();
    pos1->position = start;

    tree.insert(pos1);

    pos1->position = end;
    tree.fixPos(pos1, start);

    ns_oc::FuncCounter functor1;

    Coordinate<double> low = end - Coordinate<double>(0.1,0.1,0.1);
    Coordinate<double> high = end + Coordinate<double>(0.1,0.1,0.1);

    tree.execInBounds(low, high, functor1);

    ASSERT_TRUE(pos1->counter == 1) << "Lost after fixPos";

    delete pos1;
}

INSTANTIATE_TEST_CASE_P(OcTreeFixPos1, OcTreeFixPosTest,
    ::testing::Values(
        make_pair(Coordinate<double>(0,0,0), Coordinate<double>(0,1,0)),
        make_pair(Coordinate<double>(1,1,1), Coordinate<double>(0,-10,0)),
        make_pair(Coordinate<double>(-50,-10,1), Coordinate<double>(0,-10,0)),
        make_pair(Coordinate<double>(-50,-10,1), Coordinate<double>(0,0,0)),
        make_pair(Coordinate<double>(-10,-10,-10), Coordinate<double>(500,500,1000)),
        make_pair(Coordinate<double>(1,1,1), Coordinate<double>(5000, 5000, 5000))
    ));
