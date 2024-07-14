#include <gtest/gtest.h>
#include <vector>

#include "../../../src/World/ThreadArea.h"

// TESTING CALLBACKS

// 1. Create thread area
// 2. Create callbacks
// 3. Each callback will set some flag
// 4. Compare flags to determine which was called

namespace callback_tests{
    // a bool vector wouldn't reveal if a callback was repeatedly called
    std::vector<int> callback_flags;

    void testing_callback(void* obj, ThreadArea* _){
        // In reality obj would be the object the method should be executed on,
        // but here I 'hijacked' it for the test to be simpler since that's not
        // what's being tested here
        int val;
        memcpy(&val, &obj, 4);

        if(val < 0 || val >= callback_flags.size()){
            ASSERT_TRUE(0) << "Broken test";
        }

        callback_flags[val]++;
    }
}

class ThreadAreaCallbacksTest : public ::testing::Test{
private:
    virtual void SetUp(){
        callback_tests::callback_flags = vector<int>(5,0);
    };

    virtual void TearDown(){

    };

public:
    void assert_flags(vector<int> expected){
        ASSERT_TRUE(callback_tests::callback_flags.size() == expected.size())
            << "vector size mismatch";

        for(int i=0; i<expected.size(); i++){
            ASSERT_TRUE(expected[i] == callback_tests::callback_flags[i])
                << "Value mismatch: [" << i << "] was "
                << callback_tests::callback_flags[i] << " instead of " << expected[i];
        }

    }

};


TEST_F(ThreadAreaCallbacksTest, test1){
    ThreadArea tArea = ThreadArea(false);

    void* obj;
    int val = 2;

    memcpy(&obj, &val, 4);
    tArea.callbacks.add(1, &callback_tests::testing_callback, obj);
    tArea.callbacks.exec_tick(1, &tArea);


    vector<int> expected(5,0);
    expected[2] = 1;

    assert_flags(expected);
};

TEST_F(ThreadAreaCallbacksTest, test2){
    ThreadArea tArea = ThreadArea(false);

    void* obj;
    int val = 2;

    memcpy(&obj, &val, 4);
    tArea.callbacks.add(1, &callback_tests::testing_callback, obj);
    tArea.callbacks.add(3, &callback_tests::testing_callback, obj);
    tArea.callbacks.exec_tick(1, &tArea);
    tArea.callbacks.exec_tick(2, &tArea);

    vector<int> expected(5,0);
    expected[2] = 1;
    assert_flags(expected);

    tArea.callbacks.exec_tick(3, &tArea);
    expected[2] = 2;
    assert_flags(expected);

};

TEST_F(ThreadAreaCallbacksTest, test3){
    ThreadArea tArea = ThreadArea(false);

    void* obj;

    int val = 2;
    memcpy(&obj, &val, 4);
    tArea.callbacks.add(5, &callback_tests::testing_callback, obj);

    val = 3;
    memcpy(&obj, &val, 4);
    tArea.callbacks.add(2, &callback_tests::testing_callback, obj);
    tArea.callbacks.exec_tick(2, &tArea);

    vector<int> expected(5,0);
    expected[3] = 1;
    assert_flags(expected);

    tArea.callbacks.exec_tick(5, &tArea);
    expected[2] = 1;
    assert_flags(expected);

};

TEST_F(ThreadAreaCallbacksTest, test4){
    ThreadArea tArea = ThreadArea(false);

    void* obj;

    int val = 0;
    memcpy(&obj, &val, 4);
    tArea.callbacks.add(3, &callback_tests::testing_callback, obj);

    val = 1;
    memcpy(&obj, &val, 4);
    tArea.callbacks.add(2, &callback_tests::testing_callback, obj);

    val = 2;
    memcpy(&obj, &val, 4);
    tArea.callbacks.add(6, &callback_tests::testing_callback, obj);

    val = 3;
    memcpy(&obj, &val, 4);
    tArea.callbacks.add(5, &callback_tests::testing_callback, obj);

    val = 4;
    memcpy(&obj, &val, 4);
    tArea.callbacks.add(1, &callback_tests::testing_callback, obj);

    tArea.callbacks.exec_tick(1, &tArea);
    vector<int> expected(5,0);
    expected[4] = 1;
    assert_flags(expected);

    tArea.callbacks.exec_tick(2, &tArea);
    expected[1] = 1;
    assert_flags(expected);

    tArea.callbacks.exec_tick(3, &tArea);
    expected[0] = 1;
    assert_flags(expected);

    tArea.callbacks.exec_tick(4, &tArea);
    assert_flags(expected);


    tArea.callbacks.exec_tick(5, &tArea);
    expected[3] = 1;
    assert_flags(expected);

    tArea.callbacks.exec_tick(6, &tArea);
    expected[2] = 1;
    assert_flags(expected);

};

TEST_F(ThreadAreaCallbacksTest, test5){
    ThreadArea tArea = ThreadArea(false);

    tArea.callbacks.exec_tick(2, &tArea);
    assert_flags(vector<int>(5,0));

};
