#ifndef AGENT_HPP_
#define AGENT_HPP_

#include <atomic>
#include <chrono>
#include <map>
#include <mutex>
#include <queue>
#include <thread>

#include <curl/curl.h>
#include <fmt/format.h>

namespace loki
{

class Agent
{

public:
	enum class LogLevel { Debug = 0, Info = 1, Warn = 2, Error = 3 };
	enum class Protocol { Protobuf, Json };

	Agent(const std::map<std::string, std::string> &labels,
		  int flush_interval,
		  int max_buffer,
		  LogLevel log_level,
		  LogLevel print_level,
		  Protocol protocol);

	~Agent();

	bool Done();

	template <typename... Args>
	void Debugf(fmt::string_view format, const Args&... args)
	{
		Log(format, fmt::make_format_args(args...), LogLevel::Debug);
	}

	template <typename... Args>
	void Infof(fmt::string_view format, const Args&... args)
	{
		Log(format, fmt::make_format_args(args...), LogLevel::Info);
	}

	template <typename... Args>
	void Warnf(fmt::string_view format, const Args&... args)
	{
		Log(format, fmt::make_format_args(args...), LogLevel::Warn);
	}

	template <typename... Args>
	void Errorf(fmt::string_view format, const Args&... args)
	{
		Log(format, fmt::make_format_args(args...), LogLevel::Error);
	}

	void Flush();

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	LogLevel log_level_;
	LogLevel print_level_;
	Protocol protocol_;
	std::string compiled_labels_;

	// the queue is unique for each agent
	std::queue<std::pair<timespec, std::string>> logs_;
	std::mutex mutex_;

	CURL *curl_;

	void Log(fmt::string_view format, fmt::format_args args, LogLevel level);
	void Log(const std::string &line, LogLevel level);
	void FlushJson();
	void FlushProto();

};

} // namespace loki

#endif /* AGENT_HPP_ */
