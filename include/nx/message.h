//
// Copyright (C) 2013 Jacob McIntosh <nacitar at ubercpp dot com>
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

/// @file message.h
/// @brief An android-esque message.

#ifndef INCLUDE_NX_MESSAGE_H_
#define INCLUDE_NX_MESSAGE_H_

/// @brief Library namespace.
namespace nx {

class Message {
  unsigned int id_;
  void* data_;
 public:
  Message(unsigned int id = 0, void* data = nullptr);
  Message(const Message& message);
  void* data() const;
  void setData(void* data);
  unsigned int id() const;
  void setId(unsigned int id);
};

}  // namespace nx

#endif  // INCLUDE_NX_MESSAGE_H_
