#ifndef BUILDER_HPP_
#define BUILDER_HPP_

#include "registry.hpp"

namespace loki
{

class Builder
{

public:
	Builder();
	Builder& Labels(const std::map<std::string, std::string> &labels);
	Builder& FlushInterval(int flush_interval);
	Builder& MaxBuffer(int max_buffer);
	Builder& LogLevel(Agent::LogLevel log_level);
	Builder& PrintLevel(Agent::LogLevel print_level);
	Builder& Protocol(Agent::Protocol protocol);

	Registry Build();

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	Agent::LogLevel log_level_;
	Agent::LogLevel print_level_;
	Agent::Protocol protocol_;

};

} // namespace loki

#endif /* BUILDER_HPP_ */
