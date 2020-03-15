#ifndef AGENT_HPP_
#define AGENT_HPP_

#include <map>
#include <mutex>
#include <queue>

#include <curl/curl.h>
#include <fmt/format.h>

// #define USE_PROTOBUF

#if defined(__has_include) and defined(USE_PROTOBUF)
#  if __has_include("snappy.h") and __has_include("google/protobuf/port_def.inc")
#    define HAS_PROTOBUF
#  endif
#endif

namespace loki
{

/// \brief ASCII escape codes.
enum class Color : int {
	Black   =  30,
	Red     =  31,
	Green   =  32,
	Yellow  =  33,
	Blue    =  34,
	Magenta =  35,
	Cyan    =  36,
	White   =  37
};

/// \brief Logging levels.
enum class Level : int {
	Debug = 0,
	Info  = 1,
	Warn  = 2,
	Error = 3,

	/// \brief Don't print anything. Default value.
	Disable = 4
};

class Agent
{
protected:
	std::map<std::string, std::string> labels_;
	int flush_interval_;
	int max_buffer_;
	Level log_level_;
	Level print_level_;
	std::string compiled_labels_;
	std::array<Color, 4> colors_;

	std::queue<std::pair<std::string, timespec>> logs_;
	CURL *curl_;

	std::mutex mutex_;

	/// \brief Escape " character in string.
	std::string Escape(const std::string &line) const;

	std::string Format(fmt::string_view &&format, fmt::format_args &&args) const;

	/// \brief Handle incoming logs.
	void Log(std::string &&line, Level level);

	/// \brief Printing function.
	/// TODO: implement a user-defined print callaback
	void Print(const std::string &line, Level level, timespec ts) const;

public:
	Agent(
		const std::map<std::string, std::string> &labels,
		int flush_interval,
		int max_buffer,
		Level log_level,
		Level print_level,
		std::array<Color, 4> colors);

	~Agent();

	/// \brief Return true if log contains any elements.
	bool Done();

	/// \brief Flush all queued log lines.
	/// TODO: add endpoint configuration
	void Flush();

	/// \brief Send message to log queue with `Debug` priority.
	template <typename... Args>
	void Debugf(const std::string &format, const Args&... args)
	{
		Log(Format(format, fmt::make_format_args(args...)), Level::Debug);
	}

	/// \brief Send message to log queue with `Info` priority.
	template <typename... Args>
	void Infof(const std::string &format, const Args&... args)
	{
		Log(Format(format, fmt::make_format_args(args...)), Level::Info);
	}

	/// \brief Send message to log queue with `Warning` priority.
	template <typename... Args>
	void Warnf(const std::string &format, const Args&... args)
	{
		Log(Format(format, fmt::make_format_args(args...)), Level::Warn);
	}

	/// \brief Send message to log queue with `Error` priority.
	template <typename... Args>
	void Errorf(const std::string &format, const Args&... args)
	{
		Log(Format(format, fmt::make_format_args(args...)), Level::Error);
	}

};

class AgentJson final : public Agent
{
public:
	AgentJson(
		const std::map<std::string, std::string> &labels,
		int flush_interval,
		int max_buffer,
		Level log_level,
		Level print_level,
		std::array<Color, 4> colors);
	void Flush();
};


#if defined(HAS_PROTOBUF)
class AgentProto final : public Agent
{
public:
	AgentProto(
		const std::map<std::string, std::string> &labels,
		int flush_interval,
		int max_buffer,
		Level log_level,
		Level print_level,
		std::array<Color, 4> colors);
	void Flush();
};
#endif

} // namespace loki

#endif /* AGENT_HPP_ */
