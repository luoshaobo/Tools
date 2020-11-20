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
 *  file     sql_defines.h
 *  brief    Defines related to sql statements for http cache db.
 *  author   Maksym Mozok
 *
 *  addtogroup Doxygen group
 *  {
 */
#ifndef FSM_TRANSFERMANAGER_SQL_DEFINES_H_
#define FSM_TRANSFERMANAGER_SQL_DEFINES_H_

/// ** INCLUDES *****************************************************************
#include "sql_defines.h"
#include <string>


namespace fsm
{

namespace transfermanager
{
//this string should coresponds to persistence_config_fsm_ssm.xml definition.
const std::string kDbNameKey = "cache_db_name_key";

const std::string kSqlCreate = "CREATE TABLE ";
const std::string kSqlInsertReplace = "INSERT OR REPLACE INTO ";
const std::string kSqlSelect = "SELECT ";
const std::string kSqlDelete = "DELETE FROM ";
const std::string kSqlUpdate = "UPDATE ";
const std::string kSqlDrop = "DROP TABLE ";
const std::string kSqlIfExists = "IF EXISTS ";
const std::string kSqlIfNotExists = "IF NOT EXISTS ";
const std::string kSqlWhere = "WHERE ";
const std::string kSqlAnd = "AND ";
const std::string kSqlSet = "SET ";

//table name
const std::string kCacheTableName ="FSMCACHE ";

//table columns
const std::string kSqlUrl = "_url_";
const std::string kSqlRcvdTimestamp = "_rcvdtimestamp_";
const std::string kSqlExpTimestamp = "_exptimestamp_";
const std::string kSqlDatablob = "_datablob_";
const std::string kSqlBlobDataOffset = "_size_t_";
const std::string kSqlEtag = "_etag_";
const std::string kSqlValid = "_valid_";
const std::string kSqlROWID = "_ROWID_";

const std::string kSqlTableSstruct = "(" + kSqlUrl + " TEXT PRIMARY KEY NOT NULL, " + kSqlRcvdTimestamp + " INTEGER NOT NULL, "+ kSqlExpTimestamp + " INTEGER NOT NULL, " + kSqlDatablob + " BLOB, " + kSqlBlobDataOffset + " INTEGER,  "+ kSqlEtag +" TEXT, " + kSqlValid +" INTEGER NOT NULL)";

const std::string kSqlGetROWID = "SELECT ROWID FROM " + kCacheTableName + kSqlWhere + kSqlUrl + "=?1";
const std::string kSqlGetByROWID = kSqlSelect +"* FROM " + kCacheTableName + kSqlWhere + kSqlROWID + "=?1";

const std::string kExportCmdHead = "sqlite3 -header -csv ";
const std::string kExportQuery = " select * from " + kCacheTableName + "; > ";
const std::string kImportCmdHead = "sqlite3 ";

//std::string export_tail

} // namespace transfermanager
} // namespace fsm

#endif // FSM_TRANSFERMANAGER_SQL_DEFINES_H_

/** \}    end of addtogroup */
