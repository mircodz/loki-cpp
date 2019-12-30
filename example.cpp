#include <loki/builder.hpp>
#include <loki/parser.hpp>

int main() {
	using namespace loki;

	// create a registry
	auto registry = Builder{}
					.LogLevel(Agent::LogLevel::Debug)
					.PrintLevel(Agent::LogLevel::Debug)
					.Colorize(Agent::LogLevel::Warn, Agent::TermColor::Yellow)
					.Colorize(Agent::LogLevel::Error, Agent::TermColor::Red)
					.Protocol(Agent::Protocol::Protobuf)
					.FlushInterval(100)
					.MaxBuffer(1000)
					.Labels({{"key", "value"}})
					.Build();

	// check if loki is up
	if (!registry.Ready()) return 1;

	// create an agent with extended labels
	auto &agent = registry.Add({{"foo", "bar"}});

	// add logs to queue and forcefully flush
	for (int i = 0; i < 5; ++i) {
		agent.Infof("Hello, World!");
		agent.Debugf("Hello, World!");
		agent.Warnf("Hello, World!");
		agent.Errorf("Hello, World!");
	}
	agent.Flush();
}
