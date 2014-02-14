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

/// @file handler.cc
/// @brief Implementation for handler.h

#include "nx/handler.h"
#include "nx/looper.h"

/// @brief Library namespace.
namespace nx {

Handler::Handler()
   : Handler(Looper::myLooper(),NULL) {
}
Handler::Handler(Callback* callback)
    : Handler(Looper::myLooper(),callback) {
}
Handler::Handler(Looper*looper)
   : Handler(looper, NULL) {
}
Handler::Handler(Looper* looper, Callback* callback)
    : looper_(looper), callback_(callback) {
  if (!looper) {
    throw std::runtime_error("Looper specified is nullptr.  Did you call"
        " Looper::prepare()?");
  }
}

void Handler::dispatchMessage(Message message) {
  if (callback_) {
    if (callback_->handleMessage(message)) {
      return;
    }
  }
  handleMessage(message);
}
Looper* Handler::looper() {
  return looper_;
}
const Looper* Handler::looper() const {
  return looper_;
}

bool Handler::hasMessages(unsigned int id) const {
  return looper()->hasMessages(this, id);
}

bool Handler::hasMessages(unsigned int id, void* data) const {
  return looper()->hasMessages(this, id, true, data);
}

bool Handler::sendEmptyMessage(
    unsigned int id, Handler::SteadyTimePoint triggerTime) {
  looper_->send(MessageEnvelope(this, Message(id)), triggerTime);
  return true; // TODO
}

bool Handler::sendEmptyMessage(
    unsigned int id, std::chrono::milliseconds delay) {
  looper_->send(MessageEnvelope(this, Message(id)), delay);
  return true; // TODO
}

void Handler::removeMessages(unsigned int id) {
  looper_->remove(this, id);
}
void Handler::removeMessages(unsigned int id, void* data) {
  looper_->remove(this, id, true, data);
}

void Handler::handleMessage(Message message) {
}

}  // namespace nx
