#include "registry.hpp"
#include "agent.hpp"

#include "detail/utils.hpp"

#include <iostream>

namespace loki
{

template <typename T>
Registry<T>::Registry(
		const std::map<std::string, std::string> &labels,
		int flush_interval,
		int max_buffer,
		Level log_level,
		Level print_level,
		std::array<Color, 4> colors)
	: labels_{labels}
	, flush_interval_{flush_interval}
	, max_buffer_{max_buffer}
	, log_level_{log_level}
	, print_level_{print_level}
	, colors_{colors}
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	thread_ = std::thread([this]() {
		while (!close_request_.load()) {
			// wait some time between flushes, force when receiving a close request
			if (std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::system_clock::now() - last_flush_).count() > flush_interval_ || close_request_.load()) {
				for (auto &agent : agents_)
					agent->Flush();
				last_flush_ = std::chrono::system_clock::now();
			}
		}
	});
}

template <typename T>
Registry<T>::~Registry()
{
	close_request_.store(true);
	for (auto &agent : agents_)
		agent->Flush();
	if (thread_.joinable()) {
		thread_.join();
	}
	curl_global_cleanup();
}

template <typename T>
bool Registry<T>::Ready() const
{
	CURL *curl = curl_easy_init();
	auto r = detail::http::get(curl, "http://127.0.0.1:3100/ready").code == 200;
	curl_easy_cleanup(curl);
	return r;
}

template <typename T>
std::vector<Metric> Registry<T>::Metrics() const
{
	CURL *curl = curl_easy_init();
	auto r = detail::http::get(curl, "http://127.0.0.1:3100/metrics").body;
	curl_easy_cleanup(curl);
	Lexer lexer{r};
	Parser parser{lexer.tokens()};
	return parser.metrics();
}

template <typename T>
T &Registry<T>::Add(std::map<std::string, std::string> &&labels)
{
	std::lock_guard<std::mutex> lock{mutex_};
	for (auto &p : labels_)
		labels.emplace(p);
	auto agent = std::make_unique<T>(labels, flush_interval_, max_buffer_, log_level_, print_level_, colors_);
	auto &ref = *agent;
	agents_.push_back(std::move(agent));
	return ref;
}

template class Registry<AgentJson>;
#if defined(HAS_PROTOBUF)
template class Registry<AgentProto>;
#endif

} // namespace loki
