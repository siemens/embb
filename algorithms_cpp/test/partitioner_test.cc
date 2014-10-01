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

#include <iostream>

#include <partitioner_test.h>

#include <embb/algorithms/internal/partition.h>

#include <vector>
#include <list>

PartitionerTest::PartitionerTest() {
  CreateUnit("algorithms partitioner test").
  Add(&PartitionerTest::TestBasic, this);
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

