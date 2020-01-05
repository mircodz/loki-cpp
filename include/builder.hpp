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
	Builder& LogLevel(Level log_level);
	Builder& PrintLevel(Level print_level);
	Builder& FlushProtocol(Protocol protocol);
	Builder& Colorize(Level level, TermColor color);

	Registry Build();

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	Level log_level_;
	Level print_level_;
	Protocol protocol_;

	std::array<TermColor, 4> colors_;

};

} // namespace loki

#endif /* BUILDER_HPP_ */
