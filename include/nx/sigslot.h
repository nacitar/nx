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

/// @file sigslot.h
/// @brief Signals and slots for event delegation and dispatch.
/// @todo Needs to support mutexes, and thread delegation.

#ifndef INCLUDE_NX_SIGSLOT_H_
#define INCLUDE_NX_SIGSLOT_H_


#include <set>
#include <list>
#include <map>
#include "nx/core.h"

/// @brief Library namespace.
namespace nx {

// Forward declaration due to tight coupling with SignalBase
class SlotRegistrar;

/// @cond nx_detail
namespace detail {

/// @brief A base type for all slots
template <typename... Arguments>
class SlotBase {
 public:
  virtual ~SlotBase() = default;
  virtual void call(Arguments... args) = 0;
};

/// @brief Slots for standalone functions.
template <typename... Arguments>
class SlotFunction : public SlotBase<Arguments...> {
 public:
  typedef Function<void, Arguments...> FunctionType;
  FunctionType function_;
  explicit SlotFunction(FunctionType function)
      : function_(function) {
  }
  virtual ~SlotFunction() = default;
  virtual void call(Arguments... args) {
    function_(args...);
  }
};

/// @brief Slots for member functions.
template <typename Class, typename... Arguments>
class SlotMemberFunction : public SlotBase<Arguments...> {
 public:
  typedef MemberFunction<void, Class, Arguments...> MemberFunctionType;
 private:
  Class* object_;
  MemberFunctionType memberFunction_;
 public:
  SlotMemberFunction(Class*object, MemberFunctionType memberFunction)
      : object_(object)
      , memberFunction_(memberFunction) {
  }
  virtual ~SlotMemberFunction() = default;
  virtual void call(Arguments... args) {
    (object_->*memberFunction_)(args...);
  }
};

/// @brief A base for all signals, which allows for generic handling of all
/// signals.
class SignalBase {
 protected:
  // These methods are used by signals to utilize the friend relationship with
  // the SlotRegistrar to add/remove from its signal list.
  void registrarConnectedToSignal(SlotRegistrar* registrar);
  void registrarDisconnectedFromSignal(SlotRegistrar* registrar);

 public:
  virtual ~SignalBase() = default;
  virtual void disconnect(SlotRegistrar* registrar) = 0;
  virtual void clear() = 0;
};

}  // namespace detail
/// @endcond




/// @brief A object that keeps track of slot connections to signals.  Can be
/// used to group connections together for connection/disconnection.
class SlotRegistrar {
  // managed by SignalBase
  std::set<detail::SignalBase*> connectedSignals_;
 public:
  /// @brief Invokes clear() to disconnect correctly.
  ~SlotRegistrar();

  /// @brief Disconnects all slots connected to the provided signal.
  void disconnect(detail::SignalBase*signal);

  /// @brief Disconnects all slots registered on this registrar.
  void clear();

  friend class detail::SignalBase;
};

/// @brief A signal which can be utilized to emit an event and dispatch it to
/// all connected slots, allowing for event-driven development.
template <typename... Arguments>
class Signal : public detail::SignalBase {
  typedef Signal<Arguments...> SignalType;
  typedef detail::SlotBase<Arguments...> SlotBaseType;

  std::list<SlotBaseType*> connectedSlots_;
  std::map<SlotRegistrar*,
      std::list<
          typename decltype(connectedSlots_)::iterator>> registrarToSlots_;

  /// @brief Connects any slot type
  ///
  /// @param registrar The SlotRegistrar
  /// @param slotBaseType A 'new'-allocated Slot whose ownership will be
  /// transferred.
  void connect(SlotRegistrar*registrar, SlotBaseType*slotBaseType) {
    auto iterator = connectedSlots_.insert(connectedSlots_.end(),
        slotBaseType);
    // store iterators for deletion
    auto& slotList = registrarToSlots_[registrar];
    if (slotList.empty()) {
      registrarConnectedToSignal(registrar);
    }
    slotList.push_back(iterator);
  }

 public:
  virtual ~Signal() {
    clear();
  }

  /// @brief Connects any class member function.
  template <class Class>
  void connect(SlotRegistrar*registrar, Class*object,
      MemberFunction<void, Class, Arguments...> memberFunction) {
    // deleted in disconnect
    connect(registrar, new detail::SlotMemberFunction<Class, Arguments...>(
        object, memberFunction));
  }

  /// @brief Connects any free function.
  void connect(SlotRegistrar*registrar,
      Function<void, Arguments...> function) {
    // deleted in disconnect
    connect(registrar, new detail::SlotFunction<Arguments...>(function));
  }

  /// @brief Connects another signal of the same type.
  void connect(SlotRegistrar*registrar, SignalType* signal) {
    connect(registrar, signal, &SignalType::emit);
  }

  /// @brief Disconnects all slots connected to this signal.
  virtual void clear() {
    // Care is taken in how we approach this; we know the the call will remove
    // the item from this map, so we can't iterate in such a way that we access
    // the iterators after they're removed.
    while (!registrarToSlots_.empty()) {
      disconnect(registrarToSlots_.begin()->first);
    }
  }

  /// @brief Disconnects all slots connected through the provided registrar.
  virtual void disconnect(SlotRegistrar* registrar) {
    auto objectIt = registrarToSlots_.find(registrar);
    if (objectIt != registrarToSlots_.end()) {
      for (auto it : objectIt->second) {
        delete *it;  // deleting the slots created above
        connectedSlots_.erase(it);
      }
      registrarToSlots_.erase(objectIt);
      registrarDisconnectedFromSignal(registrar);
    }
  }

  /// @brief Emits the signal to all connected slots.
  void emit(Arguments... args) {
    for (auto& slot : connectedSlots_) {
      slot->call(args...);
    }
  }
};

}  // namespace nx

#endif  // INCLUDE_NX_SIGSLOT_H_
