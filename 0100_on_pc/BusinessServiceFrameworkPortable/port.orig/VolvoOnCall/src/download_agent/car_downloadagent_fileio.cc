///////////////////////////////////////////////////////////////////
// Copyright (C) 2019 Continental AG and subsidiaries
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  @file car_downloadagent_fileio.cpp
//  This file handles the OTA
// @project     GLY_TCAM
// @subsystem   VolvoOnCall
// @author
// @Init date   07-Feb-2019
///////////////////////////////////////////////////////////////////
#include <iostream>
#include <algorithm>

#include "download_agent/car_downloadagent_fileio.h"
#include "dlt/dlt.h"
#include "ipcb_data.hpp"
#include "pugixml/pugixml.hpp"

DLT_IMPORT_CONTEXT(dlt_voc);

void CarDownloadAgentFileIO::Print(const OpOTAInstallationSummary_Notification_Data& data)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"installationinorderid:[%s]",data.installationsummary.installationinorderid.uuid.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"timestamp:[%s]",data.installationsummary.timestamp.timestamp1.c_str());
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"repeatresets:[%ud]",data.installationsummary.repeatresets);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"totalinstallationtime:[%ud]",data.installationsummary.totalinstallationtime);
    int i = 0;
    int j = 0;
    std::for_each(data.installationsummary.ecusummary.begin(),data.installationsummary.ecusummary.end(),[&i,&j](const Ecusummarydata_Data& ecuSummarydata){
        ++i;
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"%d ecu.addr:[%s]",i,ecuSummarydata.ecu.addr.c_str());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"%d ecuretries:[%ud]",i,ecuSummarydata.ecuretries);
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"%d ecuinstallationstatus:[%s]",i,ecuSummarydata.ecuinstallationstatus.c_str());

        j = 0;
        const std::vector<SoftwarePartInstallationsummaryData_Data>& installationSummary = ecuSummarydata.softwarepartinstallationsummary;
        std::for_each(installationSummary.begin(),installationSummary.end(),[&i,&j](const SoftwarePartInstallationsummaryData_Data& partSummary){
            ++j;
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"%d %d partid:[%s]",i,j,partSummary.partidentifier.partid.c_str());
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"%d %d softwarepartretries:[%ud]",i,j,partSummary.softwarepartretries);
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"%d %d measuredinstallationtime:[%ud]",i,j,partSummary.measuredinstallationtime); 
            DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,"%d %d softwarepartinstallationstatus:[%s]",i,j,partSummary.softwarepartinstallationstatus.c_str()); 
        });
    });
}

bool CarDownloadAgentFileIO::Read( const char *fileName, OpOTAInstallationSummary_Notification_Data& data )
{
  DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CarDownloadAgentFileIO::%s", __FUNCTION__);

  pugi::xml_document xmlDoc;
  if (!xmlDoc.load_file(fileName, pugi::parse_default, pugi::encoding_utf8))
  {
       DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CarDownloadAgentFileIO::%s read file error : %s", __FUNCTION__, fileName);
       return false;
  }

  pugi::xml_node nodeRoot = xmlDoc.child(ROOT);
  pugi::xml_node nodeIS = nodeRoot.child(INSTALLATIONSUMMARY);

  // repeatresets
  data.installationsummary.repeatresets = nodeIS.attribute(REPEATRESETS).as_uint();

  // totalinstallationtime
  data.installationsummary.totalinstallationtime = nodeIS.attribute(TOTALINSTALLATIONTIME).as_uint();

  // installationinorderid.uuid
  pugi::xml_node nodeIOI = nodeIS.child(INSTALLATIONINORDERID);
  data.installationsummary.installationinorderid.uuid = nodeIOI.attribute(UUID).as_string();

  // timestamp.timestamp1
  pugi::xml_node nodeTS = nodeIS.child(TIMESTAMP);
  data.installationsummary.timestamp.timestamp1 = nodeTS.attribute(TIMESTAMP1).as_string();

  // ecusummarys
  pugi::xml_node nodeESS = nodeIS.child(ECUSUMMARYS);
  if (nodeESS.empty()) {
       data.installationsummary.ecusummary.clear();
       return true;
  }

  // ecusummary
  for (pugi::xml_node nodeES = nodeESS.first_child(); nodeES; nodeES = nodeES.next_sibling())
  {
       // ecusummary
       Ecusummarydata_Data ecuSummary;
       // ecuretries
       ecuSummary.ecuretries = nodeES.attribute(ECURETRIES).as_uint();
       // ecuinstallationstatus
       ecuSummary.ecuinstallationstatus = nodeES.attribute(ECUINSTALLATIONSTATUS).as_string();
       
       // ecu.addr
       pugi::xml_node nodeECU = nodeES.child(ECU);
       ecuSummary.ecu.addr = nodeECU.attribute(ADDR).as_string();

       // softwarepartinstallationsummarys
       pugi::xml_node nodeSRISS = nodeES.child(SOFTWAREPARTINSTALLATIONSUMMARYS);
       if (nodeSRISS.empty()) {
            ecuSummary.softwarepartinstallationsummary.clear();
            continue;
       }

       for (pugi::xml_node nodeSRIS = nodeSRISS.first_child(); nodeSRIS; nodeSRIS = nodeSRIS.next_sibling())
       {
            SoftwarePartInstallationsummaryData_Data data;
            // softwarepartretries
            data.softwarepartretries = nodeSRIS.attribute(SOFTWAREPARTRETRIES).as_uint();
            // measuredinstallationtime
            data.measuredinstallationtime = nodeSRIS.attribute(MEASUREDINSTALLATIONTIME).as_uint();
            // softwarepartinstallationstatus
            data.softwarepartinstallationstatus = nodeSRIS.attribute(SOFTWAREPARTINSTALLATIONSTATUS).as_string();

            // partidentifier.partid
            pugi::xml_node nodeP = nodeSRIS.child(PARTIDENTIFIER);
            data.partidentifier.partid = nodeP.attribute(PARTID).as_string();

            ecuSummary.softwarepartinstallationsummary.push_back(data);
       }

       data.installationsummary.ecusummary.push_back(ecuSummary);
  }

  return true;
}

bool CarDownloadAgentFileIO::Write(const OpOTAInstallationSummary_Notification_Data& data, const char *szXmlFileName)
{
  DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CarDownloadAgentFileIO::%s", __FUNCTION__);
  
  pugi::xml_document xmlDoc;

  // write version
  pugi::xml_node pre = xmlDoc.prepend_child(pugi::node_declaration);
  pre.append_attribute(VERSION) = DEFAULTVERSION;
  pre.append_attribute(ENCODING) = DEFAULTENCODING;

  // root item
  pugi::xml_node nodeRoot = xmlDoc.append_child(ROOT);

  // installationsummary->[repeatresets,totalinstallationtime,installationinorderid,timestamp,ecusummary_s]
  const InstallationSummary_Data& installationsummary = data.installationsummary;
  pugi::xml_node nodeIS = nodeRoot.append_child(INSTALLATIONSUMMARY);

  // repeatresets
  nodeIS.append_attribute(REPEATRESETS).set_value(installationsummary.repeatresets);

  // totalinstallationtime
  nodeIS.append_attribute(TOTALINSTALLATIONTIME).set_value(installationsummary.totalinstallationtime);
  // installationinorderid
  {
       const UUID_Data& installationinOrderId = data.installationsummary.installationinorderid;
       pugi::xml_node nodeInstallationinorderId = nodeIS.append_child(INSTALLATIONINORDERID);
       nodeInstallationinorderId.append_attribute(UUID).set_value(installationinOrderId.uuid.c_str());
  }
  // timestamp
  {
       const ISOTimeStamp_Data& timestamp = data.installationsummary.timestamp;
       pugi::xml_node nodeTimestamp = nodeIS.append_child(TIMESTAMP);
       nodeTimestamp.append_attribute(TIMESTAMP1).set_value(timestamp.timestamp1.c_str());
  }

  // ecusummarys
  const std::vector<Ecusummarydata_Data>& ecusummarys = data.installationsummary.ecusummary;
  if (!ecusummarys.empty()) {
       pugi::xml_node nodeEcusummarys = nodeIS.append_child(ECUSUMMARYS);
       for (auto item = ecusummarys.begin(); item != ecusummarys.end(); ++item)
       {
            // ecusummary -> [ecuretries,ecuinstallationstatus,ecu,softwarepartinstallationsummary_s]
            const Ecusummarydata_Data& ecusummary = *item;
            pugi::xml_node nodeES = nodeEcusummarys.append_child(ECUSUMMARY);

            // ecuretries
            nodeES.append_attribute(ECURETRIES).set_value(ecusummary.ecuretries);
            // ecuinstallationstatus
            nodeES.append_attribute(ECUINSTALLATIONSTATUS).set_value(ecusummary.ecuinstallationstatus.c_str());
            // ecu
            {
                 pugi::xml_node nodeEcu = nodeES.append_child(ECU);
                 nodeEcu.append_attribute(ADDR).set_value(ecusummary.ecu.addr.c_str());
            }
            // softwarepartinstallationsummary_s
            const std::vector<SoftwarePartInstallationsummaryData_Data>& ecusummarys = ecusummary.softwarepartinstallationsummary;
            if (!ecusummarys.empty()) {

                 pugi::xml_node nodeSoftwarepartInstallationsummarys = nodeES.append_child(SOFTWAREPARTINSTALLATIONSUMMARYS);
                 for (auto item2 = ecusummarys.begin(); item2 != ecusummarys.end(); ++item2)
                 {
                      const SoftwarePartInstallationsummaryData_Data& softwarepartinstallationsummary = *item2;

                      //[ partidentifier,softwarepartretries, measuredinstallationtime,softwarepartinstallationstatus ]
                      pugi::xml_node nodeSIS = nodeSoftwarepartInstallationsummarys.append_child(SOFTWAREPARTINSTALLATIONSUMMARY);

                      // softwarepartretries
                      nodeSIS.append_attribute(SOFTWAREPARTRETRIES).set_value(softwarepartinstallationsummary.softwarepartretries);

                      // measuredinstallationtime
                      nodeSIS.append_attribute(MEASUREDINSTALLATIONTIME).set_value(softwarepartinstallationsummary.measuredinstallationtime);

                      // softwarepartinstallationstatus
                      nodeSIS.append_attribute(SOFTWAREPARTINSTALLATIONSTATUS).set_value(
                        softwarepartinstallationsummary.softwarepartinstallationstatus.c_str());

                      // ecu
                      {
                           pugi::xml_node nodePartidentifier = nodeSIS.append_child(PARTIDENTIFIER);
                           nodePartidentifier.append_attribute(PARTID).set_value(
                             softwarepartinstallationsummary.partidentifier.partid.c_str());
                      }
                 }
            }
       }
  }

  return xmlDoc.save_file(szXmlFileName, "\t", 1U, pugi::encoding_utf8);
}


bool CarDownloadAgentFileIO::Write(const OpOTAExceptionReports_Notification_Data& data, const char *szXmlFileName)
{
    pugi::xml_document xmlDoc;

    // write version
    pugi::xml_node pre = xmlDoc.prepend_child(pugi::node_declaration);
    pre.append_attribute(VERSION) = DEFAULTVERSION;
    pre.append_attribute(ENCODING) = DEFAULTENCODING;

    // root item
    pugi::xml_node nodeRoot = xmlDoc.append_child(OTAEXCEPTIONREPORTS_DATA);
    // OTAExceptionReports_Notification_Data->[exceptionreportmsgremaining,exceptionreports]
    nodeRoot.append_attribute(EXCEPTIONREPORTMSGREMAINING).set_value(data.exceptionreportmsgremaining);

    pugi::xml_node nodeEPS = nodeRoot.append_child(EXCEPTIONREPORTS);

    std::for_each(data.exceptionreports.begin(), data.exceptionreports.end(), [this,&nodeEPS](const ExceptionReport1_Data& e) {
        pugi::xml_node nodeEP = nodeEPS.append_child(EXCEPTIONREPORT);

        // timestamp
        pugi::xml_node nodeTS = nodeEP.append_child(TIMESTAMP);
        nodeTS.append_attribute(TIMESTAMP1).set_value(e.timestamp.timestamp1.c_str());

        // exceptionreport
        nodeEP.append_attribute(ISSUERID).set_value(e.issuerid.c_str());

        // exceptionmessage
        pugi::xml_node nodeEM = nodeEP.append_child(EXCEPTIONMESSAGE);
        nodeEM.append_attribute(ACTIVITY).set_value(e.exceptionmessage.activity.c_str());
        nodeEM.append_attribute(ACTION).set_value(e.exceptionmessage.action.c_str());
        nodeEM.append_attribute(EXCEPTION).set_value(e.exceptionmessage.exception.c_str());

        // installationorderid.id.uuid
        if(e.installationorderid.id_choice)
        {
            pugi::xml_node nodeIOI = nodeEP.append_child(INSTALLATIONORDERID);
            pugi::xml_node nodeId = nodeIOI.append_child(ID);
            nodeId.append_attribute(UUID).set_value(e.installationorderid.id.uuid.c_str());
        }

        // clientconfigurationid.version.uuid
        if (e.clientconfigurationid.version_choice)
        {
            pugi::xml_node nodeCCI = nodeEP.append_child(CLIENTCONFIGURATIONID);
            pugi::xml_node nodeV = nodeCCI.append_child(VERSION);
            nodeV.append_attribute(UUID).set_value(e.clientconfigurationid.version.uuid.c_str());
        }

        // datafilename.file.name
        if (e.datafilename.file_choice)
        {
            pugi::xml_node nodeDFN = nodeEP.append_child(DATAFILENAME);
            pugi::xml_node nodeF = nodeDFN.append_child(FILE);
            nodeF.append_attribute(NAME).set_value(e.datafilename.file.name.c_str());
        }

        // addr.addr.addr
        if (e.addr.addr_choice)
        {
            pugi::xml_node nodeAddr = nodeEP.append_child(ADDR);
            pugi::xml_node nodeAddrAddr = nodeAddr.append_child(ADDR);
            nodeAddrAddr.append_attribute(ADDR).set_value(e.addr.addr.addr.c_str());
        }

        // key.key.key
        if (e.key.key_choice)
        {
            pugi::xml_node nodeKey = nodeEP.append_child(KEY);
            pugi::xml_node nodeKeykey = nodeKey.append_child(KEY);
            nodeKeykey.append_attribute(KEY).set_value(e.key.key.key.c_str());
        }

        // softwarepartidentifier.id.partid
        if (e.softwarepartidentifier.id_choice)
        {
            pugi::xml_node nodeSPI = nodeEP.append_child(SOFTWAREPARTIDENTIFIER);
            pugi::xml_node nodeId = nodeSPI.append_child(ID);
            nodeId.append_attribute(PARTID).set_value(e.softwarepartidentifier.id.partid.c_str());
        }

        // datablock.blockNumber
        if (e.datablock.blockNumber_choice)
        {
            pugi::xml_node nodeDB = nodeEP.append_child(DATABLOCK);
            nodeDB.append_attribute(BLOCKNUMBER).set_value(e.datablock.blockNumber.c_str());
        }
    });

    return xmlDoc.save_file(szXmlFileName, "\t", 1U, pugi::encoding_utf8);
}

bool CarDownloadAgentFileIO::Read(const char *fileName, OpOTAExceptionReports_Notification_Data& data)
{
    pugi::xml_document xmlDoc;
    if (!xmlDoc.load_file(fileName, pugi::parse_default, pugi::encoding_utf8))
    {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_INFO, "CarDownloadAgentFileIO::%s read file error : %s", __FUNCTION__, fileName);
        return false;
    }

    // Notification_Data
    pugi::xml_node nodeOTAERD = xmlDoc.child(OTAEXCEPTIONREPORTS_DATA);

    // exceptionreportmsgremaining
    data.exceptionreportmsgremaining = nodeOTAERD.attribute(EXCEPTIONREPORTMSGREMAINING).as_uint();

    // exceptionreports
    pugi::xml_node nodeERS = nodeOTAERD.child(EXCEPTIONREPORTS);
    if (nodeERS.empty()) {
        data.exceptionreports.clear();
        return true;
    }

    for (pugi::xml_node nodeER = nodeERS.first_child(); nodeER; nodeER = nodeER.next_sibling())
    {
        // ecusummary
        ExceptionReport1_Data exceptionReport;

        // issuerid
        exceptionReport.issuerid = nodeER.attribute(ISSUERID).as_string();

        // timestamp.timestamp1
        pugi::xml_node nodeTS = nodeER.child(TIMESTAMP);
        exceptionReport.timestamp.timestamp1 = nodeTS.attribute(TIMESTAMP1).as_string();

        // exceptionmessage [activity,action,exception]
        pugi::xml_node nodeEM = nodeER.child(EXCEPTIONMESSAGE);
        exceptionReport.exceptionmessage.activity = nodeEM.attribute(ACTIVITY).as_string();
        exceptionReport.exceptionmessage.action = nodeEM.attribute(ACTION).as_string();
        exceptionReport.exceptionmessage.exception = nodeEM.attribute(EXCEPTION).as_string();

        // installationorderid.id.uuid
        pugi::xml_node nodeIOI = nodeER.child(INSTALLATIONORDERID);
        if (!nodeIOI.empty())
        {
            pugi::xml_node nodeId = nodeIOI.child(ID);
            exceptionReport.installationorderid.id.uuid = nodeId.attribute(UUID).as_string();
            exceptionReport.installationorderid.id_choice = true;
            exceptionReport.installationorderid.noid_choice = false;
        }
        else
        {
            exceptionReport.installationorderid.id_choice = false;
            exceptionReport.installationorderid.noid_choice = true;
            exceptionReport.installationorderid.id.uuid.clear();
        }

        // clientconfigurationid.version.uuid
        pugi::xml_node nodeCCI = nodeER.child(CLIENTCONFIGURATIONID);
        if (!nodeCCI.empty())
        {
            pugi::xml_node nodeId = nodeIOI.child(ID);
            exceptionReport.clientconfigurationid.version.uuid = nodeId.attribute(UUID).as_string();
            exceptionReport.clientconfigurationid.version_choice = true;
            exceptionReport.clientconfigurationid.noversion_choice = false;
        }
        else
        {
            exceptionReport.clientconfigurationid.version_choice = false;
            exceptionReport.clientconfigurationid.noversion_choice = true;
            exceptionReport.clientconfigurationid.version.uuid.clear();
        }

        // datafilename.file.name
        pugi::xml_node nodeDF = nodeER.child(DATAFILENAME);
        if (!nodeDF.empty())
        {
            pugi::xml_node nodeF = nodeDF.child(FILE);
            exceptionReport.datafilename.file.name = nodeF.attribute(NAME).as_string();
            exceptionReport.datafilename.file_choice = true;
            exceptionReport.datafilename.nofile_choice = false;
        }
        else
        {
            exceptionReport.datafilename.file_choice = false;
            exceptionReport.datafilename.nofile_choice = true;
            exceptionReport.datafilename.file.name.clear();
        }

        // addr.addr.addr
        pugi::xml_node nodeA = nodeER.child(ADDR);
        if (!nodeA.empty())
        {
            pugi::xml_node nodeAA = nodeA.child(ADDR);
            exceptionReport.addr.addr.addr = nodeAA.attribute(ADDR).as_string();
            exceptionReport.addr.addr_choice = true;
            exceptionReport.addr.noaddr_choice = false;
        }
        else
        {
            exceptionReport.addr.addr_choice = false;
            exceptionReport.addr.noaddr_choice = true;
            exceptionReport.addr.addr.addr.clear();
        }

        // key.key.key
        pugi::xml_node nodeK = nodeER.child(KEY);
        if (!nodeK.empty())
        {
            pugi::xml_node nodeKK = nodeK.child(KEY);
            exceptionReport.key.key.key = nodeKK.attribute(KEY).as_string();
            exceptionReport.key.key_choice = true;
            exceptionReport.key.nokey_choice = false;
        }
        else
        {
            exceptionReport.key.key_choice = false;
            exceptionReport.key.nokey_choice = true;
            exceptionReport.key.key.key.clear();
        }

        // softwarepartidentifier.id.partid
        pugi::xml_node nodeSPI = nodeER.child(SOFTWAREPARTIDENTIFIER);
        if (!nodeSPI.empty())
        {
            pugi::xml_node nodeI = nodeSPI.child(ID);
            exceptionReport.softwarepartidentifier.id.partid = nodeI.attribute(PARTID).as_string();
            exceptionReport.softwarepartidentifier.id_choice = true;
            exceptionReport.softwarepartidentifier.noid_choice = false;
        }
        else
        {
            exceptionReport.softwarepartidentifier.id_choice = false;
            exceptionReport.softwarepartidentifier.noid_choice = true;
            exceptionReport.softwarepartidentifier.id.partid.clear();
        }

        // datablock.blockNumber
        pugi::xml_node nodeDB = nodeER.child(DATABLOCK);
        if (!nodeDB.empty())
        {
            exceptionReport.datablock.blockNumber = nodeDB.attribute(BLOCKNUMBER).as_string();
            exceptionReport.datablock.blockNumber_choice = true;
            exceptionReport.datablock.noData_choice = false;
        }
        else
        {
            exceptionReport.datablock.blockNumber_choice = false;
            exceptionReport.datablock.noData_choice = true;
            exceptionReport.datablock.blockNumber.clear();
        }

        data.exceptionreports.push_back(exceptionReport);
    }

    return true;
}

void CarDownloadAgentFileIO::Print(const OpOTAExceptionReports_Notification_Data& data)
{
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "exceptionreportmsgremaining:%d", data.exceptionreportmsgremaining);
    DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "exceptionreports.size():%u", data.exceptionreports.size());

    std::for_each(data.exceptionreports.begin(), data.exceptionreports.end(), [](const ExceptionReport1_Data& excepData) {
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "%s", "\n");
        // timestamp
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "timestamp.timestamp1:%s", excepData.timestamp.timestamp1.c_str());

        // issuerid
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "excepData.issuerid:%s", excepData.issuerid.c_str());

        // exceptionmessage
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "excepData.exceptionmessage.activity:%s", excepData.exceptionmessage.activity.c_str());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "excepData.exceptionmessage.action:%s", excepData.exceptionmessage.action.c_str());
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG, "excepData.exceptionmessage.exception:%s", excepData.exceptionmessage.exception.c_str());

        // installationorderid
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.installationorderid.noid_choice:%s",
            excepData.installationorderid.noid_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.installationorderid.id_choice:%s",
            excepData.installationorderid.id_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.installationorderid.id.uuid:%s",
            excepData.installationorderid.id.uuid.c_str());

        // clientconfigurationid
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.clientconfigurationid.noversion_choice:%s",
            excepData.clientconfigurationid.noversion_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.clientconfigurationid.version_choice:%s",
            excepData.clientconfigurationid.version_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.clientconfigurationid.version.uuid:%s",
            excepData.clientconfigurationid.version.uuid.c_str());

        // datafilename
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.datafilename.file_choice:%s", excepData.datafilename.file_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.datafilename.nofile_choice:%s", excepData.datafilename.nofile_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.datafilename.file.name:%s", excepData.datafilename.file.name.c_str());

        // addr
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.addr.addr_choice:%s", excepData.addr.addr_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.addr.noaddr_choice:%s", excepData.addr.noaddr_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.addr.addr.addr:%s", excepData.addr.addr.addr.c_str());

        //key
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.key.key_choice:%s", excepData.key.key_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.key.nokey_choice:%s", excepData.key.nokey_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.key.key.key:%s", excepData.key.key.key.c_str());

        // softwarepartidentifier
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.softwarepartidentifier.id_choice:%s", excepData.softwarepartidentifier.id_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.softwarepartidentifier.noid_choice:%s", excepData.softwarepartidentifier.noid_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.softwarepartidentifier.id.partid:%s", excepData.softwarepartidentifier.id.partid.c_str());

        // datablock
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.datablock.blockNumber_choice:%s", excepData.datablock.blockNumber_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.datablock.noData_choice:%s", excepData.datablock.noData_choice ? "true" : "false");
        DLT_LOG_STRINGF(dlt_voc, DLT_LOG_DEBUG,
            "excepData.datablock.blockNumber:%s", excepData.datablock.blockNumber.c_str());
    });
}

