#include "builder.hpp"

int main() {
	using namespace loki;

	auto agent = Builder()
					.LogLevel(Agent::Info)
					.FlushInterval(5000)
					.MaxBuffer(1000)
					.Labels({{"key", "value"}})
					.Build();

	if (!agent.Ready()) return 1;

	agent.QueueLog("Hello from the queue!");
	agent.Flush();

	auto other = agent.Extend({{"yet_another_key", "yet_another_value"}});
	other.Log("Hello, World!");
	other.AsyncLog("Hello There!");
}
