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

#include <iostream>

#include <partitioner_test.h>

#include <embb/algorithms/internal/partition.h>

#include <vector>
#include <list>

PartitionerTest::PartitionerTest()
  : partitioned_array_(NULL)
  , partitioned_array_size_(16384) {
  // Size of array to be partitioned should be power of 2
  CreateUnit("TestBasic")
    .Add(&PartitionerTest::TestBasic, this);
  CreateUnit("TestLargeRange")
    .Pre(&PartitionerTest::TestLargeRangePre, this)
    .Add(&PartitionerTest::TestLargeRange, this)
    .Post(&PartitionerTest::TestLargeRangePost, this);
}

void PartitionerTest::TestBasic() {
  int A[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
  const int N = (sizeof(A) / sizeof(int) );

  embb::algorithms::internal::ChunkPartitioner< int* >
    partitioner(A, A + N, 5);
  PT_EXPECT_EQ_MSG(*(partitioner[0].GetFirst()), 1, "Get start iterator");
  PT_EXPECT_EQ_MSG(*(partitioner[0].GetLast()-1), 3, "Get end iterator");
  PT_EXPECT_EQ_MSG(*(partitioner[1].GetFirst()), 4, "Get start iterator");
  PT_EXPECT_EQ_MSG(*(partitioner[1].GetLast()-1), 6, "Get end iterator");
  PT_EXPECT_EQ_MSG(*(partitioner[2].GetFirst()), 7, "Get start iterator");
  PT_EXPECT_EQ_MSG(*(partitioner[2].GetLast()-1), 9, "Get end iterator");
  PT_EXPECT_EQ_MSG(*(partitioner[3].GetFirst()), 10, "Get start iterator");
  PT_EXPECT_EQ_MSG(*(partitioner[3].GetLast()-1), 11, "Get end iterator");
  PT_EXPECT_EQ_MSG(*(partitioner[4].GetFirst()), 12, "Get start iterator");
  PT_EXPECT_EQ_MSG(*(partitioner[4].GetLast()-1), 13, "Get end iterator");

  PT_EXPECT_EQ_MSG(partitioner.Size(), size_t(5), "Check count of partitions");

  embb::algorithms::internal::BlockSizePartitioner< int* >
    partitioner2(A, A + N, 5);

  PT_EXPECT_EQ_MSG(*(partitioner2[0].GetFirst()), 1, "Get start iterator");
  PT_EXPECT_EQ_MSG(*(partitioner2[0].GetLast() - 1), 5, "Get end iterator");
  PT_EXPECT_EQ_MSG(*(partitioner2[1].GetFirst()), 6, "Get start iterator");
  PT_EXPECT_EQ_MSG(*(partitioner2[1].GetLast() - 1), 10, "Get end iterator");
  PT_EXPECT_EQ_MSG(*(partitioner2[2].GetFirst()), 11, "Get start iterator");
  PT_EXPECT_EQ_MSG(*(partitioner2[2].GetLast() - 1), 13, "Get end iterator");

  PT_EXPECT_EQ_MSG(partitioner2.Size(), size_t(3), "Check count of partitions");
}

void PartitionerTest::TestLargeRangePre() {
  partitioned_array_ = new int[partitioned_array_size_];
  for (size_t i = 0; i < partitioned_array_size_; ++i) {
    partitioned_array_[i] = static_cast<int>(i);
  }
}

void PartitionerTest::TestLargeRangePost() {
  delete[] partitioned_array_;
}

void PartitionerTest::TestLargeRange() {
  // Test chunk partitioner with increasing number of chunks:
  for (size_t num_chunks = 2;
       num_chunks < partitioned_array_size_;
       num_chunks *= 2) {
    embb::algorithms::internal::ChunkPartitioner<int *>
      chunk_partitioner(
      partitioned_array_,
      partitioned_array_ + partitioned_array_size_,
      num_chunks);
    int last_value_prev = -1;
    PT_EXPECT_EQ(num_chunks, chunk_partitioner.Size());
    // Iterate over chunks in partition:
    for (size_t chunk = 0; chunk < chunk_partitioner.Size(); ++chunk) {
      int first_value = *(chunk_partitioner[chunk].GetFirst());
      int last_value  = *(chunk_partitioner[chunk].GetLast() - 1);
      PT_EXPECT_LT(first_value, last_value);
      // Test seams between chunks: chunk[i].last + 1 == chunk[i+1].first
      PT_EXPECT_EQ((last_value_prev + 1), first_value);
      last_value_prev = last_value;
    }
  }
  // Test block size partitioner with increasing chunk size:
  for (size_t block_size = 1;
       block_size < partitioned_array_size_;
       block_size *= 2) {
    embb::algorithms::internal::BlockSizePartitioner<int *>
      chunk_partitioner(
      partitioned_array_,
      partitioned_array_ + partitioned_array_size_,
      block_size);
    int last_value_prev = -1;
    // Iterate over chunks in partition:
    for (size_t chunk = 0; chunk < chunk_partitioner.Size(); ++chunk) {
      int first_value = *(chunk_partitioner[chunk].GetFirst());
      int last_value  = *(chunk_partitioner[chunk].GetLast() - 1);
      if (block_size == 1) {
        PT_EXPECT_EQ(first_value, last_value);
      } else {
        PT_EXPECT_LT(first_value, last_value);
      }
      // Test seams between chunks: chunk[i].last + 1 == chunk[i+1].first
      PT_EXPECT_EQ((last_value_prev + 1), first_value);
      last_value_prev = last_value;
    }
  }
}

