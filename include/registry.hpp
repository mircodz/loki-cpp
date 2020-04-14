#ifndef LOKI_REGISTRY_HPP_
#define LOKI_REGISTRY_HPP_

#include <atomic>
#include <thread>

#include "agent.hpp"
#include "parser.hpp"

namespace loki
{

/// \brief Handles the creation and flushing of agents.
///
/// The class is thread-safe.
template <typename AgentType>
class Registry
{

public:
	Registry(
		std::map<std::string, std::string> &&labels,
		std::size_t flush_interval,
		std::size_t max_buffer,
		Level log_level,
		Level print_level,
		std::array<Color, 4> colors);
	~Registry();

	[[nodiscard]] bool Ready() const;

	/// \brief Retrieve Loki's Promethus metrics.
	[[nodiscard]] std::vector<Metric> Metrics() const;

	[[nodiscard]] AgentType &Add(std::map<std::string, std::string> &&labels);

private:
	std::map<std::string, std::string> labels_;
	std::size_t flush_interval_;
	std::size_t max_buffer_;
	Level log_level_;
	Level print_level_;

	std::array<Color, 4> colors_;

	std::vector<std::unique_ptr<AgentType>> agents_;
	std::mutex mutex_;

	/// \brief Handles the flushing of the registered agents.
	///
	/// Periodically flushes each agent's queue.
	/// Blocks registry destruction until all agents are done.
	std::thread thread_;

	std::atomic<bool> close_request_;

};

} // namespace loki

#endif /* LOKI_REGISTRY_HPP_ */
