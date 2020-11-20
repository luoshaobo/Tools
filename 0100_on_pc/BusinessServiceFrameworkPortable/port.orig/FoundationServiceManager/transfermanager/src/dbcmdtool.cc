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
 *  \file     dbcmdtool.cc
 *  \brief    dbcmdtool is a HttpCacheDb commandline utility
 *  \author   Maksym Mozok
 *
 *  \addtogroup transfermanager
 *  \{
 */

// ** INCLUDES *****************************************************************
#include "dbcmdtool.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <ctime>
#include <atomic>
#include <functional>
#include <iostream>
#include <string>
#include <cstring>
#include <tuple>
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator
#include <experimental/filesystem>
#include <getopt.h>

#include "sql_defines.h"
#include "http_cache_db.h"


namespace fs = std::experimental::filesystem;

namespace fsm
{

namespace transfermanager
{
void Usage(void)
{ //acdefmnprtu:v
    std::cout<<"Switches and parameters: "<<std::endl;
    std::cout<<"-a action do Add/Del/Get/backup/restore in case of wildcards only the first record is affected" <<std::endl;
    std::cout<<"-c column name to set/get or * all will be processed"<<std::endl;
    std::cout<<"-d data string to process"<<std::endl;
    std::cout<<"-e expiration timestamp in UNIX-seconds"<<std::endl;
    std::cout<<"-f file name for data to set/get"<<std::endl;
    std::cout<<"-m delimiter in exported csv file"<<std::endl;
    std::cout<<"-n name of db"<<std::endl;
    std::cout<<"-p path to db"<<std::endl;
    std::cout<<"-r received timestamp in UNIX-seconds"<<std::endl;
    std::cout<<"-t eTags string"<<std::endl;
    std::cout<<"-u url to find/add/update"<<std::endl;
    std::cout<<"-v valid flag 0/1"<<std::endl;
}

int DbBackup(const fs::path &backup_path, fsm::transfermanager::HttpCacheDb &cachedb)
{
    int rc = 0;
    fs::path fspath = "./backup/";
    std::string time_str;
    char time_buff[20]= {0};
    std::time_t backup_time = std::time(nullptr);
    std::string dbname = cachedb.GetDbNameStr();
    std::string dbpath = cachedb.GetDbPathStr();


    std::tm* time_tm = localtime(&backup_time);
    if (time_tm != nullptr)
    {
        std::strftime(time_buff, 20, "%F-%H%M%S", time_tm );
        time_str.assign(time_buff).shrink_to_fit();
        std::string csv_time_file = dbname+"-" + time_str.append(".csv");

        std::string export_cmd = fsm::transfermanager::kExportCmdHead + dbpath + dbname +
            fsm::transfermanager::kExportQuery + csv_time_file;

        if(backup_path.empty() && (!fs::exists(fspath)))
        {
            fs::create_directory(fspath);
        }

        rc = std::system(export_cmd.c_str());
    }
    else
    {
        std::cerr<<"DbBackup localtime is NULL"<<std::endl;
        rc = -1;
    }
    return rc;
}

int DbRestore(const std::string &backup_file, fsm::transfermanager::HttpCacheDb &cachedb)
{
    int rc = 0;
    std::string csvfile = backup_file;
    std::string import_cmd = fsm::transfermanager::kImportCmdHead + cachedb.GetDbNameStr().c_str() +
        "'.mode csv' '.import" + csvfile + " " + kCacheTableName + "'";

    //ToDo execute restore
    rc = std::system(import_cmd.c_str());

    return rc;
}

} // transfermanager namespace

} // fsm namespace

int main(int argc, char *argv[])
{
    int rc = 0;
    int commandline_parameter_index = 0;
    std::string _data_column_delimiter= "|";
    CommandCmd cmd = CommandCmd::kCmdUnknown;
    extern char *optarg;
    fs::path backup_path("./");
    fsm::transfermanager::CacheDbRecord dbrecord_tuple;
    fsm::transfermanager::CacheValidityDbRecord dbvalidity_tuple;
    fsm::transfermanager::CachePairDbRecord pair_tuple;
    std::string _dbpath;
    std::string _db_name;
    std::string _url;
    std::string _column;
    std::string _data;
    std::string _datafile;
    std::ifstream inData;
    std::vector<char> data_blob;
    std::string _command;
    std::time_t _received_time_t = 0;
    std::time_t _exp_time_t = 0;
    std::vector<char> _blob_t;
    std::size_t _blob_datasize_size_t = 0;
    std::string _etags;
    bool _valid = true;

    while ((commandline_parameter_index = getopt(argc, argv, "a:c:d:e:f:m:n:r:t:u:v:")) != (-1))
    {
        switch (commandline_parameter_index)
        {
        case 'a':
        {
            _command.assign(optarg,std::strlen(optarg));
            std::cout<<"_command = "<<_command<<std::endl;
            if(!_command.compare("Add"))
                cmd = CommandCmd::kCmdAdd;
            else if(!_command.compare("Del"))
                cmd = CommandCmd::kCmdDel;
            else if(!_command.compare("Get"))
                cmd = CommandCmd::kCmdGet;
            else if(!_command.compare("backup"))
                cmd = CommandCmd::kCmdBackup;
            else if(!_command.compare("restore"))
                cmd = CommandCmd::kCmdRestore;
            else //Command unknown
                cmd = CommandCmd::kCmdUnknown;
            break;
        }
        case 'p':
        {
            _dbpath.assign(optarg,std::strlen(optarg));
            std::cout<<"_dbpath = "<<_dbpath<<std::endl;
            backup_path.assign(_dbpath+"/backup/");
            break;
        }
        case 'n':
        {
            _db_name.assign(optarg,std::strlen(optarg));
            std::cout<<"_db_name = "<<_db_name<<std::endl;
            break;
        }
        case 'u':
        {
            _url.assign(optarg,std::strlen(optarg));
            std::cout<<"_url = "<<_url<<std::endl;
            break;
        }
        case 'c':
        {
            _column.assign(optarg,std::strlen(optarg));
            std::cout<<"_column = "<<_column<<std::endl;
            break;
        }
        case 'd':
        {
            _data.assign(optarg,std::strlen(optarg));
            std::cout<<"string _data = "<<_data<<std::endl;
            break;
        }
        case 'e':
        {
            _exp_time_t = std::stol(std::string(optarg,std::strlen(optarg)));
            std::cout<<"_exp_time_t = "<<_exp_time_t<<std::endl;
            break;
        }
        case 'f':
        {
            _datafile.assign(optarg,std::strlen(optarg));
            std::cout<<"_datafile = "<<_datafile<<std::endl;
            inData.open(_datafile,std::ios::binary);
            if(inData.is_open())
            {
                inData.seekg(std::ios::end);
                _blob_datasize_size_t = static_cast<size_t>(inData.tellg());
                inData.seekg(std::ios::beg);
                data_blob.resize(static_cast<unsigned long>(_blob_datasize_size_t));
                inData.read(data_blob.data(),static_cast<long>(_blob_datasize_size_t));
                std::cout<<"data to vector = "<<data_blob.size()<<std::endl;
            }
            else
            {
                std::cerr<<"Stream "<<_datafile<<" does not exist!"<<std::endl;
            }
            if(inData.is_open())
            {
                inData.close();
            }
            break;
        }
        case 'r':
        {
            _received_time_t = std::stol(std::string(optarg,std::strlen(optarg)));
            std::cout<<"received_time_t = "<<_received_time_t<<std::endl;
            break;
        }
        case 't':
        {
            _etags = std::string(optarg,std::strlen(optarg));
            std::cout<<"etags = "<<_etags<<std::endl;
            break;
        }
        case 'm':
        {
            _data_column_delimiter = std::string(optarg,std::strlen(optarg));
            std::cout<<"_data_column_delimiter = "<<_data_column_delimiter<<std::endl;
            break;
        }
        case 'v':
        {
            _valid = std::stol(std::string(optarg,std::strlen(optarg)));
            std::cout<<"_valid = "<<_valid<<std::endl;
            break;
        }
        case '?':
        {
            rc = 1;
            fsm::transfermanager::Usage();
            break;
        }
        }//switch
    }//while

    fsm::transfermanager::HttpCacheDb cachedb(_dbpath,_db_name);

    if((_url.empty() && CommandCmd::kCmdBackup != cmd)||
       (_url.empty() && CommandCmd::kCmdRestore != cmd))
    {
        fsm::transfermanager::Usage();
        rc = static_cast<int>(fsm::transfermanager::DbErrorCode_t::kInternalError);
    }

    if(fsm::transfermanager::DbErrorCode::kNone != cachedb.Init(_dbpath,_db_name))
    {
       rc = static_cast<int>(cachedb.ExtendedSqlErrorGet());
       std:: cerr<<"Unable open db = "<<(_dbpath+_db_name)<<" rc = "<<rc<<std::endl;
    }

    if(!rc)
    {
        bool existence_flag = false;
        switch(cmd)
        {
            case CommandCmd::kCmdAdd:
            {
                auto record_data_ptr = std::make_shared<fsm::transfermanager::CacheDbRecord>(dbrecord_tuple);

                if(true != _url.empty())
                {
                    // if record exsits then only the provided fields will be updated, i.e. prefill dbrecord_tuple with existing values
                    std::get<fsm::transfermanager::kUrlIdx>(dbrecord_tuple) = _url;
                    cachedb.DoesDbRecordExist(_url,existence_flag);

                    if( true == existence_flag )
                    {
                        cachedb.GetDbRecord(_url,record_data_ptr);
                        std::cout<<"Updating existing record"<<std::endl;
                    }
                }
                else
                {
                    std:: cerr<<"Empty url provided "<<std::endl;
                    rc = static_cast<int>(fsm::transfermanager::DbErrorCode_t::kInternalError);
                }
                if(0 != _received_time_t)
                {
                    std::get<fsm::transfermanager::kRcvTimestampIdx>(dbrecord_tuple) = _received_time_t;
                }

                if(0 != _exp_time_t)
                {
                    std::get<fsm::transfermanager::kExpTimestampIdx>(dbrecord_tuple) = _exp_time_t;
                }


                if(!data_blob.empty())
                {
                    std::get<fsm::transfermanager::kBlobIdx>(dbrecord_tuple) = data_blob;
                    std::get<fsm::transfermanager::kBlobOffset>(dbrecord_tuple) = data_blob.size();
                }
                else
                {
                    std::cerr<<"data_blob is empty"<< std::endl;
                }

                if(!_etags.empty())
                {
                    std::get<fsm::transfermanager::kEtagIdx>(dbrecord_tuple) = _etags;
                }

                std::get<fsm::transfermanager::kValidIdx>(dbrecord_tuple) = _valid;

                if(static_cast<int>(fsm::transfermanager::DbErrorCode::kNone) == rc)
                {
                    if(cachedb.AddDbRecord(dbrecord_tuple) != fsm::transfermanager::DbErrorCode_t::kNone)
                    {
                        std::cout<<"cCmd = " << _command<<std::endl;
                        std::cerr<<"Sqlite3 error = "<<cachedb.ExtendedSqlErrorGet()<<std::endl;
                    }
                }
                break;
            }
            case CommandCmd::kCmdDel:
            {
                if(!_url.empty())
                {
                    cachedb.DoesDbRecordExist(_url,existence_flag);
                    if(true == existence_flag)
                    {
                        if(cachedb.DelDbRecord(_url) != fsm::transfermanager::DbErrorCode_t::kNone)
                        {
                            std::cout<<"cCmd = " << _command<<std::endl;
                            std::cerr<<"Sqlite3 error = "<<cachedb.ExtendedSqlErrorGet()<<std::endl;
                        }
                    }
                    else
                    {
                        std:: cerr<<"kCmdDel: record with  url = '"<< _url <<"' does not exist"<<std::endl;
                    }
                }
                else
                {
                    std:: cerr<<"Empty url provided "<<std::endl;
                    rc = static_cast<int>(fsm::transfermanager::DbErrorCode_t::kInternalError);
                }
                break;
            }
            case CommandCmd::kCmdGet:
            {
                fsm::transfermanager::CacheDbRecord record_data;
                auto record_data_ptr = std::make_shared<fsm::transfermanager::CacheDbRecord>(record_data);
                if(_url.empty())
                {
                    std::cerr<<"kCmdGet: no _url provided"<<std::endl;
                    rc = static_cast<int>(fsm::transfermanager::DbErrorCode::kInternalError);
                    break;
                }

                if(!rc)
                {
                    std::time_t received_time_t = 0;
                    std::time_t exp_time_t = 0;
                    std::vector<char> datablob_;
                    std::size_t size_t_size = 0;
                    std::string etags;
                    bool _valid = true;
                    fsm::transfermanager::DbErrorCode_t dberrorcode = cachedb.DoesDbRecordExist(_url, _valid);
                    if((dberrorcode == fsm::transfermanager::DbErrorCode_t::kRowReturn) && (record_data_ptr != nullptr))
                    {
                        dberrorcode = cachedb.GetDbRecord(_url, record_data_ptr);
                        rc = static_cast<int>(dberrorcode);
                        std::tie(std::ignore, received_time_t, exp_time_t,
                        datablob_, size_t_size, etags,_valid) = *record_data_ptr;
                    }
                    else
                    {
                        rc = static_cast<int>(dberrorcode);
                        std::cerr<<"Can't find record error rc = "<<rc<<std::endl;
                    }

                    if(dberrorcode == fsm::transfermanager::DbErrorCode_t::kRowReturn)
                    {
                        std::cout<<_url<<_data_column_delimiter;
                        std::cout<<received_time_t<<_data_column_delimiter;
                        std::cout<<exp_time_t<<_data_column_delimiter;
                        std::copy(datablob_.begin(), datablob_.end(), std::ostream_iterator<char>(std::cout));
                        std::cout<<_data_column_delimiter;
                        std::cout<<size_t_size<<_data_column_delimiter;
                        std::cout<<etags<<_data_column_delimiter;
                        std::cout<<_valid<<std::endl;
                    }
                    else
                    {
                        std::cerr<<"Can't get record rc = "<<rc<<std::endl;
                    }
                }
                break;
            }
            case CommandCmd::kCmdBackup:
            {
                fsm::transfermanager::DbBackup(backup_path,cachedb);
                break;
            }
            case CommandCmd::kCmdRestore:
            {
                fsm::transfermanager::DbRestore(_datafile,cachedb);
                break;
            }
            case CommandCmd::kCmdUnknown:
            default:
            {
                rc = (-1);
                std::cerr<<"Command '"<< static_cast<int>(cmd) <<"' unknown rc ="<<rc<<std::endl;
                fsm::transfermanager::Usage();
            }
        } //switch
    }
    std::cout<< "rc = " << rc <<std::endl;
    return rc;
}

 /** \}    end of addtogroup */
