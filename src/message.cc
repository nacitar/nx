//
// Copyright (C) 2014 Jacob McIntosh <nacitar at ubercpp dot com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/// @file message.cc
/// @brief Implementation for message.h

#include "nx/message.h"

/// @brief Library namespace.
namespace nx {

Message::Message(unsigned int id, void* data)
    : id_(id)
    , data_(data) {
}
Message::Message(const Message& message) {
  id_ = message.id_;
  data_ = message.data_;
}
void* Message::data() const {
  return data_;
}
void Message::setData(void* data) {
  data_ = data;
}
unsigned int Message::id() const {
  return id_;
}
void Message::setId(unsigned int id) {
  id_ = id;
}

}  // namespace nx
