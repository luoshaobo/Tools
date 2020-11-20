#include "dlt/dlt.h"
#include <string>
#include <vector>
#include <memory>
#include "vds_data_gen.h"
#include "vds_marben_adapt_gen.h"
#include "vds_marben_adapt_base.h"

std::shared_ptr<VDServiceRequest_Model> VdsMarbenAdaptGen::VDServiceRequest_Decode(const std::string &path,asn_wrapper::VDServiceRequest* inputObj)
{
 return std::make_shared<VDServiceRequest_Model>();
}

bool VdsMarbenAdaptGen::VDServiceRequest_Encode(const VDServiceRequest_Model &pData, asn_wrapper::VDServiceRequest* inputObj)
{
 return true;
}


VdsMarbenAdaptBase::~VdsMarbenAdaptBase()
{
}


VdsMarbenAdaptBase::VdsMarbenAdaptBase(asn_wrapper::VDServiceRequest*)
{
}
