#include <chrono>
#include <iostream>

#include "agent.hpp"
#include "common.hpp"

Agent::Agent(const std::map<std::string, std::string> &labels,
			 const int flush_interval,
			 const int max_buffer,
			 const LogLevels log_level)
	: labels_(labels)
	, flush_interval_(flush_interval)
	, max_buffer_(max_buffer)
	, log_level_(log_level)
{
}

Agent::~Agent()
{
}

bool Agent::Ready()
{
	auto response = http::get("127.0.0.1", 3100, "/ready");
	return http::get_code(response) == 200 ? 1 : 0;
}

std::vector<std::string> Agent::Metrics()
{
	auto response = http::full_get("127.0.0.1", 3100, "/metrics");
	return split("", "\n");
}

void Agent::Log(std::string msg)
{
	std::string payload;
	payload += R"({"streams": [{ "stream": )";
	payload += R"({ "foo": "bar" })";
	payload += R"(, "values": [)";
	payload += R"( [ ")";
	payload += std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count());
	payload += R"(", ")";
	payload += msg;
	payload += R"(" ] )";
	payload += R"(] }]})";
}
