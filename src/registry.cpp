#include "registry.hpp"
#include "agent.hpp"

#include "detail/utils.hpp"

namespace loki {
 
template <typename AgentType>
Registry<AgentType>::Registry(
		std::map<std::string, std::string> &&labels,
		std::size_t flush_interval,
		std::size_t max_buffer,
		Level log_level,
		Level print_level,
		const std::string &remote_host,
		std::array<Color, 4> colors)
	: labels_{std::move(labels)}
	, flush_interval_{flush_interval}
	, max_buffer_{max_buffer}
	, log_level_{log_level}
	, print_level_{print_level}
	, remote_host_{remote_host}
	, colors_{colors} {
	curl_global_init(CURL_GLOBAL_DEFAULT);
	thread_ = std::thread([this]() {
		while (!close_request_.load()) {
			for (auto &agent : agents_) {
				agent->Flush();
			}
			std::chrono::milliseconds timespan(flush_interval_);
			std::this_thread::sleep_for(timespan);
		}
	});
}

template <typename AgentType>
Registry<AgentType>::~Registry() {
 	close_request_.store(true);
	for (auto &agent : agents_) {
		agent->Flush();
	}
	if (thread_.joinable()) {
		thread_.join();
	}
	curl_global_cleanup();
}

template <typename AgentType>
bool Registry<AgentType>::Ready() const {
	CURL *curl = curl_easy_init();
	auto r = detail::get(curl, fmt::format("http://{}/ready", remote_host_)).code == 200;
	curl_easy_cleanup(curl);
	return r;
}

template <typename AgentType>
std::vector<Metric> Registry<AgentType>::Metrics() const {
	CURL *curl = curl_easy_init();
	auto r = detail::get(curl, fmt::format("http://{}/metrics", remote_host_)).body;
	curl_easy_cleanup(curl);
	Lexer lexer{r};
	Parser parser{lexer.tokens()};
	return parser.metrics();
}
*/

template <typename AgentType>
AgentType &Registry<AgentType>::Add(std::map<std::string, std::string> &&labels) {
	std::lock_guard<std::mutex> lock{mutex_};
	labels.insert(labels_.begin(), labels_.end());
	auto agent = std::make_unique<AgentType>(labels, flush_interval_, max_buffer_, log_level_, print_level_, remote_host_, colors_);
	auto &ref = *agent;
	agents_.push_back(std::move(agent));
	return ref;
}

template class Registry<AgentJson>;
#if defined(HAS_PROTOBUF)
template class Registry<AgentProto>;
#endif

} // namespace loki
