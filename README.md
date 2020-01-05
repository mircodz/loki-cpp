# Loki Client Library for Modern C++

[![CI Status](https://github.com/mircodezorzi/loki-cpp/workflows/Continuous%20Integration/badge.svg)](https://github.com/mircodezorzi/loki-cpp/actions?workflow=Continuous+Integration)

Inspired by [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp).

## Usage

```cpp
#include <loki/builder.hpp>
#include <loki/parser.hpp>

int main() {
  using namespace loki;

  // create a registry
  auto registry = Builder{}
                  .LogLevel(Level::Warn)
                  .PrintLevel(Level::Debug)
                  .FlushProtocol(Protocol::Protobuf)
                  .Colorize(Level::Warn, TermColor::Yellow)
                  .Colorize(Level::Error, TermColor::Red)
                  .FlushInterval(100)
                  .MaxBuffer(1000)
                  .Labels({{"key", "value"}})
                  .Build();

  // check if loki is up
  if (!registry.Ready()) return 1;

  // create an agent with extended labels
  auto &agent = registry.Add({{"foo", "bar"}});

  // add logs to queue and wait for flush
  agent.Debugf("Hello, {}!", "Debug");
  agent.Infof("Hello, {}!", "Info");
  agent.Warnf("Hello, {}!", "Warn");
  agent.Errorf("Hello, {}!", "Error");
}
```

### Metrics Parser

```cpp
std::string s = registry.Metrics();
Parser parser{s};
for (const auto metric : parser.metrics())
   // ...
```

## Installation

```bash
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=ON
make loki-cpp -j4 -DWITH_LOGGING
make check # run tests
make bench # run benchmarks
sudo make install
```

## Building with loki-cpp

```bash
g++ example.cpp -std=c++17 -lloki-cpp
```

## Dependencies

- fmt
- libcurl
- protobuf
- snappy
