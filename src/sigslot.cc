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

/// @file sigslot.cc
/// @brief Implementation for sigslot.h

#include "nx/sigslot.h"

/// @brief Library namespace.
namespace nx {

namespace detail {

void SignalBase::registrarConnectedToSignal(SlotRegistrar* registrar) {
  registrar->connectedSignals_.insert(this);
}
void SignalBase::registrarDisconnectedFromSignal(
    SlotRegistrar* registrar) {
  registrar->connectedSignals_.erase(this);
}

}  // namespace detail

void SlotRegistrar::disconnect(detail::SignalBase*signal) {
  signal->disconnect(this);
}

void SlotRegistrar::clear() {
  // Care is taken in how we approach this; we know the signal will remove
  // itself from this set, so we can't iterate in such a way that we access
  // the iterators after they're removed.
  while (!connectedSignals_.empty()) {
    disconnect(*(connectedSignals_.begin()));
  }
}

}  // namespace nx
