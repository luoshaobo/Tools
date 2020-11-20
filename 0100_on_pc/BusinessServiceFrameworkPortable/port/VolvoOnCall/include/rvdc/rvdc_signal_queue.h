///////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Continental Automotive GmbH
// Unless required by applicable law or agreed to in writing, // software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	@file rvdc_signal_queue.h
//	This file handles the RVDC statemachine for application.

// @project 	GLY_TCAM
// @subsystem	Application
// @author		Nie Yujin
// @Init date	28-Jan-2019
///////////////////////////////////////////////////////////////////
#ifndef RVDC_SIGNAL_QUEUE_H__
#define RVDC_SIGNAL_QUEUE_H__

#include "rvdc_data.h"
#include <vector>
#include <algorithm>
namespace volvo_on_call
{
struct HigherPrioritySignal
{
  bool operator()(const RvdcSignal& lSignal, const RvdcSignal& rSignal)
  {
    return (lSignal.getIndex() < rSignal.getIndex());
  }
};
 
class RvdcSignalQueue
{
public:
  RvdcSignalQueue()
  {
  };

  virtual ~RvdcSignalQueue()
  {
    m_vRvdcSignalQueue.clear();
  }

  int getSize() const
  {
    return m_vRvdcSignalQueue.size();
  }

  bool isEmpty() const
  {
    return m_vRvdcSignalQueue.empty();
  }

  void addRvdcSignal(RvdcSignal& data)
  {
      m_vRvdcSignalQueue.push_back(data);
      std::sort(m_vRvdcSignalQueue.begin(), m_vRvdcSignalQueue.end(), HigherPrioritySignal());
  }

  RvdcSignal getNextRvdcSignal() const
  {
    RvdcSignal ret;
    if (m_vRvdcSignalQueue.empty() == false)
    {
      ret = *(m_vRvdcSignalQueue.begin());
    }

    return ret;
  }

  bool removeRvdcSignal(uint32_t index)
  {
    bool bRemoved = false;
    for (RvdcSignalVectorIter it = m_vRvdcSignalQueue.begin(); it != m_vRvdcSignalQueue.end(); ++it)
    {
      if ((*it).getIndex() == index)
      {
        m_vRvdcSignalQueue.erase(it);
        bRemoved = true;
        break;
      }
    }

    std::sort(m_vRvdcSignalQueue.begin(), m_vRvdcSignalQueue.end(), HigherPrioritySignal());

    return bRemoved;
  }

  bool removeRvdcSignal(RvdcSignal& data)
  {
    return removeRvdcSignal(data.getIndex());
  }

  bool updateRvdcSignal(uint32_t index, SignalStatus state)
  {
    bool ret = false;
    for (RvdcSignalVectorIter it = m_vRvdcSignalQueue.begin(); it != m_vRvdcSignalQueue.end(); ++it)
    {
      if ((*it).getIndex() == index)
      {
        (*it).setStatus(state);
        ret = true;
        break;
      }
    }

    return ret;
  }

protected:
  typedef std::vector<RvdcSignal> RvdcSignalVector;
  typedef RvdcSignalVector::iterator RvdcSignalVectorIter;

  RvdcSignalVector m_vRvdcSignalQueue;
};
}
#endif // RVDC_SIGNAL_QUEUE_H__