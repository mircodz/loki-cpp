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
                  .LogLevel(Agent::LogLevel::Warn)
                  .PrintLevel(Agent::LogLevel::Warn)
                  .Protocol(Agent::Protocol::Protobuf)
                  .FlushInterval(100)
                  .MaxBuffer(1000)
                  .Labels({{"key", "value"}})
                  .Build();

  // check if loki is up
  if (!registry.Ready()) return 1;

  // create an agent with extended labels
  auto &agent = registry.Add({{"foo", "bar"}});

  // add logs to queue and wait for flush
  agent.Infof("Hello, World!");
  agent.Debugf("Hello, World!");
  agent.Warnf("Hello, World!");
  agent.Errorf("Hello, World!");
}
```

## Building

```bash
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=ON
make loki-cpp -j4
make check # run tests
make bench # run benchmarks
sudo make install
```

## Dependencies

- fmt
- libcurl
- protobuf
- snappy
