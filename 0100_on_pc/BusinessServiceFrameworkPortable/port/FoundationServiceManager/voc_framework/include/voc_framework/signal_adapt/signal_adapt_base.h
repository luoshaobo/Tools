///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file signal_adapt_base.h

// @project		GLY_TCAM
// @subsystem	FSM
// @author		Lin Haixia
// @Init date	22-Feb-2019
///////////////////////////////////////////////////////////////////

#ifndef SIGNAL_ADAPT_BASE_H
#define SIGNAL_ADAPT_BASE_H

#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
//#include "VDSasn.h"
#include "app_data.h"
//#include "vds_data_cus.h"
//#include "vds_data_gen.h"

#define random(x) (rand()%x)

#define FSM_REQUEST_ID_MIN_VALUE 0
#define FSM_REQUEST_ID_MAX_VALUE 255

class SignalAdaptBase
{
public:
    ////////////////////////////////////////////////////////////
    // @brief :Constructs.
    // @param[in]  VDServiceRequest vdsObj
    // @return
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    SignalAdaptBase(void * vdsObj);
    SignalAdaptBase(asn_wrapper::VDServiceRequest* vdsObj);

    ~SignalAdaptBase();

    ////////////////////////////////////////////////////////////
    // @brief :unpack request ASN1C structure.(all app can call this function to unpack)
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    std::shared_ptr<AppDataBase> UnpackService();

    virtual std::shared_ptr<AppDataBase> UnpackServiceItf()
    {
        return UnpackService();
    }

protected:
    ////////////////////////////////////////////////////////////
    // @brief :pack response to ASN1C structure.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    bool PackService(VDServiceRequest_Model &vdsData, const AppDataBase &appData);

    ////////////////////////////////////////////////////////////
    // @brief :unpack RequestHeader.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    bool GetHeadPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, RequestHeader_Data& appData);

    ////////////////////////////////////////////////////////////
    // @brief :pack RequestHeader.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    virtual bool SetHeadPayload(VDServiceRequest_Model &vdsData, const RequestHeader_Data &appData);

    ////////////////////////////////////////////////////////////
    // @brief :unpack RequestBody. override by each app.
    // @return     True if successfully set, false otherwise
    // @author     linhaixia, 22-Feb-2019
    ////////////////////////////////////////////////////////////
    virtual bool GetBodyPayload(std::shared_ptr<VDServiceRequest_Model> vdsData, AppDataBase *& appData){  return true; };

public:
    asn_wrapper::VDServiceRequest* m_vdsObj;


private:
    int m_requestId = FSM_REQUEST_ID_MIN_VALUE;
    int getRequestId()
    {
        srand((int)time(0));
        m_requestId = random(FSM_REQUEST_ID_MAX_VALUE);

        return m_requestId;
    }
};

#endif

