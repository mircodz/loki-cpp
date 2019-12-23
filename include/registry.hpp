#ifndef REGISTRY_HPP_
#define REGISTRY_HPP_

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
		  Agent::LogLevel log_level,
		  Agent::LogLevel print_level,
		  Agent::Protocol protocol);
	~Registry();

	bool Ready() const;

	/// \brief Retrieve Loki's Promethus metrics as a string.
	std::string Metrics() const;

	Agent &Add(std::map<std::string, std::string> labels = {});

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	Agent::LogLevel log_level_;
	Agent::LogLevel print_level_;
	Agent::Protocol protocol_;

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

#endif /* REGISTRY_HPP_ */
