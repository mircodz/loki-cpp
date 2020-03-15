#include "builder.hpp"

namespace loki
{

template <typename T>
Builder<T>::Builder()
	: labels_{}
	, flush_interval_{5000}
	, max_buffer_{10000}
	, log_level_{Level::Info}
	, print_level_{Level::Disable}
	, colors_{{Color::White, Color::White, Color::White, Color::White}} {}

template <typename T>
Builder<T>& Builder<T>::Labels(const std::map<std::string, std::string> &labels)
{
	labels_ = labels;
	return *this;
}

template <typename T>
Builder<T>& Builder<T>::FlushInterval(int flush_interval)
{
	flush_interval_ = flush_interval;
	return *this;
}

template <typename T>
Builder<T>& Builder<T>::MaxBuffer(int max_buffer)
{
	max_buffer_ = max_buffer;
	return *this;
}

template <typename T>
Builder<T>& Builder<T>::LogLevel(Level log_level)
{
	log_level_ = log_level;
	return *this;
}

template <typename T>
Builder<T>& Builder<T>::PrintLevel(Level print_level)
{
	print_level_ = print_level;
	return *this;
}

template <typename T>
Builder<T>& Builder<T>::Colorize(Level level, Color color)
{
	colors_[static_cast<int>(level)] = color;
	return *this;
}

template <typename T>
Registry<T> Builder<T>::Build()
{
	return {labels_, flush_interval_, max_buffer_, log_level_, print_level_, colors_};
}

template class Builder<AgentJson>;
#if defined(HAS_PROTOBUF)
template class Builder<AgentProto>;
#endif

} // namespace loki
