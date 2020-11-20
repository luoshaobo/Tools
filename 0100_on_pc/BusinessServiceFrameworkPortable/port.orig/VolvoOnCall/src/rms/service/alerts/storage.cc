////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file storage.cc
//  This file handles the communication with the VuC, using the OTP VCS-interface
 
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author      Shen Jiaojiao
// @Init date   12-Sep-2018
////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <zlib.h>
#include <unistd.h>
#include <boost/random.hpp>

#include <rms/service/alerts/storage.h>
#include <rms/util/utils.h>
#include "dlt/dlt.h"

DLT_IMPORT_CONTEXT(dlt_voc);

namespace nvm {

using namespace boost::filesystem;
using namespace std;


uint32_t Data::mSeqId = 1;
//------------------------------------------------------------------------------
boost::shared_ptr<Data> convert2Supplementary(
        boost::shared_ptr<Data>& xevinfo) {
    return boost::shared_ptr<Data>(new DataSupplementary(xevinfo->getData()));
}


///////////////////////////////////////////////////////////////////////////////
//! \fn Storage::Storage(const std::string& workFolder)
//! \brief Constructor
//! \param[in] workFolder Parent folder for storing data
//! \return void
///////////////////////////////////////////////////////////////////////////////
Storage::Storage(const std::string& workFolder):
    mDirectory(boost::filesystem::path(workFolder.c_str())) {
    init();
}

///////////////////////////////////////////////////////////////////////////////
//! \fn Storage::~Storage()
//! \brief destructor
//! \return void
///////////////////////////////////////////////////////////////////////////////

Storage::~Storage(){
}

///////////////////////////////////////////////////////////////////////////////
//! \fn Storage::init()
//! \brief Data storage initialization. Create folders for different data types
//! \return void
///////////////////////////////////////////////////////////////////////////////
void Storage::init() const {
    boost::filesystem::path paths [] = {
            boost::filesystem::path(mDirectory.path()/DataRealTime::typeName()),
            boost::filesystem::path(mDirectory.path()/DataSupplementary::typeName()),
            boost::filesystem::path(mDirectory.path()/DataWarning::typeName()),
    };
    for (uint32_t i = 0; i < sizeof(paths)/sizeof(paths[0]); ++i) {
        if (!exists(paths[i])) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Creating directory %s",
                paths[i].generic_string().c_str());
            create_directories(paths[i]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//! \fn Storage::push(const boost::shared_ptr<Data>& xevinfo)
//! \brief save data on file system. Depends on type data will be stored in different prefix names
//! \param[in] xevinfo Pointer to data which should be stored
//! \return true if success
///////////////////////////////////////////////////////////////////////////////
bool Storage::push(const boost::shared_ptr<Data>& xevinfo) const {
    return save(xevinfo);
}

///////////////////////////////////////////////////////////////////////////////
//! \fn Storage::front()
//! \brief Read oldest data from file system
//! \return pointer to Data which were read
///////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<Data> Storage::front() const {
    boost::shared_ptr<Data> xevinfo[] = {
            boost::shared_ptr<Data>(new DataWarning()),
            boost::shared_ptr<Data>(new DataRealTime()),
            boost::shared_ptr<Data>(new DataSupplementary()),
    };
    boost::shared_ptr<Data> sp;
    std::vector<path> files;

    for (uint32_t i = 0; i < sizeof(xevinfo)/sizeof(xevinfo[0]); ++i) {
        files.clear();
        boost::filesystem::path path = mDirectory.path()/xevinfo[i]->getTypeName();
        std::copy(directory_iterator(path), directory_iterator(),
                std::back_inserter(files));
        if (files.empty()) {
            continue;
        }
        std::sort(files.begin(), files.end());
        sp = load(files.front());
        if (sp) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Load first available alert file %s",
                    files.front().generic_string().c_str());
            break;
        }
    }

    return sp;
}

///////////////////////////////////////////////////////////////////////////////
//! \fn Storage::erase(const boost::shared_ptr<Data>& xevinfo)
//! \brief erase xevcdm data
//! \param[in] xevinfo Pointer to data which should be erased
//! \return true if success
///////////////////////////////////////////////////////////////////////////////
bool Storage::erase(const boost::shared_ptr<Data>& xevinfo) const {
    bool rc = false;
    do {
        if (!xevinfo) {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RMS: No Input data");
            break;
        }
        if (xevinfo->getPath().empty()) {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RMS: Cannot erase empty path");
            break;
        }
        if (!exists(xevinfo->getPath())) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: File %s not found",
                    xevinfo->getPath().generic_string().c_str());
            break;
        }
        rc = remove(xevinfo->getPath());
        //::sync();
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Erase file %s", xevinfo->getPath().generic_string().c_str())
    } while(false);
    return rc;
}


///////////////////////////////////////////////////////////////////////////////
//! \fn Storage::size()
//! \brief count storage size
//! \return count of files on FS
///////////////////////////////////////////////////////////////////////////////
size_t Storage::size() const {
    boost::filesystem::path dir = mDirectory.path();
    boost::filesystem::path paths [] = {
            boost::filesystem::path(dir/DataWarning::typeName()),
            boost::filesystem::path(dir/DataRealTime::typeName()),
            boost::filesystem::path(dir/DataSupplementary::typeName()),
    };
    size_t sz = 0;
    for (uint32_t i = 0; i < sizeof(paths)/sizeof(paths[0]); ++i) {
        sz += size(paths[i]);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Found %d files in folder %s", sz,
            paths[i].generic_string().c_str());
    }
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Found total %d files in parent folder %s", sz,
        dir.generic_string().c_str());
    return sz;
}

///////////////////////////////////////////////////////////////////////////////
//! \fn Storage::sync(std::map<nvm::Data::Type,nvm::Storage::AlertQueue >& aqueue)
//! \brief TODO
//! \param[in] aqueue TODO
//! \return true if success
///////////////////////////////////////////////////////////////////////////////
bool Storage::sync(std::map<nvm::Data::Type,
        nvm::Storage::AlertQueue >& aqueue) const {
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Storage::%s", __FUNCTION__);
    bool rc = true;
    std::list<path> files;

    boost::filesystem::path dir = mDirectory.path();
    std::pair<nvm::Data::Type, boost::filesystem::path> adata[] = {
        {make_pair(DataWarning::type(),         boost::filesystem::path(dir/DataWarning::typeName()))},
        {make_pair(DataRealTime::type(),        boost::filesystem::path(dir/DataRealTime::typeName()))},
        {make_pair(DataSupplementary::type(),   boost::filesystem::path(dir/DataSupplementary::typeName()))},
    };


    for (uint32_t i = 0; i < sizeof(adata)/sizeof(adata[0]); ++i) {
        files.clear();
        // fill list with existing paths of available alerts from flash
        std::copy(directory_iterator(adata[i].second), directory_iterator(),
                std::back_inserter(files));
        files.sort();
        // go through paths
        uint32_t filesCounter = 0;
        for (std::list<path>::iterator it = files.begin(); it != files.end();
                ++it, ++filesCounter) {
            boost::shared_ptr<Data> xevinfo = load(*it);
            if (!xevinfo) {
                continue;
            }
            if (filesCounter < Constants::LOG_LOAD_FILES) {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Load %d bytes from the file %s", xevinfo->getData().size(),
                        xevinfo->getPath().generic_string().c_str());
            } else if (filesCounter == Constants::LOG_LOAD_FILES) {
                DLT_LOG_STRING(dlt_voc, DLT_LOG_DEBUG, "RMS: Skip logging loading files");
            }

            // find if already have it in memory
            AlertQueue::iterator ait = std::find(aqueue[adata[i].first].begin(),
                    aqueue[adata[i].first].end(), xevinfo);
            if (ait != aqueue[adata[i].first].end()) {
                continue;
            }
            // add to memory storage
            aqueue[adata[i].first].push_back(xevinfo);
            if (filesCounter < Constants::LOG_LOAD_FILES) {
                DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Add supplementary alert %s to memory",
                        xevinfo->getPath().generic_string().c_str());
            }
        }
        if (filesCounter) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Loaded total %d files", filesCounter);
        }

        bool saveRc = true;
        // Now go through memory and save all new alerts to flash
        for (AlertQueue::iterator ait = aqueue[adata[i].first].begin();
                ait != aqueue[adata[i].first].end(); ++ait) {
            if (!(*ait)->getPath().empty()) {
                continue;
            }
            saveRc = save(*ait);
            rc = rc ? saveRc : rc;
        }

        if (adata[i].first != DataSupplementary::type()) {
            continue;
        }
        // Remove old supplementary alerts
        while (aqueue[adata[i].first].size()
                > Settings::getInstance()->getMaxTotalAlerts()) {
            erase(aqueue[adata[i].first].front());
            aqueue[adata[i].first].pop_front();
        }
    }
    ::sync();
    return rc;
}

///////////////////////////////////////////////////////////////////////////////
//! \fn Storage::save(const boost::shared_ptr<Data>& xevinfo)
//! \brief save data on file system. Depends on type data will be stored in different prefix names
//! \param[in] xevinfo Pointer to data which should be stored
//! \return true if success
///////////////////////////////////////////////////////////////////////////////
bool Storage::save(const boost::shared_ptr<Data>& xevinfo) const {
    std::vector<uint8_t> raw;
    bool rc = false;
    do {
        if (!xevinfo) {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RMS: No Input data");
            break;
        }
        if (xevinfo->getData().empty()) {
            DLT_LOG_STRING(dlt_voc, DLT_LOG_ERROR, "RMS:Cannot save empty data");
            break;
        }
        boost::filesystem::path dir = mDirectory.path()/xevinfo->getTypeName();
        //if (xevinfo->getType() == DataWarning::type()) {
        //    if (!(size(dir) < Settings::getInstance()->getMaxWarningAlerts())) {
        //        DLT_LOG_STRING(dlt_voc, DLT_LOG_INFO, "RMS: Warning alerts limit has been reached");
        //        break;
        //    }
        //}

        boost::filesystem::path path = dir/genFileName();
        boost::filesystem::ofstream ofs(path);

        raw = xevinfo->getData();
        uint32_t crc = crc32(0, raw.data(), raw.size());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: crc = %08X", crc);
        //printDump(raw.data(), raw.size());
        std::copy((uint8_t*)&crc, (uint8_t*)((uint32_t)&crc+sizeof(crc)),
                back_inserter(raw));
        //printDump(raw.data(), raw.size());
        ofs.write(reinterpret_cast<char*>(raw.data()), raw.size());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "RMS: Save %d bytes to the file %s", raw.size(), path.c_str());
        xevinfo->setPath(path);
        rc = true;
    } while (false);
    return rc;
}

///////////////////////////////////////////////////////////////////////////////
//! \fn Storage::load(const boost::filesystem::path& file)
//! \brief read data from file
//! \param[in] file File path
//! \return Pointrt to Data
///////////////////////////////////////////////////////////////////////////////
boost::shared_ptr<Data> Storage::load(const boost::filesystem::path& file) const {
    boost::shared_ptr<Data> xevinfo;
    boost::shared_ptr<Data> temp;
    do {
        if (file.generic_string().empty()) {
            break;
        }
        if (!exists(file)) {
            break;
        }
        if (file.generic_string().rfind(DataWarning::typeName())
                != std::string::npos) {
            temp.reset(new DataWarning());
        } else if (file.generic_string().rfind(DataRealTime::typeName())
                != std::string::npos) {
            temp.reset(new DataRealTime());
        } else if (file.generic_string().rfind(DataSupplementary::typeName())
                != std::string::npos) {
            temp.reset(new DataSupplementary());
        }
        temp->setPath(file);
        size_t fileSize = boost::filesystem::file_size(file);
        if (!fileSize) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: File %s has %d size",
                file.generic_string().c_str(),
                fileSize);
            erase(temp);
            break;
        }
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "RMS: Load %d bytes from the file %s", fileSize,
                file.generic_string().c_str());
        boost::filesystem::ifstream ifs(file);
        std::vector<uint8_t> raw(fileSize, 0);
        ifs.read(reinterpret_cast<char*>(raw.data()), raw.size());
        uint32_t crcRead = *reinterpret_cast<uint32_t*>(&raw.at(raw.size() -
                sizeof(crcRead)));
        raw.resize(raw.size() - sizeof(crcRead));
        uint32_t crcCalc = crc32(0, raw.data(), raw.size());
        if (crcCalc != crcRead) {
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_ERROR, "RMS: CRC mismatch calc(%08X) != read(%08X). "
                    "File %s will be deleted", crcCalc, crcRead,
                    file.generic_string().c_str());
            erase(temp);
            break;
        }
        xevinfo = temp;
        xevinfo->setData(raw);
    } while(false);
    return xevinfo;
}

///////////////////////////////////////////////////////////////////////////////
//! \fn Storage::size(const boost::filesystem::path& dir)
//! \brief count files in folder
//! \param[in] folder path
//! \return count of files
///////////////////////////////////////////////////////////////////////////////
size_t Storage::size(const boost::filesystem::path& dir) const {
    return std::count_if(directory_iterator(dir),
                         directory_iterator(),
                         static_cast<bool(*)(const path&)>(is_regular_file));
}

///////////////////////////////////////////////////////////////////////////////
//! \fn Storage::genFileName()
//! \brief generate unique file name
//! \return string with unique file name
///////////////////////////////////////////////////////////////////////////////
std::string Storage::genFileName() const {
    static boost::random::uniform_int_distribution<> bnd(1,0xFF);
    static boost::random::mt19937 rng;
    static uint32_t counter = 0;
    Time tm = Settings::getInstance()->getUTCTime();
    ++counter;
    size_t buffSize = snprintf(NULL, 0,
            "%02d%02d%02d%02d%02d%02d_%d_%02X.xev",
            tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, counter,
            bnd(rng));

    std::vector<char> buff(buffSize+1 , 0);
    snprintf(buff.data(), buff.size(),
            "%02d%02d%02d%02d%02d%02d_%d_%02X.xev",
            tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, counter,
            bnd(rng));
    return buff.data();
}
}
