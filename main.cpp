#include <iostream>

#include "builder.hpp"

int main()
{
	auto agent = Builder()
					.LogLevel(Info)
					.FlushInterval(5000)
					.MaxBuffer(1000)
					.Labels({{"key", "value"}})
					.Build();

	std::cout << "ready " << agent.Ready() << std::endl;
	std::cout << "metrics " << std::endl;
	for(auto const &i : agent.Metrics())
		std::cout << i << std::endl;

	agent.Log("");

}
