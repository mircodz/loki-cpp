#include <chrono>
#include <iostream>
#include <future>

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

void Agent::Log(std::string msg)
{
	std::string payload;
	payload += R"({"streams":[{"stream":)";
	payload += compiled_labels_;
	payload += R"(,"values":[[")";
	payload += std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
	payload += R"(",")";
	payload += msg;
	payload += R"("]]}]})";
	http::post("127.0.0.1", 3100, "/loki/api/v1/push", payload);
}

void Agent::Log(std::chrono::system_clock::time_point ts, std::string msg)
{
	std::string payload;
	payload += R"({"streams":[{"stream":)";
	payload += compiled_labels_;
	payload += R"(,"values":[[")";
	payload += std::to_string(ts.time_since_epoch().count());
	payload += R"(",")";
	payload += msg;
	payload += R"("]]}]})";
	http::post("127.0.0.1", 3100, "/loki/api/v1/push", payload);
}

void Agent::QueueLog(std::string msg)
{
	logs_.emplace(std::make_pair(std::chrono::system_clock::now(), msg));
}

void Agent::AsyncLog(std::string msg)
{
	auto future = std::async(std::launch::async, &Agent::AsyncLog, this, msg);
}

Stream Agent::Add(std::map<std::string, std::string> labels)
{
	labels.merge(labels_);
	return Stream(labels);
}

void Agent::Flush()
{
	while (!logs_.empty()) {
		const auto &[k, v] = logs_.front();
		Log(k, v);
		logs_.pop();
	}
}
