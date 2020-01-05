#ifndef LOKI_REGISTRY_HPP_
#define LOKI_REGISTRY_HPP_

#include <atomic>
#include <thread>

#include "agent.hpp"

namespace loki
{

/// \brief Handles the creation and flushing of agents.
///
/// The class is thread-safe.
class Registry
{

public:
	Registry(const std::map<std::string, std::string> &labels,
		  int flush_interval,
		  int max_buffer,
		  Level log_level,
		  Level print_level,
		  Protocol protocol,
		  std::array<TermColor, 4> colors);
	~Registry();

	bool Ready() const;

	/// \brief Retrieve Loki's Promethus metrics as a string.
	std::string Metrics() const;

	Agent &Add(std::map<std::string, std::string> labels = {});

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	Level log_level_;
	Level print_level_;
	Protocol protocol_;

	std::array<TermColor, 4> colors_;

	std::vector<std::unique_ptr<Agent>> agents_;
	std::mutex mutex_;

	/// \brief Handles the flushing of the registered agents.
	///
	/// Periodically flushes each agent's queue.
	/// Blocks registry destruction until all agents are done.
	std::thread thread_;

	std::chrono::system_clock::time_point last_flush_;
	std::atomic<bool> close_request_;

};

} // namespace loki

#endif /* LOKI_REGISTRY_HPP_ */
