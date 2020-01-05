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
