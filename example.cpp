#include <loki/builder.hpp>

int main() {
	using namespace loki;

	// create a registry
	auto registry = Builder<AgentJson>{}
										.LogLevel(Level::Warn)
										.PrintLevel(Level::Debug)
										.Colorize(Level::Warn, Color::Yellow)
										.Colorize(Level::Error, Color::Red)
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
