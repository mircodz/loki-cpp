#include "builder.hpp"

namespace loki
{

Builder::Builder()
	: labels_{}
	, flush_interval_{5000}
	, max_buffer_{10000}
	, log_level_{Agent::LogLevel::Info}
	, print_level_{Agent::LogLevel::Disable}
	, protocol_{Agent::Protocol::Protobuf}
	, colors_{{ Agent::TermColor::White,
				Agent::TermColor::White,
				Agent::TermColor::White,
				Agent::TermColor::White  }} {}

Builder& Builder::Labels(const std::map<std::string, std::string> &labels)
{
	labels_ = labels;
	return *this;
}

Builder& Builder::FlushInterval(int flush_interval)
{
	flush_interval_ = flush_interval;
	return *this;
}

Builder& Builder::MaxBuffer(int max_buffer)
{
	max_buffer_ = max_buffer;
	return *this;
}

Builder& Builder::LogLevel(Agent::LogLevel log_level)
{
	log_level_ = log_level;
	return *this;
}

Builder& Builder::PrintLevel(Agent::LogLevel print_level)
{
	print_level_ = print_level;
	return *this;
}

Builder& Builder::Protocol(Agent::Protocol protocol)
{
	protocol_ = protocol;
	return *this;
}

Builder& Builder::Colorize(Agent::LogLevel level, Agent::TermColor color)
{
	colors_[static_cast<int>(level)] = color;
	return *this;
}

Registry Builder::Build()
{
	return Registry{labels_, flush_interval_, max_buffer_, log_level_, print_level_, protocol_, colors_};
}

} // namespace loki
