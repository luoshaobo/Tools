/**
 * Copyright (C) 2018 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
% * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     unittest_str2uint.cpp
 *  \brief    Foundation Services Manager unit test for cache_utils.* functions
 *  \author   Florian Schindler
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <gtest/gtest.h>
#include <inttypes.h>
#include <limits.h>
#include "cache_utils.h"

namespace fsm
{

namespace transfermanager
{

#define RANDOM_INIT_VALUE 0xFFFFFFF00FFFFFFF

TEST(Str2uint, test_case_123)
{
    // normal number; shall succeed
    unsigned long long int value = RANDOM_INIT_VALUE;  // initialise with value other than expected
    std::string str("123");
    bool result = false;  // initialise with value other than expected
    result = Str2uint(str, value);
    EXPECT_EQ(value, 123);
    EXPECT_EQ(result, true);
}


TEST(Str2uint, test_case_123_spaces)
{
    // normal number; shall succeed
    unsigned long long int value = RANDOM_INIT_VALUE;  // initialise with value other than expected
    std::string str(" 123 ");
    bool result = false;  // initialise with value other than expected
    result = Str2uint(str, value);
    EXPECT_EQ(value, 123);
    EXPECT_EQ(result, true);
}


TEST(Str2uint, test_case_AB123)
{
    // number with characters; shall fail
    unsigned long long int value = RANDOM_INIT_VALUE;  // initialise with value other than expected
    std::string str("AB 123 ");
    bool result = true;  // initialise with value other than expected
    result = Str2uint(str, value);
    EXPECT_EQ(value, RANDOM_INIT_VALUE);  // original value should not be changed
    EXPECT_EQ(result, false);
}


TEST(Str2uint, test_case_123ab)
{
    // number with characters; shall fail
    unsigned long long int value = RANDOM_INIT_VALUE;  // initialise with value other than expected
    std::string str("123 ab ");
    bool result = true;  // initialise with value other than expected
    result = Str2uint(str, value);
    EXPECT_EQ(value, RANDOM_INIT_VALUE);  // original value should not be changed
    EXPECT_EQ(result, false);
}


TEST(Str2uint, test_case_123_12)
{
    // two number separated by space; shall fail
    unsigned long long int value = RANDOM_INIT_VALUE;  // initialise with value other than expected
    std::string str("123  12 ");
    bool result = true;  // initialise with value other than expected
    result = Str2uint(str, value);
    EXPECT_EQ(value, RANDOM_INIT_VALUE);  // original value should not be changed
    EXPECT_EQ(result, false);
}


TEST(Str2uint, test_case_abc)
{
    // number with characters; shall fail
    unsigned long long int value = RANDOM_INIT_VALUE;  // initialise with value other than expected
    std::string str("abc");
    bool result = true;  // initialise with value other than expected
    result = Str2uint(str, value);
    EXPECT_EQ(value, RANDOM_INIT_VALUE);  // original value should not be changed
    EXPECT_EQ(result, false);
}


TEST(Str2uint, test_case_empty_string)
{
    // empty string; shall fail
    unsigned long long int value = RANDOM_INIT_VALUE;  // initialise with value other than expected
    std::string str("");
    bool result = true;  // initialise with value other than expected
    result = Str2uint(str, value);
    EXPECT_EQ(value, RANDOM_INIT_VALUE);  // original value should not be changed
    EXPECT_EQ(result, false);
}

TEST(Str2uint, test_case_overflow)
{
    // number too long for lon long int (64 bit) should cause an overflow, thus fail
    unsigned long long int value = RANDOM_INIT_VALUE;  // initialise with value other than expected
    std::string str("18446744073709551617");  // max value would be: 18446744073709551615
    bool result = true;  // initialise with value other than expected
    result = Str2uint(str, value);
    EXPECT_EQ(value, RANDOM_INIT_VALUE);  // original value should not be changed
    EXPECT_EQ(result, false);
}

TEST(Str2uint, test_case_max_value)
{
    // maximum value for 64 unsigned int; should succeed
    unsigned long long int value = RANDOM_INIT_VALUE;  // initialise with value other than expected
    std::string str = std::to_string(ULLONG_MAX);  // 18446744073709551615
    bool result = false;  // initialise with value other than expected
    result = Str2uint(str, value);
    EXPECT_EQ(value, ULLONG_MAX);
    EXPECT_EQ(result, true);
}


} // namespace transfermanager
} // namespace fsm
/** \}    end of addtogroup */
