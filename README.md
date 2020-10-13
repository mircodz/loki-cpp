# Loki client written in modern c++

[![CI Status](https://github.com/mircodezorzi/loki-cpp/workflows/Continuous%20Integration/badge.svg)](https://github.com/mircodezorzi/loki-cpp/actions?workflow=Continuous+Integration)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/3ce68eb8a73444448f1c475eb3a222c5)](https://www.codacy.com/manual/mircodezorzi/loki-cpp?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=mircodezorzi/loki-cpp&amp;utm_campaign=Badge_Grade)

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

Now to compile our example run:

```bash
g++ example.cpp -std=c++17 -lloki-cpp
```

## Metrics Parser

`loki-cpp` also ships with a [parser](https://github.com/mircodezorzi/loki-cpp/blob/master/include/parser.hpp) to allow an easier interaction with Loki's metrics, an example of how to use it:

```cpp
for (const auto metric : registry.Metrics()) {
  fmt::print("{}: {}\n", metric.name, metric.value);
}
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

## Todo

  - Write tests for each component