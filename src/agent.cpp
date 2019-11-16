#include <future>

#include "agent.hpp"
#include "common.hpp"

namespace loki
{

std::chrono::system_clock::time_point Agent::last_flush_{};
std::queue<std::pair<std::chrono::system_clock::time_point, std::string>> Agent::logs_{{}};
std::mutex Agent::lock_{};
std::atomic<bool> Agent::close_request_{false};
std::thread Agent::thread_{};
std::atomic<int> Agent::instances_{0};
CURL *Agent::curl_{NULL};

Agent::Agent(const std::map<std::string, std::string> &labels,
			 int flush_interval,
			 int max_buffer,
			 LogLevels log_level)
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
	if (!curl_) {
		curl_global_init(CURL_GLOBAL_DEFAULT);
		curl_ = curl_easy_init();
	}
	++instances_;
}

Agent::~Agent()
{
	close_request_.store(true);
	if (thread_.joinable()) {
		thread_.join();
	}
	// make sure to cleanup the curl handle only once all instances are destroyed
	--instances_;
	if (!instances_.load()) {
		curl_easy_cleanup(curl_);
		curl_ = NULL;
		curl_global_cleanup();
	}
}

void Agent::Spin()
{
	// make sure to initialize thread only once
	if (!thread_.joinable()) {
		thread_ = std::thread([this]() {
			while (!close_request_.load()) {
				if (std::chrono::duration(std::chrono::system_clock::now() - last_flush_).count() > flush_interval_) {
					Flush();
					last_flush_ = std::chrono::system_clock::now();
				}
			}
		});
	}
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
	std::lock_guard<std::mutex> guard(lock_);

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

Agent Agent::Extend(std::map<std::string, std::string> labels)
{
	labels.merge(labels_);
	return Agent{labels, flush_interval_, max_buffer_, log_level_};
}

void Agent::Flush()
{
	int count = 0;
	std::string msg = "[";
	lock_.lock();
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
	lock_.unlock();
	msg.pop_back();
	msg += "]";
	if (count)
		BulkLog(msg);
}

} // namespace loki
