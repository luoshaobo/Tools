///////////////////////////////////////////////////////////////////
// Copyright (C) 2019 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file car_downloadagent_fileio.h
//  This file handles OTA
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author
// @Init date   14-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef VOC_TRANSACTIONS_CAR_DOWNLOAD_AGENT_FILEIO_H_
#define VOC_TRANSACTIONS_CAR_DOWNLOAD_AGENT_FILEIO_H_

class OpOTAInstallationSummary_Notification_Data;
class OpOTAExceptionReports_Notification_Data;

class CarDownloadAgentFileIO
{
public:
    template<typename T>
    bool WriteData(const T& data, const char *fileName)
    {
        bool writeResult = true;
        try
        {
            writeResult = Write(data, fileName);
        }
        catch (const std::exception&)
        {
            writeResult = false;
        }
        return writeResult;
    }

    template<class T>
    bool ReadData(T& data, const char *fileName)
    {
        bool readResult = true;
        try
        {
            readResult = Read(fileName, data);
        }
        catch (const std::exception&)
        {
            readResult = false;
        }
        return readResult;
    }

    void Print(const OpOTAInstallationSummary_Notification_Data& data);

    void Print(const OpOTAExceptionReports_Notification_Data& data);

private:
    bool Read( const char *fileName, OpOTAInstallationSummary_Notification_Data& data);
    bool Write(const OpOTAInstallationSummary_Notification_Data& data, const char *szXmlFileName);

    bool Read(const char *fileName, OpOTAExceptionReports_Notification_Data& data);
    bool Write(const OpOTAExceptionReports_Notification_Data& data, const char *szXmlFileName);

private:

    // comment
    const char* VERSION = "version";
    const char* ENCODING = "encoding";
    const char* DEFAULTVERSION = "1.0";
    const char* DEFAULTENCODING = "utf - 8";

    // OpOTAExceptionReports
    const char* OTAEXCEPTIONREPORTS_DATA = "OTAExceptionReports_Data";
    const char* EXCEPTIONREPORTMSGREMAINING = "exceptionreportmsgremaining";
    const char* EXCEPTIONREPORTS = "exceptionreports";
    const char* EXCEPTIONREPORT = "exceptionreport";
    const char* ISSUERID = "issuerid";
    const char* EXCEPTIONMESSAGE = "exceptionmessage";
    const char* ACTIVITY = "activity";
    const char* ACTION = "action";
    const char* EXCEPTION = "exception";
    const char* INSTALLATIONORDERID = "installationorderid";
    const char* ID = "id";
    const char* CLIENTCONFIGURATIONID = "clientconfigurationid";
    const char* DATAFILENAME = "datafilename";
    const char* NAME = "name";
    const char* FILE = "file";
    const char* KEY = "key";
    const char* SOFTWAREPARTIDENTIFIER = "softwarepartidentifier";
    const char* DATABLOCK = "datablock";
    const char* BLOCKNUMBER = "blockNumber";

    // OTAInstallationSummary
    const char* ROOT = "Notification_Data";
    const char* INSTALLATIONSUMMARY = "installationsummary";
    const char* REPEATRESETS = "repeatresets";
    const char* TOTALINSTALLATIONTIME = "totalinstallationtime";
    const char* INSTALLATIONINORDERID = "installationinorderid";
    const char* UUID = "uuid";
    const char* TIMESTAMP = "timestamp";
    const char* TIMESTAMP1 = "timestamp1";
    const char* ECUSUMMARYS = "ecusummarys";
    const char* ECUSUMMARY = "ecusummary";
    const char* ECURETRIES = "ecuretries";
    const char* ECUINSTALLATIONSTATUS = "ecuinstallationstatus";
    const char* ECU = "ecu";
    const char* ADDR = "addr";
    const char* SOFTWAREPARTINSTALLATIONSUMMARYS = "softwarepartinstallationsummarys";
    const char* SOFTWAREPARTINSTALLATIONSUMMARY = "softwarepartinstallationsummary";
    const char* SOFTWAREPARTRETRIES = "softwarepartretries";
    const char* MEASUREDINSTALLATIONTIME = "measuredinstallationtime";
    const char* SOFTWAREPARTINSTALLATIONSTATUS = "softwarepartinstallationstatus";
    const char* PARTIDENTIFIER = "partidentifier";
    const char* PARTID = "partid";
};

#endif
