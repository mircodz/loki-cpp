#include "builder.hpp"

namespace loki
{

Builder::Builder()
	: labels_{{}}
	, flush_interval_{5000}
	, max_buffer_{1000}
	, log_level_{Agent::Info} {}

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

Builder& Builder::LogLevel(Agent::LogLevels log_level)
{
	log_level_ = log_level;
	return *this;
}

Agent Builder::Build()
{
	return Agent{labels_, flush_interval_, max_buffer_, log_level_};
}

} // namespace loki
