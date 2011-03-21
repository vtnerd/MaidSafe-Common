/* Copyright (c) 2011 maidsafe.net limited
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "maidsafe/common/file_chunk_store.h"
#include "maidsafe/common/crypto.h"
#include "maidsafe/common/utils.h"

#include "boost/filesystem/fstream.hpp"

namespace fs = boost::filesystem;

namespace maidsafe {

bool FileChunkStore::Init(const fs::path &storage_location) {
  try {
    //  empty the dir if it already exists
    if (fs::exists(storage_location))
      fs::remove_all(storage_location);

    if (fs::create_directory(storage_location)) {
      storage_location_ = storage_location;

      ResetChunkCount();

      ChunkStore::Clear();
      ChunkStore::SetCapacity(0);

      initialised_ = true;
    } else
        return false;
  } catch(...) {
    return false;
  }
  return true;
}

std::string FileChunkStore::Get(const std::string &name) {
  fs::path file_path(ChunkNameToFilePath(name));
  std::string content;
  if (!ReadFile(file_path, &content))
    return "";
  return content;
}

bool FileChunkStore::Get(const std::string &name,
                         const fs::path &sink_file_name) {

  fs::path source_file_path(ChunkNameToFilePath(name));

  boost::system::error_code ec;

  if (fs::exists(source_file_path, ec)) {
    if (ec)
      return false;
    fs::copy_file(source_file_path, sink_file_name,
                  fs::copy_option::overwrite_if_exists, ec);
    if (ec)
      return false;

    return true;
  }

  return false;
}

bool FileChunkStore::Store(const std::string &name,
                           const std::string &content) {
  if (name.empty())
    return false;

  if (Has(name))
    return true;

  fs::path chunk_file(ChunkNameToFilePath(name));

  if (content.empty())
    return false;

  if (!WriteFile(chunk_file, content))
    return false;

  //IncreaseSize(content.size());
  ChunkAdded(content.size());
  return true;
}

bool FileChunkStore::Store(const std::string &name,
                           const fs::path &source_file_name,
                           bool delete_source_file) {

  if (name.empty())
    return false;

  fs::path chunk_file(ChunkNameToFilePath(name));
  boost::system::error_code ec;

  //  does the chunk already exist
  if (!fs::exists(chunk_file, ec)) {

    std::uintmax_t file_size(fs::file_size(source_file_name, ec));

    //  is source file valid
    if (file_size && !ec) {
      try {
        if (delete_source_file)
          fs::rename(source_file_name, chunk_file);
        else
          fs::copy_file(source_file_name, chunk_file,
                    fs::copy_option::overwrite_if_exists);

        ChunkAdded(file_size);
        return true;
      } catch(...) {
        return false;
      }
    }
  } else {
    if (delete_source_file)
      fs::remove(source_file_name, ec);
    return true;
  }
  return false;
}

bool FileChunkStore::Delete(const std::string &name) {
  if (name.empty())
    return false;

  //  check non existant chunk
  if(!Has(name))
    return true;

  fs::path chunk_file(ChunkNameToFilePath(name));
  boost::system::error_code ec;

  std::uintmax_t file_size(fs::file_size(chunk_file, ec));
  fs::remove(chunk_file, ec);

  if (ec)
    return false;

  ChunkRemoved(file_size);

  return true;
}

bool FileChunkStore::MoveTo(const std::string &name,
                            ChunkStore *sink_chunk_store) {
  if (name.empty() || !sink_chunk_store)
    return false;

  if (!Has(name))
    return false;

  fs::path chunk_file(ChunkNameToFilePath(name));
  boost::system::error_code ec;

  try {
    std::uintmax_t file_size(Size(name));

    if (sink_chunk_store->Store(name, chunk_file, 1)) {
      ChunkRemoved(file_size);
      return true;
    }
  } catch (...) {
  }

  return false;
}

bool FileChunkStore::Has(const std::string &name) {
  if (name.empty())
    return false;

  fs::path chunk_file(ChunkNameToFilePath(name));
  boost::system::error_code ec;

  if (fs::exists(chunk_file, ec))
    return true;
  return false;
}

bool FileChunkStore::Validate(const std::string &name) {
  if (name.empty())
    return false;

  if (name == crypto::Hash<crypto::SHA512>(Get(name)))
    return true;

  //  invalid! delete it
  ChunkRemoved(Size(name));
  Delete(name);
  return false;
}

std::uintmax_t FileChunkStore::Size(const std::string &name) {
  if (name.empty())
    return 0;

  try {
    fs::path chunk_file(ChunkNameToFilePath(name));
    std::uintmax_t size = fs::file_size(chunk_file);
    return size;
  } catch (...) {
    return 0;
  }
}

std::uintmax_t FileChunkStore::Count() {
  return chunk_count_;
}

bool FileChunkStore::Empty() {
  return ((chunk_count_)? false : true);
}

void FileChunkStore::Clear() {
  ChunkStore::Clear();
  ResetChunkCount();
  fs::remove_all(storage_location_);
}

fs::path FileChunkStore::ChunkNameToFilePath(const std::string &chunk_name) {
  return fs::path(storage_location_ / EncodeToHex(chunk_name));
}

std::uintmax_t FileChunkStore::GetChunkCount(const fs::path &location) {
  boost::uintmax_t count(0);
  try {
  for (fs::directory_iterator it(location);
       it != boost::filesystem::directory_iterator(); ++it) {
         boost::system::error_code ec;
    if (boost::filesystem::is_regular_file(it->status())) {
      ++count;
      } else if (fs::is_directory(it->path(), ec)) {
        count += GetChunkCount(it->path());
      }
    }
  } catch(...){}
  return count;
}

void FileChunkStore::ChunkAdded(const std::uintmax_t &delta){
  IncreaseSize(delta);
  IncreaseChunkCount();
}

void FileChunkStore::ChunkRemoved(const std::uintmax_t &delta){
  DecreaseSize(delta);
  DecreaseChunkCount();
}

}  // namespace maidsafe
