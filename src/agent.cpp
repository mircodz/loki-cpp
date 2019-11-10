#include <chrono>
#include <iostream>

#include "agent.hpp"
#include "common.hpp"

Agent::Agent(const std::map<std::string, std::string> &labels,
			 int flush_interval,
			 int max_buffer,
			 LogLevels log_level)
	: labels_(labels)
	, flush_interval_(flush_interval)
	, max_buffer_(max_buffer)
	, log_level_(log_level)
{
	compiled_labels_ += "{";
	for (const auto &label : labels_) {
		compiled_labels_ += "\"" + label.first + "\":\"" + label.second + "\",";
	}
	compiled_labels_.pop_back();
	compiled_labels_ += "}";
}

Agent::~Agent()
{
}

bool Agent::Ready()
{
	auto response = http::get("127.0.0.1", 3100, "/ready");
	return http::detail::get_code(response) == 200 ? true : false;
}

std::string Agent::Metrics()
{
	return http::full_get("127.0.0.1", 3100, "/metrics");
}

/**
 * @todo Add message to queue and bulk flush logs
 */
void Agent::Log(std::string msg)
{
	std::string payload;
	payload += R"({"streams":[{"stream":)";
	payload += compiled_labels_;
	payload += R"(,"values":[[")";
	payload += std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count());
	payload += R"(",")";
	payload += msg;
	payload += R"("]]}]})";
	http::post("127.0.0.1", 3100, "/loki/api/v1/push", payload);
}

/**
 * @todo optimize function by avoiding so many copies
 */
Stream Agent::Add(std::map<std::string, std::string> labels)
{
	labels.merge(labels_);
	return Stream(labels);
}


