#include "builder.hpp"

Builder::Builder()
	: labels_({})
	, flush_interval_(5000)
	, max_buffer_(1000)
	, log_level_(Info) {}

Builder& Builder::Labels(const std::map<std::string, std::string> &labels)
{
	labels_ = labels;
	return *this;
}

Builder& Builder::FlushInterval(const int flush_interval)
{
	flush_interval_ = flush_interval;
	return *this;
}

Builder& Builder::MaxBuffer(const int max_buffer)
{
	max_buffer_ = max_buffer;
	return *this;
}

Builder& Builder::LogLevel(const LogLevels log_level)
{
	log_level_ = log_level;
	return *this;
}

Agent Builder::Build()
{
	return Agent{labels_, flush_interval_, max_buffer_, log_level_};
}