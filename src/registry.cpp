#include "registry.hpp"
#include "agent.hpp"

#include "detail/utils.hpp"

#include <iostream>

namespace loki
{

Registry::Registry(const std::map<std::string, std::string> &labels,
				   int flush_interval,
				   int max_buffer,
				   Level log_level,
				   Level print_level,
				   Protocol protocol,
				   std::array<TermColor, 4> colors)
	: labels_{labels}
	, flush_interval_{flush_interval}
	, max_buffer_{max_buffer}
	, log_level_{log_level}
	, print_level_{print_level}
	, protocol_{protocol}
	, colors_{colors}
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	thread_ = std::thread([this]() {
		while (!close_request_.load()) {
			// wait some time between flushes, force when receiving a close request
			if (std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::system_clock::now() - last_flush_).count() > flush_interval_ || close_request_.load()) {
				for (auto &agent : agents_)
					if (!agent->Done())
						agent->Flush();
				last_flush_ = std::chrono::system_clock::now();
			}
		}
	});
}

Registry::~Registry()
{
	close_request_.store(true);
	if (thread_.joinable()) {
		thread_.join();
	}
	curl_global_cleanup();
}

bool Registry::Ready() const
{
	CURL *curl = curl_easy_init();
	return detail::http::get(curl, "http://127.0.0.1:3100/ready").code == 200;
	curl_easy_cleanup(curl);
}

std::string Registry::Metrics() const
{
	CURL *curl = curl_easy_init();
	return detail::http::get(curl, "http://127.0.0.1:3100/metrics").body;
	curl_easy_cleanup(curl);
}

Agent &Registry::Add(std::map<std::string, std::string> labels)
{
	std::lock_guard<std::mutex> lock{mutex_};
	for (const auto &p : labels_)
		labels.emplace(p);
	auto agent = std::make_unique<Agent>(labels, flush_interval_, max_buffer_, log_level_, print_level_, protocol_, colors_);
	auto &ref = *agent;
	agents_.push_back(std::move(agent));
	return ref;
}

} // namespace loki
