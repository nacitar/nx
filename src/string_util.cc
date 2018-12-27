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

/// @file string_util.cc
/// @brief Implementation for string_util.h

#include "nx/string_util.h"

/// @brief Library namespace.
namespace nx {

std::vector<std::string> ArgumentParser(const std::string&argument_string) {
  std::vector<std::string> arguments;
  std::string buffer;
  char escape = '\0';
  char quote = '\0';
  bool was_quote = false;
  const std::vector<std::string>::size_type length = argument_string.size();
  for (std::vector<std::string>::size_type i = 0; i < length; ++i) {
    char ch = argument_string[i];
    if (quote) {
      // in a quote
      if (escape) {
        // in an escape too
        if (ch == quote) {
          // escaping the same quote gives just that quote
          buffer.push_back(ch);
        } else {
          // escaping anything else gives the escape and char as written
          buffer.push_back(escape);
          buffer.push_back(quote);
        }
        // in either case, this terminates the escape.
        escape = 0;
        continue;
      } else if (ch == quote) {
        // specifying the same quote again terminates the quote.
        quote = 0;
        was_quote = true;
        continue;
      }
    } else {
      if (escape != 0) {
        // all escapes outside quotes which are supported simply output the
        // second character, as we aren't handling special ones like \t or \n
        buffer.push_back(ch);
        escape = 0;
        continue;
      } else {
        // outside of quotes and escapes
        switch (ch) {
          // we can tokenize
          case ' ':
            if (was_quote || !buffer.empty()) {
              arguments.push_back(std::move(buffer));
              was_quote = false;
            }
            continue;
          // we can start quotes
          case '"':
          case '\'':
            quote = ch;
            continue;
        }
      }
    }
    // escape handling and default handling can fall through from either branch
    // to here
    switch (ch) {
      case '\\':
        escape = ch;
        break;
      default:
        buffer.push_back(ch);
    }
  }
  if (escape != 0) {
    // makes trailing escapes be appended (erroneous string, though, IMO)
    buffer.push_back(escape);
  }
  if (was_quote || !buffer.empty()) {
    // add the final string
    arguments.push_back(std::move(buffer));
  }
  return arguments;
}

}  // namespace nx
