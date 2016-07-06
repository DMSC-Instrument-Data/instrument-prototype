#include "FixedLengthVector.h"
#include <gtest/gtest.h>

class TestFixedLengthVector : public FixedLengthVector<TestFixedLengthVector, double>
{
public:
    using FixedLengthVector<TestFixedLengthVector, double>::FixedLengthVector;
    virtual ~TestFixedLengthVector(){}
};


TEST(fixed_length_vector_test, test_default_construction){
    TestFixedLengthVector vector;
    EXPECT_EQ(vector.size(), 0);
}

TEST(fixed_length_vector_test, test_using_initalizer_list){
    TestFixedLengthVector vector{1,2,3};
    EXPECT_EQ(vector.size(), 3);
    EXPECT_EQ(vector[0], 1.0);
    EXPECT_EQ(vector[2], 3.0);
}

TEST(fixed_length_vector_test, test_move_construction){
    TestFixedLengthVector vector(std::vector<double>(10, 1.0));
    EXPECT_EQ(vector.size(), 10);
    EXPECT_EQ(vector[0], 1.0);
    EXPECT_EQ(vector[1], 1.0);
}

TEST(fixed_length_vector_test, test_bad_assignment){
    TestFixedLengthVector a(std::vector<double>(10, 1.0));
    // Make b. B is a different size.
    TestFixedLengthVector b(std::vector<double>(9, 1.0));
    EXPECT_THROW(a = b, std::logic_error);
}

TEST(fixed_length_vector_test, test_non_const_access){
    TestFixedLengthVector vector{1,2,3};

    EXPECT_EQ(vector[0], 1);
    // assign new value
    vector[0] = 2;
    EXPECT_EQ(vector[0], 2);
}

TEST(fixed_length_vector_test, test_const_access){
    const TestFixedLengthVector vector{1,2,3};
    EXPECT_EQ(vector[0], 1);
}







