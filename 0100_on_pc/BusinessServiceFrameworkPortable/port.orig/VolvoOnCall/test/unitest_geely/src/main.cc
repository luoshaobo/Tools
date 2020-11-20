/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     main.cc
 *  \brief    Volvo On Call, unit tests, main function
 *  \author   Axel Fagerstedt
 *
 *  \addtogroup SW_package
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <openssl/evp.h>

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    int rc = 0;

    ::testing::InitGoogleTest(&argc, argv);

    //OpenSSL Init
    OpenSSL_add_all_algorithms();

    rc = RUN_ALL_TESTS();

    //OpenSSL cleanup
    EVP_cleanup();

    return rc;
}

/** \}    end of addtogroup */
