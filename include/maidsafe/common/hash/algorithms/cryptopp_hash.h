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
#ifndef MAIDSAFE_COMMON_HASH_ALGORITHMS_CRYPTOPP_HASH_H_
#define MAIDSAFE_COMMON_HASH_ALGORITHMS_CRYPTOPP_HASH_H_

#include <array>
#include <cstdint>

#include "maidsafe/common/hash/algorithms/hash_algorithm_base.h"
#include "maidsafe/common/types.h"

namespace maidsafe {

template<typename HashAlgorithm>
class CryptoppHash : public detail::HashAlgorithmBase<CryptoppHash<HashAlgorithm>> {
 public:
  using Digest = std::array<byte, HashAlgorithm::DIGESTSIZE>;

  CryptoppHash()
    : hash_algorithm() {
  }

  void Update(const byte* in, std::uint64_t inlen) {
    hash_algorithm.Update(in, inlen);
  }

  Digest Finalize() {
    Digest digest{{}};
    assert(digest.size() == hash_algorithm.DigestSize());
    hash_algorithm.Final(digest.data());
    return digest;
  }

 private:
  HashAlgorithm hash_algorithm;
};

}  // namespace maidsafe

#endif  // MAIDSAFE_COMMON_HASH_ALGORITHMS_CRYPTOPP_HASH_H_
