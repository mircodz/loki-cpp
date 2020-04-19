#ifndef AGENT_HPP_
#define AGENT_HPP_

#include <map>
#include <mutex>
#include <queue>

#include <curl/curl.h>
#include <fmt/format.h>

#define USE_PROTOBUF

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
	White   =  37,
};

/// \brief Logging levels.
enum class Level : int {
	Debug = 0,
	Info  = 1,
	Warn  = 2,
	Error = 3,

	/// \brief Don't print anything. Default value.
	Disable = 4,
};

class Agent
{
protected:
	std::map<std::string, std::string> labels_;
	std::size_t flush_interval_;
	std::size_t max_buffer_;
	Level log_level_;
	Level print_level_;
	std::string compiled_labels_;
	std::array<Color, 4> colors_;

	std::queue<std::pair<std::string, timespec>> logs_;
	CURL *curl_;

	std::mutex mutex_;

	/// \brief Escape " character in string.
	void Escape(std::string &line, const std::string &str);

	/// \brief Handle incoming logs.
	void Log(std::string &&line, Level level);

	/// \brief Printing function.
	/// TODO: implement a user-defined print callaback
	void Print(const std::string &line, Level level, timespec ts) const;

	void BuildLabels();

public:
	Agent(
			std::map<std::string, std::string> &&labels,
			std::size_t flush_interval,
			std::size_t max_buffer,
			Level log_level,
			Level print_level,
			std::array<Color, 4> colors)
	: labels_{labels}
	, flush_interval_{flush_interval}
	, max_buffer_{max_buffer}
	, log_level_{log_level}
	, print_level_{print_level}
	, colors_{colors}
	, logs_{}
	, mutex_{}
	{
		curl_ = curl_easy_init();
	}

	~Agent() {
		curl_easy_cleanup(curl_);
		curl_ = nullptr;
	}

	/// \brief Return true if log contains any elements.
	bool Done() {
		std::lock_guard<std::mutex> lock{mutex_};
		return !logs_.empty();
	}

	/// \brief Flush all queued log lines.
	/// TODO: add endpoint configuration
	void Flush() {}

	/// \brief Send message to log queue with `Debug` priority.
	template <typename... Args>
	void Debugf(const std::string &format, const Args&... args)
	{
		fmt::format_args argspack = fmt::make_format_args(args...);
		Log(fmt::vformat(format, argspack), Level::Debug);
	}

	/// \brief Send message to log queue with `Info` priority.
	template <typename... Args>
	void Infof(const std::string &format, const Args&... args)
	{
		fmt::format_args argspack = fmt::make_format_args(args...);
		Log(fmt::vformat(format, argspack), Level::Info);
	}

	/// \brief Send message to log queue with `Warning` priority.
	template <typename... Args>
	void Warnf(const std::string &format, const Args&... args)
	{
		fmt::format_args argspack = fmt::make_format_args(args...);
		Log(fmt::vformat(format, argspack), Level::Warn);
	}

	/// \brief Send message to log queue with `Error` priority.
	template <typename... Args>
	void Errorf(const std::string &format, const Args&... args)
	{
		fmt::format_args argspack = fmt::make_format_args(args...);
		Log(fmt::vformat(format, argspack), Level::Error);
	}

};

class AgentJson final : public Agent
{
protected:
	void BuildLabels();
public:
	AgentJson(
			std::map<std::string, std::string> &&labels,
			std::size_t flush_interval,
			std::size_t max_buffer,
			Level log_level,
			Level print_level,
			std::array<Color, 4> colors)
	: Agent{std::move(labels), flush_interval, max_buffer, log_level, print_level, colors} {
		BuildLabels();
	}
	void Flush();
};


#if defined(HAS_PROTOBUF)
class AgentProto final : public Agent
{
protected:
	void BuildLabels();
public:
	AgentProto(
			std::map<std::string, std::string> &&labels,
			std::size_t flush_interval,
			std::size_t max_buffer,
			Level log_level,
			Level print_level,
			std::array<Color, 4> colors)
	: Agent{std::move(labels), flush_interval, max_buffer, log_level, print_level, colors} {
		BuildLabels();
	}
	void Flush();
};
#endif

} // namespace loki

#endif /* AGENT_HPP_ */
