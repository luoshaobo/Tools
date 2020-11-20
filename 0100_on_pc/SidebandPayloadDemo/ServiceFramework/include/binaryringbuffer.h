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
 */

/**
 * @file       binaryringbuffer.h
 * @project    VCC TCAM
 * @subsystem  CommunicationManager
 * @version    0.1
 * @date       20170228
 * @brief      Ring buffer for dealing with byte streams, i.e. socket communication
 * @author     Bernd Winter
 *
 * @par Module-History:
 * @verbatim
 *  Date       Author                  Reason
 *  20170228   Bernd Winter            init
 *
 * @endverbatim
 *
 */

#ifndef BINARYRINGBUFFER_H__
#define BINARYRINGBUFFER_H__

#include <cstddef>

class BinaryRingBuffer {
public:
  BinaryRingBuffer(std::size_t capacity);
  ~BinaryRingBuffer();

  BinaryRingBuffer & operator=(const BinaryRingBuffer&) = delete;
  BinaryRingBuffer(const BinaryRingBuffer&) = delete;
  BinaryRingBuffer() = delete;

  std::size_t get_size() const;
  std::size_t get_capacity() const;
  std::size_t get_capacity_left() const;

  /**
   * Write data to buffer and return number of bytes written. Written bytes can be less than the
   * requested amount if the maximum capacity has been reached.
   */
  std::size_t write(const char *data, std::size_t bytes);

  /**
   * Read Return number of bytes read.
   */
  std::size_t read(char *data, std::size_t bytes);

  //TODO: maybe add std::size_t BinaryRingBuffer::read(std::string& data, std::size_t bytes) or std::size_t BinaryRingBuffer::readAppend(std::string& data, std::size_t bytes)

  /**
   * Reset buffer.
   */
  void reset();

private:
  std::size_t m_start_idx;
  std::size_t m_end_idx;
  std::size_t m_size;
  std::size_t m_capacity;
  char *m_buffer;
};



#endif //BINARYRINGBUFFER_H__
