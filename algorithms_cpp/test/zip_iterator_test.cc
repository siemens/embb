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

#include <iterator>
#include <vector>
#include <deque>

#include <embb/algorithms/zip_iterator.h>
#include <embb/algorithms/for_each.h>
#include <embb/algorithms/reduce.h>
#include <embb/algorithms/scan.h>

#include <zip_iterator_test.h>

typedef std::vector<int>::iterator VectorIterator;
typedef std::vector<int>::const_iterator constVectorIterator;

struct DotProductFunctor {
  template<typename TypeA, typename TypeB>
  int operator()(embb::algorithms::ZipPair<TypeA, TypeB> pair) const {
    return pair.First() * pair.Second();
  }

  template<typename TypeA, typename TypeB>
  int operator()(int lhs, embb::algorithms::ZipPair<TypeA, TypeB> rhs) const {
    return lhs + rhs.First() * rhs.Second();
  }

  int operator()(int lhs, int rhs) const {
    return lhs + rhs;
  }
};

/**
 * Functor to compute the square of a number.
 * The result overwrites the original number.
 */
struct Square {
  void operator()(embb::algorithms::ZipPair<int &, int &> pair) const {
    pair.First() = pair.First() * pair.First();
    pair.Second() = pair.Second() * pair.Second();
  }
};

ZipIteratorTest::ZipIteratorTest() {
  CreateUnit("Zip foreach")
        .Add(&ZipIteratorTest::TestZipForEach, this);
  CreateUnit("Zip Reduce")
       .Add(&ZipIteratorTest::TestZipReduce, this);
  CreateUnit("Zip Scan")
         .Add(&ZipIteratorTest::TestZipScan, this);
  CreateUnit("Iterator Types")
        .Add(&ZipIteratorTest::TestIteratorTypes, this);
  CreateUnit("Double Zip")
          .Add(&ZipIteratorTest::TestDoubleZip, this);
}

void ZipIteratorTest::TestZipForEach() {
  using embb::algorithms::ForEach;
  std::vector<int> vectorA(kCountSize);
  std::vector<int> vectorB(kCountSize);
  for (size_t i = 0; i < kCountSize; i++) {
    vectorA[i] = static_cast<int>((i + 1) % 1000);
    vectorB[i] = static_cast<int>((i + 2) % 1000);
  }
  ForEach(
      embb::algorithms::Zip(vectorA.begin(), vectorB.begin()),
      embb::algorithms::Zip(vectorA.end(), vectorB.end()),
      Square());
  for (size_t i = 0; i < kCountSize; i++) {
    int expected = static_cast<int>((i + 1) % 1000);
    expected = expected * expected;
    PT_EXPECT_EQ(vectorA[i], expected);
    expected = static_cast<int>((i + 2) % 1000);
    expected = expected * expected;
    PT_EXPECT_EQ(vectorB[i], expected);
  }
}

void ZipIteratorTest::TestZipReduce() {
  long sum = 0;
  std::vector<int> vectorA(kCountSize);
  std::vector<int> vectorB(kCountSize);
  for (size_t i = 0; i < kCountSize; i++) {
    vectorA[i] = static_cast<int>(i+2);
    vectorB[i] = static_cast<int>(i+2);
    sum += static_cast<long>((i + 2) * (i + 2));
  }

  embb::algorithms::ZipIterator<constVectorIterator, constVectorIterator>
  start_iterator = embb::algorithms::Zip(
    std::vector<int>::const_iterator(vectorA.begin()),
    std::vector<int>::const_iterator(vectorB.begin()));
  embb::algorithms::ZipIterator<constVectorIterator, constVectorIterator>
  end_iterator = embb::algorithms::Zip(
    std::vector<int>::const_iterator(vectorA.end()),
    std::vector<int>::const_iterator(vectorB.end()));

  PT_EXPECT_EQ(embb::algorithms::Reduce(start_iterator, end_iterator, 0,
      std::plus<int>(), DotProductFunctor()), sum);
}

void ZipIteratorTest::TestZipScan() {
  std::vector<int> vectorA(kCountSize);
  std::vector<int> vectorB(kCountSize);
  std::vector<int> vectorOut(kCountSize);

  for (size_t i = 0; i < kCountSize; i++) {
    vectorA[i] = static_cast<int>(i+1);
    vectorB[i] = static_cast<int>(i+2);
  }

  Scan(embb::algorithms::Zip(vectorA.begin(), vectorB.begin()),
       embb::algorithms::Zip(vectorA.end(), vectorB.end()),
       vectorOut.begin(), 0, std::plus<int>(), DotProductFunctor(),
       embb::mtapi::ExecutionPolicy(), 0);

  long sum = 0;
  for (size_t i = 0; i < kCountSize; i++) {
    sum += vectorA[i] * vectorB[i];
    PT_EXPECT_EQ(sum, vectorOut[i]);
  }
}

void ZipIteratorTest::TestIteratorTypes() {
  long sum = 0;

  std::vector<int> vectorA(kCountSize);
  std::vector<int> vectorB(kCountSize);

  int arrayA[kCountSize];
  int arrayB[kCountSize];

  std::deque<int> dequeA(kCountSize);
  std::deque<int> dequeB(kCountSize);

  const int constArrayA[] = {2, 3, 4, 5, 6, 7};
  const int constArrayB[] = {2, 3, 4, 5, 6, 7};

  for (size_t i = 0; i < kCountSize; i++) {
    vectorA[i] = static_cast<int>(i + 2);
    vectorB[i] = static_cast<int>(i + 2);
    arrayA[i] = static_cast<int>(i + 2);
    arrayB[i] = static_cast<int>(i + 2);
    dequeA[i] = static_cast<int>(i + 2);
    dequeB[i] = static_cast<int>(i + 2);

    sum += static_cast<long>((i + 2) * (i + 2));
  }

  using embb::algorithms::Zip;
  PT_EXPECT_EQ(Reduce(Zip(vectorA.begin(), vectorB.begin()),
                      Zip(vectorA.end(), vectorB.end()), 0, std::plus<int>(),
                      DotProductFunctor()), sum);
  PT_EXPECT_EQ(Reduce(Zip(dequeA.begin(), dequeB.begin()),
                      Zip(dequeA.end(), dequeB.end()), 0, std::plus<int>(),
                      DotProductFunctor()), sum);
  PT_EXPECT_EQ(Reduce(Zip(arrayA, arrayB),
                      Zip(arrayA + kCountSize, arrayB + kCountSize), 0,
                      std::plus<int>(), DotProductFunctor()), sum);
  PT_EXPECT_EQ(Reduce(Zip(constArrayA, constArrayB),
                      Zip(constArrayA + kCountSize, constArrayB + kCountSize),
                      0, std::plus<int>(), DotProductFunctor()), sum);
}

struct MultiDotProductFunctor{
  mtapi_int64_t operator()(
      embb::algorithms::ZipPair<embb::algorithms::ZipPair<int&, int&>,
      embb::algorithms::ZipPair<int&, int&> > rhs) const {
    return mtapi_int64_t(rhs.First().First()) * rhs.First().Second() *
           rhs.Second().First() * rhs.Second().Second();
  }
};

void ZipIteratorTest::TestDoubleZip() {
  mtapi_int64_t sum = 0;
  std::vector<int> vectorA(kCountSize);
  std::vector<int> vectorB(kCountSize);
  std::vector<int> vectorC(kCountSize);
  std::vector<int> vectorD(kCountSize);
  for (size_t i = 0; i < kCountSize; i++) {
    vectorA[i] = static_cast<int>(i + 1);
    vectorB[i] = static_cast<int>(i + 2);
    vectorC[i] = static_cast<int>(i + 3);
    vectorD[i] = static_cast<int>(i + 4);
    sum += vectorA[i] * vectorB[i] * vectorC[i] * vectorD[i];
  }

  using embb::algorithms::Zip;
  PT_EXPECT_EQ(Reduce(
               Zip(Zip(vectorA.begin(), vectorB.begin()),
                   Zip(vectorC.begin(), vectorD.begin())),
               Zip(Zip(vectorA.end(), vectorB.end()),
                   Zip(vectorC.end(), vectorD.end())),
               mtapi_int64_t(0), std::plus<mtapi_int64_t>(),
               MultiDotProductFunctor()), sum);
}
