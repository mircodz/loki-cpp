# Loki client written in modern c++

[![CI Status](https://github.com/mircodezorzi/loki-cpp/workflows/Continuous%20Integration/badge.svg)](https://github.com/mircodezorzi/loki-cpp/actions?workflow=Continuous+Integration)

Inspired by [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp).

## Usage

```cpp
#include <loki/builder.hpp>

int main() {
  using namespace loki;

  // Create a registry
  auto registry = Builder<AgentJson>{}
                    .Remote("127.0.0.1:3000")
                    .LogLevel(Level::Warn)
                    .PrintLevel(Level::Debug)
                    .Colorize(Level::Warn,  Color::Yellow)
                    .Colorize(Level::Error, Color::Red)
                    .FlushInterval(100)
                    .MaxBuffer(1000)
                    .Labels({
                      {"namespace", "production"},
                      {"location", "en"}
                    })
                    .Build();

  // Check if Loki is up
  if (!registry.Ready()) return 1;

  // Create an agent with extended labels
  auto &logger = registry.Add({{"process", "foobar"}});

  // Add logs to queue and wait for flush
  logger.Debugf("Hello, {}!", "Debug");
  logger.Infof("Hello, {}!", "Info");
  logger.Warnf("Hello, {}!", "Warn");
  logger.Errorf("Hello, {}!", "Error");
}
```

Now to compile the example run:

```bash
g++ example.cpp -std=c++17 `pkg-config loki-cpp --cflags --libs`
```

## Installation

To compile a debug build (and run all tests) run:

```sh
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
ctest -V
```

To install the debug build run:

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make install
```

### Dependencies
  - fmt
  - curl
  - protobuf + snappy (optional)
