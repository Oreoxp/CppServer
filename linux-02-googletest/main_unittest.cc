#include <limits.h>
#include "main.h"
#include "gtest/gtest.h"
namespace {
    TEST(FactorialTest, Negative) {
         EXPECT_EQ(1, Factorial(-5));
         EXPECT_EQ(1, Factorial(-100000));
    }
    
    class testA : public testing::Test {
        protected:
        virtual void SetUp(){

        } 

        virtual void TearDown(){

        }
    };
}

int main(){
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();

}