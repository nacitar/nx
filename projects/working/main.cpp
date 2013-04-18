#include <nx/application.hpp>
#include <nx/time.hpp>

#include <iostream>
// Create my application
struct MyApplication : nx::Application {
  int main() {

    nx::msec_t last=0;
    while (true)
    {
      nx::msec_t now=nx::fixed_msec();
      if (last != now)
      {
        std::cout << +now << std::endl;
        last = now;
      }
    }
    return 0;
  }
};

// Set this as the default application
nx::Application& nx::application = *new MyApplication();
