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

/// @file handler.cc
/// @brief Implementation for handler.h

#include "nx/handler.h"
#include "nx/looper.h"

/// @brief Library namespace.
namespace nx {

Handler::Handler()
    : Handler(Looper::threadLooper().get(), NULL) {
}
Handler::Handler(Callback* callback)
    : Handler(Looper::threadLooper().get(), callback) {
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
  return looper_->hasMessages(this, id);
}

bool Handler::hasMessages(unsigned int id, void* data) const {
  return looper_->hasMessages(this, id, true, data);
}

bool Handler::sendMessageAtFrontOfQueue(Message message) {
  return looper_->send(MessageEnvelope(this, message), SteadyTimePoint::min());
}

bool Handler::sendMessage(
    Message message, Handler::SteadyTimePoint triggerTime) {
  return looper_->send(MessageEnvelope(this, message), triggerTime);
}

bool Handler::sendMessage(
    Message message, std::chrono::milliseconds delay) {
  return looper_->send(MessageEnvelope(this, message), delay);
}

bool Handler::sendEmptyMessage(
    unsigned int id, Handler::SteadyTimePoint triggerTime) {
  return looper_->send(MessageEnvelope(this, Message(id)), triggerTime);
}

bool Handler::sendEmptyMessage(
    unsigned int id, std::chrono::milliseconds delay) {
  return looper_->send(MessageEnvelope(this, Message(id)), delay);
}

void Handler::removeMessages(unsigned int id) {
  looper_->remove(this, id);
}
void Handler::removeMessages(unsigned int id, void* data) {
  looper_->remove(this, id, true, data);
}

void Handler::handleMessage(Message message) {
}


// HandlerThread

void HandlerThread::threadFunction() {
  Looper::prepare();
  { // arbitrary block
    std::lock_guard<std::mutex> lock(mutex_);
    looper_ = Looper::threadLooper();
    conditionVariable_.notify_all();
  }
  Looper::loop();
}

HandlerThread::HandlerThread(const std::string& name)
    : name_(name)
    , looper_(nullptr) {
  threadObject_.reset(new std::thread(&HandlerThread::threadFunction, this));
}
HandlerThread::~HandlerThread() {
  getLooper()->quit();
  if (threadObject_->joinable()) {
    try {
      threadObject_->join();
    } catch (...) {
      // TODO(nacitar): std::cerr << "Error while joining." << std::endl;
    }
  }
}
Looper* HandlerThread::getLooper() {
  // TODO(nacitar): spinlock?
  std::unique_lock<std::mutex> lock(mutex_);
  if (!looper_) {
    conditionVariable_.wait(lock);
  }
  looper_->waitForLoop();
  return looper_.get();
}

void HandlerThread::join() {
  return threadObject_->join();
}

}  // namespace nx
