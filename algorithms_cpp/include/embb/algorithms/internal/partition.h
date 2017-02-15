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

#ifndef EMBB_ALGORITHMS_INTERNAL_PARTITION_H_
#define EMBB_ALGORITHMS_INTERNAL_PARTITION_H_

#include <embb/mtapi/mtapi.h>

namespace embb {
namespace algorithms {
namespace internal {
/**
 * A chunk descriptor.
 *        
 * Describes a single partition of a 1-dimensional
 * partitioning, using first and last iterator.
 *
 * \tparam  RAI  Type of the iterator.
 */

template<typename RAI>
class ChunkDescriptor {
 private:
  RAI first_;
  RAI last_;

 public:
  /**
   * Constructor.
   *
   * \param first The first iterator.
   * \param last  The last iterator
   */
  ChunkDescriptor(RAI first, RAI last);

  /**
   * Gets the first iterator.
   *
   * \return  The first iterator.
   * 
   * \waitfree
   */
  RAI GetFirst() const;

  /**
   * Gets the last iterator.
   *
   * \return  The last iterator.
   * 
   * \waitfree
   */
  RAI GetLast() const;
};

/**
 * Partitioner Interface.
 *        
 * Describes the interface for accessing a 1-dimensional partitioning. 
 *
 * \tparam  RAI  Type of the iterator.
 */
template<typename RAI>
class IPartitioner {
 public:
  virtual ~IPartitioner() {}

 private:
  /**
   * Gets the amount of partitions.
   *
   * \return  A size_t.
   * 
   * \waitfree
   */
  virtual size_t Size() = 0;

  /**
   * Gets a single partition.
   *
   * \param index Zero-based index of the partitioner. Range: [0;Size()-1]
   *
   * \return  The indexed value.
   * 
   * \waitfree
   */
  virtual const ChunkDescriptor<RAI> operator[](
    size_t const& index) const = 0;
};

/**
 * A block size partitioner.
 *
 * Partitions a 1-dim. collection of elements with total order that provides a
 * forward iterator into partitions of size chunkSize. If no chunkSize is given,
 * chunkSize is set to 1. 
 * 
 * Example:
 * 
 *    int A[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13 };
 *    const int N = (sizeof(A) / sizeof(int) );
 *    embb::algorithms::ChunkPartitioner< int* > partitioner(A, A + N, 5);
 * 
 *    With that, the array is partitioned into chunks of size 5. We therefore
 *    have following partitions:
 *    1: [1,2,3,4,5]  
 *    2: [6,7,8,9,10]  
 *    3: [11,12,13]
 * 
 * \tparam  RAI  Type of the iterator.
 */
template<typename RAI>
class BlockSizePartitioner : IPartitioner < RAI > {
 private:
  RAI first_;
  RAI last_;
  size_t chunk_size_;
  size_t elements_count_;
  size_t chunks_;

 public:
  /**
   * Constructor.
   *
   * 
   * \param first     The first iterator of the collection.
   * \param last      The last iterator of the collection.
   * \param chunkSize (Optional) size of the chunk.
   */
  BlockSizePartitioner(
    RAI first, RAI last, size_t chunkSize = 1);

  /**
   * See IPartitioner
   *
   * \waitfree
   */
  virtual size_t Size();

  /**
   * See IPartitioner
   *
   * \waitfree
   */
  virtual const ChunkDescriptor<RAI> operator[](
    size_t const& index) const;
};

/**
 * A chunk partitioner.
 *
 * Partitions a 1-dim. collection of elements with total order that provides a
 * forward iterator into amountChunks partitions. If no amountChunks is given,
 * the collection is split into number of cores partitions.
 * 
 * It is avoided to have unbalanced partitions, so they are equally "filled" up.
 * With that, we have at most two partition sizes: basic_size and basic_size+1.
 * The partitions with basic_size+1 are the ones at the firstning.
 * 
 * If a higher number of chunks shall be produced, is more than contained elements, 
 * the number of chunks is reduced to the number of elements.
 * 
 * Example:
 * 
 * int A[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13 };
 * const int N = (sizeof(A) / sizeof(int) );
 * embb::algorithms::ChunkPartitioner< int* > partitioner(A, A + N, 5);
 * 
 * With that, the array is partitioned into chunks of size 5. We therefore
 * have following partitions:
 * 1: [1,2,3]  
 * 2: [3,5,6]  
 * 3: [7,8,9]  
 * 4: [10,11]  
 * 5: [12,13]   
 *  
 * \tparam  RAI  Type of the iterator.
 */
template<typename RAI>
class ChunkPartitioner : IPartitioner < RAI > {
 private:
  size_t size_;
  size_t elements_count_;
  RAI first_;
  RAI last_;
  size_t standard_chunk_size_;
  size_t bigger_chunk_count_;

 public:
  /**
   * See IPartitioner
   *
   * \waitfree
   */
  virtual size_t Size();

  /**
   * Constructor.
   *
   * See class documentation.
   * 
   * \waitfree
   *
   * \param first         The first.
   * \param last           The last.
   * \param amountChunks  (Optional) the amount chunks.
   */
  ChunkPartitioner(RAI first, RAI last,
    size_t amountChunks = 0);

  /**
   * See IPartitioner
   *
   * \waitfree
   */
  virtual const ChunkDescriptor<RAI> operator[](
    size_t const& index) const;
};

}  // namespace internal
}  // namespace algorithms
}  // namespace embb

#include <embb/algorithms/internal/partition-inl.h>


#endif  // EMBB_ALGORITHMS_INTERNAL_PARTITION_H_
