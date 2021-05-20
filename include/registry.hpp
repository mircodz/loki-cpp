#pragma once

#include <atomic>
#include <thread>

#include "agent.hpp"
#include "detail/utils.hpp"

namespace loki {

/// \brief Handles the creation and flushing of agents.
///
/// Periodically flushes each agent's queue.
/// Blocks registry destruction until all agents are done.
///
/// The class is thread-safe.
template <typename AgentType>
class Registry {
public:
	Registry(std::unordered_map<std::string, std::string> &&labels,
	         std::size_t flush_interval,
	         std::size_t max_buffer,
	         Level log_level,
	         Level print_level,
	         std::string &&remote_host,
	         std::array<Color, 4> colors)
	    : labels_{std::move(labels)},
	      flush_interval_{flush_interval},
	      max_buffer_{max_buffer},
	      log_level_{log_level},
	      print_level_{print_level},
	      remote_host_{std::move(remote_host)},
	      colors_{colors} {
		curl_global_init(CURL_GLOBAL_DEFAULT);
		thread_ = std::thread([this]() {
			while (!close_request_.load()) {
				for (auto &agent : agents_) {
					agent->Flush();
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(flush_interval_));
			}
		});
	}

	CURL *curl = curl_easy_init();

	Registry &operator=(const Registry &) = delete;

	Registry(const Registry &) = delete;

	~Registry() {
		close_request_.store(true);
		for (auto &agent : agents_) {
			agent->Flush();
		}
		if (thread_.joinable()) {
			thread_.join();
		}
		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}

	/// \brief Check whether Loki is up and running.
	[[nodiscard]] bool Ready() {
		return detail::get(curl, fmt::format("http://{}/ready", remote_host_)).code == 200;
	}

	/// \brief Create an agent that will be handled by the registry.
	///
	/// \param labels Set of labels that will be used by the agent.
	///               The labels are merged together with the registry's ones.
	[[nodiscard]] AgentType &Add(std::unordered_map<std::string, std::string> &&labels) {
		std::lock_guard<std::mutex> lock{mutex_};
		labels.insert(labels_.begin(), labels_.end());
		auto agent = std::make_unique<AgentType>(labels, flush_interval_, max_buffer_, log_level_, print_level_,
		                                         remote_host_, colors_);
		auto &ref  = *agent;
		agents_.push_back(std::move(agent));
		return ref;
	}

private:
	const std::unordered_map<std::string, std::string> labels_;
	const std::size_t flush_interval_;
	const std::size_t max_buffer_;
	const Level log_level_;
	const Level print_level_;
	const std::string remote_host_;
	const std::array<Color, 4> colors_;

	std::vector<std::unique_ptr<AgentType>> agents_;

	/// \brief Handles the flushing of the registered agents.
	std::thread thread_;

	std::mutex mutex_;
	std::atomic<bool> close_request_;
};

}  // namespace loki
