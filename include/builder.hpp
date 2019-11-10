#ifndef BUILDER_HPP_
#define BUILDER_HPP_

#include "agent.hpp"

class Builder
{
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	LogLevels log_level_;

public:
	Builder();

	Builder& Labels(const std::map<std::string, std::string> &labels);
	Builder& FlushInterval(int flush_interval);
	Builder& MaxBuffer(int max_buffer);
	Builder& LogLevel(LogLevels log_level);

	Agent Build();

};

#endif /* BUILDER_HPP_ */
