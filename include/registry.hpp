#ifndef REGISTRY_HPP_
#define REGISTRY_HPP_

#include <map>
#include <mutex>
#include <string>

#include "agent.hpp"

namespace loki
{

class Registry
{

public:
	enum LogLevels { Debug, Info, Warn, Error };

	Registry(const std::map<std::string, std::string> &labels,
		  int flush_interval,
		  int max_buffer,
		  Agent::LogLevels log_level);
	~Registry();

	bool Ready();
	std::string Metrics();

	Agent &Add(std::map<std::string, std::string> labels = {});

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	Agent::LogLevels log_level_;

	std::vector<std::unique_ptr<Agent>> agents_;

	std::mutex mutex_;

	// handle continuous flushing
	std::chrono::system_clock::time_point last_flush_;
	std::atomic<bool> close_request_;
	std::thread thread_;

};

} // namespace loki

#endif /* REGISTRY_HPP_ */
