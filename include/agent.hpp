#ifndef AGENT_HPP_
#define AGENT_HPP_

#include <map>
#include <mutex>
#include <queue>

#include <curl/curl.h>
#include <fmt/format.h>

namespace loki
{

/// ASCII escape codes
enum class TermColor : int {
	Black   =  30,
	Red     =  31,
	Green   =  32,
	Yellow  =  33,
	Blue    =  34,
	Magenta =  35,
	Cyan    =  36,
	White   =  37
};

enum class Level : int {
	Debug = 0,
	Info  = 1,
	Warn  = 2,
	Error = 3,

	// default printing level
	Disable = 4
};

/// Protocol to be used when flushing logs to Loki
enum class Protocol : int {
	Protobuf, Json
};

class Agent
{

public:
	Agent(const std::map<std::string, std::string> &labels,
		  int flush_interval,
		  int max_buffer,
		  Level log_level,
		  Level print_level,
		  Protocol protocol,
		  std::array<TermColor, 4> colors);

	~Agent();

	/// \brief Return true if log contains any elements.
	bool Done();

	template <typename... Args>
	void Debugf(fmt::string_view format, const Args&... args)
	{
		Log(fmt::vformat(format, fmt::make_format_args(args...)), Level::Debug);
	}

	template <typename... Args>
	void Infof(fmt::string_view format, const Args&... args)
	{
		Log(fmt::vformat(format, fmt::make_format_args(args...)), Level::Info);
	}

	template <typename... Args>
	void Warnf(fmt::string_view format, const Args&... args)
	{
		Log(fmt::vformat(format, fmt::make_format_args(args...)), Level::Warn);
	}

	template <typename... Args>
	void Errorf(fmt::string_view format, const Args&... args)
	{
		Log(fmt::vformat(format, fmt::make_format_args(args...)), Level::Error);
	}

	void Flush();

private:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	Level log_level_;
	Level print_level_;
	Protocol protocol_;
	std::string compiled_labels_;

	std::array<TermColor, 4> colors_;

	// the queue is unique for each agent
	std::queue<std::pair<timespec, std::string>> logs_;
	std::mutex mutex_;

	CURL *curl_;

	void Log(const std::string &line, Level level);
	void FlushJson();
	void FlushProto();

};

} // namespace loki

#endif /* AGENT_HPP_ */
