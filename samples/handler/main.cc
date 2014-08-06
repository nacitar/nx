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

/// @file main.cc
/// @brief Sample project to try out the Handler class.

#include <iostream>
#include <stdexcept>
#include "nx/core.h"
#include "nx/application.h"
#include "nx/to_string.h"
#include "nx/looper.h"
#include "nx/handler.h"

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <map>
#include <thread>

class TimeTracker {
  typedef std::chrono::time_point<std::chrono::steady_clock> time_point;

  time_point start_;

 public:
  TimeTracker() {
    reset();
  }

 void reset() {
    using namespace std::chrono;
   start_ = steady_clock::now();
 }
 unsigned long long elapsed() {
    using namespace std::chrono;
    time_point now = steady_clock::now();
    return duration_cast<milliseconds>(now-start_).count();

   }
};

class MyHandler : public nx::Handler {

  TimeTracker tracker_;
  TimeTracker lastTracker_;
  const std::string label_;
 public:
  //using Handler::Handler;
  MyHandler(const std::string&label,nx::Looper* looper)
      : nx::Handler(looper)
      , label_(label) {
  }

  void handleMessage(nx::Message message) {
    std::cout << label_ << " Message: " << message.id() << " "
        << tracker_.elapsed() << " " << lastTracker_.elapsed() << std::endl;
    lastTracker_.reset();
    if (message.id() == 3) {
      looper()->quit();
    }
  }
};


/// @brief The class for the blocker application.
class HandlerApplication : public nx::Application {
  nx::HandlerThread messageThread;
  nx::Looper* looper_;
  MyHandler handler_;
  MyHandler otherHandler_;
 public:

  HandlerApplication()
      : messageThread("Message Thread")
      , looper_(messageThread.getLooper())
      , handler_("A", looper_)
      , otherHandler_("B", looper_)
  {
  }

  int main() {
    using namespace std::chrono;
    otherHandler_.sendEmptyMessage(2,milliseconds(3500));
    handler_.sendEmptyMessage(1,milliseconds(2000));
    handler_.sendEmptyMessage(2,milliseconds(2000));
    handler_.sendEmptyMessage(3,milliseconds(5000));
    handler_.removeMessages(2);
    messageThread.join();
    return 0;
  }
};

/// @brief Function to lazy-load the application; required by nx_main.cc
nx::Application& nx::GetApplication() {
  static HandlerApplication app;
  return app;
}
