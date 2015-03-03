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
#ifndef MAIDSAFE_COMMON_HASH_HASH_CHRONO_H_
#define MAIDSAFE_COMMON_HASH_HASH_CHRONO_H_

#include <chrono>

#include "maidsafe/common/hash/hash_numeric.h"

namespace maidsafe {

template<typename HashAlgorithm, typename Rep, typename Period>
void HashAppend(HashAlgorithm& hash, const std::chrono::duration<Rep, Period>& duration) {
  hash(duration.count());
}

template<typename HashAlgorithm, typename Clock, typename Duration>
void HashAppend(HashAlgorithm& hash, const std::chrono::time_point<Clock, Duration>& time_point) {
  hash(time_point.time_since_epoch());
}

}  // namespace maidsafe

#endif  // MAIDSAFE_COMMON_HASH_HASH_CHRONO_H_
