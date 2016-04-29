/*
 * Copyright (c) 2014-2016, Siemens AG. All rights reserved.
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

#ifndef EMBB_CONTAINERS_BLOCKING_MAP_H_
#define EMBB_CONTAINERS_BLOCKING_MAP_H_

#include <embb/base/base.h>
#include <map>

namespace embb {
namespace containers {

/**
 * Blocking map.
 *
 * \tparam Key Key type of the elements in the map.
 * \tparam Value Value type of the elements in the map.
 */
template< typename Key, typename Value>
class BlockingMap {
  typedef embb::base::Mutex Mutex;
  typedef embb::base::LockGuard<> LockGuard;

 private:
   /**
    * Internal map from the standard library.
    */
   std::map<Key, Value> internalMap;

   /**
    * Mutex for synchronizing concurrent accesses to the structure.
    */
   Mutex mutex;

 public:
   /**
    * Creates an empty map.
    */
   BlockingMap();

   /**
    * Inserts a new element (key,value) in the map, if no elements
    * with the same key already exists.
    * 
    * \return \c true if the inserting succeeded,
    * \c false otherwise.
    */
   bool Insert(
     const Key& key,
     /**< [IN] Constant reference to key of the element to insert*/
     const Value& value
     /**< [IN] Constant reference to value of the element to insert*/
     );

   /**
    * Erases the element with the specified key, if such an element exists.
    *
    * \return \c true if erasing was successfull, \c false otherwise.
    */
   bool Erase(
     const Key& key
     /**< [IN] Constant reference to the key of the element to erase*/);

   /*
    * Checks if the map contains an element with the specified key.
    *
    * \return \c true if the the map contains the element, \c false
    * otherwise
    */
   bool Contains(
     const Key& key
     /**< [IN] Constant reference to key of the element
     to search for*/);

   /**
   * Accesses the element with the specified key, if such an element exists.
   * If it does not exists, creates an element with the specified key.
   *
   * \return Reference to the value with the specified key.
   */
   Value& operator[](
     const Key& key
     /**< [IN] Constant reference to key of the element to access*/);

};

}
}

#include <embb/containers/internal/blocking_map-inl.h>

#endif // EMBB_CONTAINERS_BLOCKING_MAP_H_

