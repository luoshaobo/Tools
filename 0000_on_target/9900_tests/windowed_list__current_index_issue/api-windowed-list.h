#ifndef API_WINDOWED_LIST_H_INCLUDED
#define API_WINDOWED_LIST_H_INCLUDED

#include <cstdint>
#include <array>
#include <algorithm>
#include <limits>

#include <iostream>

/* ConOp:
 * The windowed lists (WL) purpose is to provide a performance optimized way to access lists, which will
 * not fetch the whole list at once, but at least the items that are required immediately.
 * The WL has a Capacity (the number of items that are actually stored in the cache) and a
 * fullListSize_ (which represents the real size of the underlying list). In most usecasew fullListSize_ is much greater than Capacity.
 * Whenever the upper layer (GUI) requests an item, which is not in the cache, a new window is requested.
 * Besides this "cache miss" situations, the WL tries to predict,
 * which items will be requried next and already requests them in advance.
 * */

/* Assumptions:
 * - The BL will always provide as much items as possible - which means, if possible, BL will try to fill the complete capacity
 * - The BL will call setFullListSize before calling update
 * - The BL will always respond to onCacheMiss calls
 * */

namespace api
{
  template<typename Item, std::size_t Capacity, std::size_t PrefetchZoneSize = 0>
  class WindowedList
  {
    static_assert(Capacity > (2*PrefetchZoneSize), "Buffer capacity should be bigger then (2 x prefetch zone size)");

  public:
    explicit WindowedList(const Item & dummy) :
      start_{},
      count_{},
      lowMark_{PrefetchZoneSize},
      highMark_{Capacity - PrefetchZoneSize},
      items_(),
      dummy_(dummy),
      requestReason {RequestReason::UNKNOWN},
      lastRequestedIndex_ {std::numeric_limits<std::size_t>::max()},
      fullListSize_ {0}
    {}

    template<typename InputIt>
    void update(std::size_t start, InputIt first, InputIt last)
    {
      update(start, first, std::distance(first, last));
    }

    template<typename InputIt>
    void update(std::size_t start, InputIt first, std::size_t count)
    {
      if (fullListSize_ < (start + count))
      {
        // fullListSize_ has not been set correctly by the BL
        fullListSize_ = (start + count);
      }

      requestReason = RequestReason::UNKNOWN;
      lastRequestedIndex_ = std::numeric_limits<std::size_t>::max();
      start_ = start;
      count_ = std::min(count, items_.size());
      std::copy_n(first, count_, items_.begin());
    }

    std::size_t getStartingIndex() const
    {
      return start_;
    }

    std::size_t getSize() const
    {
      return count_;
    }

    std::size_t getFullListSize() const
    {
      return fullListSize_;
    }

    void setFullListSize(std::size_t fullListSize)
    {
      fullListSize_ = fullListSize;
    }

    // *** added newly ***
    template<typename OnCacheMiss>
    Item getItem2(std::size_t index, OnCacheMiss onCacheMiss, bool isCurrentIndex)
    {
        Item returnItem(dummy_);

        if (isCurrentIndex) {
            if ((start_ <= index) && (index < (start_ + count_))) {
                const std::size_t localIndex{index - start_};
                returnItem = items_[localIndex];
                return returnItem;
            }
        }

        return getItem(index, onCacheMiss);
    }

    template<typename OnCacheMiss>
    Item getItem(std::size_t index, OnCacheMiss onCacheMiss)
    {
      Item returnItem(dummy_);


      if ((start_ <= index) && (index < (start_ + count_)))
      {
        // Requested item is in the cache

        const std::size_t localIndex{index - start_};
        returnItem = items_[localIndex];

        //PrefetchZone handling
        if (index < (start_ + lowMark_) && (start_ != 0))
        {
          //List is scrolling backward
          index = (index > (Capacity/2)) ? (index - (Capacity/2)) : 0; //prefetch 1/2 of the capacity in scrolling direction
          if(requestReason != RequestReason::PREFETCH_BACKWARD) //Don't send another prefetch request before BL has responded
          {
            onCacheMiss(index);
            requestReason = RequestReason::PREFETCH_BACKWARD;
            lastRequestedIndex_ = index;
          }
        }
        else if ((index > (start_ + highMark_)) && ((start_ + count_) != fullListSize_))
        {
          // List is scrolling forward
          index = ((index + (Capacity/2)) < fullListSize_) ? (index - (Capacity/2)) : (fullListSize_- Capacity); //prefetch 1/2 of the capacity in scrolling direction
          if(requestReason != RequestReason::PREFETCH_FORWARD) //Don't send another prefetch request before BL has responded
          {
            onCacheMiss(index);
            requestReason = RequestReason::PREFETCH_FORWARD;
            lastRequestedIndex_ = index;
          }
        }
      }
      else
      {
        if (fullListSize_ < Capacity)
        {
          // Complete list fits inside the WindowedList
          index = 0;
        }
        else
        {
          if(index < (Capacity/2))
          {
            // Index is at the beginning of the list --> start with 0
            index = 0;
          }
          else
          {
            if ((index + (Capacity/2)) > fullListSize_)
            {
              // index is at the end of the list
              index = fullListSize_ - Capacity;
            }
            else
            {
              // index is in the middle of the list
              index = (index - (Capacity/2));
            }
          }
        }

        // request only on first cache miss or if the next request is outside the previously requested window
        if((requestReason != RequestReason::CACHE_MISS) ||
          ((index < lastRequestedIndex_) || (index >= lastRequestedIndex_ + Capacity)))
        {
          onCacheMiss(index);
          requestReason = RequestReason::CACHE_MISS;
          lastRequestedIndex_ = index;
        }
      }

      return returnItem;
    }

  private:
    enum class RequestReason
    {
        UNKNOWN,
        CACHE_MISS,
        PREFETCH_BACKWARD,
        PREFETCH_FORWARD
    };

    std::size_t start_;
    std::size_t count_;
    std::size_t lowMark_;
    std::size_t highMark_;
    std::array<Item, Capacity> items_;
    Item dummy_;

    //Used for request debouncing
    RequestReason requestReason;
    std::size_t lastRequestedIndex_;

    std::size_t fullListSize_ = std::numeric_limits<std::size_t>::max();

    friend std::ostream& operator <<(std::ostream& stream, const WindowedList& wl)
    {
      stream << "WindowedList{start:" << wl.start_ << ", count:" << wl.count_ << ", dummy:" << wl.dummy_ << "}";
      return stream;
    }
  };
}

#endif
