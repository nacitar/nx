#include <nx/core.hpp>
#include <nx/application.hpp>

#include <iostream>
// Create my application
struct MyApplication : nx::Application {
  int main() {
    nx::uint_least_t<32> x;
    std::cout << sizeof(x) << std::endl;
    return 0;
  }
};

// Set this as the default application
nx::Application& nx::application = *new MyApplication();
