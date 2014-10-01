/*
 * Copyright (c) 2014, Siemens AG. All rights reserved.
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

template<typename ForwardIterator>
ChunkDescriptor<ForwardIterator>::ChunkDescriptor(ForwardIterator first,
    ForwardIterator last) :
    first(first), last(last) {
}

template<typename ForwardIterator>
ForwardIterator ChunkDescriptor<ForwardIterator>::GetFirst() const {
  return first;
}

template<typename ForwardIterator>
ForwardIterator ChunkDescriptor<ForwardIterator>::GetLast() const {
  return last;
}

template<typename ForwardIterator>
BlockSizePartitioner<ForwardIterator>::BlockSizePartitioner(
    ForwardIterator first, ForwardIterator last, size_t chunkSize) :
    first(first), last(last), chunkSize(chunkSize) {
  elements_count = static_cast<size_t>(std::distance(first, last));
  chunks = elements_count / chunkSize;
  if (elements_count % chunkSize != 0)
    chunks++;
}

template<typename ForwardIterator>
size_t BlockSizePartitioner<ForwardIterator>::Size() {
  return chunks;
}

template<typename ForwardIterator>
const ChunkDescriptor<ForwardIterator>
  BlockSizePartitioner<ForwardIterator>::operator[](
    size_t const& index) const {
  ForwardIterator first_new = first;
  std::advance(first_new, index * chunkSize);

  ForwardIterator last_new = first_new;

  if (index == elements_count / chunkSize) {
    std::advance(last_new, elements_count % chunkSize);
  } else {
    std::advance(last_new, chunkSize);
  }

  return ChunkDescriptor<ForwardIterator>(first_new, last_new);
}

template<typename ForwardIterator>
size_t ChunkPartitioner<ForwardIterator>::Size() {
  return size;
}

template<typename ForwardIterator>
ChunkPartitioner<ForwardIterator>::ChunkPartitioner(ForwardIterator first,
    ForwardIterator last, size_t amountChunks) :
    first(first), last(last) {
  if (amountChunks > 0) {
    size = amountChunks;
  } else {
    // if no concrete chunk size was given, use number of cores...
    mtapi::Node& node = mtapi::Node::GetInstance();
    size = node.GetCoreCount();
  }

  elements_count = static_cast<size_t>(std::distance(first, last));
  if (size > elements_count) {
    // if we want to make more chunks than we have elements, correct
    // the number of chunks
    size = elements_count;
  }
  standard_chunk_size = elements_count / size;
  bigger_chunk_count = elements_count % size;
}

template<typename ForwardIterator>
const ChunkDescriptor<ForwardIterator>
  ChunkPartitioner<ForwardIterator>::operator[](
    size_t const& index) const {
  typedef typename std::iterator_traits<ForwardIterator>::difference_type
      difference_type;
  size_t prec_elements_count = 0;

  if (index <= bigger_chunk_count) {
    prec_elements_count = index * (standard_chunk_size + 1);
  } else {
    prec_elements_count = (standard_chunk_size + 1) * bigger_chunk_count
        + standard_chunk_size * (index - bigger_chunk_count);
  }

  size_t cur_elements_count =
      (index < bigger_chunk_count) ?
          (standard_chunk_size + 1) : standard_chunk_size;

  ForwardIterator first_new = first;
  std::advance(first_new, prec_elements_count);

  first_new = first + static_cast<difference_type>(prec_elements_count);
  ForwardIterator last_new = first_new;
  std::advance(last_new, cur_elements_count);

  return ChunkDescriptor<ForwardIterator>(first_new, last_new);
}

}  // namespace internal
}  // namespace algorithms
}  // namespace embb

#endif  // EMBB_ALGORITHMS_INTERNAL_PARTITION_INL_H_
