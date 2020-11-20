///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file rvdc_mdp_queue.h
//	This file handles the RVDC statemachine for application.

// @project 	GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	28-Jan-2019
///////////////////////////////////////////////////////////////////
#ifndef RVDC_MDP_QUEUE_H__
#define RVDC_MDP_QUEUE_H__

#include "rvdc_data.h"
#include <vector>
#include <algorithm>

namespace volvo_on_call
{
struct HighPriority
{
  bool operator()(const RvdcMDP& lMDP, const RvdcMDP& rMDP)
  {
    return (lMDP.getPriority() < rMDP.getPriority());
  }
};
 
class RvdcMDPQueue
{
public:
  RvdcMDPQueue()
  {
  };

  virtual ~RvdcMDPQueue()
  {
    m_vRvdcMDPQueue.clear();
  }

  int getSize() const
  {
    return m_vRvdcMDPQueue.size();
  }

  bool isEmpty() const
  {
    return m_vRvdcMDPQueue.empty();
  }

  void addRvdcMDP(RvdcMDP& data)
  {
      m_vRvdcMDPQueue.push_back(data);
      std::sort(m_vRvdcMDPQueue.begin(), m_vRvdcMDPQueue.end(), HighPriority());
  }

  RvdcMDP getNextRvdcMDP() const
  {
    RvdcMDP ret;
    if (m_vRvdcMDPQueue.empty() == false)
    {
      ret = *(m_vRvdcMDPQueue.begin());
    }

    return ret;
  }

  bool isSendingRvdcMDP()
  {
    bool bSending = false;
    for (RvdcMDPVectorIter it = m_vRvdcMDPQueue.begin(); it != m_vRvdcMDPQueue.end(); ++it)
    {
      if ((*it).getStatus() == eMDPUpStsWaitAck)
      {
        m_vRvdcMDPQueue.erase(it);
        bSending = true;
        break;
      }
    }

    return bSending;
  }
  
  bool removeRvdcMDP(uint32_t index)
  {
    bool bRemoved = false;
    for (RvdcMDPVectorIter it = m_vRvdcMDPQueue.begin(); it != m_vRvdcMDPQueue.end(); ++it)
    {
      if ((*it).getIndex() == index)
      {
        m_vRvdcMDPQueue.erase(it);
        bRemoved = true;
        break;
      }
    }

    std::sort(m_vRvdcMDPQueue.begin(), m_vRvdcMDPQueue.end(), HighPriority());

    return bRemoved;
  }

  bool removeRvdcMDP(RvdcMDP& data)
  {
    return removeRvdcMDP(data.getIndex());
  }

protected:
  typedef std::vector<RvdcMDP> RvdcMDPVector;
  typedef RvdcMDPVector::iterator RvdcMDPVectorIter;

  RvdcMDPVector m_vRvdcMDPQueue;
};
}
#endif // RVDC_MDP_QUEUE_H__