#include "builder.hpp"

int main()
{
	auto agent = Builder()
					.LogLevel(Info)
					.FlushInterval(5000)
					.MaxBuffer(1000)
					.Labels({{"key", "value"}})
					.Build();

	agent.Ready();
	agent.Metrics();
}
