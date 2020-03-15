#ifndef BUILDER_HPP_
#define BUILDER_HPP_

#include "registry.hpp"

namespace loki
{

template <typename T>
class Builder
{

public:
	Builder();
	Builder<T>& Labels(const std::map<std::string, std::string> &labels);
	Builder<T>& FlushInterval(int flush_interval);
	Builder<T>& MaxBuffer(int max_buffer);
	Builder<T>& LogLevel(Level log_level);
	Builder<T>& PrintLevel(Level print_level);
	Builder<T>& Colorize(Level level, Color color);

	Registry<T> Build();

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	Level log_level_;
	Level print_level_;

	std::array<Color, 4> colors_;
};

} // namespace loki

#endif /* BUILDER_HPP_ */
