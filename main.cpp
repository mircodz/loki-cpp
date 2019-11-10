#include "builder.hpp"
#include "stream.hpp"

#include <iostream>

int main()
{
	auto agent = Builder()
					.LogLevel(Info)
					.FlushInterval(5000)
					.MaxBuffer(1000)
					.Labels({{"key", "value"}})
					.Build();

	std::cout << agent.Metrics() << std::endl;
	std::cout << agent.Ready() << std::endl;

	auto s = agent.Add({{"yet_another_key", "yet_another_value"}});

	s.Log("Hello, World!");

}
