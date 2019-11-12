#ifndef BUILDER_HPP_
#define BUILDER_HPP_

#include "agent.hpp"

namespace loki
{

class Builder
{
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	Agent::LogLevels log_level_;

public:
	Builder();

	Builder& Labels(const std::map<std::string, std::string> &labels);
	Builder& FlushInterval(int flush_interval);
	Builder& MaxBuffer(int max_buffer);
	Builder& LogLevel(Agent::LogLevels log_level);

	Agent Build();
};

} // namespace loki

#endif /* BUILDER_HPP_ */
