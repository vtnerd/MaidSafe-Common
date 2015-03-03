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
#ifndef MAIDSAFE_COMMON_HASH_WRAPPERS_UNSEEDED_HASH_H_
#define MAIDSAFE_COMMON_HASH_WRAPPERS_UNSEEDED_HASH_H_

#include <type_traits>

namespace maidsafe {

template<typename HashAlgorithm, typename HashedType = void>
class UnseededHash {
 public:
  template<typename Type>
  decltype(std::declval<HashAlgorithm>().Finalize()) operator()(Type&& value) const {
    HashAlgorithm hash{};
    StartHash<HashedType>(hash, std::forward<Type>(value));
    return hash.Finalize();
  }

 private:
  template<typename Type>
  using IsGeneric = std::is_same<void, Type>;

  template<typename Test, typename Type>
  static typename std::enable_if<IsGeneric<Test>::value>::type StartHash(
      HashAlgorithm& hash, Type&& value) {
    hash(std::forward<Type>(value));
  }

  template<typename Type>
  static typename std::enable_if<!IsGeneric<Type>::value>::type StartHash(
      HashAlgorithm& hash, const Type& value) {
    hash(value);
  }
};

}  // namespace maidsafe

#endif  // MAIDSAFE_COMMON_HASH_WRAPPERS_UNSEEDED_HASH_H_
