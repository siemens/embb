/*
 * Copyright (c) 2014-2017, Siemens AG. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef EMBB_ALGORITHMS_INTERNAL_PARTITION_INL_H_
#define EMBB_ALGORITHMS_INTERNAL_PARTITION_INL_H_

namespace embb {
namespace algorithms {
namespace internal {

template<typename RAI>
ChunkDescriptor<RAI>::ChunkDescriptor(
  RAI first, RAI last) :
  first_(first), last_(last) {
}

template<typename RAI>
RAI ChunkDescriptor<RAI>::GetFirst() const {
  return first_;
}

template<typename RAI>
RAI ChunkDescriptor<RAI>::GetLast() const {
  return last_;
}

template<typename RAI>
BlockSizePartitioner<RAI>::BlockSizePartitioner(
    RAI first, RAI last, size_t chunkSize) :
    first_(first), last_(last), chunk_size_(chunkSize) {
  elements_count_ = static_cast<size_t>(std::distance(first_, last_));
  chunks_ = elements_count_ / chunk_size_;
  if (elements_count_ % chunk_size_ != 0) {
    chunks_++;
  }
}

template<typename RAI>
size_t BlockSizePartitioner<RAI>::Size() {
  return chunks_;
}

template<typename RAI>
const ChunkDescriptor<RAI>
  BlockSizePartitioner<RAI>::operator[](
    size_t const & index) const {
  typedef typename std::iterator_traits<RAI>::difference_type
    difference_type;
  RAI first_new(first_);
  first_new += static_cast<difference_type>(chunk_size_ * index);
  RAI last_new(first_new);
  if (index >= chunks_ - 1) {
    last_new = last_;
  } else {
    last_new += static_cast<difference_type>(chunk_size_);
  }
  return ChunkDescriptor<RAI>(first_new, last_new);
}

template<typename RAI>
size_t ChunkPartitioner<RAI>::Size() {
  return size_;
}

template<typename RAI>
ChunkPartitioner<RAI>::ChunkPartitioner(
  RAI first, RAI last, size_t amountChunks) :
  first_(first), last_(last) {
  if (amountChunks > 0) {
    size_ = amountChunks;
  } else {
    // if no concrete chunk size was given, use number of cores
    embb::mtapi::Node& node = embb::mtapi::Node::GetInstance();
    size_ = node.GetWorkerThreadCount();
  }
  elements_count_ = static_cast<size_t>(std::distance(first_, last_));
  if (size_ > elements_count_) {
    // if we want to make more chunks than we have elements, correct
    // the number of chunks
    size_ = elements_count_;
  }
  standard_chunk_size_ = elements_count_ / size_;
  bigger_chunk_count_  = elements_count_ % size_;
}

template<typename RAI>
const ChunkDescriptor<RAI>
  ChunkPartitioner<RAI>::operator[](
    size_t const& index) const {
  typedef typename std::iterator_traits<RAI>::difference_type
      difference_type;
  // Number of element preceding elements in the given chunk
  size_t prec_elements_count = 0;
  if (index <= bigger_chunk_count_) {
    prec_elements_count = index * (standard_chunk_size_ + 1);
  } else {
    prec_elements_count =
      (standard_chunk_size_ + 1) * bigger_chunk_count_ +
      (standard_chunk_size_ * (index - bigger_chunk_count_));
  }
  size_t cur_elements_count = (index < bigger_chunk_count_)
           ? (standard_chunk_size_ + 1)
           : standard_chunk_size_;
  RAI first_new(first_);
  first_new += static_cast<difference_type>(prec_elements_count);
  RAI last_new(first_new);
  last_new += static_cast<difference_type>(cur_elements_count);
  return ChunkDescriptor<RAI>(first_new, last_new);
}

}  // namespace internal
}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INTERNAL_PARTITION_INL_H_
