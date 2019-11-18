# Loki Client Library for Modern C++

Inspired by [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp).

## Usage

```cpp
#include "builder.hpp"
#include "parser.hpp"

#include <fmt/format.h>

int main() {
    using namespace loki;

    // create a registry
    auto registry = Builder()
                    .LogLevel(Agent::Info)
                    .FlushInterval(5000)
                    .MaxBuffer(1000)
                    .Labels({{"key", "value"}})
                    .Build();

    // check if loki is up
    if (!registry.Ready()) return 1;

    // parse metrics
    std::string s = registry.Metrics();
    fmt::print("{}\n", s);

    Parser parser{s};

    for (const auto &m : parser.metrics()) {
        fmt::print("{}: ", m.metric);
        for (const auto &[k, v] : m.labels)
            fmt::print("{} = {}, ", k, v);
        fmt::print("\b\b  \n");
    }

    // create an agent with default labels
    auto &agent = registry.Add();

    // add logs to queue and forcefully flush
    agent.QueueLog("Hello from foo!");
    agent.QueueLog("Hello from bar!");
    agent.QueueLog("Hello from baz!");
    agent.Flush();

    // create an agent with extended labels
    auto &other = registry.Add({{"foo", "bar"}});

    // blocking and non-blocking log
    other.Log("Hello, World!");
    other.AsyncLog("Hello There!");
}
```

## Building

```bash
mkdir build
cd build
cmake ..
make -j4
make check # run tests
make bench # run benchmarks
```

## Moving Forward

- Rework automatic flushing thread.
- Move cmake build from a binary to a shared library.
