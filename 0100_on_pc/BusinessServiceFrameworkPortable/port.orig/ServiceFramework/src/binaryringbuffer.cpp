#include "binaryringbuffer.h"
#include <cstring>
#include <algorithm>
#include "comalog.h"


BinaryRingBuffer::BinaryRingBuffer(std::size_t capacity) :
    m_start_idx(0), m_end_idx(0), m_size(0), m_capacity(capacity) {
  m_buffer = new char[capacity];
}

BinaryRingBuffer::~BinaryRingBuffer() {
  delete[] m_buffer;
}

std::size_t BinaryRingBuffer::write(const char* data, std::size_t bytes) {
  if (bytes == 0)
    return 0;

  std::size_t bytes_to_write = std::min(bytes, m_capacity - m_size);

  // Write in a single step
  if (bytes_to_write <= m_capacity - m_end_idx) {
    std::memcpy(m_buffer + m_end_idx, data, bytes_to_write);
    m_end_idx += bytes_to_write;
    if (m_end_idx == m_capacity)
      m_end_idx = 0;
  }
  // Write in two steps -> note ringbuffer-end
  else {
    std::size_t size_1 = m_capacity - m_end_idx;

    //check boundaries
    if (size_1 <= bytes)
    {
      std::memcpy(m_buffer + m_end_idx, data, size_1);
    }
    else
    {
      COMALOG(COMALOG_ERROR, "%s datasize:%ld but write attempt size1:%d\n", __FUNCTION__, bytes, size_1);
      return 0;
    }

    std::size_t size_2 = bytes_to_write - size_1;

    if (size_2 <= bytes - size_1)
    {
      memcpy(m_buffer, data + size_1, size_2);
      m_end_idx = size_2;
    }
    else
    {
      COMALOG(COMALOG_ERROR, "%s datasize:%ld-size1:%d but write attempt size2:%d\n", __FUNCTION__, bytes, size_1, size_2);
      return 0;
    }
  }

  m_size += bytes_to_write;
  return bytes_to_write;
}

std::size_t BinaryRingBuffer::get_size() const {
  return m_size;
}
std::size_t BinaryRingBuffer::get_capacity() const {
  return m_capacity;
}
std::size_t BinaryRingBuffer::get_capacity_left() const {
  return m_capacity - m_size;
}

void BinaryRingBuffer::reset() {
  m_start_idx = 0;
  m_end_idx = 0;
  m_size = 0;
}

std::size_t BinaryRingBuffer::read(char *data, std::size_t bytes) {
  if (bytes == 0)
    return 0;

  std::size_t capacity = m_capacity;
  std::size_t bytes_to_read = std::min(bytes, m_size);

  // Read in a single step
  if (bytes_to_read <= capacity - m_start_idx) {
    std::memcpy(data, m_buffer + m_start_idx, bytes_to_read);
    m_start_idx += bytes_to_read;
    if (m_start_idx == capacity)
      m_start_idx = 0;
  }
  // Read in two steps
  else {

    std::size_t size_1 = capacity - m_start_idx;

    //check boundaries
    if (size_1 <= bytes)
    {
      std::memcpy(data, m_buffer + m_start_idx, size_1);
    }
    else
    {
      COMALOG(COMALOG_ERROR, "%s datasize:%ld but write attempt size1:%d\n", __FUNCTION__, bytes, size_1);
      return 0;
    }

    std::size_t size_2 = bytes_to_read - size_1;
    if (size_2 <= bytes - size_1)
    {
      memcpy(data + size_1, m_buffer, size_2);
      m_start_idx = size_2;
    }
    else
    {
      COMALOG(COMALOG_ERROR, "%s datasize:%ld-size1:%d but write attempt size2:%d\n", __FUNCTION__, bytes, size_1, size_2);
      return 0;
    }
  }

  m_size -= bytes_to_read;
  return bytes_to_read;
}
