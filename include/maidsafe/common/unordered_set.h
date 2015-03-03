/*  Copyright 2014 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */
#ifndef MAIDSAFE_COMMON_UNORDERED_SET_H_
#define MAIDSAFE_COMMON_UNORDERED_SET_H_

#include <memory>
#include <unordered_set>

#include "maidsafe/common/hash/algorithms/siphash.h"
#include "maidsafe/common/hash/wrappers/seeded_hash.h"

namespace maidsafe {

template<
  typename Key,
  typename Equal = std::equal_to<Key>,
  typename Allocator = std::allocator<Key>>
using unordered_set = std::unordered_set<Key, SeededHash<SipHash>, Equal, Allocator>;

}  // namespace maidsafe

#endif  // MAIDSAFE_COMMON_UNORDERED_SET_H_
