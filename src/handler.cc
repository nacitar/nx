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
   : Handler::Handler(Looper::myLooper()) {
}
Handler::Handler(Looper*looper)
   : looper_(looper) {
  if (!looper) {
    throw std::runtime_error("Looper specified is nullptr.");
  }
}
Looper* Handler::looper() {
  return looper_;
}

bool Handler::sendEmptyMessage(int id, Handler::SteadyTimePoint triggerTime) {
  looper_->send(MessageEnvelope(this,id),triggerTime);
  return true; // TODO
}

bool Handler::sendEmptyMessage(int id, std::chrono::milliseconds delay) {
  looper_->send(MessageEnvelope(this,id),delay);
  return true; // TODO
}

void Handler::removeMessages(int id) {
  looper_->remove(id);
}

//void Handler::handleMessage(Message message)=0;

}  // namespace nx
