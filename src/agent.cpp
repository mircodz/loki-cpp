#include <future>

#include "agent.hpp"
#include "common.hpp"

namespace loki
{

Agent::Agent(const std::map<std::string, std::string> &labels,
			 int flush_interval,
			 int max_buffer,
			 Agent::LogLevels log_level)
	: labels_{labels}
	, flush_interval_{flush_interval}
	, max_buffer_{max_buffer}
	, log_level_{log_level}
{
	compiled_labels_ = "{";
	for (const auto &label : labels_) {
		compiled_labels_ += "\"";
		compiled_labels_ += label.first;
		compiled_labels_ += "\":\"";
		compiled_labels_ += label.second;
		compiled_labels_ += "\",";
	}
	compiled_labels_.pop_back();
	compiled_labels_ += "}";

	curl_ = curl_easy_init();
}

Agent::~Agent()
{
	// make sure to cleanup the curl handle only once all instances are destroyed
	curl_easy_cleanup(curl_);
	curl_ = NULL;
}

bool Agent::Ready()
{
	return http::cget(curl_, "http://127.0.0.1:3100/ready").code == 200;
}

std::string Agent::Metrics()
{
	return http::cget(curl_, "http://127.0.0.1:3100/metrics").body;
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
	http::cpost(curl_, "http://127.0.0.1:3100/loki/api/v1/push", payload);
}

void Agent::BulkLog(std::string msg)
{
	std::string payload;
	payload += R"({"streams":[{"stream":)";
	payload += compiled_labels_;
	payload += R"(,"values":)";
	payload += msg;
	payload += R"(}]})";
	http::cpost(curl_, "http://127.0.0.1:3100/loki/api/v1/push", payload);
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
	http::cpost(curl_, "http://127.0.0.1:3100/loki/api/v1/push", payload);
}

bool Agent::QueueLog(std::string msg)
{
	std::lock_guard<std::mutex> lock{mutex_};

	if (logs_.size() < max_buffer_) {
		logs_.emplace(std::make_pair(std::chrono::system_clock::now(), msg));
		return false;
	} else {
		return true;
	}
}

void Agent::AsyncLog(std::string msg)
{
	auto future = std::async(std::launch::async, static_cast<void(Agent::*)(std::string)>(&Agent::Log), this, msg);
}

void Agent::Flush()
{
	std::lock_guard<std::mutex> lock{mutex_};

	int count = 0;
	std::string msg = "[";
	while (!logs_.empty()) {
		const auto &[k, v] = logs_.front();
		msg += R"([")";
		msg += std::to_string(k.time_since_epoch().count());
		msg += R"(",")";
		msg += v;
		msg += R"("],)";
		logs_.pop();
		count++;
	}
	msg.pop_back();
	msg += "]";
	if (count)
		BulkLog(msg);
}

} // namespace loki
