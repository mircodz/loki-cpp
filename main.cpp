#include "builder.hpp"

#include <fmt/format.h>

int main() {
	using namespace loki;

	auto registry = Builder()
					.LogLevel(Agent::Info)
					.FlushInterval(5000)
					.MaxBuffer(1000)
					.Labels({{"key", "value"}})
					.Build();

	// create an agent with default labels
	auto &agent = registry.Add();

	// check if loki is up
	if (!agent.Ready()) return 1;

	// print metrics
	fmt::print("{}\n", agent.Metrics());

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
