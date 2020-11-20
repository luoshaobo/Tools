/**
 * Copyright (C) 2017 Continental Automotive GmbH
 *
 * Licensed under the "Supplier unrestricted white box" category, as
 * described in the 20160322-1_SW-amendment TCAM.pdf agreement
 * between Volvo Cars and Continental Automotive GmbH.
 *
 * Unless required by applicable law or agreed to in writing, software
% * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *  \file     unittest_http_cache_db.h
 *  \brief    Foundation Services Manager http cache database unit tests with real underlying sqlite3 database
 *  \author   Florian Schindler
 *
 *  \addtogroup unittest
 *  \{
 */

/******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

namespace fsm
{

namespace transfermanager
{
// common defines:
#define SQLLITE_PERSISTENCY_TEST_DB_NAME "unittest_http_cache.sqlite3"
#define SQLLITE_PERSISTENCY_LOCAL_TEST_DB_DIR "./local/"
#define SQLLITE_PERSISTENCY_PUBLIC_TEST_DB_DIR "./public/"

} //transfermanager

} //fsm
