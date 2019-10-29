#ifndef BUILDER_H
#define BUILDER_H

#include "agent.hpp"

class Builder
{
public:
	Builder();

	Builder& Labels(const std::map<std::string, std::string> &labels);
	Builder& FlushInterval(const int flush_interval);
	Builder& MaxBuffer(const int max_buffer);
	Builder& LogLevel(const LogLevels log_level);

	Agent Build();

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	LogLevels log_level_;

};

#endif /* BUILDER_H */
